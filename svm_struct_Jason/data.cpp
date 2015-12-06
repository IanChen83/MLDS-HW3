#include "data.h"

std::map<string,int> TypeTableFunctor::stringMap48_Int;
std::map<string,string> TypeTableFunctor::stringMap48_39;
string TypeTableFunctor::table39[48];
string TypeTableFunctor::table48[48];

TypeTableFunctor TypeTable("HMM/48_39.map");

template<typename T, int col_idx, int col_len>
void inline print(T* v){
    int e = (col_idx+1) * col_len;
    for(int i = col_idx * col_len; i < e; ++i){
        cout << std::setw(8) << v[i];
        if(i % 20 == 0)
            cout << endl;
    }
}

/****************** LoadSequences *************************
 * Input:
 *      Sequences&      : Pass by reference that store all sequences
 *      const char*     : The path to the file containing  Data (N x (1 + 48)) lines
 * */
void
LoadSequences(Sequences& seqs, const char* path){
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
        double temp = 0;
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
            ++seq->length;
        }else{
            // New sequence
            seq = new Sequence;
            seq->name.assign(name);
            seqs.insert(SequencePair(seq->name,seq));
            seq->head = seq->end = pho;
            seq->length = 1;
        }
        memset(line, 0, 900);
    }
    printf("Read %d line(s) from '%s'\n", line_count, path);
}

void
LoadAnswers(Sequences& seqs, const char* path){
    ifstream file(path);
    int line_count = 0;
    char line[25];
    Sequence* seq = 0;
    Phoneme* pho = 0;

    for(;file.getline(line, 25);){
        ++line_count;

        // Deal with name
        char* name = strtok(line, ",");
        char* tok = strtok(NULL, ",");
        char* start = strchr(name, '_');
        char* end = strchr(start + 1, '_');
        end[0] = '\0';

        if(seq != 0 && seq->name.compare(name) == 0){
            // Same sequence, load token into the phoneme
            if(pho == 0)
                pho = seq->head;
            pho->ans = TypeTable(tok);
            pho = pho->next;
        }else{
            seq = seqs[name];
            pho = seq->head;
            pho->ans = TypeTable(tok);
            pho = pho->next;
        }

        memset(line, 0, 25);
    }
    printf("Read %d line(s) from '%s'\n", line_count, path);
}

void
LoadCountMap(double* start_map, double** my_map, double* end_map, double* count_total, const char* path){
    ifstream file(path);
    char line[20];
    char* first, *second;
    char* count;
    for(int i = 0; i<48; ++i){
        count_total[i] = 0;
    }
    for(;file.getline(line, 20);){
        first = strtok(line, " ");
        second = strtok(NULL, " ");
        count = strtok(NULL, " ");
        if(first[0] == '^'){
            if(second[0] == '^' || second[0] == '$'){
            }else{
                start_map[TypeTable(second)] = double(atoi(count));
            }
    }else if(first[0] == '$'){
        }else{
            if(second[0] == '^'){
            }else if(second[0] == '$'){
                int x = TypeTable(first);
                double y = double(atoi(count));
                end_map[x] = y;
                count_total[x] += y;
                //cout << y << '\t';
            }else{
                int x = TypeTable(first);
                double y = double(atoi(count));
                my_map[x][TypeTable(second)] = y;
                count_total[x] += y;
                //cout << y << '\t';
            }
        }
        memset(line, 0, 20);
    }
}

// Count error by comparing tag and ans
int
CountError(Sequences& seqs){
    int error = 0;
    for(auto it = seqs.begin(); it != seqs.end(); ++it){
        error += it->second->countError();
    }
    return error;

}

// Calculate tag for every phoneme in the seqs
void
CalculateTag(Sequences& seqs){
   for(auto it = seqs.begin(); it != seqs.end(); ++it){
       it->second->calculateTag();
   }
}

// Output the tag
void
OutputTag(Sequences& seqs, ostream& os){
    for(auto it = seqs.begin(); it != seqs.end(); ++it){
        it->second->outputTag(os);
    }
}
/***************** Sequence member function *************/
int
Sequence::countError(){
    int error = 0;
    Phoneme* it = head;
    while(it != 0){
        if(it->tag != it->ans){
            ++error;
        }
        it = it->next;
    }
    return error;
}
void
Sequence::outputTag(ostream& os){
    Phoneme* it = head;
    while(it != 0){
        os << name << ',' << TypeTable(it->tag) << endl;
        it = it->next;
    }
}
void
Sequence::calculateTag(){
    Phoneme* it = head;
    while(it != head){
        double* v = it->value;
        double max = 0;
        int max_idx = 0;
        for(int i = 0; i < 48; ++i){
            if(v[i] > max){
                max = v[i];
                max_idx = i;
            }
        }
        it->tag = max_idx;
        it = it->next;
    }
}
/****************** Example Usage *************************
int main(){
    int startTable[48];
    int endTable[48];
    int countTable[48][48];
    LoadCountMap(startTable, countTable, endTable, "HMM/count.map");
    return 0;
}
**********************************************************/
/****************** Example Usage *************************

int main(){
    // Define a Sequences to load data
    Sequences seqs;

    // Load Sequences FIRST
    LoadSequences(seqs,"dnn_softmax_sub.ark");

    // Load Answers SUBSEQUENTLY
    LoadAnswers(seqs,"answer_map_sub.txt");

    // To iterate throught ALL the sequences
    for(auto it = seqs.begin(); it != seqs.end(); ++it){
        auto pho = it->second->head;
        for(;pho != 0;pho = pho->next){
            cout << TypeTable(pho->ans) << '\t';
        }
        cout << endl;
    }

    // If you just want to load one sequence, 'aem0_si1392' for example
    auto seq = seqs["faem0_si1392"]
    cout << TypeTable(seq->head->ans) << endl;
}
**********************************************************/
