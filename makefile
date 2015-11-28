CC=g++
CFLAGS = -g -Wall -std=c++11
OBJ = obj
BIN = bin
TEST = test

all:
	# Do nothing
	echo "Specify a target"
# Executable Files
HMM: hmm data viterbi
	$(CC) -o $(BIN)/hmm $(OBJ)/hmm.o $(OBJ)/data.o $(OBJ)/viterbi.o

# Individual Files
hmm: hmm.cpp
	$(CC) $(CFLAGS) -c hmm.cpp -o $(OBJ)/hmm.o

data: data.cpp data.h
	$(CC) $(CFLAGS) -c data.cpp -o $(OBJ)/data.o

viterbi: viterbi.cpp viterbi.h
	$(CC) $(CFLAGS) -c viterbi.cpp -o $(OBJ)/viterbi.o

# Test Files
viterbi-test: $(TEST)/viterbi-test.cpp $(TEST)/viterbi-test2.cpp viterbi
	$(CC) $(CFLAGS) $(OBJ)/viterbi.o $(TEST)/viterbi-test.cpp -o $(TEST)/viterbi-test
	$(CC) $(CFLAGS) $(OBJ)/viterbi.o $(TEST)/viterbi-test2.cpp -o $(TEST)/viterbi-test2
