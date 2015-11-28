#include "../viterbi.h"
#include <iostream>
using namespace std;
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
    auto ret = viterbi(3, 2, emit,trans,start,end);
    for(auto it = ret.begin(); it != ret.end(); ++it){
        cout << *it << endl;
    }
    return 0;
}
