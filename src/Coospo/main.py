import asyncio
from bleak import BleakClient

ADDRESS = "F0:D6:B5:F6:F0:48"
HR_UUID = "00002a37-0000-1000-8000-00805f9b34fb"
DURATION = 30  # 秒

def handler(sender, data: bytearray):
    if sender.uuid == "00002a37-0000-1000-8000-00805f9b34fb":
        flags = data[0]
        if len(data) >= 2:
            hr = data[1]  # uint8 心拍数
            print(f"❤️ HR: {hr} bpm  (flags=0x{flags:02x})")
        else:
            print(f"⚠️ 短いデータ: {data.hex(' ')}")
    else:
        print(f"[{sender.uuid[-4:]}] {data.hex(' ')}")

async def main():
    async with BleakClient(ADDRESS) as client:
        print("connected")

        services = await client.get_services()

        notify_chars = []

        print("=== SCAN CHARACTERISTICS ===")
        for service in services:
            for char in service.characteristics:
                props = char.properties
                if "notify" in props or "indicate" in props:
                    print(f"NOTIFY: {char.uuid} props={props}")
                    notify_chars.append(char.uuid)

        print("\n=== START NOTIFY ===")
        for uuid in notify_chars:
            try:
                await client.start_notify(uuid, handler)
                print(f"subscribed: {uuid}")
            except Exception as e:
                print(f"failed: {uuid} -> {e}")

        print(f"\nlistening for {DURATION} seconds...\n")
        await asyncio.sleep(DURATION)

        print("=== STOP ===")
        for uuid in notify_chars:
            try:
                await client.stop_notify(uuid)
            except:
                pass

asyncio.run(main())

