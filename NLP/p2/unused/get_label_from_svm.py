import sys
import re
import random
from collections import Counter

syslen = len(sys.argv)
if(syslen != 4):
    print("ERROR: .py [svmlabel] [traindata(segment)] [output-label]")
    exit()

fin = open(sys.argv[1], "r")
ftrain = open(sys.argv[2], "r")
fans = open(sys.argv[3], "w")

pair = []
alli = 0
for line in ftrain:
    line = line.decode("utf-8")
    words = re.findall("(\S+)#(\S+)", line)
    wrongPosition = []
    nowindex = 0
    for word in words:
        alli += 1
        svmlabel = fin.readline().strip()
        if svmlabel == "1.0":
            for i in range(len(word[0])):
                wrongPosition.append(nowindex+1)
                nowindex += 1
        nowindex += 1
    # find longest sequence
    class sequence:
        def __init__(self, s = 0, e = 0):
            self.maxstart = s
            self.maxend = e
    nowlen = 0
    maxlen = 0
    maxlenList = []
    for i in range(len(wrongPosition)):
        nowlen = 1
        start = i
        if i+1 < len(wrongPosition) and wrongPosition[i+1] == wrongPosition[i]+1 :
            nowlen += 1
            i+=1
        if nowlen >= maxlen:
            if nowlen > maxlen:
                maxlenList = []
                maxlen = nowlen
            maxlenList.append(sequence(wrongPosition[start], wrongPosition[i]))
            #print wrongPosition
    print wrongPosition
    if maxlen == 0:
        a = random.randint(0, len(words)-1)
        b = random.randint(0, len(words)-1)
        if a > b:
            c = a
            a = b
            b = c
        ansstr = "%d %d\n" % (a, b)
    else:
        random.shuffle(maxlenList)
        ansstr = "%d %d\n" % (maxlenList[0].maxstart, maxlenList[0].maxend)
    fans.write(ansstr)
    


