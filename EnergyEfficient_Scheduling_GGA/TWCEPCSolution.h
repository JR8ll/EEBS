#ifndef TWCEPCSOLUTION_H
#define TWCEPCSOLUTION_H

#include "momhsolution.h"
#include "MachineSet.h"
#include "GroupingGenome.h"
#include "GeneralSolutionTWC.h"

class TWCEPCSolution : public GeneralSolutionTWC { //: public TMOMHSolution {
public:
	TWCEPCSolution();							// standard constructor => random initialization of one individual
	TWCEPCSolution(TWCEPCSolution & parent1, TWCEPCSolution & parent2);		// Recombination (Crossover)
	TWCEPCSolution(TWCEPCSolution & parent1, TWCEPCSolution & parent2, bool nonDelay);		// Recombination (Crossover)
	TWCEPCSolution(TWCEPCSolution& solution1);	// copy constructor
	TWCEPCSolution(GroupingGenome chr, MachineSet sched, float pref, bool val, vector<double> delays);
	~TWCEPCSolution();		

	void Mutate();

	bool decodeChromosome();					// returns false if infeasible
	bool decodeChromosome_VLIST();				// using two types of LIST algorithms 
	bool decodeChromosome_ASAP();				// using LIST_ASAP
	double getTWC();
	double getEPC();
	double getCombinedObjective(double TWCWeight, double epcWeight);
	bool isValid();
	double improveTWC();						// locally improve TWC, not accepting increase of EPC
	double improveEPC();						// locally improve EPC, not accepting increase of TWC
	double improveCombined(double ratio);		// locally improve ratio * TWC + (1-ratio) * EPC
	void consolidateBatches();					// locally improve potentially both objectives by shifting jobs from later to earlier batches

	void locSswapJobs();						// local search: pairwise swap jobs between two batches
	void locSswapJobsAdj();						// local search: pairwise swap jobs between two adjacent batches
	void locSshiftJobs();						// local search: shift jobs to another batch
	void locSshiftJobsAdj();						// local search: shift jobs to another adjacent batch
	void locSsplitBatch();						// local search: split a single batch into two batches
	bool updateSchedule();				// derive the schedule from the current chromosome

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
	
	void setObjectiveValues();
	void setObjectiveValues_VLIST();
	void setObjectiveValues_ASAP();

	MachineSet getSchedule();

	void applyLocalSearch(double weightTWC, double weightEPC);

private:
	// Chromosome
	GroupingGenome _chromosome;
	vector<float> _individualPreference;		// random numbers for each gene, which define together with _TWCPref which LIST-algorithm is used to schedule the batch
	MachineSet _schedule;
	float _twcPref;								// Random-key defining the preference for the first objective function value
	bool _valid;
	vector<double> _delayT;						// borrowed from TWCEPC_4Solution
};




#endif