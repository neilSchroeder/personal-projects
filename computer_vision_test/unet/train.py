
from random import shuffle
from pyimagesearch.dataset import SegmentationDataset
from pyimagesearch.model import UNet
from pyimagesearch import config

from torch.nn import BCEWithLogitsLoss
from torch.optim import Adam
from torch.optim import AdamW
from torch.utils.data import DataLoader
from sklearn.model_selection import train_test_split
from torchvision import transforms
from imutils import paths
from tqdm import tqdm

import matplotlib.pyplot as plt
import torch
import time
import os

def main():

    imagePaths = sorted(list(paths.list_images(config.PATH_IMAGE_DATASET)))
    maskPaths = sorted(list(paths.list_images(config.PATH_MASK_DATASET)))

    split = train_test_split(imagePaths, maskPaths,
        test_size=config.TEST_SPLIT, random_state=42)

    (trainImages, testImages) = split[:2]
    (trainMasks, testMasks) = split[2:]

    f = open(config.PATHS_TEST, "w")
    f.write("\n".join(testImages))
    f.close()

    transformations = transforms.Compose([transforms.ToPILImage(),
        transforms.Resize((config.INPUT_IMAGE_HEIGHT, config.INPUT_IMAGE_WIDTH)), transforms.ToTensor()])

    trainDS = SegmentationDataset(imagePaths=trainImages, maskPaths=trainMasks, transforms=transformations)
    testDS = SegmentationDataset(imagePaths=testImages, maskPaths=testMasks, transforms=transformations)

    print(f'[INFO] found {len(trainDS)} examples in the training set')
    print(f'[INFO] found {len(testDS)} examples in the test set')

    trainLoader = DataLoader(trainDS, shuffle=True, batch_size=config.BATCH_SIZE, pin_memory=config.PIN_MEMORY, num_workers=os.cpu_count())
    testLoader = DataLoader(testDS, shuffle=False, batch_size=config.BATCH_SIZE, pin_memory=config.PIN_MEMORY, num_workers=os.cpu_count())

    unet = UNet().to(config.DEVICE)

    lossFunc = BCEWithLogitsLoss()
    opt = Adam(unet.parameters(), lr=config.INIT_LR)

    trainSteps = len(trainDS) // config.BATCH_SIZE
    testSteps = len(testDS) // config.BATCH_SIZE

    H = {"train_loss": [], "test_loss":[]}

    print("[INFO] training the network ...")
    startTime = time.time()

    for e in tqdm(range(config.NUM_EPOCHS)):

        unet.train()

        totalTrainLoss = 0.
        totalTestLoss = 0.

        for (i, (x,y)) in enumerate(trainLoader):

            (x,y) = (x.to(config.DEVICE), y.to(config.DEVICE))

            pred = unet(x)
            loss = lossFunc(pred,y)

            opt.zero_grad()
            loss.backward()
            opt.step()

            totalTrainLoss += loss

        with torch.no_grad():
            unet.eval()

            for (i,(x,y)) in enumerate(testLoader):

                (x,y) = (x.to(config.DEVICE), y.to(config.DEVICE))

                pred = unet(x)
                totalTestLoss += lossFunc(pred,y)

    
        avgTrainLoss = totalTrainLoss / trainSteps
        avgTestLoss = totalTestLoss / testSteps

        H["train_loss"].append(avgTrainLoss.cpu().detach().numpy())
        H["test_loss"].append(avgTestLoss.cpu().detach().numpy())

        print(f"[INFO] epoch: {e+1}/{config.NUM_EPOCHS}")
        print(f"Train Loss: {avgTrainLoss:.6f}, Test Loss: {avgTestLoss:.4f}")

    endTime = time.time()
    print(f'[INFO] total time to train the model: {endTime-startTime:.2f} s')

    plt.style.use("ggplot")
    plt.figure()
    plt.plot(H["train_loss"], label="train_loss")
    plt.plot(H["test_loss"], label="test_loss")
    plt.title("Training Loss on Dataset")
    plt.xlabel("Epoch")
    plt.ylabel("Loss")
    plt.legend(loc="lower left")
    plt.savefig(config.PATH_PLOT)

    torch.save(unet, config.PATH_MODEL)



if __name__ == '__main__':
    main()