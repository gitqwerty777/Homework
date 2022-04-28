import sys
import re

if(len(sys.argv) != 7):
    print("ERROR:\n usage:\n\"parsedata.py -n <totalcolumnnum> -p <parseline> <original data> <parsed data>\"\n\n\t p.s. parseline: start from 1")
    exit()

totalcnum = sys.argv[2]
    
plist = list()
for i in sys.argv[4]:
    plist.append(i)
plist.sort()
    
nin = sys.argv[5]
nout = sys.argv[6]
fin = open(nin, "r")
fout = open(nout, "w")

for sin in fin: # line
    sformat = "\s*"
    for i in range(int(totalcnum)):
        sformat = sformat + "(\S+)\s*"

    print(sformat)
    sout = re.findall(sformat, sin)

    first = True
    for i in plist:
        i = int(i)
        if first:
            first = False
        else:
            fout.write("\n")
        fout.write(sout[0][i-1])

    fout.write("\n")

fin.close()
fout.close()
