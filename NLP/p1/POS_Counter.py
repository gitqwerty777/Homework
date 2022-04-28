import sys
import re
from collections import Counter

if(len(sys.argv) != 3):
    print("ERROR: usage: POS_Counter.py <tagfile> <countfile>")
    exit()

nin = sys.argv[1]
fin = open(nin, "r")

total_counter = Counter()
for sin in fin:
    lasttag = ""
    sout = re.findall("(\S+)#(\S+)", sin)
    for s in sout:
        if lasttag == "":
            lasttag = s[1]
            continue
        total_counter[lasttag+s[1]] = total_counter[lasttag+s[1]] + 1
        lasttag = s[1]

#medium file
feature_num = len(total_counter.most_common())+1 # include unknown
labellist = list()
labelfilename = "label.txt"
flabel = open(labelfilename, "w")
for pair in total_counter.most_common():
    labellist.append(pair[0])
    flabel.write(pair[0] + '\n')
flabel.close()
    
#count all label as feature
trainlabelname = "train_label.txt"
ftrain = open(trainlabelname, "r")
fin.seek(0)
nout = sys.argv[2]
fout = open(nout, "w")
for sin in fin:
    lasttag = ""
    sout = re.findall("(\S+)#(\S+)", sin)
    svmCounter = Counter(labellist)
    for s in sout:
        if lasttag == "":
            lasttag = s[1]
            continue
        newtag = lasttag+s[1]
        if newtag in labellist:
            svmCounter[newtag] = svmCounter[newtag] + 1
        else:
            svmCounter['UNK'] = svmCounter['UNK'] + 1
        lasttag = s[1]
    svmCounter = svmCounter - Counter(labellist) # all count = 1 when initial, subtract it

    tags = svmCounter.most_common()
    tagname = [tag[0] for tag in tags]
    taglen = len(tags)

    svmValue = list()
    for label in labellist:
        if label in tagname:
            tindex = tagname.index(label)
            lindex = labellist.index(label) + 1
            svmValue.append([lindex ,float(tags[tindex][1])/taglen])
    svmValue.append([feature_num,svmCounter['UNK']])

    flabel = ftrain.readline().strip()
    fout.write(flabel + " ")
    for v in svmValue:
        s = str(v[0]) + ":" + str(v[1]) + " "
        fout.write(s)
    fout.write("\n")

fin.close()
fout.close()
