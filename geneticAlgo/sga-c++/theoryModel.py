import math

with open("theory.dat", "w") as theoryFile:
    for i in range(10):
        n = (i+1)*50
        theoryFile.write("%d %f\n" %(n, 2.7257*math.sqrt(n)))
