import socket
import struct
import threading
import numpy as np
import cv2

# socket 通信で画像を受け取るためのライブラリ
class SocketServer:
    def __init__(self, host='0.0.0.0', port=5000):
        self.host = host
        self.port = port
        self.running = False

        self.sendData : bytes = struct.pack('>iiiiQ', 0,0,0,0,0)
        self.timestamp : int = 0
        self.receiveImg : np.ndarray
        
    # def StartThread(self):
    #     self.running = True
    #     self.thread = threading.Thread(target=self.receive_loop)
    #     self.thread.start()

    # def StopThread(self):
    #     self.thread.join()
    #     self.running = False
    
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

                while self.running:
                    if g_endFlag:
                        break
                    try:
                        conn.sendall(self.sendData)
                    except (ConnectionResetError, BrokenPipeError, KeyboardInterrupt):
                       break

            g_endFlag = True

def ServerMainThread(finishEvent : threading.Event):
    global g_endFlag
    g_endFlag = False
    socketServer = SocketServer(host='0.0.0.0', port=5000)
    socketServer.receive_loop()