import os

from receiver import ReceiverThread
# Must set before any Qt / cv2 / pygame imports
os.environ['QT_AUTO_SCREEN_SCALE_FACTOR'] = '1'
os.environ['QT_ENABLE_HIGHDPI_SCALING'] = '1'
os.environ['PYGAME_DETECT_AVX2'] = '1'

import argparse
import threading
import camera_thread
import procon_ble

from bleak.backends.winrt.util import uninitialize_sta
uninitialize_sta()

def RunOnlyCameraThreadForDebug():
    finishEvent = threading.Event()
    cameraThread = threading.Thread(target=camera_thread.CameraMainThread, args=(finishEvent,))
    cameraThread.start()
    try:
        cameraThread.join()
    # ctrl + C
    except KeyboardInterrupt:
        finishEvent.set()
        cameraThread.join()

def RunOnlyBleThreadForDebug():
    finishEvent = threading.Event()
    bleThread = threading.Thread(target=procon_ble.BleMainThread, args=(finishEvent,))
    bleThread.start()
    try:
        bleThread.join()
    # ctrl + C
    except KeyboardInterrupt:
        finishEvent.set()
        bleThread.join()

def RunForRaspy():
    finishEvent = threading.Event()
    ReceiverThread(finishEvent)
    finishEvent.set()

def Main():
    finishEvent = threading.Event()

    cameraThread = threading.Thread(target=camera_thread.CameraMainThread, args=(finishEvent,))
    bleThread = threading.Thread(target=procon_ble.BleMainThread, args=(finishEvent,))
    
    cameraThread.start()
    bleThread.start()
    
    try:
        cameraThread.join()
        bleThread.join()
    
    # ctrl + C
    except KeyboardInterrupt:
        print("Stopping All Threads...")
        finishEvent.set()

        cameraThread.join()
        bleThread.join()

#メイン関数の引数を読み込む
def PaeseArgs():
    parser = argparse.ArgumentParser()

    #camera のみを動かす場合
    parser.add_argument("--only_camera",action='store_true')

    #ble のみを動かす場合
    parser.add_argument("--only_ble",action='store_true')

    #raspy を動かす場合
    parser.add_argument("--raspy",action='store_true')

    args = parser.parse_args()
    return args

if __name__ == '__main__':
    args = PaeseArgs()
    
    if args.only_camera:
        RunOnlyCameraThreadForDebug()
    
    elif args.only_ble:
        RunOnlyBleThreadForDebug()

    elif args.raspy:
        RunForRaspy()

    else:
        Main()