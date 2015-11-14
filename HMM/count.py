import numpy

input_y_file = "train_y.txt"
input_type_file = "48_idx_char.map"
output_trans_file = "count.map"
################### Data structure ########################
# A  50x50 matrix mat in which
# mat[x][y] counts the number y after x
# 0 is the START state
# 49 means the END state
mat = numpy.zeros((50, 50))
type_table = ["^"]

input_y = open(input_y_file, 'r')
input_type = open(input_type_file, 'r')

# Get type_table
for line in input_type:
    token = line.split()[0]
    type_table.append(token)

type_table.append("$")

# Example  maeb0_si1411_1,sil

# Start counting
last_header = ["", ""]
last_idx = 0
for line in input_y:
    line_split = line.split(",")
    header = line_split[0].split("_")
    token = line_split[1].split()[0]

    if last_header[0] != header[0] or last_header[1] != header[1]:
        mat[last_idx][49] += 1
        last_header = ["", ""]
        last_idx = 0

    idx = type_table.index(token)
    mat[last_idx][idx] += 1

    last_header[0], last_header[1] = header[0], header[1]
    last_idx = idx

mat[last_idx][49] += 1

output_trans = open(output_trans_file, 'w')
for i in range(50):
    for j in range(50):
        output_trans.write("%s %s %d\n" % (type_table[i], type_table[j], int(mat[i][j])))
print(mat.sum())
