#ifndef VITERBI_HEADER
#define VITERBI_HEADER
#include <list>
#include <iomanip>
#include "data.h"

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

template<typename T> value_idx<T> inline max(T*, int);
template<typename T, int a, int num> void inline print(T*);
template<int obv, int sta> list<int> viterbi(double**, double**, double*, double*);
#endif
