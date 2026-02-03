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
        estPos = []

        d01 = self.uwb.GetDistance(0, 1)
        d02 = self.uwb.GetDistance(0, 2)
        d03 = self.uwb.GetDistance(0, 3)
        d12 = self.uwb.GetDistance(1, 2)
        d13 = self.uwb.GetDistance(1, 3)

        cos12 = (d01**2 + d02**2 - d12**2) / (2 * d01 * d02)
        cos13 = (d01**2 + d03**2 - d13**2) / (2 * d01 * d03)

        sin12 = (1 - cos12**2)**(1/2)
        sin13 = (1 - cos13**2)**(1/2)

        estPos.append((0, 0))
        estPos.append((d01, 0))
        estPos.append((d02 * cos12, d02 * sin12))
        estPos.append((d03 * cos13, d03 * sin13))

        # 鏡像
        estPos.append((d02 * cos12, - d02 * sin12))
        estPos.append((d03 * cos13, - d03 * sin13))

        truePos = []
        for i in range(self.robotNum):
            self.robot[i].UpdateOdometry(dt=0.1)
            truePos.append(self.robot[i].GetPos())
        
        return truePos, estPos