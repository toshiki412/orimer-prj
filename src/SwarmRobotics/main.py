
import random
from typing import List
import numpy as np

class UWB:
    def __init__(self, robot):
        self.SensorError = 0.05 #[m]
        self.robot : Robot = []
        self.robot = robot
        pass

    def GetDistance(self, id_0, id_1):
        pos_0_x, pos_0_y = self.robot[id_0].GetPos()
        pos_1_x, pos_1_y = self.robot[id_1].GetPos()
        
        dist = np.sqrt((pos_0_x - pos_1_x)**2 + (pos_0_y - pos_1_y)**2)
        noise = np.random.normal(0, self.SensorError)  # ±5cm = σ=5cm
        measuredDist = max(0.01, dist + noise)  # 0m未満回避
        return measuredDist

class Robot:
    def __init__(self, id):
        self.id = id

        # 初期位置・姿勢ランダム
        self.x = random.uniform(0.2, 0.8)  # x位置 (m)
        self.y = random.uniform(0.2, 0.8)  # y位置 (m)
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

class SwarmController:
    def __init__(self, robotNum):
        if robotNum < 4:
            print("Error\n")
        self.robotNum = robotNum
        self.robot : List[Robot] = []

        for i in range(robotNum):
            self.robot.append(Robot(i))
        
        self.dt = 0.1

        self.uwb = UWB(self.robot)
    
    def Update(self):
        pos = []

        d01 = self.uwb.GetDistance(0, 1)
        d02 = self.uwb.GetDistance(0, 2)
        d03 = self.uwb.GetDistance(0, 3)
        d12 = self.uwb.GetDistance(1, 2)
        d13 = self.uwb.GetDistance(1, 3)

        cos12 = (d01**2 + d02**2 - d12**2) / (2 * d01 * d02)
        cos13 = (d01**2 + d03**2 - d13**2) / (2 * d01 * d03)

        sin12 = (1 - cos12**2)**(1/2)
        sin13 = (1 - cos13**2)**(1/2)

        pos.append((0, 0))
        pos.append((d01, 0))
        pos.append((d02 * cos12, d02 * sin12))
        pos.append((d03 * cos13, d03 * sin13))

        # TODO : pos を描画    
        print(pos[0], pos[1], pos[2], pos[3])
    
    def Odometry(self):
        for i in range(self.robotNum):
            self.robot[i].UpdateOdometry(dt=0.1)
            print("odometry :", self.robot[i].GetPos())
            #TODO : 真値として描画

def Main():
    ROBOT_NUM = 4

    swarm = SwarmController(ROBOT_NUM)

    while True:
        swarm.Update()
        swarm.Odometry()

if __name__ == "__main__":
    Main()        