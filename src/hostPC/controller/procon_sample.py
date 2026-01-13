from controllerManager import ControllerManager
import keyMap
import time
def main():
    controllerMgr = ControllerManager()

    if not controllerMgr.TryConnect():
        return

    print("Press Home button to exit")
    while True:

        for button in keyMap.PROCON_BUTTON_MAP.keys():
            if controllerMgr.IsButtonTriggered(button):
                print(f"Button {button} Triggered")
                
                if button == "Home":
                    return
    
        axisX, axisY = controllerMgr.GetAxisState()
        
        if abs(axisX) > 0.2 or abs(axisY) > 0.2:
            print(f"Axis X: {axisX}, Axis Y: {axisY}")
        
        time.sleep(0.1)

if __name__ == '__main__':
    main()