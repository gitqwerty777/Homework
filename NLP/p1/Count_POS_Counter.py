import sys
import re
from collections import Counter

syslen = len(sys.argv)

if(syslen < 3):
    print("ERROR: usage: POS_Counter.py <tagfile> <label(optional)> <countfile>")
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
if syslen == 3:
    nout = sys.argv[2]
else:
    trainlabelname = sys.argv[2]
    ftrain = open(trainlabelname, "r")
    nout = sys.argv[3]

fin.seek(0)
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

    svmValue = list()
    for label in labellist:
        if label in tagname:
            tindex = tagname.index(label)
            lindex = labellist.index(label) + 1
            svmValue.append([lindex ,tags[tindex][1]])
    svmValue.append([feature_num,svmCounter['UNK']])

    if syslen == 4:
        flabel = ftrain.readline().strip()
        fout.write(flabel + " ")
    else:
        fout.write("0 ")
    for v in svmValue:
        s = str(v[0]) + ":" + str(v[1]) + " "
        fout.write(s)
    fout.write("\n")

fin.close()
fout.close()
