
import numpy as np
from robot import Robot

class UWB:
    def __init__(self, robot):
        self.SensorError = 0.01 #[m]
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