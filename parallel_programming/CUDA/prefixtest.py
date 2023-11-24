with open("4input", "w") as prefixtest:
    for i in range(100000):
        prefixtest.write("10000005 %d\n" % i)
