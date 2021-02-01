#ifndef MACHINE_H
#define MACHINE_H

#include "BatchSet.h"

using namespace std;

class Machine {

private:
	// attributes´ declaration
	double msp;				// Makespan
	BatchSet queue;			// Batches to be processed

public:
	// attributes´ declaration
	static int MachineCounter;
	int id;
	vector<int> freeTimeSlots;	// Free time slots (index represents starting time)
	// constructors´ declaration
	Machine();
	Machine(int T);

	// methods´ declaration
	/// misc
	Batch& getBatch(int i);
	int getQueueSize();
	bool isEmpty();

	/// scheduling
	bool addBatch(Batch& in_Batch);
	bool addBatch(Batch& in_Batch, int start);						
	bool deleteBatch(int index);
	bool shiftBatchRight(int index);
	bool shiftBatchRight(int index, int shifts);
	bool shiftBatchLeft(int index);
	bool shiftBatchLeft(int index, int shifts);
	bool isShiftBatchRightPossible(int index);
	bool isShiftBatchLeftPossible(int index);
	int possRightShifts(int index);
	int possLeftShifts(int index);
	double twtFromBatchRightShift(int index);
	double twtFromBatchLeftShift(int index);
	double twtFromBatchRightShifts(int index, int shifts);
	double twtFromBatchLeftShifts(int index, int shifts);

	double epcFromBatchRightShift(int index, const vector<int>& ECost);
	double epcFromBatchLeftShift(int index, const vector<int>& ECost);
	double epcFromBatchRightShifts(int index, const vector<int>& ECost, int shifts);
	double epcFromBatchLeftShifts(int index, const vector<int>& ECost, int shifts);

	double twcFromBatchRightShift(int index);
	double twcFromBatchLeftShift(int index);
	double twcFromBatchRightShifts(int index, int shifts);
	double twcFromBatchLeftShifts(int index, int shifts);
	
	bool isEpcSavingPossible(const vector<int>& Ecost);
	double minTwtIncreaseByEpcSaving(const vector<int>& Ecost);
	double minTwcIncreaseByEpcSaving(const vector<int>& Ecost);
	void ImproveEpcWithMinTwtImpact(const vector<int>& Ecost);
	void ImproveEpcWithMinTwcImpact(const vector<int>& Ecost);

	void clear();													// delete all batches
	void copy(const Machine* other);
	void sortBatchesBy_Start();			
	void sortBatchesBy_wpLot();
	void listSched(vector<Batch>& batches);
	void listSched(BatchSet& batches);
	void updateCompletionTimes();
	void updateFreeTimeSlots();
	double applyLOM();												// local optimization of makespan
	double applyLOC(const vector<int>& ECost, int mspLimit);		// local optimization of EPC, mspLimit is the current msp which must not be postponed
	double applyLOCT(const vector<int>& ECost);						// local optimization of EPC respecting the given tardiness
	double improveTWT();
	double improveEPC();	
	double improveTWC();
	double improveCombinedGreedy(double ratio);						// improves the schedule only if every single shift of a batch does not increase the combined objective function
	double improveCombined(double ratio);							// improves the schedule if any number of shifts of a batch does not increase the combined objective function
	/// objective function values
	double getTCT();									// total completion time
	double getTWC();									// total weighted completion time
	double getTT();										// total tardiness
	double getTWT();									// total weighted tardiness
	double getMSP();									// makespan
	int getLoad();										// sum of (lot) processing times
	double getEPC(const vector<int>& Ecost);			// electricity power cost

};

#endif