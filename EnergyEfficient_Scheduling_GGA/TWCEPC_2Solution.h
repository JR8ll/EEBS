#ifndef TWCEPC_2SOLUTION_H
#define TWCEPC_2SOLUTION_H

#include "momhsolution.h"
#include "MachineSet.h"
#include "GroupingGenome.h"


class TWCEPC_2Solution : public TMOMHSolution {
public:
	TWCEPC_2Solution();							// standard constructor => random initialization of one individual
	TWCEPC_2Solution(TWCEPC_2Solution & parent1, TWCEPC_2Solution & parent2);		// Recombination (Crossover)
	TWCEPC_2Solution(TWCEPC_2Solution& solution1);	// copy constructor
	~TWCEPC_2Solution();		

	void Mutate();

	bool decodeChromosome();					// returns false if infeasible
	bool decodeChromosome_VLIST();				// using two types of LIST algorithms 
	bool decodeChromosome_ASAP();				// using LIST_ASAP
	double getTWC();
	double getEPC();
	bool isValid();
	double improveTWC();						// locally improve TWC, not accepting increase of EPC
	double improveEPC();						// locally improve EPC, not accepting increase of TWC
	double improveCombined(double ratio);		// locally improve ratio * TWC + (1-ratio) * EPC
	void consolidateBatches();					// locally improve potentially both objectives by shifting jobs from later to earlier batches
	
	void setObjectiveValues();
	void setObjectiveValues_VLIST();
	void setObjectiveValues_ASAP();

	

private:
	// Chromosome
	GroupingGenome _chromosome;
	vector<float> _individualPreference;		// random numbers for each gene, which define together with _twcPref which LIST-algorithm is used to schedule the batch
	MachineSet _schedule;
	float _twcPref;								// Random-key defining the preference for the first objective function value
	bool _valid;

};



#endif