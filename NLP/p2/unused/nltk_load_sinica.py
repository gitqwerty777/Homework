from nltk.corpus import sinica_treebank
import nltk
import codecs
#reader
#http://cpmarkchang.logdown.com/posts/184192-python-nltk-sinica-treebank

fout = codecs.open("sinica_word.txt", "w", "utf-8")
tw = sinica_treebank.sents()
for word in tw:
    for w in word:
        fout.write(w)
    fout.write("\n")
    
bi = nltk.bigrams(tw)
#for (a, b) in bi:


sinica_treebank.parsed_sents()[15].draw()








