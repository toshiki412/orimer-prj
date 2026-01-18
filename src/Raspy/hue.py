import threading
import time
import matplotlib.pyplot as plt
import pyaudio as pa
import numpy as np
import cv2
from PIL import Image, ImageFont, ImageDraw

# 参考
# https://skimie.com/articles/6a3bfa82712f59cb6b5a6c10d7


RATE=16000
BUFFER_SIZE=4096
# RATE=44100
# BUFFER_SIZE=16384
THRESHOLD = 1475361 # ← ここを調整（大きいほど強い音だけ反応）

HEIGHT=300
WIDTH=400
SCALE=[
	'ラ', 'ラ#', 'シ', 'ド', 'ド#', 'レ',
	'レ#', 'ミ', 'ファ', 'ファ#','ソ', 'ソ#'
]

RMS_THRESHOLD = 500
PEAK_THRESHOLD = 1e5

class HueDevice:
    def __init__(self):
        ## ストリーム準備
        self.audio = pa.PyAudio()
        self.stream = self.audio.open( rate=RATE,
		channels=1,
		format=pa.paInt16,
		input=True,
		frames_per_buffer=BUFFER_SIZE)

        ## 波形プロット用のバッファ				
        self.data_buffer = np.zeros(BUFFER_SIZE*16, int)
        
        self.scale_name = None
        self.running = False

    def __del__(self):
        self.running = False
        self.thread.join()
        self.stream.stop_stream()
        self.stream.close()
        self.audio.terminate()

    def TryConnect(self):
        self.running = True
        self.thread = threading.Thread(target=self.__Loop, daemon=True)
        self.thread.start()
        print("Hue Device connected.")
        return True
    
    def IsTriggered(self, name):
        if name == self.GetScale():
            return True
        return False

    def GetScale(self):
        return self.scale_name

    def __Loop(self):
        self.stream.start_stream()
        while self.running:
            time.sleep(0.01)
            self.scale_name = self.__Detect()
            
    def __Detect(self):
        audio_data = self.stream.read(BUFFER_SIZE, exception_on_overflow=False)

        data = np.frombuffer(audio_data, dtype='int16')

        rms = np.sqrt(np.mean(data**2))

        fd = np.fft.fft(data)
        fft_data = np.abs(fd[:BUFFER_SIZE//2])
        freq = np.fft.fftfreq(BUFFER_SIZE, d=1/RATE)

        min_f, max_f = 80, 1200  # 想定する音程帯
        valid = (freq[:BUFFER_SIZE//2] >= min_f) & (freq[:BUFFER_SIZE//2] <= max_f)
        fft_data_valid = fft_data[valid]
        freq_valid = freq[:BUFFER_SIZE//2][valid]

        peak = fft_data_valid.max()

        val = freq_valid[np.argmax(fft_data_valid)]
        offset = 0.5 if val >= 440 else -0.5
        scale_num = int(np.log2((val/440.0)**12) + offset) % len(SCALE)
        scale_name = SCALE[scale_num]

        if peak > PEAK_THRESHOLD and rms  < 50:
            return scale_name
        return None

if __name__ == "__main__":
    hue = HueDevice()
    hue.TryConnect()
    try:
        while True:
            scale = hue.GetScale()
            if scale is not None:
                print(f"Detected Scale: {scale}")
    except KeyboardInterrupt:
        pass