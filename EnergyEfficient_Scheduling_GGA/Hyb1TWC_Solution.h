#ifndef Hyb1TWC_SOLUTION_H
#define Hyb1TWC_SOLUTION_H

#include "MachineSet.h"
#include "GroupingGenome.h"
#include "TWCEPCSolution.h"
#include "TWCEPC_4Solution.h"
#include "GeneralSolutionTWC.h"

class Hyb1TWC_Solution : public TMOMHSolution {
public:
	Hyb1TWC_Solution();																	// Initialization
	Hyb1TWC_Solution(Hyb1TWC_Solution& parent1, Hyb1TWC_Solution& parent2);					// Recombination (Crossover)
	Hyb1TWC_Solution(Hyb1TWC_Solution& parent1, Hyb1TWC_Solution&  parent2, bool nonDelay);	// Recombination (Crossover)
	Hyb1TWC_Solution(Hyb1TWC_Solution& solution1);											// Copy constructor
	~Hyb1TWC_Solution();

	void Mutate();

	bool isValid();
	void setObjectiveValues();
	

private:
	// Chromosome
	GeneralSolutionTWC* _sol;		// can be either a TWTEPCSoluton (LIST) or a TWTEPC_4Solution(BD)
	double _key;			// defines which of the solution classes is chosen
	bool _valid;

};

#endif