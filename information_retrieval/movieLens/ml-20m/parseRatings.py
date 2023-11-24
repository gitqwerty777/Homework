import csv
from collections import Counter
from gensim import corpora, models, similarities
import gensim, logging

            # rating movie tag link
readLocks = [False, True, False, True]

class RatingCounter:
    def __init__(self):
        self.ratingSum = 0
        self.ratingCount = 0

    def addRating(self, rating):
        self.ratingSum += rating
        self.ratingCount += 1


class DataAnalyzer:
    def __init__(self, data):
        self.data = data

    def calculateMovieRatings(self):
        # TODO: normalize score of user
        movieRating = {}
        for rating in self.data.ratings:
            # rating.uid
            if rating.mid not in movieRating:
                movieRating[rating.mid] = RatingCounter()
        self.movieRating = movieRating

    def readMovieTagRelation(self):
        self.movieTag = {}
        with open("movieTag.txt", "r") as movieTagFile:
            movieTags = movieTagFile.read().split("\n")
            del movieTags[-1]
            for movieTag in movieTags:
                movieTag = movieTag.split(",")
                mid = int(movieTag[0])
                tag = movieTag[1]
                count = int(movieTag[2])
                if mid not in self.movieTag:
                    self.movieTag[mid] = Counter()
                self.movieTag[mid][tag] = count

    def buildLSIModel():
        texts = [[] for i in range(138493)] #max uid
        for uid, tagCounter in iter(sorted(self.movieRating)):
            for tagcountpair in tagCounter.most_common():
                for i in range(int(tagcountpair[1])):
                    texts[uid].append(tagcountpair[0]) # mid

        dictionary = corpora.Dictionary(texts)
        dictionary.save('dictionary-rating.dict')

        corpus = [dictionary.doc2bow(text) for text in texts]
        corpora.MmCorpus.serialize('corpus-rating.mm', corpus)

        id2word = gensim.corpora.Dictionary.load('dictionary-rating.dict')
        mm = gensim.corpora.MmCorpus('corpus-rating.mm')
        print ("make lsimodel")
        N = 100
        #lsi = gensim.models.lsimodel.LsiModel(corpus=mm, id2word=id2word, num_topics=N)
        lsi = gensim.models.lsimodel.LsiModel(corpus=mm, id2word=id2word, num_topics=N)
        print ("lsi topic")
        lsi.print_topics(N)
        

    def saveMovieTagRelation(self):


        '''
        texts = [[] for i in range(131262)] #max mid
        for mid, tagCounter in iter(sorted(self.movieTag.iteritems())):
            for tagcountpair in tagCounter.most_common():
                for i in range(int(tagcountpair[1])):
                    texts[mid].append(tagcountpair[0])

        dictionary = corpora.Dictionary(texts)
        dictionary.save('dictionary2.dict')
        print dictionary
        dictionary.load("dictionary2.dict")
        print dictionary

        corpus = [dictionary.doc2bow(text) for text in texts]
        corpora.MmCorpus.serialize('corpus.mm', corpus)
        corpus = corpora.MmCorpus('corpus.mm')
        tfidf = models.TfidfModel(corpus)
        corpus_tfidf = tfidf[corpus]

        # read

        id2word = gensim.corpora.Dictionary.load('dictionary2.dict')
        mm = gensim.corpora.MmCorpus('corpus.mm')
        print "make lsimodel"
        N = 100
        #lsi = gensim.models.lsimodel.LsiModel(corpus=mm, id2word=id2word, num_topics=N)
        lsi = gensim.models.lsimodel.LsiModel(corpus=corpus_tfidf, id2word=id2word, num_topics=N)
        print "lsi topic"
        lsi.print_topics(N)
        '''
        
        with open("movieTag.txt", "w") as movieTagFile:
            for mid, tagCounter in iter(sorted(self.movieTag.iteritems())):
                for tagcountpair in tagCounter.most_common():
                    movieTagFile.write("%d,%s,%d\n" % (mid, tagcountpair[0], tagcountpair[1]))

    def calculateMovieTagRelation(self):
        movieTag = {}
        tagCount = Counter()
        for tag in self.data.tags:
            if tag.mid not in movieTag:
                movieTag[tag.mid] = Counter()
            movieTag[tag.mid][tag.tag] += 1
            tagCount[tag.tag] += 1
        #  TODO: word tokenize, stem
        for mid, tagCounter in movieTag.iteritems():
            for tagcountpair in tagCounter.most_common():
                tagname = tagcountpair[0]
                if tagCount[tagname] >= 2:
                    pass
                    #print "mid = %d, tag = %s, count = %d(%d)" % (mid, tagname, tagcountpair[1], tagCount[tagname])
                else:
                    del tagCounter[tagname]
            # print tagCounter
        self.movieTag = movieTag
        self.tagCount = tagCount


class DataReader:
    def __init__(self, ratingfilename, moviefilename, tagfilename, linkfilename):
        self.data = Data()
        print ("readRatings")
        self.readRatings(ratingfilename)
        print ("readMovies")
        self.readMovies(moviefilename)
        print ("readTags")
        self.readTags(tagfilename)
        print ("readLinks")
        self.readLinks(linkfilename)
        
    def readRatings(self, ratingfilename):
        with open(ratingfilename, 'rb') as csvfile:
            ratings = csv.reader(csvfile, delimiter=',', quotechar='|')
            # delete title
            #"""
            isFirst = True
            if readLocks[0]:
                return
            for row in ratings:
                if isFirst:
                    isFirst = False
                    continue
                self.data.addRating(int(row[0]), int(row[1]), int(float(row[2])*2), row[3])
            #"""

    def readMovies(self, moviefilename):
        with open(moviefilename, 'rb') as csvfile:
            movies = csv.reader(csvfile, delimiter=',', quotechar='|')
            # delete title
            #"""
            if readLocks[1]:
                return
            isFirst = True
            for row in movies:
                if isFirst:
                    isFirst = False
                    continue
                genres = [];
                for genre in row[2].split("|"):
                    genres.append(genre)
                self.data.addMovie(int(row[0]), row[1], genres)
            #"""

    def readTags(self, tagfilename):
        with open(tagfilename, 'rb') as csvfile:
            tags = csv.reader(csvfile, delimiter=',', quotechar='|')
            # delete title
            #"""
            if readLocks[2]:
                return
            isFirst = True
            for row in tags:
                if isFirst:
                    isFirst = False
                    continue
                self.data.addTag(int(row[0]), int(row[1]), row[2], row[3])
            #"""

    def readLinks(self, linkfilename):
        with open(linkfilename, 'rb') as csvfile:
            links = csv.reader(csvfile, delimiter=',', quotechar='|')
            # delete title
            #"""
            if readLocks[3]:
                return
            isFirst = True
            for row in links:
                if isFirst:
                    isFirst = False
                    continue
                self.data.addLink(int(row[0]), row[1], row[2]) # last two are about to added in URL
            #"""            


class Data:
    # TODO: movie including - average ratings, rate num, tags's relation score
    def __init__(self):
        self.ratings = []
        self.movies = []
        self.tags = []
        self.links = []

    def addRating(self, uid, mid, r, t):
        self.ratings.append(Rating(uid, mid, r, t))

    def addMovie(self, mid, title, genre):
        self.movies.append(Movie(mid, title, genre))

    def addTag(self, uid, mid, tag, time):
        self.tags.append(Tag(uid, mid, tag, time))

    def addLink(self, mid, imdbid, tmdbid):
        self.links.append(mid, imdbid, tmdbid)


class Rating:
    def __init__(self, uid, mid, r, t):
        self.uid = uid
        self.mid = mid
        self.rating = r
        # no need itme


class Movie:
    def __init__(self, mid, title, genre):
        self.mid = mid
        self.tilte = title
        self.genre = genre

    def addRating(self, ratingsum, count):
        pass


class Tag:
    def __init__(self, uid, mid, tag, time):
        self.uid = uid
        self.mid = mid
        self.tag = tag
        # no time


class Link:
    def __init__(self, mid, imdbid, tmdbid):
        self.mid = mid
        self.imdbid = imdbid
        self.tmdbid = tmdbid
    
if __name__ == "__main__":
    logging.basicConfig(format='%(asctime)s : %(levelname)s : %(message)s', level=logging.INFO)
    dataReader = DataReader("ratings.csv", "movies.csv", "tags.csv", "links.csv")
    dataAnalyzer = DataAnalyzer(dataReader.data)
    # preprocessing
    #dataAnalyzer.calculateMovieTagRelation()
    #dataAnalyzer.saveMovieTagRelation()

    dataAnalyzer.calculateMovieRatings()
    dataAnalyzer.buildLSIModel()
    # second time
    #dataAnalyzer.readMovieTagRelation()


"""
document-word matrix = user-movie matrix
   class rating
frequency -> rating
   class rating
"""
    #dataAnalyzer.
    #dataAnalyzer.getUserSpecificRating()
    
