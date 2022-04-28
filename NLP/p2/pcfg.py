from nltk.grammar import PCFG as pcfg



p = pcfg.fromstring("S NP VP PP DEG NN")

print p

