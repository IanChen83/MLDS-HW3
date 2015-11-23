#ifndef DATA_HEADER
#define DATA_HEADER
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>

using namespace std;

struct Phoneme{
    float value[48];
    int tag;
    int ans;
    Phoneme* prev = 0;
    Phoneme* next = 0;
};

struct Sequence{
    string name;
    char* result = 0;
    Phoneme* head = 0;
    Phoneme* end = 0;
};

typedef typename std::vector<Sequence*> Sequences;

void LoadSequences(Sequences&, const char*);
//void LoadAnswers(Sequences&, const char*);
//char* trim(Answer&);
//char* trim_3(Answer&);
#endif
