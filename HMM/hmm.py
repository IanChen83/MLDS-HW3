import numpy
import theano

input_transition_file = "count.map"

input_transition = open(input_transition_file, 'r')

input_observed = []
input_emission = []
input_transition = []
input_len = 0

mat = numpy.zeros((50, 50))
type_table = ["^"]

# Get type_table
for line in input_type:
    token = line.split()[0]
    type_table.append(token)

type_table.append("$")

def load_input_data_raw():
    global input_len
    x = [_x.rstrip() for _x in open(input_emission_file, 'r')]
    y = [_y.rstrip() for _y in open(input_observed_file, 'r')]
    i = -1
    pre = ""
    cen = ""
    _n = len(x)
    for s in range(_n):
        input_line = x[s]
        ans_line = y[s]
        _11 = input_line.split()
        _12 = [float(t) for t in _11[1:]]

        _21 = ans_line.split(',')

        _13 = _11[0].split('_')
        if _13[0] == pre and _13[1] == cen:
            input_emission[i].append(_12)
            input_observed[i].append(type_table.index(_21[1]))
        else:
            pre = _13[0]
            cen = _13[1]
            input_emission.append([])
            input_observed.append([])
            i += 1
            input_emission[i].append(_12)
            input_observed[i].append(type_table.index(_21[1]))

    input_len = len(input_observed)

    z = [_z.rstrip() for _z in open(input_transition_file, 'r')]
    for line in z:
        _3 = line.split()
        mat[type_table.index(_3[0])][type_table.index(_3[1])] = int(_3[2])

def viterbi(emission, observed):
    stack = []
    last_prob = [1] * 50

    for i in range(len(observed)):
        prob = [0] * 50
        maximum = 0
        max_last = 0
        for k in range(48):         # Current observed
            local_max = 0
            for j in range(48):     # Last observed
                t = last_prob[j + 1] * mat[j + 1][k + 1] * emission[i][k + 1]
                if t > local_max:
                    local_max = t
                    if t > maximum:
                        maximum = t
                        max_last = j
            prob[k + 1] = local_max
        stack.append(type_table[max_last])
        last_prob = prob
    stack.append(type_table[last_prob.index(max(last_prob))])
    return stack

load_input_data_raw()
viterbi(input_emission, input_observed[0])
