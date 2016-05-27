import os
import re
from collections import Counter

class WordList:
    def __init__(self):
        """first layer = label, second layer = wordcounts"""
        self.dict = {}  # two layer dict
        self.stopList = []
        with open("stopList.en_US.u8", "r") as stopListFile:
            stopContent = stopListFile.read()
            self.stopList = stopContent.split("\n")

    def addCount(self, label, counter):
        if label not in self.dict:
            self.dict[label] = Counter()
        self.dict[label] += counter

    def printf(self):
        for i, counter in self.dict.iteritems():
            print counter.most_common(10)

    def pruneStopList(self):
        for label, counter in self.dict.iteritems():
            for word in self.stopList:
                del counter[word]

    def save(self):
        with open("label.txt", "w") as labelFile:
            for label, counter in self.dict.iteritems():
                labelFile.write(label+"\n")

        for label, counter in self.dict.iteritems():
            with open("./wordCount/"+label+".txt", "w") as wordCountFile:
                wordCountFile.write("%d\n" % len(counter.most_common()))
                for word, count in counter.most_common():
                    wordCountFile.write("%s %d\n" % (word, count))


def processContent(content, label, wordList):
    counter = Counter()
    words = re.findall(r"[\w']+", content)
    for word in words:
        word = word.lower()
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
