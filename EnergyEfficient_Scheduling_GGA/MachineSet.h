#ifndef MACHINESET_H
#define MACHINESET_H

#include "Machine.h"
#include "BiObjectiveSolutionSet.h"

using namespace std;

class MachineSet {

public:
	// attributes´ declaration
	vector<Machine> content;

	// constructors´ declaration
	MachineSet();

	// operator overloading
	Machine& operator[](int index);

	// methods´ declaration
	/// misc
	int size();
	void print();
	bool isEmpty();

	/// initializiation
	void addMachine(Machine inMachine);
	void clear();
	void copy(const MachineSet* other);

	/// scheduling
	int getEarliestAvailableMachine();
	void listSched(BatchSet& in_batchSet);
	void listSched(vector<Batch *>& chromosome);
	void listSched_EPC(vector<Batch *>& chromosome, const vector<int>& Ecost);
	bool listSched_EPC(Batch& in_batch, const vector<int>& Ecost);
	bool listSched_ASAP(Batch& in_batch);
	void listSched_TWT(vector<Batch *>& chromosome);
	bool listSched_TWT(Batch& in_batch);

	void listSched_variable(vector<Batch *>& chromosome, float TWT_preference);

	void balanceLoad();														// shifting the last batch(es) from machine w/ max load to machine w/ min load
	void balanceLoadSPT();													// shifting the batches following SPT

	double applyLOM();														// local optimization of Makespan
	double applyLOC(const vector<int>& ECost);								// local optimization of EPC respecting the given makespan
 	double applyLOCT(const vector<int>& ECost);								// local optimization of EPC respecting the given tardiness
	double improveTWT();
	double improveEPC();
	double improveTWC();
	double improveCombined(double ratio);

	void improveEpcWithMinTwtImpact(const vector<int>& Ecost);
	void improveEpcWithMinTwcImpact(const vector<int>& Ecost);

	bool isEpcSavingPossible(const vector<int>& Ecost);												// true if EPC can be reduced by shifting a single batch
	pair< BiObjectiveSolution, BiObjectiveSolution > getBiObjSol(const vector<int>& Ecost, int e);
	pair< BiObjectiveSolution, BiObjectiveSolution > getBiObjSolTwc(const vector<int>& Ecost, int e);

	BiObjectiveSolutionSet getLowerEpcSols(const vector<int>& Ecost, int e);
	BiObjectiveSolutionSet getLowerEpcSolsTwc(const vector<int>& Ecost, int e);
	
	/// objective function values
	double getTCT();									// total completion time
	double getTWC();									// total weighted completion time
	double getTT();										// total tardiness
	double getTWT();									// total weighted tardiness
	double getMSP();									// makespan
	double getEPC(const vector<int>& Ecost);			// electric power cost

};

#endif