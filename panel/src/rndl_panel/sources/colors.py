"""A window gliding over a soothing, continuous color field, following cubic
Bezier curves between random waypoints at a configurable base speed, with a
choice of easing rules and colorspaces. No external imaging dependency - just
stdlib colorsys/math/random."""

import argparse
import colorsys
import math
import random
import time
from pathlib import Path

from ..common import DEFAULT_BRIGHTNESS_CAP, FRAME_SIZE_BYTES, PANEL_HEIGHT, PANEL_WIDTH, write_frame_atomic
from .base import PanelSource

DEFAULT_FPS = 10.0  # matches the daemon's poll rate; no point animating faster
DEFAULT_WINDOW_SIZE = 1.5  # colorspace units spanning the panel
DEFAULT_STEP_RANGE = 2.0  # how far (in colorspace units) each new waypoint can be from the last
DEFAULT_CURVINESS = 1.0  # how far control points bulge off the straight chord between waypoints
DEFAULT_SPEED = 0.2  # colorspace units/sec the window glides along a curve
DEFAULT_SATURATION = 0.55  # pastel rather than neon
MIN_SEGMENT_DURATION_S = 1.0  # floor, in case a random waypoint lands very close to the last one
OKLAB_CHROMA_RADIUS = 0.15  # typical chroma before sRGB gamut clipping gets aggressive

Point = tuple[float, float]


# --- Easing rules: reparametrize a segment's [0, 1] time axis so motion isn't
# constant-speed even along a single curve. -----------------------------------
EASINGS = {
    "linear": lambda t: t,
    "ease-in": lambda t: t * t,
    "ease-out": lambda t: 1.0 - (1.0 - t) * (1.0 - t),
    "smoothstep": lambda t: t * t * (3.0 - 2.0 * t),  # cubic ease-in-out
}


# --- Colorspaces: u, v (continuous colorspace coordinates), saturation, value
# (both 0-1) -> (r, g, b) bytes. ----------------------------------------------
def _hsv_rgb(hue: float, saturation: float, value: float) -> tuple[int, int, int]:
    r, g, b = colorsys.hsv_to_rgb(hue, saturation, value)
    return round(r * 255), round(g * 255), round(b * 255)


def _sine_field(u: float, v: float, saturation: float, value: float) -> tuple[int, int, int]:
    """Sum of sine waves at incommensurate frequencies/phases: nearby points
    blend continuously and the pattern never obviously repeats or tiles."""
    h = math.sin(u * 0.9 + v * 0.5) + math.sin(u * 0.3 - v * 0.7 + 1.3) + math.sin((u + v) * 0.17 - 0.4)
    hue = (h / 3.0 + 1.0) / 2.0  # sum of 3 unit sines is in [-3,3] -> hue in [0,1]
    return _hsv_rgb(hue, saturation, value)


def _radial_field(u: float, v: float, saturation: float, value: float) -> tuple[int, int, int]:
    """Hue is the angle around the colorspace origin: a pinwheel that looks
    different depending on where in the space the window currently is."""
    hue = (math.atan2(v, u) / (2.0 * math.pi)) % 1.0
    return _hsv_rgb(hue, saturation, value)


def _srgb_gamma_correct(u: float) -> float:
    """sRGB gamma correction.
    Source: https://en.wikipedia.org/wiki/SRGB#The_sRGB_transfer_function_(%22gamma%22)"""
    if u <= 0.0031308:
        return 12.92 * u
    return 1.055 * (u ** (1.0 / 2.4)) - 0.055


def _oklab_to_rgb(lightness: float, a: float, b: float) -> tuple[int, int, int]:
    """Oklab -> sRGB. Matrix coefficients from the Oklab specification.
    Source: https://bottosson.github.io/posts/oklab/#converting-from-oklab-to-linear-srgb
    Ported from https://github.com/corytodd/colorspace-test/blob/main/lib/oklab.c"""
    l_ = (lightness + 0.3963377774 * a + 0.2158037573 * b) ** 3
    m_ = (lightness - 0.1055613458 * a - 0.0638541728 * b) ** 3
    s_ = (lightness - 0.0894841775 * a - 1.2914855480 * b) ** 3

    r_lin = max(0.0, min(1.0, +4.0767416621 * l_ - 3.3077115913 * m_ + 0.2309699292 * s_))
    g_lin = max(0.0, min(1.0, -1.2684380046 * l_ + 2.6097574011 * m_ - 0.3413193965 * s_))
    b_lin = max(0.0, min(1.0, -0.0041960863 * l_ - 0.7034186147 * m_ + 1.7076147010 * s_))

    return (
        round(_srgb_gamma_correct(r_lin) * 255),
        round(_srgb_gamma_correct(g_lin) * 255),
        round(_srgb_gamma_correct(b_lin) * 255),
    )


def _oklab_field(u: float, v: float, saturation: float, value: float) -> tuple[int, int, int]:
    """Glides over Oklab's a/b chroma plane at a fixed lightness, rather than
    rotating HSV hue - Oklab keeps perceived lightness and colorfulness much
    more consistent across the sweep, where HSV hue rotation at constant V is
    famously uneven. a/b are driven by bounded sinusoids of u,v (not u,v
    directly) so chroma can't grow without bound as the window wanders over a
    long session - see Curve, which never resets back toward the origin."""
    radius = max(0.0, min(1.0, saturation)) * OKLAB_CHROMA_RADIUS
    a = radius * math.sin(u * 0.6 + v * 0.3)
    b = radius * math.sin(v * 0.6 - u * 0.3 + 1.0)
    return _oklab_to_rgb(value, a, b)


COLORSPACES = {"sine": _sine_field, "radial": _radial_field, "oklab": _oklab_field}


def _bezier(p0: Point, p1: Point, p2: Point, p3: Point, t: float) -> Point:
    """Cubic Bezier: a degree-3 polynomial in t through p0..p3."""
    mt = 1.0 - t
    a, b, c, d = mt**3, 3 * mt**2 * t, 3 * mt * t**2, t**3
    return (
        a * p0[0] + b * p1[0] + c * p2[0] + d * p3[0],
        a * p0[1] + b * p1[1] + c * p2[1] + d * p3[1],
    )


class Curve:
    """Wanders a point through the colorspace along cubic-Bezier segments
    between random waypoints, each traversed at `speed` colorspace-units/sec
    (by chord length) with the segment's time axis reparametrized by `ease`."""

    def __init__(self, step_range: float, curviness: float, speed: float, ease):
        self.step_range = step_range
        self.curviness = curviness
        self.speed = speed
        self.ease = ease
        self.p0 = (0.0, 0.0)
        self.p1, self.p2, self.p3, self.duration = self._next_segment(self.p0)
        self.segment_start = time.monotonic()

    def _next_segment(self, p0: Point) -> tuple[Point, Point, Point, float]:
        p3 = (
            p0[0] + random.uniform(-self.step_range, self.step_range),
            p0[1] + random.uniform(-self.step_range, self.step_range),
        )
        dx, dy = p3[0] - p0[0], p3[1] - p0[1]
        # Control points sit 1/3 and 2/3 along the chord, bulged perpendicular
        # to it, so the path curves instead of running straight.
        nx, ny = -dy, dx
        bulge = self.curviness * random.uniform(-1.0, 1.0)
        p1 = (p0[0] + dx / 3 + nx * bulge, p0[1] + dy / 3 + ny * bulge)
        p2 = (p0[0] + 2 * dx / 3 + nx * bulge, p0[1] + 2 * dy / 3 + ny * bulge)
        duration = max(MIN_SEGMENT_DURATION_S, math.hypot(dx, dy) / self.speed)
        return p1, p2, p3, duration

    def advance(self, now: float) -> Point:
        t = (now - self.segment_start) / self.duration
        if t >= 1.0:
            self.p0 = self.p3
            self.p1, self.p2, self.p3, self.duration = self._next_segment(self.p0)
            self.segment_start = now
            t = 0.0
        return _bezier(self.p0, self.p1, self.p2, self.p3, self.ease(max(0.0, min(1.0, t))))


def render_frame(center: Point, colorspace, window_size: float, saturation: float, brightness_cap: int) -> bytes:
    """Sample a PANEL_WIDTH x PANEL_HEIGHT window of `colorspace`, `window_size`
    colorspace-units wide, centered on `center`, into row-major RGB bytes."""
    saturation = max(0.0, min(1.0, saturation))
    value = max(0.0, min(1.0, brightness_cap / 255.0))
    scale = window_size / PANEL_WIDTH
    cu, cv = center

    frame = bytearray(FRAME_SIZE_BYTES)
    for y in range(PANEL_HEIGHT):
        for x in range(PANEL_WIDTH):
            u = cu + (x - PANEL_WIDTH / 2) * scale
            v = cv + (y - PANEL_HEIGHT / 2) * scale
            r, g, b = colorspace(u, v, saturation, value)
            idx = (y * PANEL_WIDTH + x) * 3
            frame[idx : idx + 3] = bytes((max(0, min(255, r)), max(0, min(255, g)), max(0, min(255, b))))
    return bytes(frame)


class ColorsSource(PanelSource):
    name = "colors"
    help = "A window gliding over a soothing color field along eased random curves"

    def add_arguments(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument("--fps", type=float, default=DEFAULT_FPS, help=f"Animation rate (default: {DEFAULT_FPS})")
        parser.add_argument(
            "--colorspace",
            choices=list(COLORSPACES),
            default="sine",
            help="Hue field the window glides over (default: sine)",
        )
        parser.add_argument(
            "--easing",
            choices=list(EASINGS),
            default="smoothstep",
            help="How speed varies within each curve segment (default: smoothstep)",
        )
        parser.add_argument(
            "--window-size",
            type=float,
            default=DEFAULT_WINDOW_SIZE,
            help=f"Colorspace units spanning the panel; smaller is smoother (default: {DEFAULT_WINDOW_SIZE})",
        )
        parser.add_argument(
            "--step-range",
            type=float,
            default=DEFAULT_STEP_RANGE,
            help=f"How far each waypoint can be from the last, in colorspace units (default: {DEFAULT_STEP_RANGE})",
        )
        parser.add_argument(
            "--curviness",
            type=float,
            default=DEFAULT_CURVINESS,
            help=f"How far the path bulges off a straight line between waypoints (default: {DEFAULT_CURVINESS})",
        )
        parser.add_argument(
            "--speed",
            type=float,
            default=DEFAULT_SPEED,
            help=f"Base speed, in colorspace units/sec (default: {DEFAULT_SPEED})",
        )
        parser.add_argument(
            "--saturation",
            type=float,
            default=DEFAULT_SATURATION,
            help=f"Color saturation, 0 (white) to 1 (vivid) (default: {DEFAULT_SATURATION})",
        )
        parser.add_argument(
            "--brightness-cap",
            type=int,
            default=DEFAULT_BRIGHTNESS_CAP,
            help="Max per-channel value (0-255); all 256 pixels are lit continuously, so keep this "
            f"bounded (default: {DEFAULT_BRIGHTNESS_CAP})",
        )

    def run(self, framebuffer_path: Path, args: argparse.Namespace) -> None:
        print(
            f"Wandering the {args.colorspace!r} colorspace: speed {args.speed} units/s, "
            f"step range {args.step_range}, curviness {args.curviness}, easing {args.easing!r}, "
            f"window {args.window_size}, saturation {args.saturation}, brightness cap {args.brightness_cap}"
        )
        print(f"Writing frames to {framebuffer_path}")

        colorspace = COLORSPACES[args.colorspace]
        curve = Curve(args.step_range, args.curviness, args.speed, EASINGS[args.easing])
        frame_period = 1.0 / args.fps
        while True:
            tick_start = time.monotonic()
            center = curve.advance(tick_start)
            frame = render_frame(center, colorspace, args.window_size, args.saturation, args.brightness_cap)
            write_frame_atomic(frame, framebuffer_path)
            elapsed = time.monotonic() - tick_start
            time.sleep(max(0.0, frame_period - elapsed))


SOURCE = ColorsSource()
