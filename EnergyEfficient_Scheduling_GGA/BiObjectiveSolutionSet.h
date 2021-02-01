#ifndef BIOBJECTIVESOLUTIONSET_H
#define BIOBJECTIVESOLUTIONSET_H

#include "General.h"

using namespace std;

struct BiObjectiveSolution {
	string objective;
	bool solved;
	int value;
	int bound;
	double time;
	double relMIPgap;
	int eRates;
};

class BiObjectiveSolutionSet {
public:
	vector< pair<BiObjectiveSolution, BiObjectiveSolution > > solutions;
	
	pair<BiObjectiveSolution, BiObjectiveSolution >& operator[](int index);
	int size();
	void push_back(pair< BiObjectiveSolution, BiObjectiveSolution > biSol);
	void append(BiObjectiveSolutionSet& appSet);
	void save(char* filename, int timeLimit);
	void deleteDominated();
};

#endif