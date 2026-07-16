"""Renders an EKG-like waveform driven by host CPU usage.

Heart rate and spike amplitude scale with CPU load: idle looks like a calm
resting heartbeat, a busy CPU looks tachycardic.
"""

import argparse
import math
import time
from collections import deque
from pathlib import Path

import psutil

from ..common import FRAME_SIZE_BYTES, PANEL_HEIGHT, PANEL_WIDTH, write_frame_atomic
from .base import PanelSource

DEFAULT_TICK_INTERVAL_S = 0.1
DEFAULT_MIN_BPM = 50.0
DEFAULT_MAX_BPM = 180.0

BASELINE_Y = 10

DEFAULT_MIN_AMPLITUDE_PX = 3.2
DEFAULT_MAX_AMPLITUDE_PX = 8.0

CALM_COLOR = (0, 255, 90)
STRESSED_COLOR = (255, 40, 20)

# Oldest column in the trail renders at this fraction of full brightness; newest is always 1.0.
TRAIL_MIN_BRIGHTNESS = 0.04
# >1 biases the falloff toward the tail end: recent columns stay near full brightness and only
# the older half of the trail visibly dims, instead of a flat linear ramp across all 16 columns.
TRAIL_FADE_GAMMA = 2.4
# How far the current-sample pixel is blended toward white to read as a bright "beam head".
HEAD_HIGHLIGHT_MIX = 0.6

# Waveform shape (one P-QRS-T complex), as fractions of one heartbeat cycle.
P_WAVE_PHASE_END = 0.08
QRS_PHASE_END = 0.14
T_WAVE_PHASE_END = 0.4

P_WAVE_AMPLITUDE = 0.15
T_WAVE_AMPLITUDE = 0.25

# The QRS complex is three sub-segments (Q dip, R spike, S dip), given as fractions of the
# QRS window's own width, and the signal amplitude reached by the end of each sub-segment.
QRS_Q_WIDTH = 0.3
QRS_R_WIDTH = 0.3
QRS_S_WIDTH = 0.4
QRS_Q_AMPLITUDE = -0.3
QRS_R_AMPLITUDE = 1.0
QRS_S_AMPLITUDE = -0.2


def parse_rgb_hex(value: str) -> tuple[int, int, int]:
    """Parse a 6-digit hex RGB code like 'ff8800' or '#ff8800'."""
    s = value.strip().lstrip("#")
    if len(s) != 6:
        raise argparse.ArgumentTypeError(f"expected a 6-digit hex RGB code (e.g. 'ff8800'), got {value!r}")
    try:
        r = int(s[0:2], 16)
        g = int(s[2:4], 16)
        b = int(s[4:6], 16)
    except ValueError:
        raise argparse.ArgumentTypeError(f"expected a 6-digit hex RGB code (e.g. 'ff8800'), got {value!r}")
    return (r, g, b)


PEAK_HOLD_SUBSAMPLES = 200


def ekg_waveform(phase: float) -> float:
    """One heartbeat cycle for phase in [0, 1). Returns a signal in roughly [-1, 1],
    modeled loosely after a P-QRS-T complex."""
    if phase < P_WAVE_PHASE_END:
        # P wave: small rounded bump
        t = phase / P_WAVE_PHASE_END
        return P_WAVE_AMPLITUDE * math.sin(t * math.pi)

    if phase < QRS_PHASE_END:
        # QRS complex: sharp down-up-down spike
        t = (phase - P_WAVE_PHASE_END) / (QRS_PHASE_END - P_WAVE_PHASE_END)
        if t < QRS_Q_WIDTH:
            return QRS_Q_AMPLITUDE * (t / QRS_Q_WIDTH)
        if t < (QRS_Q_WIDTH + QRS_R_WIDTH):
            return QRS_Q_AMPLITUDE + (QRS_R_AMPLITUDE - QRS_Q_AMPLITUDE) * ((t - QRS_Q_WIDTH) / QRS_R_WIDTH)
        return QRS_R_AMPLITUDE + (QRS_S_AMPLITUDE - QRS_R_AMPLITUDE) * (
            (t - QRS_Q_WIDTH - QRS_R_WIDTH) / QRS_S_WIDTH
        )

    if phase < T_WAVE_PHASE_END:
        # T wave: broad rounded bump
        t = (phase - QRS_PHASE_END) / (T_WAVE_PHASE_END - QRS_PHASE_END)
        return T_WAVE_AMPLITUDE * math.sin(t * math.pi)

    return 0.0


def sample_peak_signal(phase_start: float, phase_delta: float) -> float:
    """Advance from phase_start by phase_delta, but instead of reading the waveform only at
    the endpoint, scan sub-phases across that span and keep whichever has the largest
    magnitude. This is what lets a brief QRS spike still show up even when a tick's phase
    advance is wider than the spike itself."""
    peak_signal = ekg_waveform(phase_start % 1.0)
    for step in range(1, PEAK_HOLD_SUBSAMPLES + 1):
        sub_phase = (phase_start + phase_delta * step / PEAK_HOLD_SUBSAMPLES) % 1.0
        candidate_signal = ekg_waveform(sub_phase)
        if abs(candidate_signal) > abs(peak_signal):
            peak_signal = candidate_signal
    return peak_signal


def _lerp_color(a: tuple[int, int, int], b: tuple[int, int, int], t: float) -> tuple[int, int, int]:
    t = max(0.0, min(1.0, t))
    r = round(a[0] + (b[0] - a[0]) * t)
    g = round(a[1] + (b[1] - a[1]) * t)
    bl = round(a[2] + (b[2] - a[2]) * t)
    return (r, g, bl)


def render_frame(history: deque[int], cpu_fraction: float, base_color: tuple[int, int, int] | None = None) -> bytes:
    if base_color is not None:
        color = base_color
    else:
        color = _lerp_color(CALM_COLOR, STRESSED_COLOR, cpu_fraction)

    frame = bytearray(FRAME_SIZE_BYTES)
    cols = list(history)
    last_x = len(cols) - 1

    for x, y in enumerate(cols):
        if x > 0:
            y_prev = cols[x - 1]
        else:
            y_prev = y
        y0 = min(y_prev, y)
        y1 = max(y_prev, y)

        if last_x > 0:
            t = (x / last_x) ** TRAIL_FADE_GAMMA
        else:
            t = 1.0
        fade = TRAIL_MIN_BRIGHTNESS + (1 - TRAIL_MIN_BRIGHTNESS) * t

        r = round(color[0] * fade)
        g = round(color[1] * fade)
        bl = round(color[2] * fade)
        segment_color = (r, g, bl)

        for py in range(y0, y1 + 1):
            idx = (py * PANEL_WIDTH + x) * 3
            frame[idx: idx + 3] = bytes(segment_color)

    # Highlight the current sample as a bright "beam head" on top of the fading trail.
    head_y = cols[last_x]
    head_idx = (head_y * PANEL_WIDTH + last_x) * 3
    frame[head_idx: head_idx + 3] = bytes(_lerp_color(color, (255, 255, 255), HEAD_HIGHLIGHT_MIX))
    return bytes(frame)


class CpuEkgSource(PanelSource):
    name = "cpu-ekg"
    help = "EKG-style heartbeat scaled by host CPU usage: busier CPU = faster, bigger beats"

    def add_arguments(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            "--tick-interval",
            type=float,
            default=DEFAULT_TICK_INTERVAL_S,
            help=f"Seconds between frame updates (default: {DEFAULT_TICK_INTERVAL_S})",
        )
        parser.add_argument(
            "--min-bpm",
            type=float,
            default=DEFAULT_MIN_BPM,
            help=f"Heart rate at 0%% CPU usage (default: {DEFAULT_MIN_BPM})",
        )
        parser.add_argument(
            "--max-bpm",
            type=float,
            default=DEFAULT_MAX_BPM,
            help=f"Heart rate at 100%% CPU usage (default: {DEFAULT_MAX_BPM})",
        )
        parser.add_argument(
            "--min-amplitude",
            type=float,
            default=DEFAULT_MIN_AMPLITUDE_PX,
            help=f"Spike height in pixels at 0%% CPU usage (default: {DEFAULT_MIN_AMPLITUDE_PX})",
        )
        parser.add_argument(
            "--max-amplitude",
            type=float,
            default=DEFAULT_MAX_AMPLITUDE_PX,
            help=f"Spike height in pixels at 100%% CPU usage (default: {DEFAULT_MAX_AMPLITUDE_PX})",
        )
        parser.add_argument(
            "--color",
            type=parse_rgb_hex,
            default=None,
            help="Fixed hex RGB trace color (e.g. 'ff8800'). Default: shifts from green to red as "
            "CPU usage rises",
        )

    def run(self, framebuffer_path: Path, args: argparse.Namespace) -> None:
        print(f"Writing frames to {framebuffer_path}")
        history: deque[int] = deque([BASELINE_Y] * PANEL_WIDTH, maxlen=PANEL_WIDTH)

        psutil.cpu_percent(interval=None)  # prime the internal sample window
        phase = 0.0
        while True:
            cpu_fraction = psutil.cpu_percent(interval=None) / 100.0
            bpm = args.min_bpm + (args.max_bpm - args.min_bpm) * cpu_fraction
            amplitude = args.min_amplitude + (args.max_amplitude - args.min_amplitude) * cpu_fraction

            phase_delta = (bpm / 60.0) * args.tick_interval
            signal = sample_peak_signal(phase, phase_delta)
            phase = (phase + phase_delta) % 1.0
            y = round(BASELINE_Y - signal * amplitude)
            y = max(0, min(PANEL_HEIGHT - 1, y))
            history.append(y)

            write_frame_atomic(render_frame(history, cpu_fraction, args.color), framebuffer_path)
            time.sleep(args.tick_interval)


SOURCE = CpuEkgSource()
