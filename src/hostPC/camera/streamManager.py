import cv2 
from eMeetC960Driver import EMeetC960 

class StreamManager:
    '''
    camera を使用する際に呼ぶこと
    '''
    
    def __init__(self):
        sensor = EMeetC960()
        self.param = sensor.GetSensorParam()

    def Initialize(self, imgH, imgW, ttyUSB):
        self.cap = cv2.VideoCapture(ttyUSB)
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, imgW)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, imgH)

    def Finalize(self):
        self.cap.release()
        cv2.destroyAllWindows()

    def GetFrame(self):
        success, frame = self.cap.read()
        if not success:
            print('error no frame')
            exit(0)
        return frame
    
    def DisplayFrame(frame):
        cv2.imshow("camera",frame)
        _ = cv2.waitKey(1)