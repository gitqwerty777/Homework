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
        unigramCounter[s3pre] = unigramCounter[s3pre] + 1
    if s4 != "":
        tcounter[s4] = tcounter[s4] + 1
        unigramCounter[sentence[index][1]] = unigramCounter[sentence[index][1]] + 1
            
    prePOS = "START" if (index-1 < 0) else sentence[index-1][1]
    unigramCounter[prePOS] = unigramCounter[prePOS] + 1
    unigramCounter[sentence[index][1]] = unigramCounter[sentence[index][1]] + 1
    instances.append([label, s1, s2, index, prePOS, sentence[index][1], s3, s4])
    

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

## write bigram and unigram
bf = open("train_bigram.txt", "w")
uf = open("train_unigram.txt", "w")
for f in tcounter:
    print f
    bf.write("%s %d\n" % (f, tcounter[f]))
for f in unigramCounter:
    uf.write("%s %d\n" % (f, unigramCounter[f]))
    
# write each word with features to output file
count = 0
for d in instances:
    if d[0] == True:
        fout.write("0 ")
    else:
        fout.write("1 ")

    feateaureIndexs = Counter([label.index(d[1])+1, label.index(d[2])+1])
    if d[6] != "":
        feateaureIndexs[label.index(d[6])+1] = feateaureIndexs[label.index(d[6])+1] + 1
    if d[7] != "":
        feateaureIndexs[label.index(d[7])+1] = feateaureIndexs[label.index(d[7])+1] + 1
    from operator import itemgetter
    feateaureIndexslist = sorted(feateaureIndexs.items(), key=itemgetter(0))
    for f in feateaureIndexslist:
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
