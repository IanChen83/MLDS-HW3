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

def trim_sil(emission):
    start = 0
    for i in emission:
        if i.index(max(i)) == 36:    # sil
            start += 1
            continue
        else:
            break
    end = len(emission)
    for i in reversed(emission):
        if i.index(max(i)) == 36:    # sil
            end -= 1
            continue
        else:
            break
    return start, end

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
    start, end = trim_sil(emission)
    stack = []
    last_prob = [0] * 48
    prob = [0] * 48
    for i in range(len(last_prob)):
        last_prob[i] = start_prob[i] * emission[start][i]

    for i in range(start + 1, end):
        layer = []
        for j in range(48):         # Current observed
            # Get current observed max
            local_max = 0
            local_max_last_idx = 0
            for k in range(48):     # Last observed
                t = last_prob[k] * transition[k][j] * emission[i][j]
                if t > local_max:
                    local_max = t
                    local_max_last_idx = k
            prob[j] = local_max
            layer.append(local_max_last_idx)
        stack.append(layer)
        last_prob = prob if prob[0] < 10000 else [x/10000 for x in prob]

    path = []
    for i in range(len(last_prob)):
        prob[i] = last_prob[i] * end_prob[i]
    path.append(prob.index(max(prob)))
    probe = path[0]

    for tok in reversed(stack):
        probe = tok[probe]
        path.append(probe)
    print path

a = data.load_sequences("dnn_softmax.ark")
viterbi(a[0])
#print(len(a[0]))
#print(c)
#print(len(c))

