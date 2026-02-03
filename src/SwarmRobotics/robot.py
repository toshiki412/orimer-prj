import random
import numpy as np

class Robot:
    def __init__(self, id):
        self.id = id

        # 初期位置・姿勢ランダム
        self.gX = random.uniform(0.2, 0.8)  # x位置 (m)
        self.gY = random.uniform(0.2, 0.8)  # y位置 (m)
        self.theta = random.uniform(0, 360)  # 姿勢角 (deg)

        # 速度 (cm/s -> m/s変換)
        self.v = 0.0  # 前進速度 (m/s)
        self.w = 0.0  # 旋回速度 (deg/s)
    
    def Move(self, v, w):
        self.v = v
        self.w = w
    
    # 実際は取れない．あくまでデバッグ / 描画用
    def UpdateOdometry(self, dt):
        # ラジアンに変換
        theta_rad = np.radians(self.theta)
        
        # 位置更新
        self.x += self.v * np.cos(theta_rad) * dt
        self.y += self.v * np.sin(theta_rad) * dt
        
        # 姿勢更新
        self.theta += self.w * dt
        self.theta = self.theta % 360
    
    def GetPos(self):
        return self.x, self.y