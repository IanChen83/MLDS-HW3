#ifndef VITERBI_HEADER
#define VITERBI_HEADER
#include <vector>
#include "data.h"

float transition[48][48];

vector<int> viterbi(Sequences&);

#endif
