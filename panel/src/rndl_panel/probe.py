"""Scan for the panel, dump its full GATT table, then push some random pixels
through the pixel-write characteristic.
"""

import random
import struct

from bleak import BleakClient, BleakScanner

from .common import DEFAULT_BRIGHTNESS_CAP, DIMENSIONS_CHAR_UUID, PIXEL_WRITE_CHAR_UUID, with_retry


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
