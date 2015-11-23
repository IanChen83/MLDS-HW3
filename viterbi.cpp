#include "viterbi.h"

using namespace std;

extern float transition[48][48];

vector<int> viterbi(Sequences& seqs){
}

int main(){
    Sequences seqs;
    LoadSequences(seqs,"dnn_softmax_sub.ark");
    return 0;
}
