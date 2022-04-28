import sys
from math import *
from Tkinter import *

def showImage():
    #if sys.argc == 1:
     #   print("no image file specified")
      #  imageFileName = "JPEG/monalisa.jpg"
    #else:
    imageFileName = "midoutput.raw"
    with open(imageFileName, "r") as imageFile:
        data = imageFile.read().split("\n")[0:-1]
    with open("size.txt", "r") as sizeFile:
        sizedata = sizeFile.read().split("\n")
        h = int(sizedata[0])
        w = int(sizedata[1])
        print("height %d width %d" % (h, w))

    row = []
    count = 0
    print("data length %d" % len(data))
    while count < len(data):
        col = []
        for i in range(w):
            col.append(data[count])
            count+=1
        row.append(tuple(col))
    row = tuple(row)

    root= Tk()

    im= PhotoImage(width=w, height=h)
    im.put(row)

    w= Label(root, image=im, bd=0)
    w.pack()
    mainloop()

showImage()
