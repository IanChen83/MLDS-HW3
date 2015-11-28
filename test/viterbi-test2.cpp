#include "../viterbi.h"
#include <iostream>
using namespace std;
int main(){
    // Obtained from wikipedia
    // Initialization
    double** trans = new double*[2];
    trans[0] = new double[2];
    trans[1] = new double[2];
    // 0:= Rainy, 1:= Sunny
    trans[0][0] = 0.7;
    trans[0][1] = 0.3;
    trans[1][0] = 0.4;
    trans[1][1] = 0.6;
    double** emit = new double*[3];
    emit[0] = new double[2];
    emit[1] = new double[2];
    emit[2] = new double[2];
    // 3
    emit[0][0] = 0.5;
    emit[0][1] = 0.1;
    // 1
    emit[1][0] = 0.4;
    emit[1][1] = 0.3;
    //1
    emit[2][0] = 0.1;
    emit[2][1] = 0.6;

    double start[2] = {0.6, 0.4};
    double end[2] = {1, 1};

    // Notice the template parameter!
    auto ret = viterbi(3, 2, emit,trans,start,end);
    for(auto it = ret.begin(); it != ret.end(); ++it){
        cout << *it << endl;
    }
    return 0;
}
