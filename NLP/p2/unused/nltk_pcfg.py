from itertools import islice
import nltk

productions = []
S = nltk.Nonterminal('S')
for tree in nltk.corpus.treebank.parsed_sents('wsj_0002.mrg'):
    print(tree)
    t = tree.productions()
    for i in t:
        print(i)
    productions += tree.productions()

grammar = nltk.induce_pcfg(S, productions)
for production in grammar.productions()[:10]:
    print production
