/* 
TP (Time pattern) - Repräsentation
With TWTEPC_3Solution batches are assigned to machines and specific starting times.
A batch´s position in the chromosome defines to which starting time and machine it is assigned. 
The possible starting time are distributed equally over the time horizon and the machines.
*/

#ifndef TWTEPC_3SOLUTION_H
#define TWTEPC_3SOLUTION_H

#include "momhsolution.h"
#include "MachineSet.h"
#include "GroupingGenome.h"


class TWTEPC_3Solution : public TMOMHSolution {
public:
	TWTEPC_3Solution();							// standard constructor => random initialization of one individual
	TWTEPC_3Solution(TWTEPC_3Solution & parent1, TWTEPC_3Solution & parent2);		// Recombination (Crossover)
	TWTEPC_3Solution(TWTEPC_3Solution& solution1);	// copy constructor
	~TWTEPC_3Solution();		

	void Mutate();

	bool decodeChromosome();					// returns false if infeasible
	bool decodeChromosome_ASAP();				// using LIST_ASAP
	double getTWT();
	double getEPC();
	bool isValid();
	double improveTWT();						// locally improve TWT, not accepting increase of EPC
	double improveEPC();						// locally improve EPC, not accepting increase of TWT
	double improveCombined(double ratio);		// locally improve ratio * TWT + (1-ratio) * EPC
	void locSswapJobs();						// local search: pairwise swap jobs between two batches
	void locSswapJobsAdj();						// local search: pairwise swap jobs between two adjacent batches
	void locSshiftJobs();						// local search: shift jobs to another batch
	void locSshiftJobsAdj();						// local search: shift jobs to another adjacent batch
	void locSsplitBatch();						// local search: split a single batch into two batches
	MachineSet& updateSchedule();				// derive the schedule from the current chromosome
	void consolidateBatches();					// locally improve (potentially) both objectives by shifting jobs from later to earlier batches
	void consolidateBatchesKeepPauses();		// keeping empty batches empty
	void consolidateBatchesSameMachineKeepPauses();	// considers batches of the same machine, keeping empty batches empty
	bool balanceLoad();							// locally improve (potentially) both objectives by re-assigning batches to different machines 

	void setPauses(int length);
	void setPausesAvgLength();
	void resetPauses();
	
	void setObjectiveValues();
	void setObjectiveValues_ASAP();

	MachineSet getSchedule();
	int getLastNonemptyBatch(int machine);		// get the _chromosome´s index of the last non-empty batch at machine, -1 if all batches are empty
	int getLastEmptyBatch(int machine);			// get the _chromosome´s index of the last empty batch at machine, -1 if no batch is empty
	vector<int> getPausesPerMachine();
	vector<int> getEmptyBatchesPerMachine();
	bool hasFeasibleSchedule();					// true, if the current _schedule is feasible

	static bool compByRK(const pair<int, float>& a, const pair<int, float>& b);
	void sortPrioByRK();

	void applyLocalSearch(double weightTWT, double weightEPC);

private:
	// Chromosome
	GroupingGenome _chromosome;
	vector< pair<int, float> >_priority;		// index + random numbers for each gene, which define the likeliness that for empty batches to consume a time unit
	MachineSet _schedule;
	bool _valid;

};



#endif