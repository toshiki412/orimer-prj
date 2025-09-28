import cv2
import numpy as np

class ViewManager:
    
    def __init__(self, windowSize, scale, interval):
        self.scale = scale
        self.interval = interval
        self.windowSize = windowSize
        self.window = np.zeros([self.windowSize, self.windowSize, 3])

    def ClearWindow(self):
        self.window = np.zeros([self.windowSize, self.windowSize, 3])

    def DrawAxis(self):

        # X軸 (-scale // 2 ~ scale // 2mm)
        scale_x = (self.windowSize - 100) / self.scale  # X軸スケーリング
        cv2.line(self.window, (50, self.windowSize // 2), (self.windowSize - 50, self.windowSize // 2), (255, 255, 255), 2)  # X軸（白）
        cv2.putText(self.window, "0mm", (50, self.windowSize // 2 + 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)

        # X軸の目盛り (-scale // 2 ~ scale // 2mm)
        for i in range(-self.scale // 2, self.scale // 2 + 1, self.interval):  # intervalmmごとにマーカーを追加
            x_pos = int(i * scale_x + self.windowSize // 2)  # -scale // 2~scale // 2mmを50~self.windowSize-50ピクセルにマッピング
            cv2.line(self.window, (x_pos, self.windowSize // 2 - 10), (x_pos, self.windowSize // 2 + 10), (255, 255, 255), 2)
            cv2.putText(self.window, f"{i}mm", (x_pos - 20, self.windowSize // 2 + 30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)

        # Y軸 (-scale // 2 ~ scale // 2mm)
        scale_y = (self.windowSize - 100) / self.scale  # Y軸スケーリング
        cv2.line(self.window, (self.windowSize // 2, 50), (self.windowSize // 2, self.windowSize - 50), (255, 255, 255), 2)  # Y軸（白）
        cv2.putText(self.window, "0mm", (self.windowSize // 2 + 20, self.windowSize // 2), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)

        # Y軸の目盛り (-scale // 2 ~ scale // 2mm)
        for i in range(-self.scale // 2, self.scale // 2 + 1, self.interval):  # intervalmmごとにマーカーを追加
            y_pos = int(-i * scale_y + self.windowSize // 2)  # -scale // 2~scale // 2mmを50~self.windowSize-50ピクセルにマッピング
            cv2.line(self.window, (self.windowSize // 2 - 10, y_pos), (self.windowSize // 2 + 10, y_pos), (255, 255, 255), 2)
            cv2.putText(self.window, f"{i}mm", (self.windowSize // 2 + 15, y_pos + 5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
    
    def DrawPoint(self, world_pos):
        if world_pos is None:
            return
        rate = self.windowSize/self.scale
        pX = int( - world_pos[1] * rate)
        pY = int((world_pos[0] - self.scale // 2) * rate)
        draw_x = self.windowSize//2 + pX
        draw_y = self.windowSize//2 - pY
        if 0 <= draw_x < self.windowSize and 0 <= draw_y < self.windowSize:
            cv2.circle(self.window, (draw_x, draw_y), 5, (0, 255, 0), thickness=3)

    def DisplayWindow(self):
        cv2.imshow("axis", self.window)
        cv2.waitKey(1)
