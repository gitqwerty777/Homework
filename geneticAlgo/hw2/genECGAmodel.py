import sys, math
from collections import Counter
import copy

population = []
model = []
probability = []
populationLength = 40
N = 10000

def readPopulation():
    with open("after.txt", "r") as populationFile:
        for line in populationFile:
            population.append(line.split(' ')[0:40])

def readModel():
    with open("model.txt", "r") as modelFile:
        for line in modelFile:
            line = line.strip("\n").split(" ")
            groupnum = int(line[0])
            group = []
            for i in range(groupnum):
                num = int(line[i+1]) # real index
                group.append(num)
            model.append(group)

    # append one element group
    for i in range(populationLength):
        inModel = False
        for m in model:
            if i in m:
                inModel = True
        if not inModel:
            model.append([i])
    print("model is :", model)

def calculateProbability(model, probability):
    for m in model:
        c = Counter()
        for p in population:
            part = ""
            for index in m:
                part += p[index]
            c[part] += 1
        probability.append(mostcommonToProb(c.most_common()))

def mostcommonToProb(count):
    newcount = []
    csum = 0
    for c in count:
        csum += c[1]
    for c in count:
        newcount.append([c[0], float(c[1])/csum])
    return newcount

def getModelComplexity(model):
    DModel = 0.0
    for m in model:
        DModel += pow(2, len(m))-1
    DModel *= math.log(N, 2)
    return DModel

def getDataComplexity(model, probability):
    DData = 0.0
    for probs in probability:
        for p in probs:
            DData += float(p[1]) * math.log(p[1], 2)
    DData *= -N
    return DData

def writeModel(model):
    with open("newmodel.txt", "w") as newmodelFile:
        for m in model:
            newmodelFile.write("%d" % len(m))
            for index in m:
                newmodelFile.write(" %d" % index)
            newmodelFile.write("\n")

if __name__ == "__main__":
    readPopulation()
    readModel()
    isImprove = True
    iterCount = 0
    while isImprove == True:
        iterCount += 1
        probability = []
        calculateProbability(model, probability)
        print("iteration %d" % iterCount)
        print("prob")
        print(probability)
        print("model")
        print(model)
        originalM = getModelComplexity(model)
        originalD = getDataComplexity(model, probability)
        originalComplexity = originalD + originalM
        print("original Model complexity %f" % originalM)
        print("original Data complexity %f" % originalD)

        minComplexity = originalComplexity
        minModel = copy.deepcopy(model)

        # combine 2 groups and get complexity
        # C len(model) get 2
        for i in range(len(model)):
            for j in range(i+1, len(model)):
                print("combine group %d and %d" % (i, j))
                print("model 1", model[i])
                print("model 2", model[j])
                newmodel = []
                for k in range(len(model)):
                    if k == i:
                        newmodel.append(model[i]+model[j])
                    elif k == j:
                        pass
                    else:
                        newmodel.append(model[k])
                #print(model)
                #print(newmodel)
                newprobability = []
                calculateProbability(newmodel, newprobability)
                #print("new probability: ", newprobability)
                mc = getModelComplexity(newmodel)
                dc = getDataComplexity(newmodel, newprobability)
                print("new Model complexity %f" % mc)
                print("new Data complexity %f" % dc)
                if(mc+dc < minComplexity):
                    minComplexity = mc+dc
                    minModel = copy.deepcopy(newmodel)
        print("min model is")
        print(minModel)
        print(minComplexity)
        isImprove = False
        if minComplexity < originalComplexity:
            model = copy.deepcopy(minModel)
            isImprove = True
            writeModel(minModel)
