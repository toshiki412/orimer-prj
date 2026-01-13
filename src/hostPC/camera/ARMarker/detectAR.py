### マーカーの座標を抽出する
import cv2
from cv2 import aruco
import numpy as np
import time

class MarkSearch :

    ### --- aruco設定 --- ###
    dict_aruco = aruco.getPredefinedDictionary(aruco.DICT_4X4_50)
    parameters = aruco.DetectorParameters()

    def __init__(self, cameraID):
        self.cap = cv2.VideoCapture(cameraID)

    def get_mark_coordinate(self, num_id):
        """
        静止画を取得し、所望のマークの座標を取得する
        """
        _, frame = self.cap.read()
        gray = cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY)

        corners, ids, _ = aruco.detectMarkers(gray, dict_aruco, parameters=parameters)

        ### num_id のマーカーが検出された場合 ###
        if num_id in np.ravel(ids) :
            index = np.where(ids == num_id)[0][0] #num_id が格納されているindexを抽出
            cornerUL = corners[index][0][0]
            cornerUR = corners[index][0][1]
            cornerBR = corners[index][0][2]
            cornerBL = corners[index][0][3]

            center = [ (cornerUL[0]+cornerBR[0])/2 , (cornerUL[1]+cornerBR[1])/2 ]

            cornerUL = tuple(map(int, cornerUL))
            cornerUR = tuple(map(int, cornerUR))
            cornerBL = tuple(map(int, cornerBL))
            cornerBR = tuple(map(int, cornerBR))
            center   = tuple(map(int, center))
            

            cv2.drawMarker(frame, position = cornerUL, color = (0,255,0), markerType=cv2.MARKER_CROSS, markerSize=20, thickness=2, line_type=cv2.LINE_4)
            cv2.drawMarker(frame, position = cornerUR, color = (0,255,0), markerType=cv2.MARKER_CROSS, markerSize=20, thickness=2, line_type=cv2.LINE_4)
            cv2.drawMarker(frame, position = cornerBR, color = (0,255,0), markerType=cv2.MARKER_CROSS, markerSize=20, thickness=2, line_type=cv2.LINE_4)
            cv2.drawMarker(frame, position = cornerBL, color = (0,255,0), markerType=cv2.MARKER_CROSS, markerSize=20, thickness=2, line_type=cv2.LINE_4)
            cv2.drawMarker(frame, position = center, color = (0,0,255), markerType=cv2.MARKER_CROSS, markerSize=20, thickness=2, line_type=cv2.LINE_4)            

        cv2.imshow("ar",frame)
        cv2.waitKey(1)


if __name__ == "__main__" :

    import cv2
    from cv2 import aruco
    import numpy as np
    import time

    ### --- aruco設定 --- ###
    dict_aruco = aruco.getPredefinedDictionary(aruco.DICT_4X4_50)
    parameters = aruco.DetectorParameters()

    ### --- parameter --- ###
    cameraID = 0
    cam0_mark_search = MarkSearch(cameraID)

    markID = 0

    try:
        while True:
            cam0_mark_search.get_mark_coordinate(markID)
            time.sleep(0.1)
    except KeyboardInterrupt:
        cam0_mark_search.cap.release()