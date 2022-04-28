# encoding:utf-8
import nltk
from nltk.tree import Tree
from nltk.grammar import induce_pcfg
import codecs

fin = codecs.open("parsep2train.txt", "r", 'utf-8')
productions = []
for treestr in fin:
    t = Tree.fromstring(treestr)
    productions += t.productions()
ROOT = nltk.Nonterminal('ROOT')
grammar = nltk.DependencyGrammar(productions)
pcfggrammar = nltk.grammar.induce_pcfg(ROOT, productions)
pcfgproductions = pcfggrammar.productions()
s = "他们 觉得 父母 的 行为 方式 和 价值 观念 太 不 老 了"
pcfg = nltk.grammar.PCFG(ROOT, pcfgproductions)
parser = nltk.ViterbiParser(pcfggrammar)
ans = parser.parse_all(nltk.word_tokenize(s))
print ans





