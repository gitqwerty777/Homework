import sys
import re
from collections import Counter

# this is 

syslen = len(sys.argv)

if(syslen < 4 or sys.argv[1] != "-train" and sys.argv[1] != "-test"):
    print("ERROR: usage: [-train/-test] <tagfile> <featurefile> <label(optional)>")
    exit()

nin = sys.argv[2]
fin = open(nin, "r")

total_counter = Counter()
labelpath = sys.argv[4]b
flabel = open(labelpath, "r")
for sin in fin:
    nowlabel = flabel.readline()
    lasttag = "START"
    sout = re.findall("(\S+)#(\S+)", sin)
    for s in sout:
        total_counter[lasttag+s[1]] = total_counter[lasttag+s[1]] + 1
        lasttag = s[1]
    total_counter[lasttag+"END"] = total_counter[lasttag+"END"] + 1

flabel.close()
#medium file
toption = sys.argv[1]
labellist = list()
labelfilename = "trainfeatures_correct.txt"
if toption == "-train":
    flabel = open(labelfilename, "w")
    print(total_counter.most_common())
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
for sin in fin:
    lasttag = "START"
    sout = re.findall("(\S+)#(\S+)", sin)
    sout.append(["...", "END"])
    for s in sout:
        if(s[1] == "END"):
            break
        svmCounter = Counter(labellist)
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

    #add label by <labelfile>
        if syslen == 5:
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
