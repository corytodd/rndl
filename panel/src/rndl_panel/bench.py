"""Measure real BLE throughput to the panel: how many pixel writes/bytes per
second we can sustain, for both reliable and best-effort modes, using full
256-pixel frames chunked to the negotiated ATT MTU.
"""

import random
import time

from bleak import BleakClient, BleakScanner

from .common import DEFAULT_BRIGHTNESS_CAP, PANEL_HEIGHT, PANEL_WIDTH, PIXEL_RECORD_SIZE_BYTES, PIXEL_WRITE_CHAR_UUID
from .daemon import DEFAULT_MTU, chunk_records


def random_full_frame_records(brightness_cap: int) -> bytes:
    """One [x][y][r][g][b] record for every pixel on the panel, each channel
    clamped to [0, brightness_cap] to keep worst-case current draw bounded."""
    records = bytearray()
    for y in range(PANEL_HEIGHT):
        for x in range(PANEL_WIDTH):
            r, g, b = (random.randrange(brightness_cap + 1) for _ in range(3))
            records += bytes([x, y, r, g, b])
    return bytes(records)


async def run_mode(client: BleakClient, chunks: list[bytes], duration_s: float, response: bool) -> None:
    label = "write-with-response" if response else "write-without-response"
    writes = 0
    bytes_sent = 0
    frames = 0
    start = time.perf_counter()
    deadline = start + duration_s
    while time.perf_counter() < deadline:
        for chunk in chunks:
            await client.write_gatt_char(PIXEL_WRITE_CHAR_UUID, chunk, response=response)
            writes += 1
            bytes_sent += len(chunk)
        frames += 1
    elapsed = time.perf_counter() - start

    pixels_sent = bytes_sent // PIXEL_RECORD_SIZE_BYTES
    print(f"\n[{label}]")
    print(f"  {writes} writes, {bytes_sent} bytes, {frames} full-panel frames in {elapsed:.2f}s")
    print(f"  {bytes_sent / elapsed:8.0f} bytes/sec")
    print(f"  {pixels_sent / elapsed:8.0f} pixel-updates/sec")
    print(f"  {frames / elapsed:8.2f} full-panel frames/sec ({PANEL_WIDTH}x{PANEL_HEIGHT} = {pixels_sent // max(frames, 1)} px/frame)")


async def run(device_name: str, duration_s: float, brightness_cap: int = DEFAULT_BRIGHTNESS_CAP) -> None:
    print(f"Scanning for \"{device_name}\"...")
    device = await BleakScanner.find_device_by_name(device_name, timeout=15.0)
    if device is None:
        print(f"Could not find a device advertising as \"{device_name}\"")
        return

    print(f"Found {device.address}, connecting...")
    async with BleakClient(device) as client:
        mtu = client.mtu_size or DEFAULT_MTU
        records = random_full_frame_records(brightness_cap)
        chunks = list(chunk_records(records, mtu))
        print(f"ATT MTU={mtu}, {len(chunks)} chunks per full-panel frame ({len(records)} bytes)")
        print(f"Brightness capped to {brightness_cap}/255 per channel")
        print(f"Running each mode for {duration_s:.0f}s...")

        await run_mode(client, chunks, duration_s, response=True)
        await run_mode(client, chunks, duration_s, response=False)
