
import time
import random
from typing import List
from matplotlib import pyplot as plt
from matplotlib.ticker import AutoLocator
import numpy as np

class Graphics:
    def __init__(self):
        # --- Local window (estPositions) ---
        self.fig, self.ax = plt.subplots()
        self.est_scatter  = self.ax.scatter([], [], c='r', marker='x', label='UWB')
        self.ax.set_xlim(-2, 2)
        self.ax.set_ylim(-2, 2)
        self.ax.set_aspect('equal')
        self.ax.grid()
        self.ax.legend()
        
        # --- Global window (truePositions) ---
        self.fig_true, self.ax_true = plt.subplots()
        self.true_scatter = self.ax_true.scatter([], [], c='b', label='True')
        self.ax_true.set_xlim(-4, 4)
        self.ax_true.set_ylim(-4, 4)
        self.ax_true.set_aspect('equal')
        self.ax_true.grid()
        self.ax_true.legend()

        self.ax.set_xscale("linear")
        self.ax.set_yscale("linear")
        self.ax_true.set_xscale("linear")
        self.ax_true.set_yscale("linear")

        self.ax.xaxis.set_major_locator(AutoLocator())
        self.ax.yaxis.set_major_locator(AutoLocator())
        self.ax_true.xaxis.set_major_locator(AutoLocator())
        self.ax_true.yaxis.set_major_locator(AutoLocator())

        plt.ion()
        plt.show(block=False)

    def Update(self, truePositions, estPositions):
        # --- Update estPositions (local) ---
        if len(estPositions) == 0:
            return
        local_buf = []
        for pos in estPositions:
            local_buf.append(pos)
        self.est_scatter.set_offsets(local_buf)
        
        # --- Update truePositions (global) ---
        self.true_scatter.set_offsets(truePositions)

        # --- Refresh both figures ---
        self.fig.canvas.draw_idle()
        self.fig.canvas.flush_events()
        self.fig_true.canvas.draw_idle()
        self.fig_true.canvas.flush_events()

    def IsRunning(self):
        return plt.fignum_exists(self.fig.number)

class UWB:
    def __init__(self, robot):
        self.SensorError = 0.01 #[m]
        self.robot : Robot = []
        self.robot = robot
        pass

    def GetDistance(self, id_0, id_1):
        # ロボットの位置
        pos_0_x, pos_0_y = self.robot[id_0].GetPos()
        pos_1_x, pos_1_y = self.robot[id_1].GetPos()

        # 距離計算（float確保）
        dist = float(np.sqrt((pos_0_x - pos_1_x)**2 + (pos_0_y - pos_1_y)**2))

        # ノイズ（スカラー）
        noise = float(np.random.normal(0, self.SensorError))

        # 最終距離（必ず float）
        measuredDist = max(0.01, dist + noise)

        return measuredDist

class Robot:
    def __init__(self, id):
        self.id = id

        # 初期位置・姿勢ランダム
        self.x = random.uniform(-1.8, 1.8)  # x位置 (m)
        self.y = random.uniform(-1.8, 1.8)  # y位置 (m)
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
    
    def UpdatePos(self, dx, dy):
        self.x += dx
        self.y += dy
    
    def GetPos(self):
        return self.x, self.y

    def GetId(self):
        return self.id

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

        self.estPos = []

    def EstimatePosition(self):
        estPos = []

        # Robot0, Robot1 を基準
        d01 = self.uwb.GetDistance(0, 1)
        estPos.append((0, 0))      # Robot0
        estPos.append((d01, 0))    # Robot1 x軸上

        # Robot2 以降
        for i in range(2, self.robotNum):
            d0i = self.uwb.GetDistance(0, i)
            d1i = self.uwb.GetDistance(1, i)

            # cosθ = (a² + b² - c²)/(2ab)
            cos_theta = (d01**2 + d0i**2 - d1i**2) / (2 * d01 * d0i)
            cos_theta = np.clip(cos_theta, -1.0, 1.0)
            sin_theta = np.sqrt(1 - cos_theta**2)

            x = d0i * cos_theta
            y = d0i * sin_theta

            estPos.append((x, y))

        # ------------------------
        # 鏡像を追加（y軸反転）
        # ------------------------
        mirroredPos = []
        for i, (x, y) in enumerate(estPos):
            if i >= 2:  # Robot0,1 は鏡像不要
                mirroredPos.append((x, -y))

        estPos.extend(mirroredPos)

        # ------------------------
        # 真値も更新
        # ------------------------
        truePos = []
        for i in range(self.robotNum):
            # self.robot[i].UpdateOdometry(dt=self.dt)
            truePos.append(self.robot[i].GetPos())

        self.estPos = estPos
        return truePos, estPos

    def EstimatePositionOld(self):
        estPos = []

        d01 = self.uwb.GetDistance(0, 1)
        d02 = self.uwb.GetDistance(0, 2)
        d03 = self.uwb.GetDistance(0, 3)
        d12 = self.uwb.GetDistance(1, 2)
        d13 = self.uwb.GetDistance(1, 3)

        cos12 = (d01**2 + d02**2 - d12**2) / (2 * d01 * d02)
        cos13 = (d01**2 + d03**2 - d13**2) / (2 * d01 * d03)

        cos12 = np.clip(cos12, -1.0, 1.0)
        cos13 = np.clip(cos13, -1.0, 1.0)

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
            # self.robot[i].UpdateOdometry(dt=0.1)
            truePos.append(self.robot[i].GetPos())
        
        # これは グローバル座標ではなく，ローカル座標系
        self.estPos = estPos

        return truePos, estPos
    
    def BoidsControl(self):
        # -----------------------------
        # パラメータ
        # -----------------------------
        k_cohesion   = 0.4
        k_separation = 0.15
        sep_radius   = 0.25
        max_step     = 0.02

        def local_to_global(xl, yl, pos0, pos1):
            x0, y0 = pos0
            x1, y1 = pos1
            ang = np.arctan2(y1 - y0, x1 - x0)

            xs = np.cos(ang)*xl - np.sin(ang)*yl + x0
            ys = np.sin(ang)*xl + np.cos(ang)*yl + y0
            return xs, ys

        for robot in self.robot:
            id = robot.GetId()

            # -------- local 推定位置 --------
            lx, ly = self.estPos[id]

            # -----------------------------
            # Cohesion
            # -----------------------------
            cx = cy = 0.0
            cnt = 0

            for j, pos in enumerate(self.estPos):
                if j == id:
                    continue
                cx += pos[0]
                cy += pos[1]
                cnt += 1

            if cnt > 0:
                cx /= cnt
                cy /= cnt
                coh_x = (cx - lx) * k_cohesion
                coh_y = (cy - ly) * k_cohesion
            else:
                coh_x = coh_y = 0.0

            # -----------------------------
            # Separation
            # -----------------------------
            sep_x = sep_y = 0.0

            for j, pos in enumerate(self.estPos):
                if j == id:
                    continue
                dx = lx - pos[0]
                dy = ly - pos[1]
                dist = np.hypot(dx, dy)

                if 0.0 < dist < sep_radius:
                    force = k_separation * (sep_radius - dist) / sep_radius
                    sep_x += (dx / dist) * force
                    sep_y += (dy / dist) * force

            # -----------------------------
            # 合成ベクトル（local）
            # -----------------------------
            vx = coh_x + sep_x
            vy = coh_y + sep_y

            norm = np.hypot(vx, vy)
            if norm > max_step:
                vx = vx / norm * max_step
                vy = vy / norm * max_step

            # -------- 次の local 位置 --------
            next_lx = lx + vx
            next_ly = ly + vy

            # -----------------------------
            # 鏡像 2候補 → global
            # -----------------------------
            pos0 = self.robot[0].GetPos()
            pos1 = self.robot[1].GetPos()
            prev_gx, prev_gy = robot.GetPos()

            candidates = []
            for sign in [1.0, -1.0]:
                gx, gy = local_to_global(next_lx, sign * next_ly, pos0, pos1)
                d = np.hypot(gx - prev_gx, gy - prev_gy)
                candidates.append((d, gx, gy))

            # 前回位置に近い方を採用
            _, gX, gY = min(candidates, key=lambda x: x[0])

            # -----------------------------
            # Global 位置更新
            # -----------------------------
            robot.UpdatePos(gX - prev_gx, gY - prev_gy)

    
    def LineControl(self):
        # -----------------------------
        # パラメータ
        # -----------------------------
        k_line     = 1.2     # 直線(y=0)への吸着
        k_spacing  = 0.6     # x方向の間隔制御
        spacing    = 0.25    # ロボット間隔 [m]
        max_step   = 0.02    # 最大移動量 [m]

        vx = 0
        vy = 0

        def local_to_global(xl, yl, pos0, pos1):
            x0, y0 = pos0
            x1, y1 = pos1
            ang = np.arctan2(y1 - y0, x1 - x0)

            xs = np.cos(ang) * xl - np.sin(ang) * yl + x0
            ys = np.sin(ang) * xl + np.cos(ang) * yl + y0
            return xs, ys

        for robot in self.robot:
            id = robot.GetId()

            # Robot0 は基準点（固定したいなら continue）
            # if id == 0:
            #     continue

            # -----------------------------
            # local 推定位置
            # -----------------------------
            lx, ly = self.estPos[id]

            # -----------------------------
            # y方向：直線(y=0)に吸着
            # -----------------------------
            vy = -k_line * ly

            # -----------------------------
            # x方向：間隔制御
            # -----------------------------
            # target_x = id * spacing
            # vx = k_spacing * (target_x - lx)

            # -----------------------------
            # 移動量制限（local）
            # -----------------------------
            norm = np.hypot(vx, vy)
            if norm > max_step:
                vx = vx / norm * max_step
                vy = vy / norm * max_step

            # 次の local 位置
            next_lx = lx + vx
            next_ly = ly + vy

            # -----------------------------
            # 鏡像2候補 → global
            # -----------------------------
            pos0 = self.robot[0].GetPos()
            pos1 = self.robot[1].GetPos()
            prev_gx, prev_gy = robot.GetPos()

            candidates = []
            for sign in [1.0, -1.0]:
                gx, gy = local_to_global(
                    next_lx,
                    sign * next_ly,
                    pos0,
                    pos1
                )
                d = np.hypot(gx - prev_gx, gy - prev_gy)
                candidates.append((d, gx, gy))

            # 前回位置に近い方を採用（鏡像固定）
            _, gX, gY = min(candidates, key=lambda x: x[0])

            # -----------------------------
            # Global 位置更新
            # -----------------------------
            robot.UpdatePos(
                gX - prev_gx,
                gY - prev_gy
            )
    
    def CircleControl(self):
        # -----------------------------
        # 円隊形パラメータ
        # -----------------------------
        R        = 1.0     # 円半径 [m]
        k_r      = 1.0     # 半径方向ゲイン
        k_theta  = 0.8     # 角度方向ゲイン
        max_step = 0.02    # 最大移動量

        def local_to_global(xl, yl, pos0, pos1):
            x0, y0 = pos0
            x1, y1 = pos1
            ang = np.arctan2(y1 - y0, x1 - x0)

            xs = np.cos(ang) * xl - np.sin(ang) * yl + x0
            ys = np.sin(ang) * xl + np.cos(ang) * yl + y0
            return xs, ys

        def wrap(a):
            return (a + np.pi) % (2 * np.pi) - np.pi

        N = self.robotNum

        for robot in self.robot:
            id = robot.GetId()

            lx, ly = self.estPos[id]

            # -----------------------------
            # 極座標
            # -----------------------------
            r = np.hypot(lx, ly)
            theta = np.arctan2(ly, lx)

            # -----------------------------
            # 目標角度（等間隔）
            # -----------------------------
            target_theta = 2 * np.pi * id / N

            # -----------------------------
            # 半径 & 角度 制御
            # -----------------------------
            vr = k_r * (R - r)
            vtheta = k_theta * wrap(target_theta - theta)

            # -----------------------------
            # 極 → 直交（local）
            # -----------------------------
            vx = vr * np.cos(theta) - vtheta * r * np.sin(theta)
            vy = vr * np.sin(theta) + vtheta * r * np.cos(theta)

            # -----------------------------
            # 移動量制限
            # -----------------------------
            norm = np.hypot(vx, vy)
            if norm > max_step:
                vx = vx / norm * max_step
                vy = vy / norm * max_step

            next_lx = lx + vx
            next_ly = ly + vy

            # -----------------------------
            # 鏡像2候補 → global
            # -----------------------------
            pos0 = self.robot[0].GetPos()
            pos1 = self.robot[1].GetPos()
            prev_gx, prev_gy = robot.GetPos()

            candidates = []
            for sign in [1.0, -1.0]:
                gx, gy = local_to_global(
                    next_lx,
                    sign * next_ly,
                    pos0,
                    pos1
                )
                d = np.hypot(gx - prev_gx, gy - prev_gy)
                candidates.append((d, gx, gy))

            _, gX, gY = min(candidates, key=lambda x: x[0])

            robot.UpdatePos(gX - prev_gx, gY - prev_gy)


class KeyInput:
    def __init__(self, fig):
        self.key = None
        fig.canvas.mpl_connect("key_press_event", self._on_key)

    def _on_key(self, event):
        self.key = event.key

    def get(self):
        k = self.key
        self.key = None
        return k


def Main():
    ROBOT_NUM = 40

    swarm = SwarmController(ROBOT_NUM)
    grp = Graphics()
    key_input = KeyInput(grp.fig)

    while grp.IsRunning():
        truePos, estPos = swarm.EstimatePosition()
        grp.Update(truePos, estPos)
        swarm.BoidsControl()
        swarm.LineControl()
        # swarm.CircleControl()
        time.sleep(0.02) 

if __name__ == "__main__":
    Main()        