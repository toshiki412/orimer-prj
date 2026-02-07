import asyncio
from bleak import BleakScanner, BleakClient

SERVICE_UUID = "12345678-1234-1234-1234-1234567890ab"
NOTIFY_UUID  = "12345678-1234-1234-1234-1234567890ac"
WRITE_UUID   = "12345678-1234-1234-1234-1234567890ad"

def on_notify(sender, data: bytearray):
    print(f"[NOTIFY] {list(data)}")

async def main():
    print("Scanning...")
    devices = await BleakScanner.discover(timeout=5.0)

    target = None
    for d in devices:
        if SERVICE_UUID.lower() in [s.lower() for s in d.metadata.get("uuids", [])]:
            target = d
            break

    if target is None:
        print("Device not found")
        return

    print(f"Connecting to {target.name} ({target.address})")

    async with BleakClient(target.address) as client:
        print("Connected")

        await client.start_notify(NOTIFY_UUID, on_notify)

        # テスト送信（例：btn=1, dir=2）
        data = bytes([1, 2])
        await client.write_gatt_char(WRITE_UUID, data)

        print("Write done, waiting notify...")
        await asyncio.sleep(10)

        await client.stop_notify(NOTIFY_UUID)

asyncio.run(main())
