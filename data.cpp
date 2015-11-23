#include "data.h"


/****************** LoadSequences *************************
 * Input:
 *      Sequences&      : Pass by reference that store all sequences
 *      const char*     : The path to the file containing  Data (N x (1 + 48)) lines
 * */
void LoadSequences(Sequences& seqs, const char* path){
    ifstream file(path);
    int line_count = 0;
    char line[900];
    Sequence* seq = 0;          // Current sequence
    Phoneme* pho = 0;           // Current phoneme

    for(;file.getline(line, 900);){
        ++line_count;
        pho = new Phoneme;

        // Deal with name
        char* name = strtok(line, " ");
        char* start = strchr(name, '_');
        char* end = strchr(start + 1, '_');
        end[0] = '\0';

        // Deal with value
        for(int i = 1;i < 49;++i){
            pho->value[i-1] = atof(strtok(NULL, " "));
        }

        // Find max and store the tag
        float temp = 0;
        int j = 0;
        for(int i = 0;i < 48; ++i){
            if(pho->value[i] > temp){
                temp = pho->value[i];
                j = i;
            }
        }
        pho->tag = j;

        if(seq != 0 && seq->name.compare(name) == 0){
            // Same sequence, append phoneme to the same sequence
            pho->prev = seq->end;
            seq->end->next = pho;
            seq->end = pho;
        }else{
            // New sequence
            seq = new Sequence;
            seq->name.assign(name);
            seqs.push_back(seq);
            seq->head = seq->end = pho;
        }

        memset(line, 0, 900);
    }
    printf("Read %d line(s) from '%s'\n", line_count, path);
}

int main(){
    Sequences seqs;
    LoadSequences(seqs,"dnn_softmax_sub.ark");
    cout << seqs[1]->name << endl;
}
