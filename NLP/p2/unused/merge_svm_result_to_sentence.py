import sys
import re
from collections import Counter

syslen = len(sys.argv)
if(syslen != 4):
    print("ERROR: .py [input] [test] [train_feature]")
    exit()

fdata = open(sys.argv[1], "r")
fsvm = open(sys.argv[2], "r")
ff = open(sys.argv[3], "w")

for line in fdata:
    wrongPosition = []
    words = re.findall("(\S+)#(\S+)", line)
    for i in range(len(words)):
        result = fsvm.readline().strip()
        if result == 1:
            wrongPosition.append(i+1)
    # find longest wrong sequence
    print wrongPosition
    for i in range(len(words)):
        if 
        
    
    
    
