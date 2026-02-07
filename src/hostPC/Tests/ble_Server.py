import asyncio
import struct
from bleak import BleakScanner, BleakClient

# ===== UUID（Atom 側と一致させる） =====
SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHAR_UUID    = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

# ControlState
# uint16_t btn
# uint8_t  dir
CONTROL_FMT = "<HB"

def on_notify(_, data: bytearray):
    btn, dir = struct.unpack(CONTROL_FMT, data)
    print(f"[Notify] btn={btn} dir={dir}")

async def main():
    print("Scanning for Atom BLE Server...")

    devices = await BleakScanner.discover(timeout=5.0)

    atom = None
    for d in devices:
        if d.name == "Atom-Server":
            atom = d
            break

    if atom is None:
        print("Atom-Server not found")
        return

    print(f"Found Atom-Server: {atom.address}")

    async with BleakClient(atom.address) as client:
        print("Connected to Atom-Server")

        # Notify 登録
        await client.start_notify(CHAR_UUID, on_notify)
        print("Notify started")

        # 10秒間 Notify を受信
        await asyncio.sleep(10.0)

        await client.stop_notify(CHAR_UUID)
        print("Notify stopped")

    print("Disconnected")

asyncio.run(main())
