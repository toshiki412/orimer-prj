import cv2
import socket
import numpy as np
import time
import struct

from picamera2 import Picamera2
from threading import Event

class CAMERA:
    def __init__(self):
        self.serverIp = '192.168.40.72'
        self.port     = 5000
        self.imgSize  = (1280, 720)
        self.imgFormat= "RGB888"

        self.x1 = 0
        self.y1 = 0
        self.x2 = 0
        self.y2 = 0

        self.isRunning = False

    def Initialize(self):
        self._stopEvent = Event()
        self._picam = Picamera2()
        self._picam.configure(self._picam.create_preview_configuration(main = {"format":self.imgFormat,"size":self.imgSize}))
        self._picam.start()
    
    def Streaming(self):
        self.isRunning = True
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as _client:
            _client.connect((self.serverIp, self.port))

            try:
                while not self._stopEvent.is_set():
                    frame = self._picam.capture_array()
                    _, imgEncorded = cv2.imencode('.jpg',frame,[int(cv2.IMWRITE_JPEG_QUALITY),80])
                    imgBytes = imgEncorded.tobytes()

                    timeStamp  = int(time.time()*1000).to_bytes(8, 'big')
                    infoSize = len(imgBytes).to_bytes(4, 'big')
                    _client.sendall(timeStamp + infoSize + imgBytes)

                    #int int int int64_t -> 24 bytes
                    recvData  = _client.recv(24)
                    recvValue = struct.unpack('>iiiiQ',recvData)

                    self.x1, self.y1, self.x2, self.y2 = recvValue[:4]
                    # center = (self.x1 + self.x2) / 2

                    # self._action.SetTargetO((340 - center) * 0.1 * 3.14 / 180 )
                    # self._action.LineControl()
                    # self._action.Moving(1.0)

                    # cv2.rectangle(frame, (self.x1,self.y1),(self.x2,self.y2),(0,255,0),thickness = 1, lineType = cv2.LINE_8)
                    # cv2.imshow("camera",frame)
                    if cv2.waitKey(1) == 27:
                        self.isRunning = False
                        break
            
            except KeyboardInterrupt:
                pass

            finally:
                self._picam.stop()
                cv2.destroyAllWindows()

    def GetBBox(self):
        return self.x1, self.y1, self.x2, self.y2
    
    def IsDetected(self):
        if self.x1 == 0 and self.y1 == 0 and self.x2 == 0 and self.y2 == 0:
            return False
        return True
    
    def IsRunning(self):
        return self.isRunning

def SocketClientMainThread(finishEvent : Event):
    _camera = CAMERA()
    _camera.Initialize()
    _camera.Streaming()
    finishEvent.set()
    
if __name__ == '__main__':
    finishEvent = Event()
    SocketClientMainThread(finishEvent)



