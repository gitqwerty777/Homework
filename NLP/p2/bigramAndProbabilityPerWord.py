import sys
import re
from collections import Counter

# add a feature of bigram probability 

syslen = len(sys.argv)
if(syslen != 4):
    print("ERROR: .py [input] [test] [train_feature]")
    exit()

fin = open(sys.argv[1], "r")
fout = open(sys.argv[2], "w")
ff = open(sys.argv[3], "w")

def get_word(sentence, index):
    if index == 0:
        s = "<s>" + sentence[index][0]
    elif index == len(sentence):
        s = sentence[index-1][0] + "<\s>"
    else:
        s = sentence[index-1][0] + sentence[index][0]
    return s

def get_feature(sentence, index):
    if index-1 == -1:
        s1 = "START" + sentence[index][1]
    else:
        s1 = sentence[index-1][1] + sentence[index][1]
    if index+1 == len(sentence):
        s2 = sentence[index][1] + "END"
    else:
        s2 = sentence[index][1] + sentence[index+1][1]

    return s1, s2

tcounter = Counter()
instances = []
def add_feature(sentence, index, label):
    s1, s2 = get_feature(sentence, index)
    tcounter[s1] = tcounter[s1]+1
    tcounter[s2] = tcounter[s2]+1
    instances.append([label, s1, s2, index])

pair = []
for line in fin:
    pair.append(line)
    if len(pair) == 1:
        continue
    else:
        wrongWords = re.findall("(\S+)#(\S+)", pair[0])
        correctWords = re.findall("(\S+)#(\S+)", pair[1])
        wrongOffset = 0
        for i in range(len(correctWords)): # use correct for main, if correct word and wrong word are different, change to wrong
            add_feature(correctWords, i, True) # always add correct bigram
            if i < len(wrongWords) and i+wrongOffset < len(wrongWords) and correctWords[i][1] != wrongWords[i+wrongOffset][1]: #wrong section
                if i+wrongOffset >= len(wrongWords): # iterate wrong to the end
                    break
                if i < len(correctWords):
                    nextCorrectWord = correctWords[i][1]
                else:
                    nextCorrectWord = ""
                while nextCorrectWord != wrongWords[i+wrongOffset][1]:
                    add_feature(wrongWords, i+wrongOffset, False)
                    wrongOffset = wrongOffset +1
                    if i+wrongOffset >= len(wrongWords):
                        break
        pair = []

# save features

label = []
totalWords = 0
featureNum = len(tcounter.most_common())
for bi in tcounter.most_common():
    ff.write(bi[0] + ":" + str(bi[1]) + "\n")
    label.append(bi[0])
    totalWords = totalWords + bi[1]
        
# write each word with features to output file
count = 0
for d in instances:
    if d[0] == True:
        fout.write("0 ")
    else:
        fout.write("1 ")

    ai, bi = label.index(d[1])+1, label.index(d[2])+1
    if ai > bi:
        i = ai
        ai = bi
        bi = i

    if ai == bi:
        fout.write(str(ai) +":2 ")
    else:
        fout.write(str(ai) +":1 ")
        fout.write(str(bi) +":1 ")

        #print("totalwords = %d" % totalWords)
    #print("counter: a: %d b: %d" % tcounter[ai], )
    ap = ":%f" % (float(tcounter[d[1]])*100/totalWords) + " "
    bp = ":%f" % (float(tcounter[d[2]])*100/totalWords) + " "
    fout.write(str(featureNum+1) + ap)
    fout.write(str(featureNum+2) + bp)
    fout.write("\n")
        
fin.close()
fout.close()
ff.close()
