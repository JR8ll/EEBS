#include "MachineSet.h"

using namespace std;

// constructor definitions
MachineSet::MachineSet() {
	this->content = vector<Machine>();
}

// operator overloading
Machine& MachineSet::operator[](int index) {
	return this->content[index];
}

// method definitions
/// misc
int MachineSet::size() {
	return this->content.size();
}

void MachineSet::print() {
	int mMax = this->size();
	if(mMax == 0) {
		cout << "No batches are processed on this machine." << endl;
	}
	else {
		for(unsigned i = 0; i < mMax; i++) {
			cout << "Machine " << this->content[i].id << " processes batches [";
			int bMax = this->content[i].getQueueSize();
			for(int j = 0; j < bMax; j++) {
				cout << this->content[i].getBatch(j).id ;
				if(j < bMax - 1) {
					cout << ", ";
				}
			}
			cout << "]" << endl;
		}
	}
}
bool MachineSet::isEmpty() {
	if(this->content.empty()){
		return true;
	}
	return false;
}
/// initialization
void MachineSet::addMachine(Machine inMachine) {
	this->content.push_back(inMachine);
}

void MachineSet::clear() {
	this->content.clear();
}

void MachineSet::copy(const MachineSet* other){
	this->clear();
	if(this == other) return;
	MachineSet &oth = *((MachineSet*) (other));
	int iMax = other->content.size();
	// set up new machines
	this->clear();
	for(unsigned i = 0; i < iMax; i++) {
		Machine tempM;
		this->addMachine(tempM);
		// copy machines including batches
		this->content[i].copy(&oth.content[i]);
	}
}
/// scheduling
int MachineSet::getEarliestAvailableMachine() {
	int mMax = this->size();
	int earliestAvailableMachine = 0;
	double msp = this->getMSP();
	for(int i = 0; i < mMax; i++) {
		if(this->content[i].getMSP() < msp) {
			msp = this->content[i].getMSP();
			earliestAvailableMachine = i;
		}
	}
	return earliestAvailableMachine;
}
void MachineSet::listSched(BatchSet& in_batchSet) {
	// loop through the batches
	int bMax = in_batchSet.size();
	for(unsigned i = 0; i < bMax; i++) {
		// get the machine which becomes available first
		int availableFirst = 0;
		double minMSP = 99999.0;		// TODO
		int mMax = this->size();
		for(int j = 0; j < mMax; j++) {
			double thisMSP = this->content[j].getMSP();
			if(thisMSP < minMSP) {
				minMSP = thisMSP;
				availableFirst = j;
			}
		}
		this->content[availableFirst].addBatch(in_batchSet[i]);
	}
}

void MachineSet::listSched(vector<Batch *>& chromosome) {
	// loop through the batches
	int bMax = chromosome.size();
	for(unsigned i = 0; i < bMax; i++) {
		if(!chromosome[i]->empty()) {
			int availableFirst = 0;
			double minMSP = 99999.0;		// TODO
			// loop through the machines
			int mMax = this->size();
			for(int j = 0; j < mMax; j++) {
				double thisMSP = this->content[j].getMSP();
				if(thisMSP < minMSP) {
					minMSP = thisMSP;
					availableFirst = j;
				}
			}
			this->content[availableFirst].addBatch(*chromosome[i]);
		}
	}

}
void MachineSet::listSched_EPC(vector<Batch *>& chromosome, const vector<int>& Ecost) {
	// TODO: implement
	// sort batches by BATC-II*
	this->listSched(chromosome);
}
bool MachineSet::listSched_EPC(Batch& in_batch, const vector<int>& Ecost) {
	int pBatch = in_batch.pLot;		// Lot processing assumed
	int rBatch = in_batch.r;
	int maxM = this->content.size();
	int maxT = this->content[0].freeTimeSlots.size() - pBatch;
	int lowestCost = 0;			
	for(unsigned n = 0; n < maxT; n++) {
		lowestCost += Ecost[n];				// actually highest possible cost
	}
	int bestMachine = 0;
	int bestTime = maxT;
	// identify all suitable slots and calculate the cost
	
	for(unsigned i = 0; i < maxM; i++) {
		for(unsigned t = rBatch; t < maxT; t++) {
			if(this->content[i].freeTimeSlots[t] >= pBatch) { // Batch can be started in t on i
				// calculate cost
				int myCost = 0;
				for(unsigned h = t; h < (t + pBatch); h++) {
					myCost += Ecost[h];
				}
				if( (myCost < lowestCost) || ( (myCost <= lowestCost) && (t < bestTime) ) ) {
					lowestCost = myCost; 
					bestMachine = i;
					bestTime = t;
				}
			}
		}
	}
	// schedule Batch at best time on best machine
	return this->content[bestMachine].addBatch(in_batch, bestTime);	// *in_batch
}
bool MachineSet::listSched_ASAP(Batch& in_batch) {
	int pBatch = in_batch.pLot;		// Lot processing assumed
	int rBatch = in_batch.r;
	int maxM = this->content.size();
	int maxT = this->content[0].freeTimeSlots.size() - pBatch;
	int bestMachine = 0;
	int bestTime = maxT;
	// schedule the batch as soon as possible (ASAP)
	for(unsigned i = 0; i < maxM; i++) {
		for(unsigned t = rBatch; t < maxT; t++) {
			if(this->content[i].freeTimeSlots[t] >= pBatch && t < bestTime) { // Batch can be started in t on i
				bestMachine = i;
				bestTime = t;
				break;
			}
		}
	}
	return this->content[bestMachine].addBatch(in_batch, bestTime);		// *in_batch
}
void MachineSet::listSched_TWT(vector<Batch *>& chromosome) {
	// TODO: implement
}
bool MachineSet::listSched_TWT(Batch& in_batch) {
	int pBatch = in_batch.pLot;		// Lot processing assumed
	int rBatch = in_batch.r;
	int dBatch = in_batch.earliestD;
	int maxM = this->content.size();
	int maxT = this->content[0].freeTimeSlots.size() - pBatch;
	int bestMachine = 0;
	int bestTime = maxT;
	// schedule the batch so it completes in dBatch or as early as possible
	for(unsigned i = 0; i < maxM; i++) {
		for(int t = rBatch; t < maxT; t++) {
			if(this->content[i].freeTimeSlots[t] < pBatch) {
				continue;	// scan remaining time slots of this machine
			}               
			if(dBatch - pBatch == t) {
				return this->content[i].addBatch(in_batch, t);		// batch is scheduled just in time
			}
					
			if(dBatch - pBatch > t) {	// batch can be scheduled early
				bestTime = t;
				bestMachine = i;
			} else {												// batch will be scheduled late	
				if(t < bestTime && bestTime > dBatch - pBatch) {
					// TODO: nur updaten wenn nicht bereits ein größeres t gefunden wurde mit t < d-p
					bestMachine = i;
					bestTime = t;		
				}
				break;		// scan next machine
			}
		}
	}
	return this->content[bestMachine].addBatch(in_batch, bestTime);		// *in_batch
}


void MachineSet::listSched_variable(vector<Batch *>& chromosome, float TWT_preference) {
	// for each batch decide whether it is to be scheduled for a low TWT or a low EPC
	// TWT_preference defines the probability
}

void MachineSet::balanceLoad(){
	bool loadIsBalanced = false;
	int mMax = this->content.size();
	int maxLoad, minLoad, maxLoadMachine, minLoadMachine;
	if(!this->isEmpty()) {
		while(!loadIsBalanced) {
			mMax = this->content.size();
			maxLoad = this->content[0].getLoad();
			maxLoadMachine = 0;
			minLoad = this->content[0].getLoad();
			minLoadMachine = 0;
			for(int m = 0; m < mMax; m++) {
				// TODO: implement
				int tempLoad =  this->content[m].getLoad();
				if(tempLoad > maxLoad) {
					maxLoad = tempLoad;
					maxLoadMachine = m;
				}
				if(tempLoad < minLoad) {
					minLoad = tempLoad;
					minLoadMachine = m;
				}
			}
			if(minLoadMachine == maxLoadMachine) {
				loadIsBalanced = true;
			} else { 
				if(this->content[maxLoadMachine].getLoad() > 0) {
					// move last batch from machine with highest load to machine with lowest load
					if(maxLoad > minLoad + this->content[maxLoadMachine].getBatch(this->content[maxLoadMachine].getQueueSize()-1).pLot) {
						Batch tempBatch = this->content[maxLoadMachine].getBatch(this->content[maxLoadMachine].getQueueSize()-1);
						this->content[minLoadMachine].addBatch(tempBatch);
						this->content[maxLoadMachine].deleteBatch(this->content[maxLoadMachine].getQueueSize()-1);
					} else {
						loadIsBalanced = true;
					}
				}
				else {
					loadIsBalanced = true;
				}
			}
		}
	}
}

void MachineSet::balanceLoadSPT(){
	bool loadIsBalanced = false;
	int mMax = this->content.size();
	int maxLoad, minLoad, maxLoadMachine, minLoadMachine;
	if(!this->isEmpty()) {
		while(!loadIsBalanced) {
			mMax = this->content.size();
			maxLoad = this->content[0].getLoad();
			maxLoadMachine = 0;
			minLoad = this->content[0].getLoad();
			minLoadMachine = 0;
			for(int m = 0; m < mMax; m++) {
				// TODO: implement
				int tempLoad =  this->content[m].getLoad();
				if(tempLoad > maxLoad) {
					maxLoad = tempLoad;
					maxLoadMachine = m;
				}
				if(tempLoad < minLoad) {
					minLoad = tempLoad;
					minLoadMachine = m;
				}
			}
			if(minLoadMachine == maxLoadMachine) {
				loadIsBalanced = true;
			} else { 
				if(this->content[maxLoadMachine].getLoad() > 0) {
					// find batch with min p in maxLoadMachine
					int minP = this->content[maxLoadMachine].getBatch(0).pLot;
					int minP_batch = 0;
					int maxB = this->content[maxLoadMachine].getQueueSize();
					for(int b = 1; b < maxB; b++) {
						int tempP = this->content[maxLoadMachine].getBatch(b).pLot;
						if(tempP < minP) {
							minP = tempP;
							minP_batch = b;
						} 
					}

					// move batch with min P from machine with highest load to machine with lowest load
					if(maxLoad > minLoad + this->content[maxLoadMachine].getBatch(minP_batch).pLot) {
						Batch tempBatch = this->content[maxLoadMachine].getBatch(minP_batch);
						this->content[minLoadMachine].addBatch(tempBatch);
						this->content[maxLoadMachine].deleteBatch(minP_batch);
					} else {
						loadIsBalanced = true;
					}
				}
				else {
					loadIsBalanced = true;
				}
			}
		}
	}
}	


double MachineSet::applyLOM() {							// local optimization of Makespan
	double newMSP = 0.0;
	int mMax = this->content.size();
	for(unsigned i = 0; i < mMax; i++) {
		double thisMSP = this->content[i].applyLOM();
		if(thisMSP > newMSP) {
			newMSP = thisMSP;
		}
	}
	return newMSP;
}									
double MachineSet::applyLOC(const vector<int>& ECost) {
	double newEPC = 0.0;
	int mMax = this->size();
	for(unsigned i = 0; i < mMax; i++) {
		newEPC += this->content[i].applyLOC(ECost, (int) ceil(this->getMSP()));
	}
	return newEPC;
}									// local optimization of ECP
double MachineSet::applyLOCT(const vector<int>& ECost){
	double newEPC = 0.0;
	int mMax = this->size();
	for(unsigned i = 0; i < mMax; i++) {
		newEPC += this->content[i].applyLOCT(ECost);
	}
	return newEPC;


}
double MachineSet::improveTWT() {
	double newTWT = 0.0;
	int maxM = this->size();
	for(unsigned i = 0; i < maxM; i++) {
		newTWT += this->content[i].improveTWT();
	}
	return newTWT;
}
double MachineSet::improveEPC() {
	double newEPC = 0.0;
	int maxM = this->size();
	for(unsigned i = 0; i < maxM; i++) {
		newEPC += this->content[i].improveEPC();
	}
	return newEPC;
}
double MachineSet::improveTWC(){
	double newTWC = 0.0;
	int maxM = this->size();
	for(unsigned i = 0; i < maxM; i++) {
		newTWC += this->content[i].improveTWC();
	}
	return newTWC;
}
double MachineSet::improveCombined(double ratio){
	double combined = 0.0;
	int maxM = this->size();
	for(unsigned i = 0; i < maxM; i++) {
		combined += this->content[i].improveCombined(ratio);
	}
	return combined;
}


void MachineSet::improveEpcWithMinTwtImpact(const vector<int>& Ecost) {
	double minTwtIncrease = 999999;
	int machineToOptimize = 0;			// index of the machine for which epc can be improved with minimal impact on twt
	int iMax = this->content.size();
	// identify machine to optimize
	for(int i = 0; i < iMax; i++) {
		double tempTwtIncrease = this->content[i].minTwtIncreaseByEpcSaving(Ecost);
		if(tempTwtIncrease < minTwtIncrease) {
			minTwtIncrease = tempTwtIncrease;
			machineToOptimize = i;
		}
	}
	// optimize identified machine
	this->content[machineToOptimize].ImproveEpcWithMinTwtImpact(Ecost);
}
void MachineSet::improveEpcWithMinTwcImpact(const vector<int>& Ecost) {
	double minTwcIncrease = 999999;
	int machineToOptimize = 0;			// index of the machine for which epc can be improved with minimal impact on twt
	int iMax = this->content.size();
	// identify machine to optimize
	for(int i = 0; i < iMax; i++) {
		double tempTwcIncrease = this->content[i].minTwcIncreaseByEpcSaving(Ecost);
		if(tempTwcIncrease < minTwcIncrease) {
			minTwcIncrease = tempTwcIncrease;
			machineToOptimize = i;
		}
	}
	// optimize identified machine
	this->content[machineToOptimize].ImproveEpcWithMinTwcImpact(Ecost);
}
bool MachineSet::isEpcSavingPossible(const vector<int>& Ecost){		// true if EPC can be reduced by shifting a single batch
	int mMax = this->content.size();
	for(int m = 0; m < mMax; m++) {
		if(this->content[m].isEpcSavingPossible(Ecost)) {
			return true;
		}
	}
	return false;
}												



pair< BiObjectiveSolution, BiObjectiveSolution > MachineSet::getBiObjSol(const vector<int>& Ecost, int e){
	pair< BiObjectiveSolution, BiObjectiveSolution > biSol;
	BiObjectiveSolution obj1 = BiObjectiveSolution();
	obj1.objective = "TWT";
	obj1.eRates = e;
	obj1.value = this->getTWT();

	BiObjectiveSolution obj2 = BiObjectiveSolution();
	obj2.objective = "EPC";
	obj2.eRates = e;
	obj2.value = this->getEPC(Ecost);

	biSol.first = obj1;
	biSol.second = obj2;

	return biSol;
}
pair< BiObjectiveSolution, BiObjectiveSolution > MachineSet::getBiObjSolTwc(const vector<int>& Ecost, int e){
	pair< BiObjectiveSolution, BiObjectiveSolution > biSol;
	BiObjectiveSolution obj1 = BiObjectiveSolution();
	obj1.objective = "TWC";
	obj1.eRates = e;
	obj1.value = this->getTWC();

	BiObjectiveSolution obj2 = BiObjectiveSolution();
	obj2.objective = "EPC";
	obj2.eRates = e;
	obj2.value = this->getEPC(Ecost);

	biSol.first = obj1;
	biSol.second = obj2;

	return biSol;
}
BiObjectiveSolutionSet MachineSet::getLowerEpcSols(const vector<int>& Ecost, int e){
	BiObjectiveSolutionSet sols = BiObjectiveSolutionSet();
	pair< BiObjectiveSolution, BiObjectiveSolution > biSol = this->getBiObjSol(Ecost, e);
	sols.push_back(biSol);

	bool epcSavingPossible = this->isEpcSavingPossible(Ecost);

	while(epcSavingPossible) {
		this->improveEpcWithMinTwtImpact(Ecost);
		biSol = this->getBiObjSol(Ecost, e);
		sols.push_back(biSol);
		epcSavingPossible = this->isEpcSavingPossible(Ecost);
	}

	sols.deleteDominated();
	return sols;
}
BiObjectiveSolutionSet MachineSet::getLowerEpcSolsTwc(const vector<int>& Ecost, int e){
	BiObjectiveSolutionSet sols = BiObjectiveSolutionSet();
	pair< BiObjectiveSolution, BiObjectiveSolution > biSol = this->getBiObjSolTwc(Ecost, e);
	sols.push_back(biSol);

	bool epcSavingPossible = this->isEpcSavingPossible(Ecost);

	while(epcSavingPossible) {
		this->improveEpcWithMinTwcImpact(Ecost);
		biSol = this->getBiObjSolTwc(Ecost, e);
		sols.push_back(biSol);
		epcSavingPossible = this->isEpcSavingPossible(Ecost);
	}

	sols.deleteDominated();
	return sols;
}
/// objective functions
double MachineSet::getTCT() {
	double tct = 0.0;
	int mMax = this->size();
	for(unsigned i = 0; i < mMax; i++) {
		tct += this->content[i].getTCT();
	}
	return tct;
}									// total completion time
double MachineSet::getTWC() {
	double twct = 0.0;
	int mMax = this->size();
	for(unsigned i = 0; i < mMax; i++) {
		twct += this->content[i].getTWC();
	}
	return twct;
}									// total weighted completion time
double MachineSet::getTT() {
	double tt = 0.0;
	int mMax = this->size();
	for(unsigned i = 0; i < mMax; i++) {
		tt += this->content[i].getTT();
	}
	return tt;
}									// total tardiness
double MachineSet::getTWT() {
	double twt = 0.0;
	int mMax = this->size();
	for(unsigned i = 0; i < mMax; i++) {
		twt += this->content[i].getTWT();
	}
	return twt;
}									// total weighted tardiness
double MachineSet::getMSP() {
	double msp = 0.0;
	int mMax = this->size();
	for(unsigned i = 0; i < mMax; i++) {
		if(this->content[i].getMSP() > msp) {
			msp = this->content[i].getMSP();
		}
	}
	return msp;
}


double MachineSet::getEPC(const vector<int>& Ecost) {
	double epc = 0.0;
	int mMax = this->size();
	for(unsigned i = 0; i < mMax; i++) {
		epc += this->content[i].getEPC(Ecost);
	}
	return epc;
}