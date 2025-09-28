import pygame
import time
import cv2
import numpy as np
import keyMap

class ControllerManager:

    def __init__(self):        
        # 初期化
        pygame.init()
        pygame.joystick.init()

        if pygame.joystick.get_count() == 0:
            print("[Error] Controller Not Found")
            exit(0)

        # ジョイスティック情報取得
        self.joystick = pygame.joystick.Joystick(0)
        self.joystick.init()

        print(f"Connected Controller !!! : {self.joystick.get_name()}")
    
    def GetAxisState(self):
        axisX = 0
        axisY = 0

        pygame.event.pump()
        id = keyMap.PROCON_AXIS_MAP("X")
        axisX = self.joystick.get_axis(id)

        id = keyMap.PROCON_AXIS_MAP("Y")
        axisY = self.joystick.get_axis(id)
        
        return axisX, axisY
    
    # TODO : List 型で返すような仕様にしたほうが安全
    def IsButtonTriggered(self, button):
        
        isButtonTriggered : bool = False

        if button == "ZL":
            id = keyMap.PROCON_AXIS_MAP("ZL")
            ZL = self.joystick.get_axis(id)
            if ZL > 0:
                isButtonTriggered = True
                print("Button ZL")
            
            return isButtonTriggered
        
        if button == "ZR":
            id = keyMap.PROCON_AXIS_MAP("ZR")
            ZR = self.joystick.get_axis(id)
            if ZR > 0:
                isButtonTriggered = True
                print("Button ZR")
            
            return isButtonTriggered

        buttonNum = self.joystick.get_numbuttons()
        for buttonKey in range(buttonNum):
            if self.joystick.get_button(buttonKey): 
                if buttonKey == keyMap.PROCON_AXIS_MAP(button):
                    isButtonTriggered = True
                    print(f"Button {keyMap.PROCON_BUTTON_MAP[buttonKey]}")
                    break
        
        return isButtonTriggered