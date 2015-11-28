#ifndef VITERBI_HEADER
#define VITERBI_HEADER
#include <list>
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
list<int> viterbi(int, int, double**, double**, double*, double*);
#endif
