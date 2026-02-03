import random
from typing import List
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# ===============================
# UWB
# ===============================
class UWB:
    def __init__(self, robot):
        self.SensorError = 0.05  # [m]
        self.robot = robot

    def GetDistance(self, i, j):
        xi, yi = self.robot[i].GetPos()
        xj, yj = self.robot[j].GetPos()
        d = np.hypot(xi - xj, yi - yj)
        return max(0.01, d + np.random.normal(0, self.SensorError))


# ===============================
# Robot
# ===============================
class Robot:
    def __init__(self, id):
        self.id = id
        self.x = random.uniform(-0.8, 0.8)
        self.y = random.uniform(-0.8, 0.8)
        self.theta = random.uniform(0, 360)
        self.v = 0.0
        self.w = 0.0

    def Move(self, v, w):
        self.v = v
        self.w = w

    def UpdateOdometry(self, dt):
        th = np.radians(self.theta)
        self.x += self.v * np.cos(th) * dt
        self.y += self.v * np.sin(th) * dt
        self.theta = (self.theta + self.w * dt) % 360

    def GetPos(self):
        return self.x, self.y


# ===============================
# Swarm Controller
# ===============================
class SwarmController:
    def __init__(self, n, ax):
        self.robot = [Robot(i) for i in range(n)]
        self.uwb = UWB(self.robot)
        self.dt = 0.1
        self.ax = ax

        self.true_scatter = ax.scatter([], [], c='b', label='True')
        self.est_scatter  = ax.scatter([], [], c='r', marker='x', label='UWB')
        self.mirror_scatter = ax.scatter([], [], c='g', s=100, label='Mirror')

        ax.set_xlim(-2, 2)
        ax.set_ylim(-2, 2)
        ax.set_aspect('equal')
        ax.grid()
        ax.legend()

    # -------------------------------
    # UWB reconstruction (6 points)
    # -------------------------------
    def UWBReconstruct(self):
        d01 = self.uwb.GetDistance(0, 1)
        d02 = self.uwb.GetDistance(0, 2)
        d03 = self.uwb.GetDistance(0, 3)
        d12 = self.uwb.GetDistance(1, 2)
        d13 = self.uwb.GetDistance(1, 3)

        c12 = np.clip((d01**2 + d02**2 - d12**2)/(2*d01*d02), -1, 1)
        c13 = np.clip((d01**2 + d03**2 - d13**2)/(2*d01*d03), -1, 1)

        r0 = (0, 0)
        r1 = (d01, 0)
        r2 = (d02*c12, d02*np.sqrt(1-c12**2))
        r3 = (d03*c13, d03*np.sqrt(1-c13**2))
        r4 = (r2[0], -r2[1])  # mirror of r2
        r5 = (r3[0], -r3[1])  # mirror of r3

        return [r0, r1, r2, r3, r4, r5]

    # -------------------------------
    # Global → Local
    # -------------------------------
    def _global_to_local(self, xs, ys):
        x0, y0 = self.robot[0].GetPos()
        x1, y1 = self.robot[1].GetPos()

        xs = np.array(xs) - x0
        ys = np.array(ys) - y0
        ang = np.arctan2(y1 - y0, x1 - x0)

        c, s = np.cos(-ang), np.sin(-ang)
        xl = c*xs - s*ys
        yl = s*xs + c*ys
        return xl, yl
    
    def _local_to_global(self, xl, yl):
        """
        ローカル座標 (Robot0原点, x軸: R0->R1方向) -> Global座標
        xl, yl : array-like
        """
        x0, y0 = self.robot[0].GetPos()
        x1, y1 = self.robot[1].GetPos()

        ang = np.arctan2(y1 - y0, x1 - x0)

        # 回転（逆回転の逆なので +ang）
        xs = np.cos(ang)*xl - np.sin(ang)*yl
        ys = np.sin(ang)*xl + np.cos(ang)*yl

        # 平行移動
        xs += x0
        ys += y0

        return xs, ys

    def FormationControl(self):
        # UWB再構成（ローカル座標）
        pos = self.UWBReconstruct()

        # =========================
        # 目標距離（設計パラメータ）
        # =========================
        d_star = {
            (0,1): 0.6,
            (0,2): 0.6,
            (0,3): 0.6,
            (1,2): 0.6,
            (1,3): 0.6,
        }

        K = 2.0   # 距離拘束ゲイン

        for i in range(4):
            force = np.zeros(2)

            for (a, b), d_ref in d_star.items():
                if i not in (a, b):
                    continue

                j = b if i == a else a

                dx = pos[i][0] - pos[j][0]
                dy = pos[i][1] - pos[j][1]
                d  = np.hypot(dx, dy) + 1e-6

                # 距離誤差
                e = d - d_ref

                # バネ力（勾配）
                force += -K * e * np.array([dx, dy]) / d

            # =========================
            # 力 → 速度変換（非ホロノミック）
            # =========================
            theta = np.radians(self.robot[i].theta)
            heading = np.array([np.cos(theta), np.sin(theta)])

            forward = np.dot(force, heading)

            v = np.clip(forward, -0.1, 0.1)

            if np.linalg.norm(force) > 1e-3:
                target = np.degrees(np.arctan2(force[1], force[0]))
                err = (target - self.robot[i].theta + 180) % 360 - 180
                w = np.clip(-2.0 * err, -90, 90)
            else:
                v = 0.0
                w = 0.0

            self.robot[i].Move(v, w)

    # -------------------------------
    # Boids control (FIXED)
    # -------------------------------
    def BoidsControl(self):
        all_local_pos = self.UWBReconstruct()
        all_pos = all_local_pos
        for i in range(6):
            all_pos[i] = self._local_to_global(all_local_pos[i][0], all_local_pos[i][0])

        K_ANCHOR = 10.0    # ★ Robot0への引力（かなり強く）
        K_SEP    = 1.0
        K_COH    = 0.3

        for i in range(4):
            separation = np.zeros(2)
            cohesion   = np.zeros(2)

            # =========================
            # ★ Robot0 への強制引力 ★
            # =========================
            if i != 0:
                dx0 = all_pos[0][0] - all_pos[i][0]
                dy0 = all_pos[0][1] - all_pos[i][1]
                d0  = np.hypot(dx0, dy0) + 1e-6

                # 距離に比例する強力な引力
                cohesion += K_ANCHOR * np.array([dx0, dy0]) * d0
                print(i, cohesion)

            # =========================
            # 通常 Boids（弱め）
            # =========================
            pairs = []
            for k in range(6):
                if i == k:
                    continue
                if (i == 2 and k == 4) or (i == 3 and k == 5):
                    continue

                dx = all_pos[k][0] - all_pos[i][0]
                dy = all_pos[k][1] - all_pos[i][1]
                d  = np.hypot(dx, dy)
                if d < 1e-3:
                    continue
                pairs.append((d, dx, dy))

            pairs.sort(key=lambda x: x[0])

            for d, dx, dy in pairs[:3]:
                if d < 0.4:
                    separation -= K_SEP * np.array([dx, dy])
                else:
                    cohesion   += K_COH * np.array([dx, dy])

            # =========================
            # 速度・角速度
            # =========================
            vec = cohesion + separation
            v = np.clip(0.5*np.linalg.norm(vec), 0, 0.1)

            if np.linalg.norm(vec) > 1e-3:
                target = np.degrees(np.arctan2(vec[1], vec[0]))
                err = (target - self.robot[i].theta + 180) % 360 - 180
                w = np.clip(-1.5 * err, -90, 90)
            else:
                w = 0.0

            if i != 0:
                self.robot[i].Move(v, w)
            else:
                self.robot[i].Move(0.1, 0.1)
    # -------------------------------
    # Update
    # -------------------------------
    def Update(self):
        vec_list = []  # 描画用ベクトル保存
        all_pos = self.UWBReconstruct()

        # Boids制御（またはFormation制御）
        self.BoidsControl()  

        xs, ys = [], []
        vecs = []  # Local矢印用
        for i, r in enumerate(self.robot):
            r.UpdateOdometry(self.dt)
            x, y = r.GetPos()
            xs.append(x)
            ys.append(y)

        # Local座標に変換
        xl, yl = self._global_to_local(xs, ys)

        # ベクトルもLocalに変換
        for i, r in enumerate(self.robot):
            # 進行方向の速度ベクトル
            theta = np.radians(r.theta)
            vec = np.array([r.v * np.cos(theta), r.v * np.sin(theta)])
            # 原点を Robot0 に合わせる
            vec_local_x = np.cos(-np.arctan2(self.robot[1].y - self.robot[0].y,
                                            self.robot[1].x - self.robot[0].x)) * vec[0] - \
                        np.sin(-np.arctan2(self.robot[1].y - self.robot[0].y,
                                            self.robot[1].x - self.robot[0].x)) * vec[1]
            vec_local_y = np.sin(-np.arctan2(self.robot[1].y - self.robot[0].y,
                                            self.robot[1].x - self.robot[0].x)) * vec[0] + \
                        np.cos(-np.arctan2(self.robot[1].y - self.robot[0].y,
                                            self.robot[1].x - self.robot[0].x)) * vec[1]
            vecs.append((vec_local_x, vec_local_y))

        # Scatter更新
        self.true_scatter.set_offsets(np.c_[xl[:4], yl[:4]])
        est = self.UWBReconstruct()
        ex = [p[0] for p in est[:4]]
        ey = [p[1] for p in est[:4]]
        self.est_scatter.set_offsets(np.c_[ex, ey])
        self.mirror_scatter.set_offsets(np.c_[xl[2:], -yl[2:]])

        # 既存の矢印を削除して再描画
        if hasattr(self, 'vec_quiver'):
            self.vec_quiver.remove()
        self.vec_quiver = self.ax.quiver(xl[:4], yl[:4],
                                        [v[0] for v in vecs[:4]],
                                        [v[1] for v in vecs[:4]],
                                        color='orange', scale=0.05)

        return self.true_scatter, self.est_scatter, self.mirror_scatter, self.vec_quiver


# ===============================
# Main
# ===============================
def Main():
    import copy
    fig_local, ax_local = plt.subplots()
    fig_global, ax_global = plt.subplots()

    # 元の SwarmController を初期化
    from __main__ import SwarmController
    swarm = SwarmController(4, ax_local)

    # Global用 scatter
    global_scatter = ax_global.scatter([], [], c='blue', s=80, label='Global True')
    ax_global.set_xlim(-2, 2)
    ax_global.set_ylim(-2, 2)
    ax_global.set_aspect('equal')
    ax_global.grid()
    ax_global.set_title("Global Coordinate (True Positions)")
    ax_global.legend()

    def animate(frame):
        # Local(UWB +鏡像) 更新
        local_artists = swarm.Update()

        # Global更新（真値）
        true_x = [r.GetPos()[0] for r in swarm.robot]
        true_y = [r.GetPos()[1] for r in swarm.robot]
        global_scatter.set_offsets(np.c_[true_x, true_y])
        ax_global.set_title(f"Global Coordinate (t={frame*swarm.dt:.1f}s)")

        return local_artists + (global_scatter,)

    ani = animation.FuncAnimation(fig_local, animate, frames=500, interval=100, blit=False)
    plt.show()


if __name__ == "__main__":
    Main()
