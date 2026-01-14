import math
import threading
from client import CAMERA
from gpiozero import MCP3208, PWMOutputDevice
from time import sleep
from controllerManager import ControllerManager
from action import Action

BUTTON_A = 1
BUTTON_B = 0
BUTTON_PLUS = 10

DtoR = 180.0 / math.pi
RtoD = math.pi / 180.0

 

def main():
    cameraObject = CAMERA()
    cameraObject.Initialize()

    cameraThread = threading.Thread(target=cameraObject.Streaming)
    cameraThread.start()

    # controller = ControllerManager()
    action     = Action()

    # isConnected = controller.TryConnect()

    while cameraObject.IsRunning():

        # if isConnected:
        #     #ss
        #     if controller.IsButtonTriggered("+"):
        #         break

        #     if controller.IsButtonTriggered("A"):
        #         outV = 0.75
        #     elif controller.IsButtonTriggered("B"):
        #         outV = -0.75
        #     else:
        #         outV = 0
        
        #     axisX, _ = controller.GetAxisState()
        #     outW = axisX
        #     action.ManualMoving(outV, outW)

        if cameraObject.IsDetected():
            x1, _, x2, _ = cameraObject.GetBBox()
            center = (x1 + x2) / 2
            print(f"center: {center:.2f} pixels")
            targetO = -(640 - center) * 0.1 * DtoR
            print(f"targetO: {targetO * RtoD:.2f} degrees")
            action.SetTargetO(targetO) # radians
            action.AutoMoving()
        else:
            action.ManualMoving(-0.6, 0)

    cameraThread.join()

    del controller
    del action
    del cameraObject

if __name__ == "__main__":
    main()
  