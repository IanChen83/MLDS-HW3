#include "data.h"

void LoadSequences(Sequences& seqs, const char* path){
    ifstream file(path);
    char line[900];
    char* last = new char[1];
    char* de = last;
    Sequence* x = new Sequence;
    Phoneme* pho = new Phoneme;
    x->head = x->end = pho;

    for(;file.getline(line, 900);){
        // Deal with name
        char* name = strtok(line, " ");
        char* start = strchr(name, '_');
        char* end = strchr(start + 1, '_');
        end[0] = '\0';

        // Deal with value
        for(int i = 1;i < 49;++i){
            pho->value[i-1] = atof(strtok(NULL, " "));
        }
        float temp = 0;
        int j = 0;
        for(int i = 0;i < 48; ++i){
            if(pho->value[i] > temp){
                temp = pho->value[i];
                j = i;
            }
        }
        pho->tag = j;

        if(strcmp(last, name) != 0){    // Same sequence
            pho->prev = x->end;
            x->end->next = pho;
            x->end = pho;
            pho = new Phoneme;
            continue;
        }else{                          // new sequence
            seqs.push_back(x);
            x = new Sequence;
            x->head = x->end = pho;
            pho = new Phoneme;
            continue;
        }

        memset(line, 0, 900);
    }
    delete pho;
    delete de;
}

int main(){
    Sequences seqs;
    LoadSequences(seqs,"train_x.txt");
}
