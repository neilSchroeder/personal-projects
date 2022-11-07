from pyimagesearch import config

import matplotlib.pyplot as plt
import numpy as np
import torch
import cv2
import os


def prepare_plot(original, mask, predictedMask):

    figure, ax = plt.subplots(1, 3, figsize=(10,10))

    ax[0].imshow(original)
    ax[1].imshow(mask)
    ax[2].imshow(predictedMask)

    ax[0].set_title("Image")
    ax[1].set_title("Mask")
    ax[2].set_title("Predicted Mask")

    figure.tight_layout()
    plt.show()


def make_predictions(model, imagePath):

    model.eval()

    with torch.no_grad():

        image = cv2.imread(imagePath)
        image = cv2.cvtColor(image, cv2.COLOR_BGR2LAB)
        clahe = cv2.createCLAHE()
        img_clahe = clahe.apply(image[:,:,0])
        image[:,:,0] = img_clahe
        image = image.astype("float32") / 255.0

        image = cv2.resize(image,(128,128))
        original = image.copy()

        filename = imagePath.split(os.path.sep)[-1]
        groundTruthPath = os.path.join(config.PATH_MASK_DATASET, filename)

        gtMask = cv2.imread(groundTruthPath,0)
        gtMask = cv2.resize(gtMask, (config.INPUT_IMAGE_HEIGHT, config.INPUT_IMAGE_WIDTH))


        image = np.transpose(image, (2,0,1))
        image = np.expand_dims(image, 0)
        image = torch.from_numpy(image).to(config.DEVICE)

        predMask = model(image).squeeze()
        predMask = torch.sigmoid(predMask)
        predMask = predMask.cpu().numpy()

        predMask = (predMask > config.THRESHOLD) * 255
        predMask = np.uint8(predMask)

        prepare_plot(original, gtMask, predMask)


def main():

    print("[INFO] loading image paths ...")
    imagePaths = open(config.PATHS_TEST).read().strip().split("\n")
    imagePaths = np.random.choice(imagePaths, size=10)

    print("[INFO] loading model ...")
    unet = torch.load(config.PATH_MODEL).to(config.DEVICE)

    for path in imagePaths:
        make_predictions(unet, path)


if __name__ == '__main__':
    main()
    

