import sys
import enumerate

syslen = len(sys.argv)
if(syslen != 4):
    print("ERROR: .py [3labeltexts]")
    exit()

f1 = open(sys.argv[1], "r")
f2 = open(sys.argv[2], "r")
f3 = open(sys.argv[3], "r")

priority = [1, 1, 1]

#normalize
total = 0
for p in priority:
    total += p
for i in range(len(priority)):
    priority[i] /= float(total)

#predicted_label:
for l1, l2, l3 in f1, f2, f3:
    predict_label = []
    s1 = re.findall("(\S+) (\S+)", l1)
    start = int(s1[0][0])
    end = int(s1[0][1])

    common = []
    for i in range(pstart, pend+1, 1):
        predict_label.append(i)
    



