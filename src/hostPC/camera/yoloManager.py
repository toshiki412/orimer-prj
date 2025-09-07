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
        self.result
    
    def Initialize(self):
        dummyFrame = np.array([imgSize, imgSize, 0])
        _ = self.model.predict(dummyFrame, imgsz = self.imgsz, verbose = False, conf = 0.6)

    def detect(self, frame):
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
    
    def getDetails(self,result):
        
        if result == None:
            return
        
        class_id : int
        coodinate : list = []
        print("---details---")
        
        boxes = result.boxes  # バウンディングボックス（複数個）
        for box in boxes:
            coodinate = [int(i) for i in box.xyxy[0]]  # バウンディングボックスの座標 [x1, y1, x2, y2]　実数型で送られてくるためintに治す
            conf = box.conf.item()  # 信頼度スコア
            class_id = int(box.cls)  # クラスID

            # 結果の表示(sample)
            print("class name :",result.names[class_id],"id:",class_id," conf :",conf)
            print("coodinate : ",coodinate)
            print("\n")
        print("------\n\n")


def main():

    #ここに動画の名前
    # video_path = "videos/test.mp4"
    video_path = 0
    
    cap = cv2.VideoCapture(video_path)

    # yolo_pack = _Yolo('yolov8n.pt')
    yolo_pack = _Yolo('best.pt')

    while cap.isOpened():
        
        success, frame = cap.read()
        if not success:
            break
    
        # YOLOv8で検出----------------------
        result = yolo_pack.detect(frame)
        
        # 検出結果(バウンディングボックス)を表示
        result_frame : np.ndarray = result.plot()
        cv2.imshow("YOLOv8 Detection", result_frame)

        # 'q'キーで終了
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break
        #----------------------------------------
        
        #応用に向けて...---------------
        #座標や検出対象の情報を入手
        yolo_pack.get_details(result)
        #------------------------------
        
        sleep(0.05)

    cap.release()
    cv2.destroyAllWindows()

if __name__ == '__main__':
    main()