import random

f = open('input2', 'w')

f.write("100\n")
for i in range(101):
    f.write(str(random.randint(1, 100)) + " ")
f.write("\n")
for i in range(100):
    f.write("0 ")
f.close()
