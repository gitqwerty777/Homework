import os
import re
import copy
import Stemmer
from collections import Counter


class WordList:
    def __init__(self):
        """first layer = label, second layer = wordcounts"""
        self.dict = {}  # two layer dict
        self.totalWordNumPerLabel = {}
        self.stopList = []
        self.labelCount = Counter()
        with open("stopList.en_US.u8", "r") as stopListFile:
            stopContent = stopListFile.read()
            self.stopList = stopContent.split("\n")

    def addCount(self, label, counter):
        if label not in self.dict:
            self.dict[label] = Counter()
        self.dict[label] += counter
        self.labelCount[label] += 1

    def printf(self):
        for i, counter in self.dict.iteritems():
            print counter.most_common(10)

    def pruneStopList(self):
        for label, counter in self.dict.iteritems():
            for word in self.stopList:
                del counter[word]
        # TODO: if key is number, delete it

    def read(self):
        with open("label.txt", "r") as labelFile:
            labels = (labelFile.read()).split("\n")
            del labels[-1]

        totalCount = 0
        with open("labelCount.txt", "r") as labelCountFile:
            labelCount = (labelCountFile.read()).split("\n")
            del labelCount[-1] # trailing newline
            labelCount = [labelcount.split(" ") for labelcount in labelCount]
            for labelCountPair in labelCount:
                label = labelCountPair[0]
                count = labelCountPair[1]
                self.labelCount[label] = int(count)
                totalCount += int(count)
        self.labelFrequency = copy.deepcopy(self.labelCount)
        for label, count in self.labelFrequency.iteritems():
            self.labelFrequency[label] /= float(totalCount)
        # print self.labelFrequency
        # print self.labelCount
        
        wordCountPath = "./wordCount/"
        for label in labels:
            
            file_path = wordCountPath + label + ".txt"
            # print "file_path = ", file_path
            with open(file_path, 'rb') as f:
                wordContent = f.read()
                wordContent = wordContent.split("\n")
                wordContent = [content.split(" ") for content in wordContent]

            counter = Counter()
            totalWordCount = 0
            for keyvaluepair in wordContent:
                if(len(keyvaluepair) != 2):
                    continue
                key = keyvaluepair[0]
                value = keyvaluepair[1]
                counter[key] = int(value)
                totalWordCount += int(value)
            self.totalWordNumPerLabel[label] = totalWordCount
            self.dict[label] = counter
            
        return labels

    def save(self):
        with open("label.txt", "w") as labelFile:
            for label, counter in self.dict.iteritems():
                labelFile.write(label+"\n")

        with open("labelCount.txt", "w") as labelCountFile:
            for label, count in self.labelCount.most_common():
                labelCountFile.write("%s %d\n" % (label, count))
                
        for label, counter in self.dict.iteritems():
            with open("./wordCount/"+label+".txt", "w") as wordCountFile:
                for word, count in counter.most_common():
                    wordCountFile.write("%s %d\n" % (word, count))


def processContent(content, label, wordList):
    counter = Counter()

    stemmer = Stemmer.Stemmer('english')

    words = re.findall("[\w']+", content)
    for word in words:
        if word.isdigit() or len(word) <= 1:
            continue
        word = word.lower()
        """
        try:
            word = stemmer.stemWord(word)
        except:
            pass
        """
        counter[word] += 1
    wordList.addCount(label, counter)
    
if __name__ == "__main__":
    wordList = WordList()
    trainDirectoryPath = "./20news/Train"

    print('trainDirectoryPath (absolute) = ' + os.path.abspath(trainDirectoryPath))

    for root, subdirs, files in os.walk(trainDirectoryPath):
        label = root.rsplit("/")[-1]
        # print('--\nroot = %s, label = %s' % (root, label))
        for filename in files:
            file_path = os.path.join(root, filename)
            # print('\t- file %s (full path: %s)' % (filename, file_path))
            with open(file_path, 'rb') as f:
                f_content = f.read()
                # list_file.write(('The file %s contains:\n' % filename).encode('utf-8'))
                # list_file.write(f_content)
                # list_file.write(b'\n')
                processContent(f_content, label, wordList)

    wordList.pruneStopList()
    wordList.printf()
    wordList.save()
