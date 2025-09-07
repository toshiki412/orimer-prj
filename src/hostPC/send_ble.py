import asyncio
from bleak import BleakClient

ADDRESS = "90:15:06:FA:D3:E6" 
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

async def run(command):
    print("Run sendble.py")
    async with BleakClient(ADDRESS) as client:
        print("sendble.py 1")
        await client.write_gatt_char(CHARACTERISTIC_UUID, command.encode())
        print("sendble.py 2")
        print(f"Sent: {command}")

import sys
asyncio.run(run(sys.argv[1]))
