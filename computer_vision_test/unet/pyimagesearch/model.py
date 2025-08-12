from turtle import forward
from . import config

from torch.nn import ConvTranspose2d
from torch.nn import Conv2d
from torch.nn import MaxPool2d
from torch.nn import Module
from torch.nn import ModuleList
from torch.nn import ReLU
from torch.nn import functional as F
from torchvision.transforms import CenterCrop
import torch

import pytorch_lightning as pl


class Block(Module):
    def __init__(self, inChannels, outChannels):
        super().__init__()

        self.conv1 = Conv2d(inChannels, outChannels, config.CONV_KERNEL_SIZE)
        self.relu = ReLU()
        self.conv2 = Conv2d(outChannels, outChannels, config.CONV_KERNEL_SIZE)

    def forward(self, x):
        return self.relu(self.conv2(self.relu(self.conv1(x))))


class Encoder(Module):
    # Abstracts the image by downsampling, creating feature maps
    def __init__(self, channels=config.ENCODER_CHANNELS):
        super().__init__()

        self.encBlocks = ModuleList(
            [Block(channels[i], channels[i+1])
                for i in range(len(channels) -1)]
        )
        self.pool = MaxPool2d(config.POOL_KERNEL_SIZE)

    def forward(self, x):

        blockOutputs = []

        for block in self.encBlocks:
            x = block(x)
            blockOutputs.append(x)
            x = self.pool(x)

        return blockOutputs


class Decoder(Module):
    # Up-conv of previous feature maps paired with feature maps from the Encoder
    def __init__(self, channels=config.DECODER_CHANNELS) -> None:
        super().__init__()

        self.channels = channels
        # upconvolutions
        self.upconvs = ModuleList(
            [ConvTranspose2d(channels[i], 
                             channels[i+1], 
                             config.UPCONV_KERNEL_SIZE, 
                             config.UPCONV_STRIDE_LEN
                             )
                for i in range(len(channels) - 1)
            ]
        )

        # decoder blocks
        self.decBlocks = ModuleList(
            [Block(channels[i], channels[i+1]) for i in range(len(channels)-1)]
        )


    def crop(self, encFeatures, x):
        # crops encoder features to match shape of input
        (_,_, H, W) = x.shape
        encFeatures = CenterCrop([H,W])(encFeatures)
        return encFeatures


    def forward(self, x, encFeatures):

        for i in range(len(self.channels) - 1):
            x = self.upconvs[i](x)

            # crop features, concat with inputs, decode
            encFeat = self.crop(encFeatures[i], x)
            x = torch.cat([x,encFeat], dim=1)
            x = self.decBlocks[i](x)

        return x

    
class UNet(pl.LightningModule):
    def __init__(self, encChannels=config.UNET_ENC_CHANNELS, 
                       decChannels=config.UNET_DEC_CHANNELS,
                       nbClasses=config.UNET_NUM_CLASSES,
                       retainDim=True,
                       outSize=(config.INPUT_IMAGE_HEIGHT, config.INPUT_IMAGE_WIDTH)):
        super().__init__()

        self.encoder = Encoder(encChannels)
        self.decoder = Decoder(decChannels)

        self.head = Conv2d(decChannels[-1], nbClasses, 1)
        self.retainDim = retainDim
        self.outSize = outSize


    def forward(self, x):

        # this calls self.encoder.forward(x)
        encFeatures = self.encoder(x)

        # this calls self.decoder.forward(x, encFeatures)
        decFeatures = self.decoder(encFeatures[::-1][0], encFeatures[::-1][1:])

        map = self.head(decFeatures)

        if self.retainDim:
            map = F.interpolate(map, self.outSize)

        return map

    def training_step(self, batch, batch_idx):
        x, y = batch
        y_hat = self(x) # these are the values predicted by the UNet
        loss = config.UNET_LOSS(y_hat, y)
        self.log("train_loss", loss, on_step=True, on_epoch=True, prog_bar=True)
        return loss

    def configure_optimizers(self):
        return torch.optim.Adam(self.parameters(), lr=config.INIT_LR)


