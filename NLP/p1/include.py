
fin = open("p1testlabel.txt", "r")
finc = open("p1testpre.txt", "r")
fout = open("p1testlabels.txt", "w")

for s in fin:
    st = finc.readline()
    fout.write(st.strip())
    fout.write(" ")
    fout.write(s)

fout.close()
