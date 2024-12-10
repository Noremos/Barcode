# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'

import sys

import os

#снег, дождь
# Путь к модулю (непосредсвенно к папке), т.е. сама библиотека расположена по пути
#                D:\\Programs\\C++\\Barcode\\x64\\Python\barpy.pyd
# sys.path.append('D:\\Programs\\Python\\BARPY')

# сам модуль
# sys.path.append('/Users/sam/Edu/library/Barcode/PrjBarlib/build/Python')
import ImageTopoDec as bc
import cv2


# Вывести все объекты (классы) из библиотеки
print(dir(bc))

# Для построения баркода необходиом использовать структуру Barstruct, которая инициализируется настройками
# ProcType -- как строить баркод: с 0-й яркости по 255-ю или с 255-й по 0-ю
# ColorType -- формат входного изображеия: серый, ргб (создаться 3 баркода) или адаптивный (как в изображении)
# ComponentType -- Срутктура баркода: компонента или дыра

# Сохранять ли банарные матрицы. Они отражают компоненты/дыры на каждой яркости
# 0 - вернуть баркод, длины которого посчитаны по числам бетти; 1 - вернуть баркод, длины которого посчитны по времени жизни копонент



def filter(img, revert, LL = 180):
    barcodeFactory = bc.BarcodeCreator()

    struct = bc.BarConstructor()
    struct.returnType = bc.ReturnType.barcode2d
    struct.createBinaryMasks = True
    struct.createGraph = False
    struct.setPorogStep(255)
    struct.addStructure(bc.ProcType.f0t255 if not revert else bc.ProcType.f255t0, bc.ColorType.gray, bc.ComponentType.Component)

    containet = barcodeFactory.createBarcode(img, struct)

    item = containet.getItem(0)
    bar = item.getBarcodeLines()

    binmap = np.zeros(img.shape, np.uint8)
    for bl in bar:

        if bl.len() < LL:
            continue

        #! 3 варианта
        #! Первый(медленный):
        # keyvals = bl.getPointsInDict().items()
        # for p in keyvals:
        #     binmap[p[0].y,p[0].x] += p[1]

        #! Второй(чуть быстрее):
        # keyvals = bl.getPoints()
        # for p in keyvals:
        #     binmap[p.y,p.x] += p.value.getAvgFloat()

        #! Третий(самый быстрый (в теории)):
        ksize= bl.getMatrixSize()
        for i in range(ksize):
            p = bl.getMatrixValue(i)
            binmap[p.y,p.x] += p.value.getAvgFloat()



    if not revert:
        binmap = 255 - binmap

    cv2.imshow("Filtered image",binmap)

def segment(img, revert, minSize, useBinarySegment = True):
    barcodeFactory = bc.BarcodeCreator()

    struct = bc.BarConstructor()
    struct.returnType = bc.ReturnType.barcode2d
    struct.createBinaryMasks = True
    struct.createGraph = False
    struct.setPorogStep(minSize)
    struct.addStructure(bc.ProcType.f0t255 if not revert else bc.ProcType.f255t0, bc.ColorType.gray, bc.ComponentType.Component)

    containet = barcodeFactory.createBarcode(img, struct)

    item = containet.getItem(0)
    bar = item.getBarcodeLines()

    # red=(0,0,255)
    # blue =(255,0,0)
    # green=(0,255,0)
    # colors=[red, blue, green]

    from random import randint
    colors = []
    if not useBinarySegment:
        for i in range(len(bar)):
            colors.append(np.array([randint(0, 255),randint(0, 255),randint(0, 255)]))

        binmap = np.zeros((img.shape[0],img.shape[1],3), np.uint8)
    else:
        binmap = np.zeros((img.shape[0],img.shape[1]), np.uint8)

    i=0
    for bl in bar:
        keyvals = bl.getPoints()

        if bl.len() < 40: #and len(keyvals)<500:
            continue

        if (len(keyvals)>img.shape[0]*img.shape[1]*0.9):
            continue

        for p in keyvals:
            binmap[p.y,p.x] = 255 if useBinarySegment else colors[i%len(colors)]

        i+=1

    cv2.imshow("Segmented image",binmap)
    cv2.imwrite("res2.jpg",binmap)


def restreByGraph(img, revert):
    barcodeFactory = bc.BarcodeCreator()

    struct = bc.BarConstructor()
    struct.returnType = bc.ReturnType.barcode2d
    struct.createBinaryMasks = True
    struct.createGraph = False
    # struct.createNewComponentOnAttach = True
    struct.setPorogStep(255)
    struct.addStructure(bc.ProcType.f0t255 if not revert else bc.ProcType.f255t0, bc.ColorType.gray, bc.ComponentType.Component)

    containet = barcodeFactory.createBarcode(img, struct)

    item = containet.getItem(0)
    bar = item.getBarcodeLines()

    binmap = np.zeros(img.shape, np.uint8)

    for bl in bar:
        ksize= bl.getMatrixSize()
        for i in range(ksize):
            p = bl.getMatrixValue(i)
            binmap[p.y,p.x] += p.value.getAvgFloat()


    if not revert:
        binmap = 255 - binmap

    cv2.imshow("Restored (by graph) image",binmap)


import matplotlib.pyplot as plt
import numpy as np

def scatterplot(img, revert):
    barcodeFactory = bc.BarcodeCreator()

    struct = bc.BarConstructor()
    struct.returnType = bc.ReturnType.barcode3d
    struct.createBinaryMasks = False
    struct.createGraph = False
    struct.setPorogStep(255)
    struct.addStructure(bc.ProcType.f0t255 if not revert else bc.ProcType.f255t0, bc.ColorType.gray, bc.ComponentType.Component)

    containet = barcodeFactory.createBarcode(img, struct)

    item = containet.getItem(0)
    bar = item.getBarcodeLines()


    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    n = 100

    kn=0
    for bl in bar:
        vals = bl.get3dList()

        for i in range(0,len(vals),5):
            ax.scatter(kn, vals[i].value, vals[i].count, marker='o')
        kn+=1

    ax.set_xlabel('X Label')
    ax.set_ylabel('Y Label')
    ax.set_zlabel('Z Label')

    plt.show()



# imgpath = 'D:\\Programs\\Python\\barcode\\lenna.png'
imgpath = 'D:\\Programs\\Python\\barcode\\3.png'
imgpath = 'D:\\Programs\\Python\\barcode\\CAMERA.BMP'
imgpath = 'D:\\Programs\\Python\\barcode\\boats.bmp'
img = cv2.imread(imgpath, cv2.IMREAD_GRAYSCALE)
cv2.imshow("source",img)
cv2.waitKey(1)



restreByGraph(img, False)
segment(img, False, 100, True)
filter(img, False, 5)

# scatterplot(img, False)
cv2.waitKey(0)