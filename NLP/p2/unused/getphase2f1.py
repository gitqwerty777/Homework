import re

def F1_phase2(ftrue, fpredict):
    total = [0]*2
    linenum = 0
    for line in ftrue:
        ty = re.findall("(\S+) (\S+)", line)
        start = int(ty[0][0])
        end = int(ty[0][1])
        pv = fpredict.readline().strip()
        pv = re.findall("(\S+) (\S+)", pv)
        pstart = int(pv[0][0])
        pend = int(pv[0][1])
        predict_label = []
        common = []
        for i in range(pstart, pend+1, 1):
            predict_label.append(i)
        for i in range(start, end+1, 1):
            if i in predict_label:
                common.append(i)
        recall = float(len(common))/(end-start+1)
        prec = float(len(common))/len(predict_label)
        total[0] += recall
        total[1] += prec
        linenum += 1
    total[0] /= linenum
    total[1] /= linenum
    f1 = 2 * total[0] * total[1] / (total[0] + total[1])
    print "F1 = %f" % f1

ftrue = open("train_ans.txt", "r")
fpredict = open("all4wrongnormallabel.txt", "r")
F1_phase2(ftrue, fpredict)
