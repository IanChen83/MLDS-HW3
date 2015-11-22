import numpy
from data import type_table
from data import load_answers
output_trans_file = "count.map"
################### Data structure ########################
# A  50x50 matrix mat in which
# mat[x][y] counts the number y after x
# 0 is the START state
# 49 means the END state
mat = numpy.zeros((50, 50))
ans = load_answers("answer_map.txt")

for seq in ans:
    last = 0
    for i in seq:
        mat[last][i + 1] += 1
        last = i + 1
    mat[last][49] += 1

output = open(output_trans_file, 'w')
s = '^'
e = '$'
for i in range(50):
    for j in range(50):
        if i == 0:
            s = '^'
        elif i == 49:
            s = '$'
        else:
            s = type_table[i - 1]

        if j == 0:
            e = '^'
        elif j == 49:
            e = '$'
        else:
            e = type_table[j - 1]

        output.write("%s %s %d\n" % (s,e,mat[i][j]))

