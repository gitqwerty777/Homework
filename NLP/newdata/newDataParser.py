import re
import sys

if len(sys.argv) != 3:
    print ".py [input] [output]"
    exit()

fin = open(sys.argv[1], "r")
fout = open(sys.argv[2], "w")

types = ["ESSAY", "TEXT", "SENTENCE",  "MISTAKE",  "TYPE", "CORRECTION"]
# what we want: sentence id and string, mistake id, mistake type, correction
w = dict()
c = dict()
t = dict()
nowcorrectionID = ""

def parseType(sentence):
    global nowcorrectionID
    p = re.match("^<([^\s>]+)", sentence)
    if p == None:
        return ""
    for i in range(len(types)):
        if p.groups()[0] == types[i]:
            if i == 2: #sentence id , sentence value
                p = re.match("^<(\S+) id=\"(\S+)\">(\S+)<", sentence)
                w[p.groups()[1]] = p.groups()[2]
            elif i == 3: # mistake id
                p = re.match("^<(\S+) id=\"(\S+)\">", sentence)
                nowcorrectionID = p.groups()[1]
            elif i == 4 or i == 5: # mistake type or mistake correction
                p = re.match("^<(\S+)>(\S+)<", sentence)
                if i == 4:
                    t[nowcorrectionID] = p.groups()[1]
                else:
                    c[nowcorrectionID] = p.groups()[1]

for line in fin:
    parseType(line)

for k in w.keys():
    if t[k] == "Redundant":
        s = c[k] + " " + w[k] + "\n"
    elif t[k] == "Missing":
        s = c[k] + " " + w[k] + "\n"
    else:
        continue
    print s
    fout.write(s)
    
fin.close()
fout.close()
