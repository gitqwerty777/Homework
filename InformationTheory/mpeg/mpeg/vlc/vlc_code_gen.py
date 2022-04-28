gen_dir = "./gen/"
f = open("vlc_name")
for line in f:
    vlc_in = line.strip()
    fin = open(vlc_in)
    fout = open(gen_dir+vlc_in,"w")
    fout.write('{\n')
    for l in fin:
        tmp = l.strip().split()
        fout.write('\t{b2i("%s"),{b2l("%s")' % (tmp[0],tmp[0])) 
        for i in range(1,len(tmp)):
            fout.write(', ')
            fout.write(tmp[i])
        fout.write('}},\n')
    fout.write('}\n')
        
