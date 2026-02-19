import asyncio
from bleak import BleakClient

ADDRESS = "DB:24:45:F4:90:20"
HR_UUID = "00002a37-0000-1000-8000-00805f9b34fb"
DURATION = 30  # 秒

def handler(sender, data: bytearray):
    hex_data = data.hex(" ")
    dec_data = list(data)
    print(f"[{sender}] len={len(data)} hex={hex_data} dec={dec_data}")

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

