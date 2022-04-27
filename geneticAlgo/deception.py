import random
import copy

ITERNUM = 1000000fg

length = 4
bitSets = dict()

class BitSet:
    def __init__(self, length, key, value):
        self.length = length
        self.key = key
        self.arr = [False for i in range(length)]
        nowi = 0
        while key > 0:
            self.arr[nowi] = key % 2
            key = key / 2
            nowi = nowi+1
        self.value = value

    def flip(self):
        self.key = pow(2, length) - 1 - self.key
        for bit in self.arr:
            bit = not bit

    def __str__(self):
        return "%d %f" % (self.key, self.value)

    def ToString(self):
        string = ""
        for i in range(length):
            if(self.arr[i]):
                string += "1"
            else:
                string += "0"
        return string

    def findSchema(self):
        # return all match schema
        schemas = []
        for starnum in range(1, length):
            for value in range(0, pow(2, length)):
                if(oneCount(value) == starnum):
                    #print("starnum %d, value %d" %(starnum, value))
                    schema = list(self.ToString())
                    for l in range(0, length):
                        if(value % 2 == 1):
                            schema[l] = "*"
                        value = value/2
                    schema = "".join(schema)
                    #print("find schema is %s" % schema)
                    schemas.append(schema)
        return schemas



def assignNumbers():
    global bitSets
    bitSets = dict()
    nowi = 0
    while nowi < pow(2, length):
        bitSets[nowi] = BitSet(length, nowi, random.uniform(0.0, 1.0))
        print(bitSets[nowi])
        nowi = nowi+1

def oneCount(value):
    count = 0
    while(value > 0):
        if(value % 2 == 1):
            count = count + 1
        value = value/2
    return count

def checkDeception():
    global bitSets
    maxkey = 0
    maxvalue = -1
    for key in bitSets:
        bitSet = bitSets[key]
        if bitSet.value > maxvalue:
            maxvalue = bitSet.value
            maxkey = bitSet.key
    reverse = copy.deepcopy(bitSets[maxkey])
    reverse.flip()
    print("reverse "+ str(reverse))
    schemaSum = dict()

    for key in range(pow(2, length)):
        bitSet = bitSets[key]
        #print("for key %d, add schema value" % key)
        # find all related schema and find it
        schemas = bitSet.findSchema()
        for schema in schemas:
            #print("add schema %s" % schema)
            if schema not in schemaSum:
                schemaSum[schema] = 0
            schemaSum[schema] += bitSet.value

    # find advantage schema and check with other schema
    deception = True
    for starnum in range(1, length):
        if not deception:
            break
        for value in range(0, pow(2, length)):
            if(oneCount(value) == starnum):
                #print("starnum %d, value %d" %(starnum, value))
                schema = list(reverse.ToString())
                for l in range(0, length):
                    if(value % 2 == 1):
                        schema[l] = "*"
                    value = value/2
                schema = "".join(schema)
                #print("deception: biggest schema should be %s" % schema)
                # find best schemas with the same position
                maxSchemaValue = schemaSum[schema]
                remainNumber = length - starnum
                for r in range(pow(2, remainNumber)):
                    newschema = list(schema)
                    newr = r
                    for l in range(0, length):
                        if newschema[l] != '*':
                            if newr % 2 == 1:
                                newschema[l] = '1'
                            else:
                                newschema[l] = '0'
                            newr = newr / 2
                    newschema = "".join(newschema)
                    if schemaSum[newschema] > maxSchemaValue:
                        if newschema != schema:
                            #print("find new schema %s bigger value than schema %s" % (newschema, schema))
                            deception = False
                            break
    return deception


if __name__ == "__main__":
    iteration = 0
    success = 0
    while iteration < ITERNUM:
        print("iteration %d" % iteration)
        assignNumbers()
        if checkDeception():
            success += 1
        iteration = iteration + 1
    print("iteration %d success %d" % (iteration, success))
