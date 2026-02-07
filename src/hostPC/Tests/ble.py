import asyncio
from bleak import BleakScanner, BleakClient

SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHAR_UUID    = "beb5483e-36e1-4688-b7f5-ea07361b26a8"


def handle_notify(sender: int, data: bytearray):
    """
    Notify 受信コールバック
    ControlState が生で飛んでくる想定
    """
    print(f"[Notify] from {sender}: {list(data)}")

    # 必要ならここで unpack
    # 例: btn(uint8), dir(uint8)
    if len(data) >= 2:
        btn = data[0]
        dir = data[1]
        print(f"  btn={btn}, dir={dir}")


async def main():
    print("Scanning for Atom BLE Server...")

    devices = await BleakScanner.discover(timeout=5.0)

    target = None
    for d in devices:
        uuids = d.metadata.get("uuids", []) or []
        if SERVICE_UUID.lower() in [u.lower() for u in uuids]:
            target = d
            break

    if target is None:
        print("Atom BLE Server not found")
        return

    print(f"Found Atom: {target.name} [{target.address}]")

    async with BleakClient(target.address) as client:
        print("Connected")

        # Characteristic 確認
        services = await client.get_services()
        char = services.get_characteristic(CHAR_UUID)
        if char is None:
            print("Characteristic not found")
            return

        print("Subscribing to Notify...")
        await client.start_notify(CHAR_UUID, handle_notify)

        print("Waiting for notifications (Ctrl+C to exit)...")
        while True:
            await asyncio.sleep(1)


if __name__ == "__main__":
    asyncio.run(main())
