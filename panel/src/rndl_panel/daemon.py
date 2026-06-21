"""Watch the panel framebuffer file and push only the pixels that changed to
the panel over BLE.
"""

import asyncio
import time
from pathlib import Path

from bleak import BleakClient, BleakScanner

from .common import FRAME_SIZE_BYTES, PANEL_WIDTH, PIXEL_RECORD_SIZE_BYTES, PIXEL_WRITE_CHAR_UUID, read_frame, with_retry

POLL_INTERVAL_S = 0.1  # mtime polling, not inotify/watchdog: BLE write latency of about 110ms dwarfs this either way
DEFAULT_MTU = 23


def diff_records(previous: bytes, current: bytes) -> bytearray:
    """Build [x][y][r][g][b] records for every pixel that changed between frames."""
    records = bytearray()
    for i in range(0, FRAME_SIZE_BYTES, 3):
        if current[i : i + 3] == previous[i : i + 3]:
            continue
        pixel_index = i // 3
        x = pixel_index % PANEL_WIDTH
        y = pixel_index // PANEL_WIDTH
        records += bytes([x, y, *current[i : i + 3]])
    return records


def full_frame_records(current: bytes) -> bytearray:
    """Build a [x][y][r][g][b] record for every pixel, unconditionally."""
    records = bytearray()
    for i in range(0, FRAME_SIZE_BYTES, 3):
        pixel_index = i // 3
        x = pixel_index % PANEL_WIDTH
        y = pixel_index // PANEL_WIDTH
        records += bytes([x, y, *current[i : i + 3]])
    return records


def chunk_records(records: bytes, mtu: int):
    max_records_per_write = max(1, (mtu - 3) // PIXEL_RECORD_SIZE_BYTES)
    chunk_size = max_records_per_write * PIXEL_RECORD_SIZE_BYTES
    for offset in range(0, len(records), chunk_size):
        yield records[offset : offset + chunk_size]


async def watch_and_push(client: BleakClient, framebuffer_path: Path) -> None:
    mtu = client.mtu_size or DEFAULT_MTU
    print(f"Connected, ATT MTU={mtu}, watching {framebuffer_path}")

    previous = None  # None means "unknown" and force a full resync on the first frame
    last_mtime = None

    while True:
        if framebuffer_path.exists():
            mtime = framebuffer_path.stat().st_mtime
            if mtime != last_mtime:
                current = read_frame(framebuffer_path)
                records = full_frame_records(current) if previous is None else diff_records(previous, current)
                try:
                    if records:
                        for chunk in chunk_records(records, mtu):
                            await with_retry(client.write_gatt_char, PIXEL_WRITE_CHAR_UUID, bytes(chunk), response=True)
                        print(f"{time.strftime('%H:%M:%S')} pushed {len(records) // PIXEL_RECORD_SIZE_BYTES} changed pixels")
                    previous = current
                    last_mtime = mtime
                except OSError as e:
                    # Don't advance last_mtime/previous: retry this same diff next poll cycle.
                    print(f"{time.strftime('%H:%M:%S')} write failed after retries ({e}), retrying next cycle")
        await asyncio.sleep(POLL_INTERVAL_S)


async def run(framebuffer_path: Path, device_name: str) -> None:
    print(f"Scanning for \"{device_name}\"...")
    device = await BleakScanner.find_device_by_name(device_name, timeout=15.0)
    if device is None:
        print(f"Could not find a device advertising as \"{device_name}\"")
        return

    print(f"Found {device.address}, connecting...")
    async with BleakClient(device) as client:
        await watch_and_push(client, framebuffer_path)
