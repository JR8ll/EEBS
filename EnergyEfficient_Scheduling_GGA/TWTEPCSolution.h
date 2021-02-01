/* 
LIST (List-Scheduling) - Representation
With TWTEPCSolution batches are assigned to machines using two different LIST-SCHEDULING approaches:
A certain part (defined by a random key) of the chromosome´s batches are assigned asap, 
the remaining batches are positioned into the least expensive time windows considering the time-of-use tarif
*/

#ifndef TWTEPCSOLUTION_H
#define TWTEPCSOLUTION_H

#include "momhsolution.h"
#include "MachineSet.h"
#include "GroupingGenome.h"
#include "GeneralSolution.h"



class TWTEPCSolution : public GeneralSolution { // TMOMHSolution {
public:
	TWTEPCSolution();							// standard constructor => random initialization of one individual
	TWTEPCSolution(TWTEPCSolution & parent1, TWTEPCSolution & parent2);		// Recombination (Crossover)
	TWTEPCSolution(TWTEPCSolution & parent1, TWTEPCSolution & parent2, bool nonDelay);		// Recombination (Crossover)
	TWTEPCSolution(TWTEPCSolution& solution1);	// copy constructor
	TWTEPCSolution(GroupingGenome chr, MachineSet sched, float pref, bool val, vector<double> delays);
	~TWTEPCSolution();		

	void Mutate();

	bool decodeChromosomeBasic();				// returns false if infeasible
	bool decodeChromosome();				// using two types of LIST algorithms 
	bool decodeChromosome_ASAP();				// using LIST_ASAP
	double getTWT();
	double getEPC();
	double getCombinedObjective(double twtWeight, double epcWeight);
	bool isValid();
	double improveTWT();						// locally improve TWT, not accepting increase of EPC
	double improveEPC();						// locally improve EPC, not accepting increase of TWT
	double improveCombined(double ratio);		// locally improve ratio * TWT + (1-ratio) * EPC
	void consolidateBatches();					// locally improve potentially both objectives by shifting jobs from later to earlier batches
	
	void setObjectiveValues();
	void setObjectiveValues_VLIST();
	void setObjectiveValues_ASAP();

	MachineSet getSchedule();
	bool updateSchedule();				// derive the schedule from the current chromosome
	int getPredBatchId(int batchId);			// get batch id of the predecessor of a batch, -1 if no predecessor exists
	int getSuccBatchId(int batchId);			// get batch id of the successor of a batch, -1 if no predecessor exists

	void locSswapJobs();						// local search: pairwise swap jobs between two batches
	void locSswapJobsAdj();						// local search: pairwise swap jobs between two adjacent batches
	void locSshiftJobs();						// local search: shift jobs to another batch
	void locSshiftJobsAdj();						// local search: shift jobs to another adjacent batch
	void locSsplitBatch();						// local search: split a single batch into two batches

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
	//vector<float> _individualPreference;		// random numbers for each gene, which define together with _twtPref which LIST-algorithm is used to schedule the batch
	MachineSet _schedule;
	float _twtPref;								// Random-key defining the preference for the first objective function value
	bool _valid;
	vector<double> _delayT;						// borrowed from TWTEPC_4Solution
};




#endif