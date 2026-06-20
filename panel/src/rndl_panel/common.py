"""Shared constants/helpers for the rndl panel framebuffer file and BLE protocol."""

import asyncio

PIXEL_WRITE_CHAR_UUID = "0000c5d3-6f61-a869-4129-89e9adca439c"
DIMENSIONS_CHAR_UUID = "d6e0f611-6004-4922-b343-cbadbe41ae5e"

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
