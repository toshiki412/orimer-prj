# camera module test 用
import threading
from camera.camera_main import CameraModuleManager

def main(finishEvent : threading.Event):
    cameraModule = CameraModuleManager()
    cameraModule.CameraMainLoop(finishEvent)

def CameraMainThread(finishEvent : threading.Event):
    print("Start camera thread")
    main(finishEvent)

if __name__ == '__main__':
    main(threading.Event())