import argparse
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

 

def FollowPerson():
    cameraObject = CAMERA()
    cameraObject.Initialize()

    cameraThread = threading.Thread(target=cameraObject.Streaming)
    cameraThread.start()

    action     = Action()

    while cameraObject.IsRunning():

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

    del action
    del cameraObject

def ManualControl():
    controller = ControllerManager()
    action     = Action()

    if not controller.TryConnect():
        return

    print("Press Plus button to exit")

    while True:

        for button in ["A","B","Plus"]:
            if controller.IsButtonTriggered(button):
                print(f"Button {button} Triggered")
                
                if button == "Plus":
                    return

        axisX, axisY = controller.GetAxisState()
        
        if abs(axisX) > 0.2 or abs(axisY) > 0.2:
            outV = -axisY
            outW = axisX
            action.ManualMoving(outV, outW)
        
        sleep(0.1)

def HueControl():
    from hue import HueDevice
    SCALE=[
        'ラ', 'ラ#', 'シ', 'ド', 'ド#', 'レ',
        'レ#', 'ミ', 'ファ', 'ファ#','ソ', 'ソ#'
    ]

    hueDevice = HueDevice()
    if not hueDevice.TryConnect():
        return

    action     = Action()

    print("Hue Control Started. Press Ctrl+C to exit.")

    try:
        while True:
            if hueDevice.IsTriggered("ド"):
                action.ManualMoving(0.5, 0)
            elif hueDevice.IsTriggered("レ"):
                action.ManualMoving(-0.5, 0)
            elif hueDevice.IsTriggered("ミ"):
                action.ManualMoving(0, 0.5)
            elif hueDevice.IsTriggered("ファ"):
                action.ManualMoving(0, -0.5)
            elif hueDevice.IsTriggered("ソ"):
                action.ManualMoving(0.5, 0.5)
            elif hueDevice.IsTriggered("ラ"):
                action.ManualMoving(-0.5, -0.5)
            elif hueDevice.IsTriggered("シ"):
                action.ManualMoving(0, 0)
    except KeyboardInterrupt:
        print("Exiting Hue Control.")

    del hueDevice
    del action

def ParseArgs():
    parser = argparse.ArgumentParser()

    #camera のみを動かす場合
    parser.add_argument("--follow",action='store_true')

    #manual のみを動かす場合
    parser.add_argument("--manual",action='store_true')

    #hue を動かす場合
    parser.add_argument("--hue",action='store_true')

    args = parser.parse_args()
    return args

def main():

    args = ParseArgs()

    if args.follow:
        FollowPerson()
    elif args.manual:
        ManualControl()
    elif args.hue:
        HueControl()

if __name__ == "__main__":
    main()
