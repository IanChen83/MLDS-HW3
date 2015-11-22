import numpy
import data
from data import type_table

input_transition_file = "count.map"
input_transition = open(input_transition_file, 'r')
transition = numpy.zeros((48, 48))
start_prob = [0] * 48
end_prob = [0] * 48
z = [_z.rstrip() for _z in open(input_transition_file, 'r')]
for line in z:
    _3 = line.split()
    if _3[0] == '^':
        if _3[1] == '$' or _3[1] == '^':
            continue
        else:
            start_prob[type_table.index(_3[1])] = int(_3[2])
    elif _3[0] == '$':
        continue
    else:
        if _3[1] == '^':
            continue
        elif _3[1] == '$':
            end_prob[type_table.index(_3[0])] = int(_3[2])
        else:
            transition[type_table.index(_3[0])][type_table.index(_3[1])] = int(_3[2])

def viterbi(emission):
    """
        Input   : A (n x 48) sequence
                    [
                        [0.111, 0.352, 0.522, ...],
                        [0.412, 0.241, 0.251, ...],
                        ...
                    ]
        Output  : The (1 x n) path that has biggest probability
                    [sil, sil, sil, ...]
    """
    stack = []
    last_prob = start_prob

    for i in range(len(emission)):
        prob = [0] * 48
        maximum = 0
        max_last = 0
        for k in range(48):         # Current observed
            local_max = 0
            for j in range(48):     # Last observed
                t = last_prob[j] * transition[j][k] * emission[i][k]
                if t > local_max:
                    local_max = t
                    if t > maximum:
                        maximum = t
                        max_last = j
            prob[k] = local_max
        stack.append(type_table[max_last])
        last_prob = prob if prob[0] > 10000 else [x/10000 for x in prob]
    for i in range(len(last_prob)):
        last_prob[i] = last_prob[i] * end_prob[i]
    stack.append(type_table[last_prob.index(max(last_prob))])
    return stack

a = data.load_sequences("dnn_softmax.ark")
b = data.get_answer_from_seq(a)
c = viterbi(a[0])
print(c)
