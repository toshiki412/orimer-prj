import cv2
import numpy as np
from time import sleep

#  参考
#　AIを使っての物体検出をYOLOv8を使ってやってみた　https://sakura-system.com/?p=4070
#  YOLOを使ってオブジェクト検出と座標取得をしてみる https://zenn.dev/collabostyle/articles/3d0f210c59679e

#*環境
# ultralyticsのインストールが必要かも

# $ conda install -c conda-forge ultralytics

# 上手く行かなければpipでもインストール可能
# $ pip install ultralytics

class YoloManager:

    def __init__(self, model, imgSize) -> None:
        from ultralytics import YOLO
        
        self.IsDetectSuccess = False
        self.model = YOLO(model)
        self.imgsz = imgSize

        # 最初の処理で load が走るので一回 dummy で処理しておく
        dummyFrame = np.zeros((640, 640, 3), dtype=np.uint8)  # 空画像を用意
        results = self.model.predict(dummyFrame, imgsz=640, conf=0.6, verbose=False)
        self.result = results[0]

    def Detect(self, frame):
        results = self.model.predict(frame, imgsz = self.imgsz, verbose = False, conf = 0.6)
        #基本使うのはresult[0]のみ
        self.result = results[0]

        if self.result == None:
            self.IsDetectSuccess = False
        else:
            self.IsDetectSuccess = True

        return self.IsDetectSuccess
    
    def DrawBBox(self):
        return self.result.plot()
    
    def DumpDetails(self):
        
        if self.result == None:
            return
        
        class_id : int
        coodinate : list = []
        print("---details---")
        
        boxes = self.result.boxes  # バウンディングボックス（複数個）
        for box in boxes:
            coodinate = [int(i) for i in box.xyxy[0]]  # バウンディングボックスの座標 [x1, y1, x2, y2]　実数型で送られてくるためintに治す
            conf = box.conf.item()  # 信頼度スコア
            class_id = int(box.cls)  # クラスID

            # 結果の表示(sample)
            print("class name :",self.result.names[class_id],"id:",class_id," conf :",conf)
            print("coodinate : ",coodinate)
            print("\n")
        print("------\n\n")
    
    def GetPos(self, classId)-> tuple:
        if self.result is None:
            return None

        max_conf = -1.0
        best_coordinate = None

        boxes = self.result.boxes  # バウンディングボックスのリスト
        for box in boxes:
            coordinate = [int(i) for i in box.xyxy[0]]  # [x1, y1, x2, y2]
            conf = box.conf.item()  # 信頼度スコア
            id = int(box.cls)  # クラスID

            if id == classId and conf > max_conf:
                max_conf = conf
                best_coordinate = coordinate

        if best_coordinate is None:
            return None
        
        x1, y1, x2, y2 = best_coordinate
        posX = int((x1 + x2) // 2)
        posY = int((y1 + y2) // 2)

        return (posX, posY)
