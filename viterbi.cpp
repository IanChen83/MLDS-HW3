#include <list>
#include <iostream>
#include <iomanip>
#include "data.h"
using namespace std;

/****************** Viterbi Algorithm *********************
 * Obv  : int
 *      | Number of elements in the observation space
 *
 * Sta  : int
 *      | Number of elements in the state space
 *
 * Em   : matrix
 *      | Emission matrix Em[i][j] means P(j given i)
 *
 * Tm   : matrix
 *      | Transition matrix Tm[i][j] means P(i to j)
 *
 * Start: vector
 *      | Initial probability P(i|start)
 *
 * End  : vector
 *      | End probibility
 *
 * Return *
 * ret  : list<Sta>
 *********************************************************/
template<typename T>
class value_idx{
    public:
        T value;
        int index;
        void assign(T& v, int& idx){
            v = value;
            idx = index;
        };
        value_idx(T v, int i): value(v), index(i){};
};

template<typename T>
value_idx<T> inline max(T* v, int total){
    T* x = v;
    int idx = 0;
    for(int i = 0; i < total; ++i){
        if(v[i] > *x){
            idx = i;
            x = v + i;
        }
    }
    return value_idx<T>(*x, idx);
}

template<typename T, int a, int num>
void inline print(T* v){
    int e = (a+1) * num;
    for(int i = a * num; i < e; ++i){
        cout << setw(8) << v[i];
        if(i % 20 == 0)
            cout << endl;
    }
}

template<int obv, int sta>
list<int>
viterbi(const double** Em, const double** Tm, const double* start, const double* end){
    // Initialization
    double prob[obv][sta];
    int backTrack[obv][sta];
    double temp_prob[sta];

    // Head probability
    for(int i = 0; i < sta; ++i){
        prob[0][i] = start[i] + Em[0][i];
        backTrack[0][i] = 0;
    }

    for(int i = 1; i < obv; ++i){
        for(int j = 0; j < sta; ++j){
            for(int k = 0; k < sta; ++j){
                temp_prob[k] = prob[i-1][k] + Em[i][j] + Tm[k][j];
            }
            max(temp_prob, sta).assign(prob[i][j], backTrack[i][j]);
        }
    }

    // End probability
    for(int i = 0; i < sta; ++i){
        temp_prob[i] = prob[obv-1][i] * end[i];
    }
    double final = 0;
    int index = 0;
    max(temp_prob, sta).assign(final, index);

    list<int> ret;
    for(int i = obv - 1; i >= 0; --i){
        ret.push_front(index);
        index = backTrack[i][index];
    }
    return ret;

}


int main(){
    Sequences seqs;
    LoadSequences(seqs, "HMM/dnn_softmax_sub.ark");

    return 0;
}
