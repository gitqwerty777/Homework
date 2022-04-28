import sys
import re
from collections import Counter

syslen = len(sys.argv)
if(syslen != 6):
    print("ERROR: .py [input(original tag)] [input(output of words)] [output(answer)]")
    exit()

fin = open(sys.argv[2], "r")
fresult = open(sys.argv[2], "r")
fout = open(sys.argv[3], "w")

for sentence in fin:
    result = fresult.readline()
    words = re.findall("(\S+)#(\S+)", sentence)
    sentence_len = 0
    wrongList = []
    for w in words:
        if result == "1": # wrong
            for i in range(sentence_len, sentence_len+w[0], 1):
                wrongList.append(i)
        sentence_len += len(w[0])
    print(sentence)
    print(wrongList)
