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
        wrongWords = pair[0].strip().decode('utf-8')
        print wrongWords
        wrongWords =  list(wrongWords)
        correctWords = pair[1].strip().decode('utf-8')
        print correctWords
        correctWords =  list(correctWords)
        nowcorrectIndex = 0
        nowwrongIndex = 0
        while nowwrongIndex < len(wrongWords):
            nextCorrectWord = "" if nowcorrectIndex >= len(correctWords) else correctWords[nowcorrectIndex]
            if nextCorrectWord != wrongWords[nowwrongIndex]: #wrong section
                while nextCorrectWord != wrongWords[nowwrongIndex]:
                    print "compare" + nextCorrectWord + " and " + wrongWords[nowwrongIndex]
                    wrongPosition.append(nowwrongIndex+1)#start from 1
                    nowwrongIndex = nowwrongIndex+1
                    if nowwrongIndex >= len(wrongWords):
                        break
            nowwrongIndex += 1
            nowcorrectIndex += 1
        print "wrongpos: " + str(wrongPosition)
        fans.write("%d %d\n" % (wrongPosition[0], wrongPosition[-1]))
        fsen.write(pair[0] + "\n")
        pair = []
        
    
    
