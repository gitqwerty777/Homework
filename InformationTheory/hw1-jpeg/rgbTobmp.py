import numpy, array, PIL, Image, sys
from struct import *

# http://stackoverflow.com/questions/9922402/how-to-convert-raw-images-to-png-in-python
# Use the PIL raw decoder to read the data

with open("size.txt", "r") as sizeFile:
    sizedata = sizeFile.read().split("\n")
    h = int(sizedata[0])
    w = int(sizedata[1])
    print("height %d width %d" % (h, w))

with open("outputFileName", "r") as outputFile:
    outputFileName = outputFile.read()
    print("outputFile: %s" % outputFileName)

rawData = open("rgb.raw", 'rb').read()
imgSize = (w, h)

img = Image.frombytes('RGB', imgSize, rawData, 'raw')
img.save(outputFileName)
