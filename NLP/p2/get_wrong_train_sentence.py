f = open("train_tagged.txt", "r")
fout = open("train_wrong_tagged.txt", "w")

i = 0
for line in f:
    if i % 2 == 1:
        i += 1
        continue
    fout.write(line)
    i += 1
