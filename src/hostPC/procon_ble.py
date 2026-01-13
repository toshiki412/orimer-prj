import asyncio
import threading
import time
import camera_thread
from bleak import BleakClient
import pygame
from controller.controllerManager import ControllerManager

from bleak.backends.winrt.util import uninitialize_sta
uninitialize_sta()

try:
    import pythoncom
except Exception:
    pythoncom = None

# M5 atom の情報
ADDRESS = "90:15:06:FA:D3:E6"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

async def main(finishEvent : threading.Event):
    
    controllerMgr = ControllerManager()
    
    if not controllerMgr.TryConnect():
        finishEvent.set()
        return

    async with BleakClient(ADDRESS) as client:
        print("Connected to BLE device")
        while not finishEvent.is_set():
            # アナログスティック
            axisX, axisY = controllerMgr.GetAxisState()
            if axisX > 0.5: # →
                print("→")
                axisX_str = f"{axisX:.2f}"
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'R' + axisX_str.encode())
            elif axisX < -0.5: # ←
                print("←")
                axisX_str = f"{-axisX:.2f}"
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'L' + axisX_str.encode())
                
            if axisY > 0.5: # ↓
                print("↓")
                asixY_str = f"{axisY:.2f}" 
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'B' + asixY_str.encode())
            elif axisY < -0.5: # ↑
                print("↑")
                asixY_str = f"{-axisY:.2f}"
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'F' + asixY_str.encode())

            # ボタン
            if controllerMgr.IsButtonTriggered("B"):  # Button B
                print("Bボタン -> LED ON")
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'X')
            
            if controllerMgr.IsButtonTriggered("A"):  # Button A
                print("Aボタン -> LED OFF")
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'Q')
            
            if controllerMgr.IsButtonTriggered("+"):
                print("+ボタン -> Finish")
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'E')
                finishEvent.set()

            await asyncio.sleep(0.1)

def BleMainThread(finishEvent : threading.Event):
    print("Start ble thread")
    # If pythoncom is available, initialize COM as MTA on this thread
    if pythoncom:
        try:
            pythoncom.CoInitializeEx(pythoncom.COINIT_MULTITHREADED)
        except Exception as e:
            print("pythoncom.CoInitializeEx failed:", e)
    try:
        asyncio.run(main(finishEvent))
    finally:
        if pythoncom:
            try:
                pythoncom.CoUninitialize()
            except Exception:
                pass