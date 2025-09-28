# camera module test 用
from camera.camera_main import CameraModuleManager
def main():
    cameraModule = CameraModuleManager()
    cameraModule.CameraMainLoop()

if __name__ == '__main__':
    main()