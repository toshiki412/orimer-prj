import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.patches import Circle, Arrow
import random

class Robot:
    def __init__(self, robot_id):
        self.id = robot_id

        # 初期位置・姿勢ランダム
        self.x = random.uniform(0.2, 0.8)  # x位置 (m)
        self.y = random.uniform(0.2, 0.8)  # y位置 (m)
        self.theta = random.uniform(0, 360)  # 姿勢角 (deg)
        # 速度 (cm/s -> m/s変換)
        self.v = 0.0  # 前進速度 (m/s)
        self.w = 0.0  # 旋回速度 (deg/s)
    
    def measure_distance(self, other_robot):
        """他のロボットとの距離測定 (±5cm誤差)"""
        true_dist = np.sqrt((other_robot.x)**2 + (other_robot.y)**2)
        noise = np.random.normal(0, 0.05)  # ±5cm = σ=5cm
        measured_dist = max(0.01, true_dist + noise)  # 0m未満回避
        return measured_dist
    
    def update_control(self, other_robots):
        """距離測定のみで重心方向を推定して制御"""
        if len(other_robots) == 0:
            return
        
        target_dist = 0.10  # 10cm
        
        # 1. 距離測定（自分以外）
        distances = [self.measure_distance(other) for other in other_robots]
        avg_dist = np.mean(distances)
        
        # 2. 前進速度：平均距離を10cmに保つ
        dist_error = avg_dist - target_dist
        kp_v = 0.5
        self.v = kp_v * dist_error
        self.v = np.clip(self.v, 0, 0.10)
        
        # 3. 重心方向推定：距離勾配法（分散版）
        # 各ロボットからの「引力ベクトル」を合成
        total_force_x = 0
        total_force_y = 0
        
        for other in other_robots:
            # 相対位置ベクトル（距離誤差で強度調整）
            true_dx = other.x - self.x
            true_dy = other.y - self.y
            dist = np.sqrt(true_dx**2 + true_dy**2)
            
            if dist > 0:
                # 距離が目標より遠いほど強く引き寄せる
                force_mag = (target_dist / dist) ** 2  # 逆二乗則
                total_force_x += force_mag * true_dx / dist
                total_force_y += force_mag * true_dy / dist
        
        # 4. 合成ベクトルの方向へ旋回
        if np.sqrt(total_force_x**2 + total_force_y**2) > 0.01:
            target_angle = np.degrees(np.arctan2(total_force_y, total_force_x))
            angle_error = (target_angle - self.theta + 180) % 360 - 180
            kp_w = 2.0
            self.w = -kp_w * angle_error
            self.w = np.clip(self.w, -90, 90)
        else:
            self.w = 0.0
    
    def update_pose(self, dt):
        """位置・姿勢更新（対抗二輪運動学）"""
        # ラジアンに変換
        theta_rad = np.radians(self.theta)
        
        # 位置更新
        self.x += self.v * np.cos(theta_rad) * dt
        self.y += self.v * np.sin(theta_rad) * dt
        
        # 姿勢更新
        self.theta += self.w * dt
        self.theta = self.theta % 360
    
    def get_pose(self):
        return (self.x, self.y, self.theta)

class SwarmSimulation:
    def __init__(self):
        self.robots = [Robot(i) for i in range(3)]
        self.dt = 0.1  # シミュレーション時間刻み (s)
        self.time = 0
        
        # 描画設定
        self.fig, self.ax = plt.subplots(figsize=(8, 8), facecolor='black')
        self.ax.set_xlim(-0.1, 1.1)
        self.ax.set_ylim(-0.1, 1.1)
        self.ax.set_aspect('equal')
        self.ax.set_facecolor('black')
        self.ax.axis('off')
        
        self.robot_circles = []
        self.robot_arrows = []
        self.trails = []
        
        # 初期描画要素作成
        for i in range(3):
            circle = Circle((0, 0), 0.03, fc=f'C{i}', ec='white', alpha=0.8)
            arrow = Arrow(0, 0, 0, 0, width=0.015, color='white')
            trail, = self.ax.plot([], [], 'o-', color=f'C{i}', alpha=0.5, markersize=2)
            
            self.ax.add_patch(circle)
            self.ax.add_patch(arrow)
            self.robot_circles.append(circle)
            self.robot_arrows.append(arrow)
            self.trails.append(trail)
    
    def update_control_all(self):
        """全ロボットの制御更新 (1秒ごと)"""
        if int(self.time * 10) % 10 == 0:  # 0.1s刻みで1秒間隔
            for robot in self.robots:
                other_robots = [r for r in self.robots if r != robot]
                robot.update_control(other_robots)
    
    def simulation_step(self):
        """1ステップシミュレーション"""
        self.update_control_all()
        
        # 全ロボット姿勢更新
        for robot in self.robots:
            robot.update_pose(self.dt)
            # 範囲内に制限
            robot.x = np.clip(robot.x, 0, 1.0)
            robot.y = np.clip(robot.y, 0, 1.0)
        
        self.time += self.dt
    
    def animate(self, frame):
        """アニメーション更新"""
        self.simulation_step()
        
        for i, robot in enumerate(self.robots):
            x, y, theta = robot.get_pose()
            
            # ロボット本体（円）
            self.robot_circles[i].center = (x, y)
            
            # 向き（矢印）
            arrow_len = 0.04
            dx = arrow_len * np.cos(np.radians(theta))
            dy = arrow_len * np.sin(np.radians(theta))
            self.robot_arrows[i].remove()
            self.robot_arrows[i] = Arrow(x, y, dx, dy, width=0.015, color='white')
            self.ax.add_patch(self.robot_arrows[i])
            
            # 軌跡
            trail_x, trail_y = self.trails[i].get_data()
            trail_x = np.append(trail_x, x)
            trail_y = np.append(trail_y, y)
            # 古い軌跡を削除
            if len(trail_x) > 100:
                trail_x = trail_x[-100:]
                trail_y = trail_y[-100:]
            self.trails[i].set_data(trail_x, trail_y)
            
            # 距離表示（デバッグ用）
            if frame % 50 == 0:
                dists = [robot.measure_distance(other) for other in self.robots if other != robot]
                print(f"Robot {i}: pos=({x:.2f},{y:.2f}), dists={np.mean(dists):.2f}m")
        
        self.ax.set_title(f'Swarm Robot Simulation (t={self.time:.1f}s)', color='white')
        return self.robot_circles + self.robot_arrows + self.trails
    
    def run(self):
        """シミュレーション実行"""
        anim = animation.FuncAnimation(
            self.fig, self.animate, frames=1000, interval=50, blit=False, repeat=True
        )
        plt.tight_layout()
        plt.show()

if __name__ == "__main__":
    sim = SwarmSimulation()
    sim.run()
