import sys
import re
from collections import Counter

# add a feature of bigram probability 

syslen = len(sys.argv)
if(syslen != 6):
    print("ERROR: .py [input] [test] [train_feature(POS)] [train_bigramcounter] [train_unigramcounter]")
    exit()

fin = open(sys.argv[1], "r")
fout = open(sys.argv[2], "w")
ff = open(sys.argv[3], "r")

def get_feature_additional(sentence, index):
    s1pre = ""
    if index-2 <= -1:
        if index-2 < -1:
            s1 = ""
        else:
            s1pre = "START"
            s1 = "START" + sentence[index][1]
    else:
        s1 = sentence[index-2][1] + sentence[index][1]
        s1pre = sentence[index-2][1]
    if index+2 >= len(sentence):
        if index+2 > len(sentence):
            s2 = ""
        else:
            s2 = sentence[index][1] + "END"
    else:
        s2 = sentence[index][1] + sentence[index+2][1]
    return s1, s2, s1pre

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

instances = []
def add_feature(sentence, index):
    s1, s2= get_feature(sentence, index)
    s3, s4, s3pre = get_feature_additional(sentence, index)
    prePOS = "START" if (index-1 < 0) else sentence[index-1][1]
    instances.append([None, s1, s2, index, prePOS, sentence[index][1], s3, s4])

pair = []
for line in fin:
    wrongWords = re.findall("(\S+)#(\S+)", line)
    for i in range(len(wrongWords)): # use correct for main, if correct word and wrong word are different, change to wrong
        add_feature(wrongWords, i) # always add correct bigram

label = []
featureNum = 0
for features in ff:
    bi = re.findall("(\S+):(\S+)", features)
    label.append(bi[0][0])
    featureNum += 1

# load bigram and unigram
tcounter = Counter()
unigramCounter = Counter()
bf = open(sys.argv[4], "r")
uf = open(sys.argv[5], "r")
for features in bf:
    bi = re.findall("(\S+) (\S+)", features)
    tcounter[bi[0][0]] = int(bi[0][1])
for features in uf:
    bi = re.findall("(\S+) (\S+)", features)
    unigramCounter[bi[0][0]] = int(bi[0][1])

# write features
count = 0
for d in instances:
    fout.write("0 ")
    unknown = featureNum + 1
    if d[1] in label:
        i1 = label.index(d[1])+1
    else:
        i1 = unknown
    if d[2] in label:
        i2 = label.index(d[2])+1
    else:
        i2 = unknown
    feateaureIndexs = Counter([i1, i2])
    
    if d[6] != "":
        if d[6] in label:
            i6 = label.index(d[6])+1
        else:
            i6 = unknown
        feateaureIndexs[i6] = feateaureIndexs[i6] + 1
    if d[7] != "":
        if d[7] in label:
            i7 = label.index(d[7])+1
        else:
            i7 = unknown
        feateaureIndexs[i7] = feateaureIndexs[i7] + 1
            
    from operator import itemgetter
    feateaureIndexslist = sorted(feateaureIndexs.items(), key=itemgetter(0))
    for f in feateaureIndexslist:
        fout.write(str(f[0]) + ":" + str(f[1]) + " ")

    p = ":%f " % (float(tcounter[d[2]])*tcounter[d[1]]*100/unigramCounter[d[5]]/unigramCounter[d[4]]) #thispos
    fout.write(str(featureNum+2) + p)    
    fout.write("\n")
        
fin.close()
fout.close()
ff.close()
