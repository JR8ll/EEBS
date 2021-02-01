/* 
TP (Time pattern) - Repräsentation
With TWCEPC_4Solution batches are assigned to machines and their starting times are delayed.
A twofold random key in the chromosome defines a batche´s machine and its delay relative to other batches´ delays on the same machine. 

This class is designed to tackle the special case of the problem P|p-batch, incompatible|(TWC, EPC) with uniform job sizes and all jobs having
a ready time of 0. The batch formation can be solved optimally by sorting jobs by non-increasing weights and forming batches in a first-fit manner.
This optimal batch formation can be achieved by calling the Problem::convertForSpecialCase()-method on the initialised problem instance.
As a result the batches are formed and translated to jobs, as jobs become the actual entities to be scheduled. Both job size and batch capacity are
set to one as no additional level of batching is possible. The constructor randomly assigns exactly each "job" to one "batch".
Although the implementation of the GGA does not really make sense, we try to assess the performance of components for addressing the subproblems
1: batch2machine-assignment (first part of a random-key)
2: sequencing				(locus of the gene)
3: timing / delay			(second part of a random-key).

*/

#ifndef TWCEPC_4SOLUTION_H
#define TWCEPC_4SOLUTION_H

#include "momhsolution.h"
#include "MachineSet.h"
#include "GroupingGenome.h"
#include "GeneralSolutionTWC.h"

class TWCEPC_4Solution : public GeneralSolutionTWC { //: public TMOMHSolution {
public:
	TWCEPC_4Solution();							// standard constructor => random initialization of one individual
	TWCEPC_4Solution(TWCEPC_4Solution & parent1, TWCEPC_4Solution & parent2);		// Recombination (Crossover)
	TWCEPC_4Solution(TWCEPC_4Solution & parent1, TWCEPC_4Solution & parent2, bool nonDelay);		// Recombination (Crossover) for non-delay offspring
	TWCEPC_4Solution(TWCEPC_4Solution& solution1);	// copy constructor
	TWCEPC_4Solution(GroupingGenome chr, MachineSet sched, float pref, bool val, vector<double> delays);
	~TWCEPC_4Solution();		

	void Mutate();

	bool decodeChromosome();					// returns false if infeasible
	double getTWC();
	double getEPC();
	bool isValid();
	double improveTWC();						// locally improve TWC, not accepting increase of EPC
	double improveEPC();						// locally improve EPC, not accepting increase of TWC
	double improveCombined(double ratio);		// locally improve ratio * TWC + (1-ratio) * EPC
	void locSswapJobs();						// local search: pairwise swap jobs between two batches
	void locSswapJobsAdj();						// local search: pairwise swap jobs between two adjacent batches
	void locSshiftJobs();						// local search: shift jobs to another batch
	void locSshiftJobsAdj();						// local search: shift jobs to another adjacent batch
	void locSsplitBatch();						// local search: split a single batch into two batches
	MachineSet& updateSchedule();				// derive the schedule from the current chromosome
	void consolidateBatches();					// locally improve (potentially) both objectives by shifting jobs from later to earlier batches
	bool balanceLoad();							// locally improve (potentially) both objectives by re-assigning batches to different machines 
	
	void setObjectiveValues();

	// GeneralSolution inheritance
	GroupingGenome getChromosome();
	float getTwcPref();
	bool getValid();
	vector<double> getDelayT();
	void setChromosome(GroupingGenome chr);
	void setSchedule(MachineSet sched);
	void setTwcPref(float pref);
	void setValid(bool val);
	void setDelayT(vector<double> delays);


	MachineSet getSchedule();
	int getLastNonemptyBatch(int machine);		// get the _chromosome´s index of the last non-empty batch at machine, -1 if all batches are empty
	int getLastEmptyBatch(int machine);			// get the _chromosome´s index of the last empty batch at machine, -1 if no batch is empty
	vector<int> getPausesPerMachine();
	bool hasFeasibleSchedule();					// true, if the current _schedule is feasible

	void applyLocalSearch(double weightTWC, double weightEPC);

private:
	// Chromosome
	GroupingGenome _chromosome;
	vector<double> _delayT;						// delay for T (after the last batch´s completion time) for each machine
	float _twcPref;
	MachineSet _schedule;
	bool _valid;

};



#endif