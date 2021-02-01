/* 
POS (Position based) - Representation
With TWTEPC_2Solution the batches´ sequence and machine assigned is derived from their positions in the chromosome.
*/

#ifndef TWTEPC_2SOLUTION_H
#define TWTEPC_2SOLUTION_H

#include "momhsolution.h"
#include "MachineSet.h"
#include "GroupingGenome.h"


class TWTEPC_2Solution : public TMOMHSolution {
public:
	TWTEPC_2Solution();							// standard constructor => random initialization of one individual
	TWTEPC_2Solution(TWTEPC_2Solution & parent1, TWTEPC_2Solution & parent2);		// Recombination (Crossover)
	TWTEPC_2Solution(TWTEPC_2Solution& solution1);	// copy constructor
	~TWTEPC_2Solution();		

	void Mutate();

	bool decodeChromosome();					// returns false if infeasible
	bool decodeChromosome_VLIST();				// using two types of LIST algorithms 
	bool decodeChromosome_ASAP();				// using LIST_ASAP
	double getTWT();
	double getEPC();
	bool isValid();
	MachineSet getSchedule();
	double improveTWT();						// locally improve TWT, not accepting increase of EPC
	double improveEPC();						// locally improve EPC, not accepting increase of TWT
	double improveCombined(double ratio);		// locally improve ratio * TWT + (1-ratio) * EPC
	void consolidateBatches();					// locally improve potentially both objectives by shifting jobs from later to earlier batches
	
	void setObjectiveValues();
	void setObjectiveValues_VLIST();
	void setObjectiveValues_ASAP();

	

private:
	// Chromosome
	GroupingGenome _chromosome;
	vector<float> _individualPreference;		// random numbers for each gene, which define together with _twtPref which LIST-algorithm is used to schedule the batch
	MachineSet _schedule;
	float _twtPref;								// Random-key defining the preference for the first objective function value
	bool _valid;

};



#endif