#include "viterbi.h"
using namespace std;

/****************** Viterbi Algorithm *********************
 * Parameters **
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
 * Return **
 * ret  : list<int>
 *      | int: index in the state space
 *********************************************************/
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
viterbi(double** em, double** tm, double* start, double* end){
    // Initialization
    double Em[obv][sta];
    double Tm[sta][sta];
    for(int i = 0; i < obv; ++i){
        for(int j = 0; j < sta; ++j){
            Em[i][j] = log(em[i][j]);
//            Em[i][j] = em[i][j];
        }
    }
    for(int i = 0; i < sta; ++i){
        for(int j = 0; j < sta; ++j){
            Tm[i][j] = log(tm[i][j]);
//            Tm[i][j] = tm[i][j];
        }
    }

    double prob[obv][sta];
    int backTrack[obv][sta];
    double temp_prob[sta];

    // Head probability
    for(int i = 0; i < sta; ++i){
        prob[0][i] = log(start[i]) + Em[0][i];
//        prob[0][i] = start[i] * Em[0][i];
        backTrack[0][i] = 0;
    }

    for(int i = 1; i < obv; ++i){
        for(int j = 0; j < sta; ++j){
            for(int k = 0; k < sta; ++k){
                temp_prob[k] = prob[i-1][k] + Em[i][j] + Tm[k][j];
//                temp_prob[k] = prob[i-1][k] * Em[i][j] * Tm[k][j];
            }
            max(temp_prob, sta).assign(prob[i][j], backTrack[i][j]);
        }
    }

    // End probability
    for(int i = 0; i < sta; ++i){
        temp_prob[i] = prob[obv-1][i] + log(end[i]);
//        temp_prob[i] = prob[obv-1][i] * end[i];
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

/************* Example Usage ********************
 *
int main(){

    // Initialization
    double** trans = new double*[2];
    trans[0] = new double[2];
    trans[1] = new double[2];
    trans[0][0] = 0.7;
    trans[0][1] = 0.3;
    trans[1][0] = 0.4;
    trans[1][1] = 0.6;
    double** emit = new double*[3];
    emit[0] = new double[2];
    emit[1] = new double[2];
    emit[2] = new double[2];
    emit[0][0] = 0.4;
    emit[0][1] = 0.1;
    emit[1][0] = 0.2;
    emit[1][1] = 0.5;
    emit[2][0] = 0.2;
    emit[2][1] = 0.5;

    double start[2] = {0.8, 0.2};
    double end[2] = {1, 1};

    // Notice the template parameter!
    auto ret = viterbi<3,2>(emit,trans,start,end);
    return 0;
}
************************************************/
