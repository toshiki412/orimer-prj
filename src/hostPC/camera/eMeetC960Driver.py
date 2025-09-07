class EMeetC960:
    '''
    eMeet C960 固有のパラメータを管理
    https://emeet.co.jp/products/webcam-c960
    '''
    
    def __init__(self):

        self.param = {
            # 1080p 30fps 固定フォーカスの場合の例
            'sensor_width': 1920,
            'sensor_height': 1080,
            'focal_length_mm': 2.88,
            'sensor_width_mm': 6.4 * (16 / 18.5),  # 水平方向のセンサーサイズ
            'fov_deg': 90,
        }

    def GetSensorParam(self):
        return self.param
