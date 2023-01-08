import torch
import os

# dataset path
PATH_DATASET = os.path.join("dataset","train")

# images and masks path
PATH_IMAGE_DATASET = os.path.join(PATH_DATASET, "images")
PATH_MASK_DATASET = os.path.join(PATH_DATASET, "masks") 

# output paths
BASE_OUTPUT = "output"

PATH_MODEL = os.path.join(BASE_OUTPUT, "unet_tgs_salt.pth")
PATH_PLOT = os.path.sep.join([BASE_OUTPUT, "plot.png"])
PATHS_TEST = os.path.sep.join([BASE_OUTPUT, "test_paths.txt"])

# define split
TEST_SPLIT = 0.150

# device
DEVICE = "cuda" if torch.cuda.is_available() else "cpu"

# memory pinning
PIN_MEMORY = True if DEVICE == "cuda" else False

""" Model Config """
# block
CONV_KERNEL_SIZE = 3

# encoder
POOL_KERNEL_SIZE = 2
ENCODER_CHANNELS = (3,8,16,32,64)

# decoder
DECODER_CHANNELS = (64,32,16,8)
UPCONV_KERNEL_SIZE = 2
UPCONV_STRIDE_LEN = 2

#unet 
UNET_ENC_CHANNELS = (3,8,16,32,64)
UNET_DEC_CHANNELS = (64,32,16,8)
UNET_NUM_CLASSES = 1

UNET_LOSS = torch.nn.BCEWithLogitsLoss()

NUM_CHANNELS = 1
NUM_CLASSES = 1
NUM_LEVELS = 1

INIT_LR = 0.0015
NUM_EPOCHS = 40
BATCH_SIZE = 64

""" Image Properties """
# input dimensions
INPUT_IMAGE_WIDTH = 128
INPUT_IMAGE_HEIGHT = 128

THRESHOLD = 0.5 # validation threshold for cutting on weak pixels

