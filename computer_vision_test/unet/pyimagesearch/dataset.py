from torch.utils.data import Dataset
import cv2

class SegmentationDataset(Dataset):
    def __init__(self, imagePaths, maskPaths, transforms):
        # store paths and augmentation transforms
        self.imagePaths = imagePaths
        self.maskPaths = maskPaths
        self.transforms = transforms

    def __len__(self):
        return len(self.imagePaths)

    def __getitem__(self,idx):
        # gets the image path from the current index
        imagePath = self.imagePaths[idx]

        # load image, convert BGR to RGB, load mask
        img = cv2.imread(imagePath)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2LAB)
        mask = cv2.imread(self.maskPaths[idx],0)
        clahe = cv2.createCLAHE()
        img_clahe = clahe.apply(img[:,:,0])
        img[:,:,0] = img_clahe
        
        if self.transforms is not None:
            img = self.transforms(img)
            mask = self.transforms(mask)

        return (img, mask)

        