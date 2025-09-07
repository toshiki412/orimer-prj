import asyncio
import time
from bleak import BleakClient
import pygame

# M5 atom の情報
ADDRESS = "90:15:06:FA:D3:E6"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

async def main():
    async with BleakClient(ADDRESS) as client:
        pygame.init()
        pygame.joystick.init()

        if pygame.joystick.get_count() == 0:
            print("コントローラーが接続されていません")
            return

        joystick = pygame.joystick.Joystick(0)
        joystick.init()

        print("コントローラー接続済み。ボタンを押して送信。")

        while True:
            time.sleep(0.1)
            pygame.event.pump()
            
            # アナログスティック
            axisX = axisY = 0
            for axisKey in range(joystick.get_numaxes()):
                if axisKey == 0:
                    axisX = joystick.get_axis(axisKey)
                    if axisX > 0.5: # →
                        print("→")
                        await client.write_gatt_char(CHARACTERISTIC_UUID, b'R')
                    elif axisX < -0.5: # ←
                        print("←")
                        await client.write_gatt_char(CHARACTERISTIC_UUID, b'L')
                
                if axisKey == 1:
                    axisY = joystick.get_axis(axisKey)
                    if axisY > 0.5: # ↓
                        print("↓")
                        await client.write_gatt_char(CHARACTERISTIC_UUID, b'B')
                    elif axisY < -0.5: # ↑
                        print("↑")
                        await client.write_gatt_char(CHARACTERISTIC_UUID, b'F')

            # ボタン
            if joystick.get_button(1):  # Button B
                print("Bボタン -> LED ON")
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'X')
            elif joystick.get_button(0):  # Button A
                print("Aボタン -> LED OFF")
                await client.write_gatt_char(CHARACTERISTIC_UUID, b'Q')
            await asyncio.sleep(0.1)



if __name__ == '__main__':
    asyncio.run(main())