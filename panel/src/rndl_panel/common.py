"""Shared constants/helpers for the rndl panel framebuffer file and BLE protocol."""

import asyncio
import os
import tempfile
import time
from pathlib import Path

PANEL_WIDTH = 16
PANEL_HEIGHT = 16
FRAME_SIZE_BYTES = PANEL_WIDTH * PANEL_HEIGHT * 3


def _default_state_dir() -> Path:
    """A small per-app directory, distinct from the shared system temp dir. Only this app
    writes here, so it's a narrow, low-risk target to exclude from AV real-time scanning if
    the rename race in write_frame_atomic needs to be avoided rather than just retried."""
    if os.name == "nt":
        base = Path(os.environ.get("LOCALAPPDATA", Path.home() / "AppData" / "Local"))
    else:
        base = Path(os.environ.get("XDG_STATE_HOME", Path.home() / ".local" / "state"))
    return base / "rndl-panel"


DEFAULT_FRAMEBUFFER_PATH = _default_state_dir() / "framebuffer.bin"

DEVICE_NAME = "rndl-panel"
PIXEL_WRITE_CHAR_UUID = "0000c5d3-6f61-a869-4129-89e9adca439c"
DIMENSIONS_CHAR_UUID = "d6e0f611-6004-4922-b343-cbadbe41ae5e"
PIXEL_RECORD_SIZE_BYTES = 5

WRITE_RETRIES = 5
WRITE_RETRY_BACKOFF_S = 0.2

# Windows won't let os.replace() clobber a file another process has open, unlike POSIX
# rename().
FRAME_REPLACE_RETRIES = 20
FRAME_REPLACE_BACKOFF_S = 0.005
FRAME_REPLACE_MAX_BACKOFF_S = 0.1

# 256 WS2812 LEDs at full white draw too much current. Cap this to avoid sadness.
DEFAULT_BRIGHTNESS_CAP = 32


async def with_retry(coro_fn, *args, retries: int = WRITE_RETRIES, backoff: float = WRITE_RETRY_BACKOFF_S, **kwargs):
    """Retry an async BLE operation a few times. WinRT/bleak can flake with a
    spurious Windows error: 'operation was canceled by the user' OSError.

    TODO: does Linux see this same issue? Might be my BLE adapter.
    """
    for attempt in range(1, retries + 1):
        try:
            return await coro_fn(*args, **kwargs)
        except OSError:
            if attempt == retries:
                raise
            await asyncio.sleep(backoff)


def write_frame_atomic(frame: bytes, path: Path) -> None:
    """Write a frame to disk such that readers never observe a partial write."""
    assert len(frame) == FRAME_SIZE_BYTES, f"frame must be {FRAME_SIZE_BYTES} bytes, got {len(frame)}"
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, tmp_path = tempfile.mkstemp(dir=path.parent, prefix=".panel_frame_")
    try:
        with os.fdopen(fd, "wb") as f:
            f.write(frame)
        backoff = FRAME_REPLACE_BACKOFF_S
        for attempt in range(1, FRAME_REPLACE_RETRIES + 1):
            try:
                os.replace(tmp_path, path)
                break
            except PermissionError:
                if attempt == FRAME_REPLACE_RETRIES:
                    raise
                time.sleep(backoff)
                backoff = min(backoff * 2, FRAME_REPLACE_MAX_BACKOFF_S)
    except BaseException:
        os.unlink(tmp_path)
        raise


def read_frame(path: Path) -> bytes:
    data = path.read_bytes()
    assert len(data) == FRAME_SIZE_BYTES, f"frame must be {FRAME_SIZE_BYTES} bytes, got {len(data)}"
    return data


def clamp_brightness(frame: bytes, cap: int) -> bytes:
    """Scale the whole frame down so no channel exceeds `cap`, bounding
    worst-case current draw regardless of what a source rendered. Once a channel
    started nonzero, keep it at least 1 to avoid shifting the hue.
    """
    if cap >= 255:
        return frame
    peak = max(frame) if frame else 0
    if peak <= cap:
        return frame
    scale = cap / peak

    scaled = bytearray(len(frame))
    for i in range(len(frame)):
        value = frame[i]
        if value == 0:
            scaled[i] = 0
        else:
            scaled[i] = max(1, round(value * scale))
    return bytes(scaled)
