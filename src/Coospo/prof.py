import asyncio
import time
import msvcrt
from datetime import datetime
from bleak import BleakClient

ADDRESS = "F0:D6:B5:F6:F0:48"
HR_UUID = "00002a37-0000-1000-8000-00805f9b34fb"
INTERVAL = 60  # 60秒固定区間

current_values = []
interval_start = None
avg_history = []
running = True


def check_escape():
    global running
    if msvcrt.kbhit():
        if msvcrt.getch() == b'\x1b':
            print("ESC pressed. stopping...")
            running = False


def handler(sender, data: bytearray):
    global current_values, interval_start

    if sender.uuid == HR_UUID and len(data) >= 2:
        hr = data[1]
        now = time.time()

        if interval_start is None:
            interval_start = now

        current_values.append(hr)
        print(f"HR: {hr} bpm")

        # 60秒経過で平均計算
        if now - interval_start >= INTERVAL:
            if current_values:
                avg = sum(current_values) / len(current_values)
                timestamp = datetime.now()

                avg_history.append((timestamp, avg))

                with open("src/Coospo/data.txt", "a") as f:
                    f.write(f"{timestamp.strftime('%H:%M:%S')},{avg:.2f}\n")

                print(f"{INTERVAL}秒平均: {avg:.2f} bpm")

            current_values = []
            interval_start = now


async def main():
    global running

    async with BleakClient(ADDRESS) as client:
        print("connected")

        await client.start_notify(HR_UUID, handler)
        print("Listening... (ESCで終了)\n")

        while running:
            check_escape()
            await asyncio.sleep(0.1)

        await client.stop_notify(HR_UUID)

    print("Disconnected.")
    create_graph()


def create_graph():
    if not avg_history:
        print("No data to plot.")
        return

    # ここで初めてmatplotlibをimport（重要）
    import matplotlib.pyplot as plt
    import matplotlib.dates as mdates

    times = [t for t, _ in avg_history]
    avgs = [avg for _, avg in avg_history]

    plt.figure()
    plt.plot(times, avgs, marker="o")

    plt.xlabel("Time (24h)")
    plt.ylabel("Heart Rate (bpm)")
    plt.title(f"Heart Rate {INTERVAL}[s] Interval Average")

    plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
    plt.gcf().autofmt_xdate()

    plt.savefig("src/Coospo/hr_graph.png")
    plt.close()

    print("Graph saved as src/Coospo/hr_graph.png")


if __name__ == "__main__":
    asyncio.run(main())
