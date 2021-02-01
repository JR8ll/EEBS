#ifndef SOLVER_H
#define SOLVER_H

#include "Problem.h"

using namespace std;

class Solver {
public:
	// attributes declarations
	Problem& problem;

	// constructor declarations
	Solver();
	Solver(Problem& in_Problem);

	// method declarations
};

#endif