"""
Author: Neil Schroeder
Desc: This is a little scipt that does some fancy things with image segmentation using clahe, slic, and kmeans
"""

import numpy as np
import cv2
import matplotlib.pyplot as plt
from skimage.segmentation import slic
from skimage.segmentation import mark_boundaries
from skimage.util import img_as_float
import time

def get_slic_clusters(slic_map, lab_image, rgb_image):
    # returns the centers and average color rgblab of the slic superpixels
    print("[INFO] extracting superpixel centers and average colors ...")

    centers = []
    color_avg = []

    for k in range(np.min(slic_map), np.max(slic_map)+1):
        if k%1000 == 0:
            print(f"{k}/{np.max(slic_map)}")
        truth_map = slic_map == k
        num_pixels = np.sum(truth_map)
        x_tot = np.sum(np.multiply(truth_map,
                                   np.outer(np.ones(truth_map.shape[0]), np.arange(truth_map.shape[1]))
                                  )
                      )
        y_tot = np.sum(np.multiply(truth_map,
                                   np.outer(np.arange(truth_map.shape[0]), np.ones(truth_map.shape[1]))
                                   )
                      )
        avg_lab = [np.sum(np.multiply(lab_image[:,:,i], truth_map))/float(num_pixels) for i in range(lab_image.shape[2])]

        avg_rgb = [np.sum(np.multiply(rgb_image[:,:,i], truth_map))/float(num_pixels) for i in range(rgb_image.shape[2])]

        centers.append((float(x_tot)/float(num_pixels), float(y_tot)/float(num_pixels)))
        color_avg.append([x for x in avg_rgb]+[x for x in avg_lab])

    return centers, np.array(color_avg)


def apply_colors(img, slic_map, sup_pix_colors):
    # recolors image using slic_map applying sup_pix_colors
    ret_rgb = np.zeros(img.shape)
    ret_lab = np.zeros(img.shape)
    spIDs = np.unique(slic_map.flatten())
    for index, spID in enumerate(spIDs):
        rgb = sup_pix_colors[index][0:3]
        lab = sup_pix_colors[index][3:]

        truth_map = slic_map == spID

        ret_rgb = np.add(ret_rgb, rgb*truth_map[:,:,np.newaxis])
        ret_lab = np.add(ret_lab, lab*truth_map[:,:,np.newaxis])

    return ret_rgb, ret_lab


def main():

    # read the image
    print(f"[INFO] reading image and augmenting ...")
    img = cv2.imread("img/neon.jpg")

    # convert to HSV
    img_lab = cv2.cvtColor(img, cv2.COLOR_BGR2LAB)
    img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    # run contrast limited adaptive histogram equalization
    clahe = cv2.createCLAHE()
    img_clahe = clahe.apply(img_lab[:,:,0])
    img_lab[:,:,0] = img_clahe
    #cv2.imwrite("img/neon_bgr2lab_clahe_lab2rgb.jpg", cv2.cvtColor(img_lab, cv2.COLOR_LAB2RGB))

    # generate superpixel map
    print("[INFO] creating superpixels ...")
    slic_map = slic(img_lab, n_segments = 64*64, sigma = 5)
    
    # get the centers (x,y) and avg color of each super pixel
    centers, color_avg = get_slic_clusters(slic_map, img_lab, img_rgb)
    color_avg = np.float32(color_avg) # cv2 is very picky
    
    # criteria for kmeans (end criteria(eps), num iters, eps)
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 10, 1.0)
    k = 4
    attempts = 10

    # run kmeans
    print("[INFO] segmenting image using kmeans ...")
    ret, sup_pix_label, cluster_centers = cv2.kmeans(color_avg, k, None, criteria, attempts, cv2.KMEANS_PP_CENTERS)

    # relabel image based on  
    cluster_centers = np.uint8(cluster_centers) # cast centers to 255
    sup_pix_colors = cluster_centers[sup_pix_label.flatten()] 

    # apply colors to superpixels
    print("[INFO] recoloring image ...")
    seg_img_rgb, seg_img_lab = apply_colors(img, slic_map, sup_pix_colors)
    #seg_img_rgb = cv2.cvtColor(np.uint8(seg_img_lab), cv2.COLOR_LAB2RGB)

    cv2.imwrite('img/neon_segmented_rgb_clahe.jpg', seg_img_rgb)
    cv2.imwrite('img/neon_segmented_lab_clahe.jpg', seg_img_lab)



if __name__ == "__main__":
    main()

