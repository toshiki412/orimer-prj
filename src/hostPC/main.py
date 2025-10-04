import argparse
import threading
import camera_main
import procon_ble

def RunOnlyCameraThreadForDebug():
    finishEvent = threading.Event()
    cameraThread = threading.Thread(target=camera_main.CameraMainThread, args=(finishEvent,))
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
    
def Main():
    finishEvent = threading.Event()

    cameraThread = threading.Thread(target=camera_main.CameraMainThread, args=(finishEvent,))
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

    args = parser.parse_args()
    return args

if __name__ == '__main__':
    args = PaeseArgs()
    
    if args.only_camera:
        RunOnlyCameraThreadForDebug()
    
    elif args.only_ble:
        RunOnlyBleThreadForDebug()
        
    else:
        Main()