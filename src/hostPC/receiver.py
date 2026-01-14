# camera module test 用
import threading
from camera.camera_main import CameraModuleManager
from camera.streamManager import StreamManager
from camera.yoloManager import YoloManager

from socket_connection.videoReceiver import VideoReceiver

import cv2

CLASS_ID_PETBOTTLE = 39  

def main(finishEvent : threading.Event):
    yoloMgr = YoloManager("camera/yolo_lib/yolov8s.pt", 640)

    server = VideoReceiver()
    server.start(finishEvent)

    frameId = 0
    preId = -1
    while not finishEvent.is_set():
        frameId = server.GetFrameId()
        if frameId != preId:
            preId = frameId
            frame = server.GetReceiveImg()
        
        if frame is not None:
            if yoloMgr.Detect(frame):
                bbox = yoloMgr.GetBBox(0) # person class id = 0
                if bbox is None:
                    bbox = (0,0,0,0)
                x1, y1, x2, y2 = bbox
                print(f"Detected bbox: {bbox}")
                yoloMgr.DumpDetails()
                disp = yoloMgr.DrawBBox()
                cv2.imshow("detected", disp)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
                server.UpdateSendData(x1, y1, x2, y2)
            else:
                server.UpdateSendData(0,0,0,0)

    server.stop()


def ReceiverThread(finishEvent : threading.Event):
    print("Start receiver thread")
    main(finishEvent)

if __name__ == '__main__':
    main(threading.Event())