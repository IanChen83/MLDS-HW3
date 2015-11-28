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


list<int>
viterbi(int obv, int sta, double** em, double** tm, double* start, double* end){
    // Initialization
    double** Em = new double*[obv];
    for(int i = 0; i < obv; ++i)
        Em[i] = new double[sta];
    double** Tm = new double*[sta];
    for(int i = 0; i < sta; ++i)
        Tm[i] = new double[sta];
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

    double** prob = new double*[obv];
    int ** backTrack = new int*[obv];
    for(int i = 0; i < obv; ++i){
        prob[i] = new double[sta];
        backTrack[i] = new int[sta];
    }
    double* temp_prob = new double[sta];

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

    // Clean up
    for(int i = 0; i < obv; ++i){
        delete[] prob[i];
        delete[] backTrack[i];
        delete[] Em[i];
    }
    for(int i = 0; i < sta; ++i){
        delete[] Tm[i];
    }
    delete[] Em;
    delete[] Tm;
    delete[] prob;
    delete[] backTrack;
    delete[] temp_prob;
    return ret;

}

/************* Example Usage ********************
// This is the same example in TA's HW3 slides
int main(){

    // Initialization
    double** trans = new double*[2];
    trans[0] = new double[2];
    trans[1] = new double[2];
    trans[0][0] = 0.7;  // H -> H
    trans[0][1] = 0.3;  // H -> C
    trans[1][0] = 0.4;  // C -> H
    trans[1][1] = 0.6;  // C -> C
    double** emit = new double*[3];
    emit[0] = new double[2];
    emit[1] = new double[2];
    emit[2] = new double[2];
    // 3
    emit[0][0] = 0.4;
    emit[0][1] = 0.1;
    // 1
    emit[1][0] = 0.2;
    emit[1][1] = 0.5;
    //1
    emit[2][0] = 0.2;
    emit[2][1] = 0.5;

    double start[2] = {0.8, 0.2};
    double end[2] = {1, 1};

    // Notice the template parameter!
    auto ret = viterbi<3,2>(emit,trans,start,end);
    return 0;
}
************************************************/
