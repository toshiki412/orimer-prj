import asyncio
import threading
import time
import camera_main
from bleak import BleakClient
import pygame
from controller.controllerManager import ControllerManager
# M5 atom の情報
ADDRESS = "90:15:06:FA:D3:E6"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

async def main(finishEvent : threading.Event):
    async with BleakClient(ADDRESS) as client:
        
        controllerMgr = ControllerManager()

        while not finishEvent.is_set():
            # アナログスティック
            axisX, axisY = controllerMgr.GetAxisState()
            if axisX > 0.5: # →
                print("→")
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'R')
            elif axisX < -0.5: # ←
                print("←")
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'L')
                
            if axisY > 0.5: # ↓
                print("↓")
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'B')
            elif axisY < -0.5: # ↑
                print("↑")
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'F')

            # ボタン
            if controllerMgr.IsButtonTriggered("B"):  # Button B
                print("Bボタン -> LED ON")
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'X')
            
            if controllerMgr.IsButtonTriggered("A"):  # Button A
                print("Aボタン -> LED OFF")
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'Q')
            
            await asyncio.sleep(0.1)

def BleMainThread(finishEvent : threading.Event):
    print("Start ble thread")
    asyncio.run(main(finishEvent))

if __name__ == '__main__':
    asyncio.run(main(threading.Event()))