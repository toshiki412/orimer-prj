import socket
import struct
import threading
import numpy as np
import cv2

# socket 通信で画像を受け取るためのライブラリ
class VideoReceiver:
    def __init__(self, host='0.0.0.0', port=5000):
        self.host = host
        self.port = port
        self.running = False

        self.sendData : bytes = struct.pack('>iiiiQ', 0,0,0,0,0)
        self.timestamp : int = 0
        self.receiveImg : np.ndarray
        
    def start(self):
        self.running = True
        self.thread = threading.Thread(target=self.receive_loop)
        self.thread.start()
        
    def stop(self):
        self.thread.join()
        self.running = False
    
    def UpdateSendData(self, x1 : int, y1 : int, x2 : int, y2 : int):
        self.sendData : bytes = struct.pack('>iiiiQ', x1, y1, x2, y2, self.timestamp)
    
    def GetReceiveImg(self):
        return self.receiveImg
        
    def receive_loop(self):
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

                self.UpdateSendData(0,0,0,0,0)#初期化
                while self.running:
                    if g_endFlag:
                        break
                    try:
                        # ヘッダ受信（12バイト: タイムスタンプ8 + サイズ4）
                        header = conn.recv(12)
                        if len(header) != 12:
                            break
                            
                        self.timestamp = int.from_bytes(header[:8], 'big')
                        size = int.from_bytes(header[8:], 'big')
                        
                        img_data = bytearray()
                        while len(img_data) < size:
                            remaining = size - len(img_data)
                            img_data += conn.recv(4096 if remaining > 4096 else remaining)
                            
                        self.receiveImg = cv2.imdecode(np.frombuffer(img_data, np.uint8), cv2.IMREAD_COLOR)

                        conn.sendall(self.sendData)

                        # cv2.imshow(f'Stream [{addr[0]}]', img)
                        if cv2.waitKey(1) == 27:
                            break
                            
                    except (ConnectionResetError, BrokenPipeError, KeyboardInterrupt):
                       break
            g_endFlag = True
            cv2.destroyAllWindows()