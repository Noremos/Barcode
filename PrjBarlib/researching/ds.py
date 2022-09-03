import glob
import cv2

import matplotlib.pyplot as plt
import numpy as np

from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib import colors


def getPixelsColors(nemo):
    pixel_colors = nemo.reshape((np.shape(nemo)[0]*np.shape(nemo)[1], 3))
    norm = colors.Normalize(vmin=-1.,vmax=1.)
    norm.autoscale(pixel_colors)
    pixel_colors = norm(pixel_colors).tolist()
    return pixel_colors

def showRgbColor(nemo):
    nemo = cv2.cvtColor(nemo, cv2.COLOR_BGR2RGB)
    r, g, b = cv2.split(nemo)
    fig = plt.figure()
    axis = fig.add_subplot(1, 1, 1, projection="3d")
    pixel_colors = getPixelsColors(nemo)

    axis.scatter(r.flatten(), g.flatten(), b.flatten(), facecolors=pixel_colors, marker=".")
    axis.set_xlabel("Red")
    axis.set_ylabel("Green")
    axis.set_zlabel("Blue")
    plt.show()


def showHlsColors(nemo):
    hsv_nemo = cv2.cvtColor(nemo, cv2.COLOR_BGR2HSV)
    h, s, v = cv2.split(hsv_nemo)
    fig = plt.figure()
    axis = fig.add_subplot(1, 1, 1, projection="3d")
    pixel_colors = getPixelsColors(nemo)

    axis.scatter(h.flatten(), s.flatten(), v.flatten(), facecolors=pixel_colors, marker=".")
    axis.set_xlabel("Hue")
    axis.set_ylabel("Saturation")
    axis.set_zlabel("Value")
    plt.show()


def segmentKMeans(input):
    # img = cv2.cvtColor(input,cv2.COLOR_BGR2HSV)
    img = cv2.cvtColor(input,cv2.COLOR_BGR2RGB)
    twoDimage = img.reshape((-1,3))
    twoDimage = np.float32(twoDimage)

    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 10, 1.0)
    K = 5
    attempts=10

    ret,label,center=cv2.kmeans(twoDimage,K,None,criteria,attempts,cv2.KMEANS_PP_CENTERS)
    center = np.uint8(center)
    res = center[label.flatten()]
    result_image = res.reshape((img.shape))
    cv2.imshow("result", result_image)
    cv2.imshow("source", img)
    cv2.waitKey(0)

for i in range(16):
    f = f"./res/8_m{i}_prc0col0cmp1.png"

    imgNew = cv2.imread(f, cv2.IMREAD_COLOR)
    if imgNew is None:
        continue

    # showRgbColor(imgNew)
    showHlsColors(imgNew)
    segmentKMeans(imgNew)

    continue
    fig, ax = plt.subplots(1,1)

    x = []
    y = []
    z = []
    cols = []
    plotnost = {}
    plotcols = {}
    for w in range(imgNew.shape[0]):
        for h in range(imgNew.shape[1]):
            col = imgNew[w,h]
            # x.append(col[0])
            # y.append(col[1])
            # z.append(col[2])
            # cols.append(col/255.0)
            keyw = tuple(col)
            if not keyw in plotnost:
                plotnost[keyw] = 1
                # plotcols[keyw] = col/255.0
            else:
                plotnost[keyw] += 1

    plotnost = dict(sorted(plotnost.items(), key=lambda item: item[1], reverse = True))

    keys = list(plotnost.keys())
    values = list(plotnost.values())
    # keys = range(len(plotnost))

    skeys = []
    plotcols = []
    for v in keys: #bgr -> rgb
        x.append(col[2])
        y.append(col[1])
        z.append(col[0])
        plotcols.append([v[2]/255.0, v[1]/255.0, v[0]/255.0])
        skeys.append(str(v))

    k = 0
    while values[k] > 50:
        k+=1

    skeys = skeys[:k]
    values = values[:k]
    # names = names[:k]
    # plotcols = plotcols[:k]

    ax.bar(skeys, values, color = plotcols)

    # x = x[:k]
    # y = y[:k]
    # z = z[:k]
    # Creating plot
    fig, ax3d = plt.subplots(subplot_kw={"projection": "3d"})

    ax3d.scatter3D(x, y, z, c = plotcols)
    plt.title("simple 3D scatter plot")

    plt.show()
    cv2.waitKey(0)

    # Plot the surface.
    # surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
    #                     linewidth=0, antialiased=False)

    # cset = ax.contourf(x, y, z,
    #                 zdir ='z',
    #                 offset = np.min(z),
    #                 cmap = my_cmap)
    # cset = ax.contourf(x, y, z,
    #                 zdir ='x',
    #                 offset =-5,
    #                 cmap = my_cmap)
    # cset = ax.contourf(x, y, z,
    #                 zdir ='y',
    #                 offset = 5,
    #                 cmap = my_cmap)
    # fig.colorbar(surf, ax = ax,
    #             shrink = 0.5,
    #             aspect = 5)
    
    
    # Customize the z axis.
    # ax.set_zlim(0, 255)
    # ax.zaxis.set_major_locator(plt.LinearLocator(10))
    # # A StrMethodFormatter is used automatically
    # ax.zaxis.set_major_formatter('{x:.02f}')

    # # Add a color bar which maps values to colors.
    # fig.colorbar(surf, shrink=0.5, aspect=5)


    # fig, axs = plt.subplots(1, 1, figsize=(8, 4))
    # axs.hist(imgNew, density=True, bins=30)
    # axs.grid(True)
    # # axs[1].plot
    # plt.show()

    # img = imgNew
    # Z = img.reshape((-1,3))
    # # convert to np.float32
    # Z = np.float32(Z)
    # # define criteria, number of clusters(K) and apply kmeans()
    # criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 10, 1.0)
    # K = 4
    # ret,label,center=cv2.kmeans(Z,K,None,criteria,10,cv2.KMEANS_RANDOM_CENTERS)
    # # Now convert back into uint8, and make original image
    # center = np.uint8(center)
    # res = center[label.flatten()]
    # res2 = res.reshape((img.shape))
    # cv2.imshow('res2',res2)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()
