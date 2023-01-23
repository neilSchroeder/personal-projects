import cv2
from sklearn.decomposition import PCA
import matplotlib.pyplot as plt
import numpy as np
from scipy import ndimage

from fil_finder import FilFinder2D
import astropy.units as u

import time

def skeletonize(img):

    fil = FilFinder2D(img, distance=10*u.pix, mask=img)
    fil.preprocess_image()
    fil.medskel(verbose=False)
    fil.analyze_skeletons(skel_thresh=10*u.pix, branch_thresh=40*u.pix, prune_criteria='length',verbose=False)
    return fil.skeleton_longpath


# load image 
img = cv2.imread('leaf_with_blk_bkg.jpg')
img = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY) # black and white
img = cv2.GaussianBlur(img, (5,5), 0)
_, img = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY+cv2.THRESH_OTSU)

contours, _ = cv2.findContours(img, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)

contour_canvas = np.zeros(img.shape)
for c in contours:
    cv2.drawContours(contour_canvas, c, -1, 255, 1)

pca = PCA(n_components=2)
cols, rows = np.where(contour_canvas == 255)
X = np.array([cols, rows]).T
projection = pca.fit_transform(X)

angle = np.arctan2(pca.components_[0,1], pca.components_[0,0])
angle = np.degrees(angle)

rotated = ndimage.rotate(contour_canvas, -angle)
_, rotated = cv2.threshold(rotated, 50, 255, cv2.THRESH_BINARY)

t0 = time.perf_counter()
widths = []
for row in rotated:
    vals = np.where(row > 0)[0]
    # print(vals)
    if len(vals) == 2:
        widths.append(np.amax(vals)-np.amin(vals))
t1 = time.perf_counter()
print(t1 - t0)

# t2 = time.perf_counter()
# widths = [np.amax(np.where(row > 0)[0]) - np.amin(np.where(row > 0)[0]) for row in rotated if len(np.where(row > 0)[0]) == 2]
# t3 = time.perf_counter()
# print(t3-t2)

print(f'width: mean {np.average(widths)}, median {np.median(widths)}, max {np.amax(widths)}')
print(len(widths))

plt.imshow(rotated, 'gray')
plt.show()
