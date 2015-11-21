def trimming(data):
    """
        Input   :Array of sentences (n x 48)
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
    type_convert = [line.split()[0] for line in open("38_idx_char.map", 'r')]
    def conversion(phoneme):
        m = type_convert.index(phoneme)
        return m + 97 if m < 26 else m + 39

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

def trimming_3(data):
    """
        The same as trimming(data) except that only 3-in-a-row phonemes will remain
    """
    type_convert = [line.split()[0] for line in open("38_idx_char.map", 'r')]
    def conversion(phoneme):
        m = type_convert.index(phoneme)
        return m + 97 if m < 26 else m + 39

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
