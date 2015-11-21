type_table = [line.split()[0] for line in open("48_39.map", 'r')]
type_convert = [line.split()[0] for line in open("48_idx_char.map", 'r')]

def load_sequences(data_path):
    """
        Input   :Path to the file containing n x (1 x 49) vectors
        Output  :Array of sequences (n x s x 48)
                [
                    [[0.111, 0.222121, 0.2...], [0.145, 0.3453, 0.245...], ...],
                    [[0.141, 0.1223541, 0.3...], [0.5, 0.2343, 0.2445...], ...],
                    ...
                ]
    """
    x = [_x.rstrip() for _x in open(data_path, 'r')]
    ret = []
    i = -1
    pre = ""
    cen = ""
    _n = len(x)
    for s in range(_n):
        input_line = x[s]
        _11 = input_line.split()
        _12 = [float(t) for t in _11[1:]]

        _13 = _11[0].split('_')
        if _13[0] == pre and _13[1] == cen:
            ret[i].append(_12)
        else:
            pre = _13[0]
            cen = _13[1]
            ret.append([])
            i += 1
            ret[i].append(_12)
    return ret

def load_answers(data_path):
    """
        Input   :Path to the file of answer data
        Output  :Array of sentences (n x s)
                [
                    [5, 5, 5, ...],
                    [4, 2, 4, ...],
                    ...
                ]
    """
    y = [_y.rstrip() for _y in open(data_path, 'r')]
    i = -1
    pre = ""
    cen = ""
    _n = len(y)
    ret = []
    for s in range(_n):
        ans_line = y[s]
        _11 = ans_line.split(',')

        _13 = _11[0].split('_')
        if _13[0] == pre and _13[1] == cen:
            ret[i].append(type_table.index(_11[1]))
        else:
            pre = _13[0]
            cen = _13[1]
            ret.append([])
            i += 1
            ret[i].append(type_table.index(_11[1]))
    return ret

def trim(data):
    """
        Input   :Array of sentences (n x s)
                [
                    [sil, sil, sil...],
                    [sil, aa, aa...],
                    ...
                ]
        Output  :Array of strings with L (sil originally) removed
                [
                    "auxbjosi...",
                    "aoruvnrd...",
                    ...
                ]
    """
    def conversion(phoneme):
        m = type_convert.index(phoneme)
        return chr(m + 97) if m < 26 else chr(m + 39)

    ret = []
    for sentence in data:
        last = None
        x = []
        for i in sentence:
            if i == last:
                continue
            last = i
            x.append(conversion(i))
        if x[0] == 'L':
            x.pop(0)
        if x[-1] == 'L':
            x.pop(-1)
        ret.append("".join(x))

    return ret

def trim_3(data):
    """
        The same as trimming(data) except that only 3-in-a-row phonemes will remain
    """
    def conversion(phoneme):
        m = type_convert.index(phoneme)
        return chr(m + 97) if m < 26 else chr(m + 39)

    ret = []
    for sentence in data:
        x = []
        counter = 0
        last = None
        for i in sentence:
            if i == last:
                counter += 1
                continue
            last = i
            if counter < 3:
                x.pop(-1)
            x.append(conversion(i))
            counter = 1
        if counter < 3:
            x.pop(-1)
        if x[0] == 'L':
            x.pop(0)
        if x[-1] == 'L':
            x.pop(-1)
        ret.append("".join(x))

    return ret

def convert_48_to_39(data):
    """
        Input   :Array of sentence (n x 48) with 48 types
        Output  :Array of sentence (n x 48) with 39 types
        * in-place conversion
    """
    type_convert_48 = [line.split()[0] for line in open("48_39.map")]
    type_convert_39 = [line.split()[1] for line in open("48_39.map")]
    def conversion(a):
        return type_convert_39[type_convert_48.index(a)]

    for sentence in data:
        for i in sentence:
            i = conversion(i)
    return data

def get_answer_from_seq(data):
    """
        Input   :Training output (n x s x 48)
                [
                    [[0.111, 0.222121, 0.2...], [0.145, 0.3453, 0.245...], ...],
                    [[0.141, 0.1223541, 0.3...], [0.5, 0.2343, 0.2445...], ...],
                    ...
                ]
        Output  :Array of sentences (n x s)
                [
                    [sil, sil, sil...],
                    [sil, aa, aa...],
                    ...
                ]

    """
    ret = []
    for seq in data:
        ret.append([type_table[pho.index(max(pho))] for pho in seq])
    return ret
