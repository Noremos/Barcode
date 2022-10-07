import glob
import cv2

import matplotlib.pyplot as plt
import numpy as np


bestRe = {}
for i in range(16):
    files = glob.glob(f"./processed/{i}_*.png")
    imgMask = cv2.imread(f"./tiles/{i}_bld.png", cv2.IMREAD_GRAYSCALE)
    if imgMask is None:
        continue

    mprs = [0,0]
    for f in files:
        if f.endswith('_2.png'):
            continue

        imgNew = cv2.imread(f, cv2.IMREAD_GRAYSCALE)
        newE = f.find('_')
        newE2 = f.rfind('.')
        nameE = f[newE + 1:newE2]
        if not nameE in bestRe:
            bestRe[nameE] = [0,0, 0]
        # fig, axs = plt.subplots(1, 2, figsize=(8, 4))
        # axs[0].grid(True)
        # axs[0].hist(imgNew[imgMask == 255], density=True, bins=30)

        # axs[1].grid(True)
        # axs[1].hist(imgNew[imgMask == 0], density=True, bins=30)
        # plt.show()

        r1 = imgNew[imgMask == 255].sum() / np.count_nonzero(imgMask == 255)
        r2 = imgNew[imgMask == 0].sum() / np.count_nonzero(imgMask == 0)
        r = abs(r1 - r2)
        if (r):
            mprs[0] = r
            mprs[1] = f

        bestRe[nameE][0] += r1
        bestRe[nameE][1] += r2
        bestRe[nameE][2] += 1

        # print(f"for {f}: {r}")

        # temp = imgNew.copy()
        # temp[imgMask == 0] = 0
        # cv2.imshow("biuilds", temp)
        # temp = imgNew.copy()
        # temp[imgMask == 255] = 0
        # cv2.imshow("other", temp)
        # cv2.waitKey(0)
        # plt.style.use('_mpl-gallery')

        # for h in range(img.shape[0]):
        #     for w in range(img.shape[1]):
        #         if img[h,w] != 0:
        #             imgNew[h,w]
        # # make the data
        # np.random.seed(3)
        # x = 4 + np.random.normal(0, 2, 24)
        # y = 4 + np.random.normal(0, 2, len(x))
        # # size and color:
        # sizes = np.random.uniform(15, 80, len(x))
        # colors = np.random.uniform(15, 80, len(x))

        # # plot
        # fig, ax = plt.subplots()

        # ax.scatter(x, y, s=sizes, c=colors, vmin=0, vmax=100)

        # ax.set(xlim=(0, 8), xticks=np.arange(1, 8),
        #     ylim=(0, 8), yticks=np.arange(1, 8))


        # fig, axs = plt.subplots(3, 1, figsize=(5, 15), sharex=True, sharey=True,
                                # tight_layout=True)

        # We can increase the number of bins on each axis
        # axs[0].hist2d(hist, dist2, bins=40)
        # axs[0].hist2d(hist, dist2, bins=40)

        # fig.show()
    print(f"The best one: {mprs[0]} with {mprs[1]}")

for keyR in bestRe:
    cou = bestRe[keyR][2]
    k1 = bestRe[keyR][0] / cou
    k2 = bestRe[keyR][1] / cou
    k = abs(k2-k1)
    print(f"Avg for {keyR}: {k} -- {k1}:{k2}")
