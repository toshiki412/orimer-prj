# server_advanced.py
import socket
import cv2
import numpy as np
import struct
from threading import Thread

g_endFlag = False

class _Yolo:

    def __init__(self,model) -> None:
        from ultralytics import YOLO
        self.model = YOLO(model)
        self.isReady = True
        self.isFinish = False
        self.running = True
        self.timestamp : int
        self.sendData = struct.pack('>iiiiQ',0,0,0,0,0)
        # keypointの位置毎の名称定義
        self.KEYPOINTS_NAMES = [
            "nose",  # 0
            "eye(L)",  # 1
            "eye(R)",  # 2
            "ear(L)",  # 3
            "ear(R)",  # 4
            "shoulder(L)",  # 5
            "shoulder(R)",  # 6
            "elbow(L)",  # 7
            "elbow(R)",  # 8
            "wrist(L)",  # 9
            "wrist(R)",  # 10
            "hip(L)",  # 11
            "hip(R)",  # 12
            "knee(L)",  # 13
            "knee(R)",  # 14
            "ankle(L)",  # 15
            "ankle(R)",  # 16
        ]
    
    def start(self):
        self.running = True
        self.thread = Thread(target=self._yolo_loop)
        self.thread.start()

    def stop(self):
        self.thread.join()
        self.running = False

    def set(self,frame,timestamp):
        #サーバーループのほうが早い．
        if self.isReady:
            self.frame = frame
            self.timestamp = timestamp #64bit型

            self.isReady = False

    def detect(self,frame):
        #Tips!! yoloは640×640[pix]しか受け付けないのだ！
        results = self.model.predict(frame,imgsz = (640,480), verbose = False)
        #基本使うのはresult[0]のみだと思う
        result = results[0]
        return result
    
    # def get_bbox_data(self,result):
    #     class_id : int
    #     coodinate : list = []
    #     boxes = result.boxes  # バウンディングボックス（複数個）
    #     for box in boxes:
    #         coodinate = [int(i) for i in box.xyxy[0]]  # バウンディングボックスの座標 [x1, y1, x2, y2]　実数型で送られてくるためintに治す
    #         conf = box.conf.item()  # 信頼度スコア
    #         class_id = int(box.cls)  # クラスID

    #         # 結果の表示(sample)
    #         print("class name :",result.names[class_id],"id:",class_id," conf :",conf)
    #         print("coodinate : ",coodinate)
    
    def getPersonPos(self,frame):
        #Tips!! yoloは640×640[pix]しか受け付けないのだ！
        results = self.model.predict(frame,imgsz = (640,480), verbose = False)
        #基本使うのはresult[0]のみだと思う
        result = results[0]
        class_id : int
        coodinate : list = [0,0,0,0]
        boxes = result.boxes  # バウンディングボックス（複数個）

        maxConf = 0.2
        for box in boxes:
            class_id = int(box.cls)  # クラスID
            if result.names[class_id] == "person":
                conf = box.conf.item()  # スコア
                if conf > maxConf:
                    maxConf = conf
                    coodinate = [int(i) for i in box.xyxy[0]]  # バウンディングボックスの座標 [x1, y1, x2, y2]　実数型で送られてくるためintに治す
                    
        
        return coodinate
    
    def GetResult(self):
        return self.sendData
    
    def _yolo_loop(self):
        global g_endFlag
        while self.running:
            if g_endFlag:
                break

            try:
                #更新したデータで計算する

                if not self.isReady:
                    x1,y1,x2,y2 = self.getPersonPos(self.frame)
                    self.sendData = struct.pack('>iiiiQ', x1,y1,x2,y2,self.timestamp)
                    self.isReady = True

                # if cv2.waitKey(1) == 27:
                #                 break
            except (ConnectionResetError, BrokenPipeError, KeyboardInterrupt):
                break
        
        g_endFlag = True


# model = 'yolov8n-pose.pt'
model = 'yolov10n.pt'
g_YoloPack = _Yolo(model)

class _VideoReceiver:
    def __init__(self, host='0.0.0.0', port=5000):
        self.host = host
        self.port = port
        self.running = False
        
    def start(self):
        self.running = True
        self.thread = Thread(target=self._receive_loop)
        self.thread.start()
        
    def stop(self):
        self.thread.join()
        self.running = False
        
    def _receive_loop(self):
        global g_endFlag
        print("serverloop")
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            s.bind((self.host, self.port))
            s.listen(1)
            print("waiting...")
            try:
                conn, addr = s.accept()
            except socket.timeout:
                print("accept() timed out")
            with conn:
                print(f'Connected: {addr}')
                while self.running:
                    if g_endFlag:
                        break
                    try:
                        # ヘッダ受信（12バイト: タイムスタンプ8 + サイズ4）
                        header = conn.recv(12)
                        if len(header) != 12:
                            break
                            
                        timestamp = int.from_bytes(header[:8], 'big')
                        size = int.from_bytes(header[8:], 'big')
                        
                        img_data = bytearray()
                        while len(img_data) < size:
                            remaining = size - len(img_data)
                            img_data += conn.recv(4096 if remaining > 4096 else remaining)
                            
                        img = cv2.imdecode(np.frombuffer(img_data, np.uint8), cv2.IMREAD_COLOR)
                        g_YoloPack.set(img,timestamp)
                        sendData = g_YoloPack.GetResult()
                        conn.sendall(sendData)

                        # cv2.imshow(f'Stream [{addr[0]}]', img)
                        if cv2.waitKey(1) == 27:
                            break
                            
                    except (ConnectionResetError, BrokenPipeError, KeyboardInterrupt):
                        break
            g_endFlag = True
            cv2.destroyAllWindows()

def main():
    receiver = _VideoReceiver(port=5000)
    receiver.start()
    g_YoloPack.start()
    input("Press Enter to stop...")
    receiver.stop()
    g_YoloPack.stop()

if __name__ == '__main__':
    main()