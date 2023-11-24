E = [
    [0.0],
    [0.0],
    [0.0],
    [0.0],
    [0.0]
    ]

A = [
    [1.0],
    [1.0],
    [1.0],
    [1.0],
    [1.0]
    ]

H = [
    [1.0],
    [1.0],
    [1.0],
    [1.0],
    [1.0]
    ]


LTL = [
    [0, 0, 0, 0, 0],
    [0, 2, 1, 0, 0],
    [0, 1, 2, 0, 0],
    [0, 0, 0, 2, 1],
    [0, 0, 0, 1, 1]
    ]


LLT = [
    [2, 0, 1, 1, 0],
    [0, 2, 0, 0, 1],
    [1, 0, 1, 0, 0],
    [1, 0, 0, 1, 0],
    [0, 1, 0, 0, 1]
    ]


def normalize(M):
    sums = 0.0
    for i in range(5):
        sums += M[i][0]
    for i in range(5):
        M[i][0] /= sums
    return M

for t in range(5):
    newA = [
        [0.0],
        [0.0],
        [0.0],
        [0.0],
        [0.0]
        ]
    newH = [
        [0.0],
        [0.0],
        [0.0],
        [0.0],
        [0.0]
        ]
    for i in range(0, 1):
        for j in range(0, 5):
            for k in range(0, 5):
                newA[j][i] += LTL[j][k] * A[k][i]
                newH[j][i] += LLT[j][k] * H[k][i]
    A = normalize(newA)
    H = normalize(newH)
    #print A
    #print H

print A
print H
