import sys
import os
import re
import math
import Stemmer
import argparse
from preProcess import WordList
from collections import Counter


class NaiveBayesClassifier:
    def __init__(self):
        self.loadWordCount()
        self.stemmer = Stemmer.Stemmer('english')

    def loadWordCount(self):
        self.wordList = WordList()
        self.labels = self.wordList.read()
        # print "labels", self.labels

    """
    P(document | label; model) * P(label | model) = P(label | model) * mul(P(words in document | label; model))
    TODO: english segment system
    """
    def labelDocument(self, content):
        labelProbabilities = Counter()
        for label in self.labels:
            labelProbability = self.wordList.labelFrequency[label]
            words = re.findall("[\w']+", content)
            for word in words:
                if word.isdigit() or len(word) <= 1:
                    continue
                """
                try:
                    word = self.stemmer.stemWord(word)
                except:
                    pass
                """
                wordFrequency = float(self.wordList.dict[label][word])/self.wordList.totalWordNumPerLabel[label]
                # print "freq [%s] = %f" % (word, wordFrequency)
                if wordFrequency == 0.0:  # using log
                    #wordFrequency = -10  # TODO: other normalize function
                    #wordFrequency = float(0.1)/self.wordList.totalWordNumPerLabel[label]
                    #wordFrequency = math.log(wordFrequency)
                    #good turing
                    
                else:
                    wordFrequency = math.log(wordFrequency)
                labelProbability += wordFrequency
            # print "total probability = %.3e" % labelProbability
            labelProbabilities[label] = labelProbability
        return labelProbabilities.most_common(1)[0][0]

    def __

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--inputDirectory")
    parser.add_argument("-o", "--outputFileName")
    parser.add_argument("-n", "--labelDataSize")
    args = parser.parse_args()
    print "--- args\n"
    print "i:", args.inputDirectory
    print "o:", args.outputFileName
    print "n:", args.labelDataSize
    print "---"

    naiveBayesClassifier = NaiveBayesClassifier()

    predictLabel = {}
    arg.inputDirectory = "./20news/Test"
    print('arg.inputDirectory (absolute) = ' + os.path.abspath(arg.inputDirectory))
    for root, subdirs, files in os.walk(arg.inputDirectory):
        for filename in files:
            file_path = os.path.join(root, filename)
            print "process test file:", file_path
            with open(file_path, 'rb') as f:
                f_content = f.read()
                label = naiveBayesClassifier.labelDocument(f_content)
                print "label = ", label
                print "filename", filename
                predictLabel[int(filename)] = label

    with open("predictLabel.txt", "w") as predictFile:
        for filename, label in predictLabel.iteritems():
            predictFile.write("%d %s\n" % (filename, label))

    rf = open("ans.test", "r")
    realLabel = rf.read().split("\n")
    del realLabel[-1]
    realLabels = {}
    for labelpairs in realLabel:
        labelpairs = labelpairs.split(" ")
        filename = labelpairs[0]
        label = labelpairs[1]
        realLabels[int(filename)] = label

    testcount = 0
    testcorrect = 0
    for filename, label in realLabels.iteritems():
        testcount += 1
        if predictLabel[filename] == label:
            testcorrect += 1
    print "total %d correct %d rate %f" % (testcount, testcorrect, float(testcorrect)/testcount)
