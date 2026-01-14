import math
from gpiozero import MCP3208, PWMOutputDevice

STRAIGHT_MAX_SPEED = 0.75
ALPHA = 0.25 * (1/1000) * 1.5

RtoD = math.pi / 180.0
DtoR = 180.0 / math.pi

KW = 5.5
KO = 1.5
KdT = 0.16

class Mortor:
    
    def __init__(self, PIN0, PIN1):
        self.PIN_0  = PWMOutputDevice(PIN0)
        self.PIN_1  = PWMOutputDevice(PIN1)

    def __del__(self):
        self.PIN_0.close()
        self.PIN_1.close()

    def Idling(self):
        self.PIN_0.value  = 0
        self.PIN_1.value  = 0
    
    def Forward(self,inputVol):
        
        if inputVol < 0:
            inputVol = 0
        if inputVol > 1:
            inputVol = 1

        self.PIN_0.value  = inputVol
        self.PIN_1.value  = 0
        print(f"Forward: {inputVol:.2f}")

    
    def Backward(self,inputVol):
        
        if inputVol < 0:
            inputVol = 0
        if inputVol > 1:
            inputVol = 1

        print(f"Backward: {inputVol:.2f}")
        self.PIN_0.value  = 0
        self.PIN_1.value  = inputVol
    

class Action:
    
    def __init__(self):
        '''
        odometry取得可能なら実装を考える
        self.GLnXY = (0.0, 0.0)
        self.GLoXY = (0.0, 0.0)
        self.LCnXY = (0.0, 0.0)
        self.LCoXY = (0.0, 0.0)
        '''
        self.tO    = 0.0
        self.tV    = 0.0
        self.MTnW  = 0.0
        self.MToW  = 0.0

        self.motorL = Mortor(24, 25)
        self.motorR = Mortor(22, 23)
    
    def __del__(self):
        del self.motorL
        del self.motorR

    def SetTargetO(self, tO):
        self.tO = tO

    def __LineControl(self):
        self.MTnW = self.MToW - (KW * self.MToW - KO * self.tO) * KdT
        self.MToW = self.MTnW

        # if abs(self.tO) > 30 * RtoD:
        #     self.tV = STRAIGHT_MAX_SPEED * 0.5
        # else:   
        #     self.tV = STRAIGHT_MAX_SPEED

    def AutoMoving(self):
        self.__LineControl()
        self.ManualMoving(self.tV, self.MTnW)

    def ManualMoving(self, targetV, targetW):

        if targetV == 0 and targetW == 0:
            self.motorL.Idling()
            self.motorR.Idling()
            return
        
        # inputVolLeft  = abs(targetV + ALPHA  *targetW)
        # inputVolRight = abs(targetV - ALPHA  *targetW)

        # #0 ~ 1　に正規化
        # inputVolLeft  /= 1 + ALPHA 
        # inputVolRight /= 1 + ALPHA

        # print(f"TargetV: {targetV:.2f} TargetW: {targetW:.2f}")
        # print(f"InputVolL: {inputVolLeft:.2f} InputVolR: {inputVolRight:.2f}")

         # 符号付き速度計算（abs削除）
        left_speed  = targetV + ALPHA * targetW   # 左輪：V - αW
        right_speed = targetV - ALPHA * targetW   # 右輪：V + αW
        
        # 最大値を1.0に正規化
        max_speed = max(abs(left_speed), abs(right_speed))
        if max_speed > 1.0:
            left_speed  /= max_speed
            right_speed /= max_speed
        
        left_speed += 0.3
        right_speed += 0.3
        print(f"V={targetV:.2f} W={targetW:.2f} → L={left_speed:.2f} R={right_speed:.2f}")

        # if inputVolLeft < 0.05 or inputVolRight < 0.05:
        #     self.motorL.Idling()
        #     self.motorR.Idling()

        # if targetV > 0:
        #     self.motorL.Forward(inputVolLeft)
        #     self.motorR.Forward(inputVolRight)
        
        # elif targetV < 0:
        #     self.motorL.Backward(inputVolLeft)
        #     self.motorR.Backward(inputVolRight)
        
        # else:
        #     self.motorL.Idling()
        #     self.motorR.Idling()

            # モーター制御
        if left_speed > 0:
            self.motorL.Forward(left_speed)
        else:
            self.motorL.Backward(-left_speed)
        
        if right_speed > 0:
            self.motorR.Forward(right_speed)
        else:
            self.motorR.Backward(-right_speed)
            
            
            
        

