import sys
import os
import re
import math
from preProcess import WordList
from collections import Counter

class EMClassifier:
    def __init__(self):
        self.loadWordCount()

    def loadWordCount(self):
        self.wordList = WordList()
        self.labels = self.wordList.read()

    """
    maximize likelihood of L(document | model)
    """
    def trainModel(self):
        # each word has multiple probability for different topicsy
        self.wordTopicProbability = {}
        self.wordLambda = [0.5 for label in self.labels] # TODO: sum = 1?

        previousLambda = 0
        while abs(previousLambda - self.wordLambda) < 0.001:
            self.expectation()
            self.maximization()

    def expectation(self):
        """
        P(t1 | w) = lambda * p(w|t1) / (lambda * p(w|t1)
        + (1-lambda) * p(w|t2) ...)
        """
        for word in self.wordList.words: # TODO: wordlist.words
            for label in self.labels:
                # self.wordTopicProbability[word][topic]


    def maximization(self):
        pass

    def labelDocument(self, content):
        labelProbabilities = Counter()
        for label in self.labels:
            labelProbability = self.wordList.labelFrequency[label]
            words = re.findall("[\w']+", content)
            for word in words:
                if word.isdigit() or len(word) <= 1:
                    continue
                wordFrequency = float(self.wordList.dict[label][word])/self.wordList.totalWordNumPerLabel[label]
                if wordFrequency == 0.0:  # using log
                    wordFrequency = -10  # TODO: other normalize function
                else:
                    wordFrequency = math.log(wordFrequency)
                labelProbability += wordFrequency
            # print "total probability = %.3e" % labelProbability
            labelProbabilities[label] = labelProbability
        return labelProbabilities.most_common(1)[0][0]

if __name__ == "__main__":
    emClassifier = EMClassifier()

    predictLabel = {}
    testDirectoryPath = "./20news/Test"
    print('testDirectoryPath (absolute) = ' + os.path.abspath(testDirectoryPath))
    for root, subdirs, files in os.walk(testDirectoryPath):
        for filename in files:
            file_path = os.path.join(root, filename)
            print "process test file:", file_path
            with open(file_path, 'rb') as f:
                f_content = f.read()
                label = emClassifier.labelDocument(f_content)
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
