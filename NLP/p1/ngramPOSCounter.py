import sys
import re
from collections import Counter

# this is 

syslen = len(sys.argv)

if(syslen < 4 or sys.argv[1] != "-train" and sys.argv[1] != "-test"):
    print("ERROR: usage: ngramPOSCounter.py [-train/-test] <tagfile> <featurefile> <label(optional)>")
    exit()

nin = sys.argv[2]
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
toption = sys.argv[1]
labellist = list()
labelfilename = "trainfeatures.txt"
if toption == "-train":
    flabel = open(labelfilename, "w")
    for pair in total_counter.most_common():
        labellist.append(pair[0])
        flabel.write(pair[0] + '\n')
elif toption == "-test":
    flabel = open(labelfilename, "r")
    for l in flabal:
        labellist.append(l)

feature_num = len(labellist)+1 # include unknown
flabel.close()

#count all label as feature
nout = sys.argv[3]
if syslen == 5:
    trainlabelname = sys.argv[4]
    ftrain = open(trainlabelname, "r")

fin.seek(0)
fout = open(nout, "w")
checklist = list()

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

    #new: bigram of pos
    svmValue = list()
    bvalue = 1
    for label in labellist:
        if label in tagname:
            tindex = tagname.index(label)
            lindex = labellist.index(label) + 1
            bvalue *= float(tags[tindex][1])* 1000 / total_counter[label]
            svmValue.append([lindex ,tags[tindex][1]])
    svmValue.append([feature_num,svmCounter['UNK']])
    svmValue.append([feature_num+1,bvalue])

    #add label by <labelfile>
    if syslen == 5:
        flabel = ftrain.readline().strip()
        fout.write(flabel + " ")
        print "bvalue = %f, label = %s" % (bvalue, flabel)
        checklist.append([bvalue, flabel])
    else:
        fout.write("0 ")
    for v in svmValue:
        s = str(v[0]) + ":" + str(v[1]) + " "
        fout.write(s)
    fout.write("\n")

checklist.sort()
for c in checklist:
    print(c)
    
fin.close()
fout.close()
