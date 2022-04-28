import sys, random

# generate long long random numbers
if len(sys.argv) != 3:
    print("usage: python randomHashGenerator.py [generate amount] [fileName]")
    exit(0)

num = int(sys.argv[1])
fileName = sys.argv[2]

hashSet = []

f = open(fileName, "w")
for i in range(num):
    randnum = random.getrandbits(63)-1
    while randnum in hashSet or randnum == 0:
        randnum = random.getrandbits(63)-1
        print "conflict"
    hashSet.append(randnum)
    f.write(str(randnum))
    f.write("\n")
f.close()
