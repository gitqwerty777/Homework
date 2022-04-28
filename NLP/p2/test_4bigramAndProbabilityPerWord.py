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
ff = open(sys.argv[3], "r")

def get_word(sentence, index):
    if index == 0:
        s = "<s>" + sentence[index][0]
    elif index == len(sentence):
        s = sentence[index-1][0] + "<\s>"
    else:
        s = sentence[index-1][0] + sentence[index][0]
    return s

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

tcounter = Counter()
unigramCounter = Counter()
instances = []
def add_feature(sentence, index, label):
    s1, s2 = get_feature(sentence, index)
    tcounter[s1] = tcounter[s1]+1
    tcounter[s2] = tcounter[s2]+1
    s3, s4, s3pre = get_feature_additional(sentence, index)
    if s3 != "":
        tcounter[s3] = tcounter[s3] + 1
    if s4 != "":
        tcounter[s4] = tcounter[s4] + 1
            
    prePOS = "START" if (index-1 < 0) else sentence[index-1][1]
    instances.append([label, s1, s2, index, prePOS, sentence[index][1], s3, s4])
    

pair = []
for line in fin:
    wrongWords = re.findall("(\S+)#(\S+)", line)
    for i in range(len(wrongWords)):
        add_feature(wrongWords, i, True)

# read features
label = []
totalWords = 0
featureNum = len(tcounter.most_common())
for line in ff:
    bi = re.findall("(\S+):(\S+)", line)
    label.append(bi[0][0])
    totalWords = totalWords + int(bi[0][1])

## write bigram and unigram
tcounter = Counter()
unigramCounter = Counter()
bf = open("train_bigram.txt", "r")
uf = open("train_unigram.txt", "r")
for line in bf:
    line = re.findall("(\S+)\s+(\S+)", line)
    line = line[0]
    tcounter[line[0]] = int(line[1])
for line in uf:
    line = re.findall("(\S+) (\S+)", line)
    line = line[0]
    unigramCounter[line[0]] = int(line[1])
    
# write each word with features to output file
count = 0
for d in instances:
    if d[0] == True:
        fout.write("0 ")
    else:
        fout.write("1 ")

    featuresIndexs = Counter()
    if d[1] in label:
        featuresIndexs[label.index(d[1])+1] += 1
    if d[2] in label:
        featuresIndexs[label.index(d[2])+1] += 1
    if d[6] != "" and d[6] in label:
        featuresIndexs[label.index(d[6])+1] = featuresIndexs[label.index(d[6])+1] + 1
    if d[7] != "" and d[7] in label:
        featuresIndexs[label.index(d[7])+1] = featuresIndexs[label.index(d[7])+1] + 1
    from operator import itemgetter
    featuresIndexslist = sorted(featuresIndexs.items(), key=itemgetter(0))
    for f in featuresIndexslist:
        fout.write(str(f[0]) + ":" + str(f[1]) + " ")

        #print "this pos is " + d[4]
    '''p1 = ":%f " % (float(tcounter[d[1]])*100/unigramCounter[d[4]]) #prepos
    p2 = ":%f " % (float(tcounter[d[2]])*100/unigramCounter[d[5]]) #thispos
    fout.write(str(featureNum+1) + p1)
    fout.write(str(featureNum+2) + p2)'''

    p = ":%f " % (float(tcounter[d[2]])*tcounter[d[1]]*100/unigramCounter[d[5]]/unigramCounter[d[4]]) #thispos
    fout.write(str(featureNum+1) + p)    
    fout.write("\n")
        
fin.close()
fout.close()
ff.close()
