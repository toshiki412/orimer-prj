import asyncio
import struct
from bleak import BleakScanner, BleakClient

SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHAR_UUID    = "beb5483e-36e1-4688-b7f5-ea07361b26a8"


def handle_notify(sender: int, data: bytearray):
    """
    Atom → Windows Notify 受信
    """
    print(f"[Notify] from {sender}: {list(data)}")

    # ControlState = uint16 btn + uint8 dir (+ padding)
    if len(data) >= 3:
        btn, dir = struct.unpack_from("<HB", data, 0)
        print(f"  btn=0x{btn:04X}, dir={dir}")


async def send_loop(client: BleakClient):
    """
    Windows → Atom 送信ループ
    """
    btn = 0

    while True:
        btn += 1
        dir = 2  # Right とか適当に

        # Atom 側 ControlState と **完全一致** させること
        payload = struct.pack("<HB", btn, dir)

        print(f"[Write] btn=0x{btn:04X}, dir={dir}")
        await client.write_gatt_char(
            CHAR_UUID,
            payload,
            response=False   # WRITE_NR 想定
        )

        await asyncio.sleep(1.0)


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

        services = await client.get_services()
        char = services.get_characteristic(CHAR_UUID)
        if char is None:
            print("Characteristic not found")
            return

        print("Subscribing to Notify...")
        await client.start_notify(CHAR_UUID, handle_notify)

        print("Start send loop")
        await send_loop(client)


if __name__ == "__main__":
    asyncio.run(main())
