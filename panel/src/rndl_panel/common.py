"""Shared constants/helpers for the rndl panel framebuffer file and BLE protocol."""

import asyncio
import os
import tempfile
from pathlib import Path

PANEL_WIDTH = 16
PANEL_HEIGHT = 16
FRAME_SIZE_BYTES = PANEL_WIDTH * PANEL_HEIGHT * 3

DEFAULT_FRAMEBUFFER_PATH = Path(tempfile.gettempdir()) / "rndl_panel_framebuffer.bin"

DEVICE_NAME = "rndl-panel"
PIXEL_WRITE_CHAR_UUID = "0000c5d3-6f61-a869-4129-89e9adca439c"
DIMENSIONS_CHAR_UUID = "d6e0f611-6004-4922-b343-cbadbe41ae5e"
PIXEL_RECORD_SIZE_BYTES = 5

WRITE_RETRIES = 5
WRITE_RETRY_BACKOFF_S = 0.2

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
        os.replace(tmp_path, path)
    except BaseException:
        os.unlink(tmp_path)
        raise


def read_frame(path: Path) -> bytes:
    data = path.read_bytes()
    assert len(data) == FRAME_SIZE_BYTES, f"frame must be {FRAME_SIZE_BYTES} bytes, got {len(data)}"
    return data
