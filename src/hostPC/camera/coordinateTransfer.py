import math
import numpy as np
from camera.eMeetC960Driver import EMeetC960

def RtoD(rad):
    return rad * 180 / math.pi

def DtoR(degree):
    return degree * math.pi / 180

class CoordinateTransfer():

    def __init__(self, w, h, camH, camO):
        self.sensorParam = EMeetC960().GetSensorParam()
        self.IMG_W = w
        self.IMG_H = h
        self.CAMERA_O = camO
        self.CAMERA_H = camH

    def __NormalizeImgCoords(self, imgX, imgY):
        x = + imgX - self.IMG_W * 0.5
        y = - imgY + self.IMG_H * 0.5
        normalizedImgX = x / (self.IMG_W * 0.5)
        normalizedImgY = y / (self.IMG_H * 0.5)
        # print(normalizedImgX, normalizedImgY)
        return normalizedImgX, normalizedImgY

    def __NormalizedImgXY2CameraXYZ(self, imgX, imgY):
        verticalFov = self.sensorParam['fov_deg']
        cameraX = 1.0/math.tan(DtoR(verticalFov / 2))
        cameraY = -imgX
        cameraZ = imgY

        return cameraX, cameraY, cameraZ

    def __CameraXYZ2worldXYOnFloor(self, cameraX, cameraY, cameraZ):
        sin_O = math.sin(DtoR(-self.CAMERA_O))
        cos_O = math.cos(DtoR(-self.CAMERA_O))
        cameraXYZ = np.array([cameraX, cameraY, cameraZ, 1])

        rotationMatrix = np.array([[cos_O, 0, sin_O, 0],
                                [0, 1, 0, 0],
                                [-sin_O, 0, cos_O, 0],
                                [0, 0, 0, 1]])
        # print(cameraXYZ)
        worldXYZ = rotationMatrix @ cameraXYZ
        # print(worldXYZ)
        wX, wY, wZ, _ = worldXYZ

        k = abs(self.CAMERA_H) / abs(wZ)
        # print(k)

        floorX =  k * wX
        floorY =  k * wY
        return floorX, floorY

    def Img2World(self, posInPixel)-> tuple:
        x, y = posInPixel
        nX, nY = self.__NormalizeImgCoords(x, y)
        cX, cY, cZ = self.__NormalizedImgXY2CameraXYZ(nX, nY)
        fX, fY = self.__CameraXYZ2worldXYOnFloor(cX, cY, cZ)
        return (fX, fY)