import numpy
import data
from data import type_table

input_transition_file = "count.map"
input_transition = open(input_transition_file, 'r')
transition = numpy.zeros((50, 50))
z = [_z.rstrip() for _z in open(input_transition_file, 'r')]
for line in z:
    _3 = line.split()
    transition[type_table.index(_3[0])][type_table.index(_3[1])] = int(_3[2])


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
                t = last_prob[j + 1] * transition[j + 1][k + 1] * emission[i][k]
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
