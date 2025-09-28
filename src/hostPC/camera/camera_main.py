from camera.streamManager import StreamManager
from camera.viewManager import ViewManager
from camera.coordinateTransfer import CoordinateTransfer
from camera.yoloManager import YoloManager

FRAME_COUNT_MAX = 300

CLASS_ID_PETBOTTLE = 39  # pikmin に似てるから代用

class CameraModuleManager:

    def __init__(self):
        self.streamMgr = StreamManager()
        self.viewMgr = ViewManager(windowSize = 500, scale = 3000, interval = 500) 
        self.coordinateTransfer = CoordinateTransfer(1920, 1080, 800, -30)
        self.yoloMgr = YoloManager("camera/yolo_lib/yolov8n.pt", 640)

    def CameraMainLoop(self):
        
        self.streamMgr.Open(imgH = 1920, imgW = 1080, ttyUSB = 0)
        frameCount = 0

        while True:

            frame = self.streamMgr.GetFrame()
            
            if self.yoloMgr.Detect(frame):
                self.yoloMgr.DumpDetails()
                frame = self.yoloMgr.DrawBBox()
                robotPosInPixel : tuple = self.yoloMgr.GetPos(CLASS_ID_PETBOTTLE)
                if robotPosInPixel is not None:
                    robotPosInMiliMeter : tuple = self.coordinateTransfer.Img2World(robotPosInPixel)
                    
            self.viewMgr.ClearWindow()
            self.viewMgr.DrawAxis()
            self.viewMgr.DrawPoint(robotPosInMiliMeter)
            self.viewMgr.DisplayWindow()

            self.streamMgr.DisplayFrame(frame)

            frameCount += 1
            if frameCount > FRAME_COUNT_MAX:
                break
        
        self.streamMgr.Close()

