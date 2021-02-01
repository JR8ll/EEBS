/* 
Hybrid I - for min(TWT, EPC), combines the LIST- with the BD-representation
*/

#ifndef Hyb1_SOLUTION_H
#define Hyb1_SOLUTION_H

#include "MachineSet.h"
#include "GroupingGenome.h"
#include "TWTEPCSolution.h"
#include "TWTEPC_4Solution.h"
#include "GeneralSolution.h"

class Hyb1_Solution : public TMOMHSolution {
public:
	Hyb1_Solution();																	// Initialization
	Hyb1_Solution(Hyb1_Solution& parent1, Hyb1_Solution& parent2);					// Recombination (Crossover)
	Hyb1_Solution(Hyb1_Solution& parent1, Hyb1_Solution&  parent2, bool nonDelay);	// Recombination (Crossover)
	Hyb1_Solution(Hyb1_Solution& solution1);											// Copy constructor
	~Hyb1_Solution();

	void Mutate();

	bool isValid();
	void setObjectiveValues();
	MachineSet getSchedule();
	

private:
	// Chromosome
	GeneralSolution* _sol;		// can be either a TWTEPCSoluton (LIST) or a TWTEPC_4Solution(BD)
	double _key;			// defines which of the solution classes is chosen
	bool _valid;

};

#endif