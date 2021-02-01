/* 
BD (Batch delay) - Repräsentation
With TWTEPC_4Solution batches are assigned to machines and their starting times are delayed.
A twofold random key in the chromosome defines a batche´s machine and its delay relative to other batches´ delays on the same machine. 
*/

#ifndef TWTEPC_4SOLUTION_H
#define TWTEPC_4SOLUTION_H

#include "momhsolution.h"
#include "MachineSet.h"
#include "GroupingGenome.h"
#include "GeneralSolution.h"


class TWTEPC_4Solution : public GeneralSolution { //TMOMHSolution {
public:
	TWTEPC_4Solution();							// standard constructor => random initialization of one individual
	TWTEPC_4Solution(TWTEPC_4Solution & parent1, TWTEPC_4Solution & parent2);		// Recombination (Crossover)
	TWTEPC_4Solution(TWTEPC_4Solution & parent1, TWTEPC_4Solution & parent2, bool nonDelay);		// Recombination (Crossover)
	TWTEPC_4Solution(TWTEPC_4Solution& solution1);	// copy constructor
	TWTEPC_4Solution(GroupingGenome chr, MachineSet sched, float pref, bool val, vector<double> delays);
	~TWTEPC_4Solution();		

	void Mutate();

	bool decodeChromosome();					// returns false if infeasible
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
	bool balanceLoad();							// locally improve (potentially) both objectives by re-assigning batches to different machines 
	
	void setObjectiveValues();

	MachineSet getSchedule();
	int getLastNonemptyBatch(int machine);		// get the _chromosome´s index of the last non-empty batch at machine, -1 if all batches are empty
	int getLastEmptyBatch(int machine);			// get the _chromosome´s index of the last empty batch at machine, -1 if no batch is empty
	
	vector<int> getPausesPerMachine();
	bool hasFeasibleSchedule();					// true, if the current _schedule is feasible

	void applyLocalSearch(double weightTWT, double weightEPC);

	// GeneralSolution inheritance
	GroupingGenome getChromosome();
	float getTwtPref();
	bool getValid();
	vector<double> getDelayT();

	void setChromosome(GroupingGenome chr);
	void setSchedule(MachineSet sched);
	void setTwtPref(float pref);
	void setValid(bool val);
	void setDelayT(vector<double> delays);

private:
	// Chromosome
	GroupingGenome _chromosome;
	vector<double> _delayT;						// delay for T (after the last batch´s completion time) for each machine
	float _twtPref;								// borrowed from TWTEPCSolution
	MachineSet _schedule;
	bool _valid;

};



#endif