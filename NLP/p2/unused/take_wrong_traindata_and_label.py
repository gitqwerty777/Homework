import sys
import re
from collections import Counter

syslen = len(sys.argv)
if(syslen != 4):
    print("ERROR: .py [train(with correct and wrong)] [wrong sentence in train] [answer label]")
    exit()

fin = open(sys.argv[1], "r")
fsen = open(sys.argv[2], "w")
fans = open(sys.argv[3], "w")

pair = []
for line in fin:
    pair.append(line)
    if len(pair) == 1:
        continue
    else:
        wrongPosition = []
        correctWords = re.findall("(\S+)", pair[1])
        wrongOffset = 0
        
        for i in range(len(correctWords)):
            if i < len(wrongWords) and i+wrongOffset < len(wrongWords) and correctWords[i][1] != wrongWords[i+wrongOffset][1]: #wrong section
                if i+wrongOffset >= len(wrongWords): # iterate wrong to the end
                    break
                if i < len(correctWords):
                    nextCorrectWord = correctWords[i][1]
                else:
                    nextCorrectWord = ""
                while nextCorrectWord != wrongWords[i+wrongOffset][1]:
                    wrongPosition.append(i+wrongOffset)
                    wrongOffset = wrongOffset +1
                    if i+wrongOffset >= len(wrongWords):
                        break
        if len(correctWords) + wrongOffset <= len(wrongWords):
            wrongPosition.append(len(wrongWords)-1)
        offset = 0
        if len(correctWords) > len(wrongWords)+offset:
            wrongPosition.append(len(correctWords)+offset)
            offset += 1
        print "wrongpos: " + str(wrongPosition)
        print "correctlen = %d, wronglen = %d" % (len(correctWords), len(wrongWords))
        lengthList = []
        nowlen = 0
        for word in wrongWords:
            lengthList.append(nowlen)
            nowlen += len(word)
        lengthList.append(lengthList[-1])
        wrongStart = lengthList[wrongPosition[0]]
        wrongEnd = lengthList[wrongPosition[-1]+1]
        fans.write("%d %d\n" % (wrongStart, wrongEnd))
        fsen.write(pair[0] + "\n")
        pair = []
        
    
    
