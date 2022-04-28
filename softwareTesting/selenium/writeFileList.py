import os, sys
rootdir = './gnugo/'

with open("gnugo-file-list.txt", "w") as fileListFile:
    for subdir, dirs, files in os.walk(rootdir):
        for file in files:
            relativeFileName = os.path.join(subdir, file)
            if os.path.isfile(relativeFileName):
                print(relativeFileName)
                fileListFile.write(relativeFileName+"\n")
