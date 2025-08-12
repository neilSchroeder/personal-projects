
from random import shuffle
from pyimagesearch.dataset import SegmentationDataset
from pyimagesearch.model import UNet
from pyimagesearch import config

from torch.nn import BCEWithLogitsLoss
from torch.optim import Adam
from torch.optim import AdamW
from torch.utils.data import DataLoader
import pytorch_lightning as pl
from sklearn.model_selection import train_test_split
from torchvision import transforms
from imutils import paths
from tqdm import tqdm

import matplotlib.pyplot as plt
import torch
import time
import os

def main():

    ## setup ##

    # locate images and masks
    imagePaths = sorted(list(paths.list_images(config.PATH_IMAGE_DATASET)))
    maskPaths = sorted(list(paths.list_images(config.PATH_MASK_DATASET)))

    # split the data
    split = train_test_split(imagePaths, maskPaths,
        test_size=config.TEST_SPLIT, random_state=42)

    (trainImages, testImages) = split[:2]
    (trainMasks, testMasks) = split[2:]

    # make a list of images to use as test
    f = open(config.PATHS_TEST, "w")
    f.write("\n".join(testImages))
    f.close()

    # define transformations
    transformations = transforms.Compose([transforms.ToPILImage(),
        transforms.Resize((config.INPUT_IMAGE_HEIGHT, config.INPUT_IMAGE_WIDTH)), transforms.ToTensor()])

    # setup the datasets
    trainDS = SegmentationDataset(imagePaths=trainImages, maskPaths=trainMasks, transforms=transformations)
    testDS = SegmentationDataset(imagePaths=testImages, maskPaths=testMasks, transforms=transformations)

    print(f'[INFO] found {len(trainDS)} examples in the training set')
    print(f'[INFO] found {len(testDS)} examples in the test set')

    trainLoader = DataLoader(trainDS, shuffle=True, batch_size=config.BATCH_SIZE, pin_memory=config.PIN_MEMORY, num_workers=os.cpu_count())
    testLoader = DataLoader(testDS, shuffle=False, batch_size=config.BATCH_SIZE, pin_memory=config.PIN_MEMORY, num_workers=os.cpu_count())

    unet = UNet()

    trainer = pl.Trainer(max_epochs=config.NUM_EPOCHS)

    print("[INFO] training the network ...")
    startTime = time.time()

    trainer.fit(unet, train_dataloaders=trainLoader)

    endTime = time.time()
    print(f'[INFO] total time to train the model: {endTime-startTime:.2f} s')

    

    # here we test
    trainer.test(dataloaders=testLoader)



if __name__ == '__main__':
    main()