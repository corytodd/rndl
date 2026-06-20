"""Scan for the panel, dump its full GATT table, then push some random pixels
through the pixel-write characteristic.
"""
import asyncio
import random
import struct

from bleak import BleakClient, BleakScanner

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


async def run(device_name: str, num_random_pixels: int, brightness_cap: int = DEFAULT_BRIGHTNESS_CAP) -> None:
    print(f"Scanning for \"{device_name}\"...")
    device = await BleakScanner.find_device_by_name(device_name, timeout=15.0)
    if device is None:
        print(f"Could not find a device advertising as \"{device_name}\"")
        return

    print(f"Found {device.address}, connecting...")
    async with BleakClient(device) as client:
        print(f"Connected: {client.is_connected}")
        for service in client.services:
            print(f"\n[Service] {service.uuid} ({service.description})")
            for char in service.characteristics:
                props = ",".join(char.properties)
                print(f"  [Characteristic] {char.uuid} ({char.description}) - {props}")
                for desc in char.descriptors:
                    print(f"    [Descriptor] {desc.uuid} ({desc.description})")

        dims_raw = await with_retry(client.read_gatt_char, DIMENSIONS_CHAR_UUID)
        width, height = struct.unpack("<HH", dims_raw)
        print(f"\nPanel dimensions: {width}x{height}")

        records = bytearray()
        for _ in range(num_random_pixels):
            x = random.randrange(width)
            y = random.randrange(height)
            r, g, b = (random.randrange(brightness_cap + 1) for _ in range(3))
            records += bytes([x, y, r, g, b])
            print(f"  pixel ({x:2d},{y:2d}) -> rgb({r},{g},{b})")

        print(f"\nWriting {num_random_pixels} random pixels...")
        await with_retry(client.write_gatt_char, PIXEL_WRITE_CHAR_UUID, bytes(records), response=True)
        print("Done.")
