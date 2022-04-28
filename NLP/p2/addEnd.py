# encoding: utf-8
fin = open("p2train_seg.txt", "r")
fout = open("p2train-sentence.txt", "w")
for line in fin:
    fout.write(line.strip() + ".\n")
    
