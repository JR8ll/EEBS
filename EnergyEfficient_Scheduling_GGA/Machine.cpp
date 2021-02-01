#include "Machine.h"
#include "Functions.h"		// TODO this might cause problems

using namespace std;


int Machine::MachineCounter = 0;


// constructor defintions
Machine::Machine(){
	this->id = ++Machine::MachineCounter;
	this->msp = 0.0;
	this->queue = BatchSet();
	for(unsigned i = 0; i < Global::problem->T; i++) {
		this->freeTimeSlots.push_back(Global::problem->T - i);
	}
}

Machine::Machine(int tMax) {
	this->id = ++Machine::MachineCounter;
	this->msp = 0.0;
	this->queue = BatchSet();
	for(unsigned i = 0; i < tMax; i++) {
		this->freeTimeSlots.push_back(tMax - i);
	}
}


// method definitions
/// misc
Batch& Machine::getBatch(int i) {
	return this->queue.getBatch(i);
}

int Machine::getQueueSize(){
	return this->queue.size();
}

bool Machine::isEmpty(){
	if(this->queue.empty()) {
		return true;
	}
	return false;
}
/// scheduling
bool Machine::addBatch(Batch& in_Batch) {
	int rBatch = in_Batch.r;
	int pBatch = in_Batch.pLot;						// Lot processing assumed
	int tMax = this->freeTimeSlots.size();
	int startBatch = 0;
	if(this->queue.empty()){						// this is the first batch on this machine
		in_Batch.setStart(rBatch);					// schedule asap
	}
	else {
		startBatch = max<int>(rBatch, (int) ceil(this->getMSP()));
		in_Batch.setStart(startBatch);	
	}
	this->queue.addBatch(in_Batch);					// asap
	this->msp = in_Batch.getC();					// update makespan

 
	/* TODO: verify, that freeTimeSlots are not needed for TWTEPC_3
	if(!in_Batch.empty()) {
	// update free time slots during the batch´s processing
		for(unsigned i = startBatch; i < min(startBatch + pBatch, tMax); i++) {
			this->freeTimeSlots[i] = 0;
		}
		// update free time slots before the batch´s processing
		int f = 1;	
		for(int j = (startBatch - 1); j >= 0; j--) {
			if(this->freeTimeSlots[j] == 0) {
				break;
			}
			else {
				this->freeTimeSlots[j] == f;
				f++;
			}
		}
	}
	*/ 
	return true;
}
bool Machine::addBatch(Batch& in_Batch, int start) {
	int rBatch = in_Batch.r;
	int pBatch = in_Batch.pLot;			// Lot processing assumed
	int tMax = this->freeTimeSlots.size();
	if( (rBatch > start) || (pBatch > this->freeTimeSlots[start]) ) {
		return false;			// Batch not yet available or insufficient free time slots for processing
	}
	in_Batch.setStart(start);
	this->queue.addBatch(in_Batch);
	this->msp = this->getMSP();
	// update free time slots during the batch´s processing
	for(unsigned i = start; i < min(start + pBatch, tMax); i++) {
		this->freeTimeSlots[i] = 0;
	}
	// update free time slots before the batch´s processing
	int f = 1;	
	for(int j = (start - 1); j >= 0; j--) {
		if(this->freeTimeSlots[j] == 0) {
			break;
		}
		else {
			this->freeTimeSlots[j] = f;
			f++;
		}
	}

	return true;
}
bool Machine::deleteBatch(int index) {
	if(index >= 0 && index < this->queue.size()) {
		this->queue.erase(index);
		this->updateFreeTimeSlots();
		return true;
	}
	return false;
}
bool Machine::shiftBatchRight(int index) {
	if(index >= 0 && index < this->queue.size() ) {
		int tMax = this->freeTimeSlots.size();
		int firstSlot, lastSlot;
		this->queue.sortBy_Start();

		// case 1: last batch
		if( index ==  this->queue.size() - 1) {			
			if ( this->queue[index].getC() < tMax) {
				// shift batch right
				this->queue[index].setStart(this->queue[index].getStart() + 1);		// setStart also sets C
				// update free time slots
				this->freeTimeSlots[this->queue[index].getC() - 1] = 0;
				if (index == 0) {
					// case 1b: only batch
					// update time slots [0, (start-1)]	
					firstSlot = 0;	
				} 
				else {
					// update time slots [ predecessors.c , (start-1)]
					firstSlot = this->queue[index-1].getC();	
				}
				lastSlot = this->queue[index].getStart();
				for(unsigned i = firstSlot; i < lastSlot; i++) {
					this->freeTimeSlots[i] = lastSlot - i;
				}
				return true;
			} 
			else {
				// no space for shifting
				return false;
			}
		} 

		// case 2: first batch
		if ( index == 0 ) {
			if ( this->queue[index].getC() < this->queue[index+1].getStart() ) {
				// shift batch right
				this->queue[index].setStart(this->queue[index].getStart() + 1);		// setStart also sets C
				// update free time slots
				this->freeTimeSlots[this->queue[index].getC() - 1] = 0;
				// update time slots [0, (start-1)]
				firstSlot = 0;
				lastSlot = this->queue[index].getStart();
				for(unsigned i = firstSlot; i < lastSlot; i++) {
					this->freeTimeSlots[i] = lastSlot - i;
				}
				return true;
			} 
			else {
				// no space for shifting
				return false;
			}
		}

		// case 3: neither 1st nor last batch
		if ( this->queue[index].getC() < this->queue[index+1].getStart() ) {
			// shift batch right
			this->queue[index].setStart(this->queue[index].getStart() + 1);			// setStart also sets C
			// update free time slots
			this->freeTimeSlots[this->queue[index].getC() - 1] = 0;
			// update time slots [ predecessor.c , (start-1) ]
			firstSlot = this->queue[index-1].getC();
			lastSlot = this->queue[index].getStart();
			for(unsigned i = firstSlot; i < lastSlot; i++) {
				this->freeTimeSlots[i] = lastSlot - i;
			}
			return true;
		}
		else {
			// no space for shifting
			return false;
		}
	}
	return false;
}
bool Machine::shiftBatchRight(int index, int shifts) {
	if(index >= 0 && index < this->queue.size() ) {
		if(shifts <= this->possRightShifts(index) ) {
			for(unsigned i = 0; i < shifts; i++) {
				this->shiftBatchRight(index);
			}
			return true;
		}
	}
	return false;
}
bool Machine::shiftBatchLeft(int index) {
	if(index >= 0 && index < this->queue.size() ) {
		int firstSlot, lastSlot;
		int tMax = this->freeTimeSlots.size();
		this->queue.sortBy_Start();

		if ( index == 0 ) {
			firstSlot = 0;
		}
		else {
			firstSlot = this->queue[index-1].getC();
		}

		if ( this->queue[index].getStart() > firstSlot && this->queue[index].getStart() > this->queue[index].r ) {
			// shift batch left
			this->queue[index].setStart(this->queue[index].getStart() - 1);		// setStart also sets C
			lastSlot = this->queue[index].getStart();
			// update free time slots
			this->freeTimeSlots[this->queue[index].getStart()] = 0;
			if ( this->queue[index].getC() == ( tMax - 1 ) ) {
				this->freeTimeSlots[this->queue[index].getC()] = 1;
			}
			else {
				this->freeTimeSlots[this->queue[index].getC()] = this->freeTimeSlots[this->queue[index].getC() + 1] + 1;
			}
			for(unsigned i = 0; i < (lastSlot - firstSlot); i++) {
				this->freeTimeSlots[firstSlot+i] = lastSlot - firstSlot - i;
			}
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}



bool Machine::shiftBatchLeft(int index, int shifts) {
	if(index >= 0 && index < this->queue.size() ) {
		if(shifts <= this->possLeftShifts(index) ) {
			for(unsigned i = 0; i < shifts; i++) {
				this->shiftBatchLeft(index);
			}
			return true;
		}
	}
	return false;
}
bool Machine::isShiftBatchRightPossible(int index){
	if(index >= 0 && index < this->queue.size() ) {
		int tMax = this->freeTimeSlots.size();
		int bMax = this->queue.size();
		this->queue.sortBy_Start();
		if(this->queue[index].getC() == tMax) {
			return false;
		}	
		if( index == (bMax-1) ) {
			return true;
		}

		if ( this->queue[index].getC() < this->queue[index+1].getStart() ) {
			return true;
		}
		return false;
	}
	return false;	
	
	
}
bool Machine::isShiftBatchLeftPossible(int index){
	if(index >= 0 && index < this->queue.size() ) {
		int tMax = this->freeTimeSlots.size();
		int bMax = this->queue.size();
		this->queue.sortBy_Start();
		if(this->queue[index].getStart() == 0 || this->queue[index].getStart() <= this->queue[index].r) {
			return false;
		}
		
		if( index == 0 ) {
			return true;
		}
	
		if ( this->queue[index].getStart() > this->queue[index-1].getC() ) {
			return true;
		}
		return false;

	}
	return false;
}
int Machine::possRightShifts(int index) {
	if(index >= 0 && index < this->queue.size() ) {
		int tMax = this->freeTimeSlots.size();
		int bMax = this->queue.size();
		this->queue.sortBy_Start();
		if( index == (bMax - 1) ) {
			return (tMax - this->queue[index].getC());
		}
		return (this->queue[index+1].getStart() - this->queue[index].getC());
	}
	return 0;
}
int Machine::possLeftShifts(int index) {
	if(index >= 0 && index < this->queue.size() ) {
		if(index == 0) {
			return max(this->queue[index].getStart() - this->queue[index].r, 0);
		}
		return (this->queue[index].getStart() - max(this->queue[index-1].getC(), this->queue[index].r));
	}
	return 0;
}

double Machine::twtFromBatchRightShift(int index){
	if( this->isShiftBatchRightPossible(index) ) {
		double oldTWT = this->queue[index].getTWT();
		double newTWT = oldTWT;
		for(unsigned i = 0; i < this->queue[index].numJobs; i++) {
			if(this->queue[index].getJob(i).d <= this->queue[index].getC()) {
				newTWT += this->queue[index].getJob(i).w;
			}
		}
		return newTWT - oldTWT;
	}
	else {
		cout << "WARNNING Machine::twtFromBatchRightShift(int) called, but impossible." << endl;
		return -1;
	}
}
double Machine::twtFromBatchLeftShift(int index){
	if( this->isShiftBatchLeftPossible(index) ) {
		double oldTWT = this->queue[index].getTWT();
		double newTWT = 0;
		for(unsigned i = 0; i < this->queue[index].numJobs; i++) {
			if(this->queue[index].getJob(i).d < this->queue[index].getC()) {
				newTWT -= this->queue[index].getJob(i).w;
			}
		}
		return newTWT - oldTWT;
	}
	cout << "WARNNING Machine::twtFromBatchLeftShift(int) called, but impossible." << endl;
	return -1;
}
double Machine::twtFromBatchRightShifts(int index, int shifts) {
	double oldTWT = this->queue[index].getTWT();
	double newTWT = 0;
	for(unsigned i = 0; i < this->queue[index].numJobs; i++) {
		newTWT += max( (this->queue[index].getC() + shifts)- this->queue[index].getJob(i).d, 0) * this->queue[index].getJob(i).w;
	}
	return newTWT - oldTWT;
}
double Machine::twtFromBatchLeftShifts(int index, int shifts) {
	double oldTWT = this->queue[index].getTWT();
	double newTWT = 0;
	for(unsigned i = 0; i < this->queue[index].numJobs; i++) {
		newTWT += max((this->queue[index].getC() - shifts)- this->queue[index].getJob(i).d, 0) * this->queue[index].getJob(i).w;
	}
	return newTWT - oldTWT;
}
double Machine::epcFromBatchRightShift(int index, const vector<int>& ECost){
	if( this->isShiftBatchRightPossible(index) )  {
		return ECost[this->queue[index].getC()] - ECost[this->queue[index].getStart()];
	}
	else {
		cout << "WARNNING Machine::epcFromBatchRightShifts(int, vector<int>) called, but impossible." << endl;
		return -1;
	}
}
double Machine::epcFromBatchLeftShift(int index, const vector<int>& ECost){
	if( this->isShiftBatchLeftPossible(index) ) {
		return ECost[this->queue[index].getStart()-1] - ECost[this->queue[index].getC()-1];
	}
	cout << "WARNNING Machine::epcFromBatchLeftShifts(int, vector<int>) called, but impossible." << endl;
	return -1;
}
double Machine::epcFromBatchRightShifts(int index, const vector<int>& ECost, int shifts) {
	double oldEPC = this->queue[index].getEPC(ECost);
	double newEPC = 0;
	for(unsigned i = 0; i < this->queue[index].pLot; i++) {			// Lot processing assumed
		newEPC += ECost[this->queue[index].getStart() + shifts + i];
	}
	return newEPC - oldEPC;
}
double Machine::epcFromBatchLeftShifts(int index, const vector<int>& ECost, int shifts) {
	double oldEPC = this->queue[index].getEPC(ECost);
	double newEPC = 0;
	for(unsigned i = 0; i < this->queue[index].pLot; i++) {			// Lot processing assumed
		newEPC += ECost[this->queue[index].getStart() - shifts + i];
	}
	return newEPC - oldEPC;
}
double Machine::twcFromBatchRightShift(int index){
	if( this->isShiftBatchRightPossible(index) ) {
		double oldTWC = this->queue[index].getTWC();
		double newTWC = oldTWC;
		for(unsigned i = 0; i < this->queue[index].numJobs; i++) {
			if(this->queue[index].getJob(i).d <= this->queue[index].getC()) {
				newTWC += this->queue[index].getJob(i).w;
			}
		}
		return newTWC - oldTWC;
	}
	else {
		cout << "WARNNING Machine::twcFromBatchRightShift(int) called, but impossible." << endl;
		return -1;
	}
}
double Machine::twcFromBatchLeftShift(int index){
	if( this->isShiftBatchLeftPossible(index) ) {
		double oldTWC = this->queue[index].getTWC();
		double newTWC = 0;
		for(unsigned i = 0; i < this->queue[index].numJobs; i++) {
			if(this->queue[index].getJob(i).d < this->queue[index].getC()) {
				newTWC -= this->queue[index].getJob(i).w;
			}
		}
		return newTWC - oldTWC;
	}
	cout << "WARNNING Machine::twcFromBatchLeftShift(int) called, but impossible." << endl;
	return -1;
}
double Machine::twcFromBatchRightShifts(int index, int shifts) {
	double oldTWC = this->queue[index].getTWC();
	double newTWC = 0;
	for(unsigned i = 0; i < this->queue[index].numJobs; i++) {
		newTWC += max( (this->queue[index].getC() + shifts)- this->queue[index].getJob(i).d, 0) * this->queue[index].getJob(i).w;
	}
	return newTWC - oldTWC;
}
double Machine::twcFromBatchLeftShifts(int index, int shifts) {
	double oldTWC = this->queue[index].getTWC();
	double newTWC = 0;
	for(unsigned i = 0; i < this->queue[index].numJobs; i++) {
		newTWC += max((this->queue[index].getC() - shifts)- this->queue[index].getJob(i).d, 0) * this->queue[index].getJob(i).w;
	}
	return newTWC - oldTWC;
}
void Machine::clear() {
	int iMax = this->queue.size();
	for(unsigned i = 0; i < iMax; i++) {
		this->queue.clear();
	}
	int tMax = this->freeTimeSlots.size();
	for(unsigned j = 0; j < tMax; j++) {
		this->freeTimeSlots[j] = tMax - j;
	}
	this->msp = 0;
}
void Machine::copy(const Machine* other) {
	if (this == other) return;
	Machine &oth = *((Machine*) (other));
	this->queue.clear();
	// copy the BatchSet
	this->queue.copy(&oth.queue);
}
void Machine::sortBatchesBy_Start(){
	this->queue.sortBy_Start();
}
void Machine::sortBatchesBy_wpLot(){
	this->queue.sortBy_wpLot(false);
}
void Machine::listSched(vector<Batch>& batches) {
}

void Machine::updateCompletionTimes() {
	int maxB = this->queue.size();

	// UPDATE: considered batch_r 08.11.2018
	int tempMSP = 0;
	for(unsigned i = 0; i < maxB; i++) {
		tempMSP = max(tempMSP, this->queue.getBatch(i).r) + this->queue.getBatch(i).pLot;
		this->queue.getBatch(i).setStart(tempMSP - this->queue.getBatch(i).pLot);
		//this->queue.getBatch(i).setC(tempMSP); // done with 'setStart()'
	}
}


void Machine::updateFreeTimeSlots() {
	if(!this->queue.empty()) {
		this->queue.sortBy_Start();
		int tMax = this->freeTimeSlots.size();
		int t = 0;
		int tNext = tMax;
		int lastBatch = this->queue.size() - 1;
		int nextBatch = 0;
		while (t < tMax) {
			if(nextBatch <= lastBatch) {
				tNext = this->queue[nextBatch].getStart();
			} else {
				tNext = tMax;	
				while( t < tNext ) {
					// set free slots till time horizon
					this->freeTimeSlots[t] = tNext - t;
					t++;
				}
				return;
			}
			// tNext is either the start of the next batch or the end of the time horizon
			
			while ( t < this->queue[nextBatch].getStart() ) {
			// set numbers of time slots before the next batch
				this->freeTimeSlots[t] = tNext - t;
				t++;
			} 
			// t == next batch´s start
			
			while( t < this->queue[nextBatch].getC()) {
				this->freeTimeSlots[t] = 0;
				t++;
			}
			// t == a batch´s completion
			nextBatch++;
			
		}
	} 
	// else, no batches in this queue
}
double Machine::applyLOM() {
	this->queue.sortBy_r(true);		// TODO: sorting should not imply a change of starting and completion times
	return this->getMSP();
}
double Machine::applyLOC(const vector<int>& ECost, int mspLimit) {
	int maxB = this->queue.size();
	this->queue.sortBy_Start();
	if(this->queue[maxB-1].getC() < mspLimit){		// TODO: batches might not be in correct order => implement BatchSet::sortByStart() 
		// First batch: shift right this batch if epc is reduced
		int currentStart = this->queue[maxB-1].getStart();
		int bestStart = currentStart;
		double bestEPC = this->queue[maxB-1].getEPC(ECost);
		for(unsigned t = (currentStart + 1); t <= (mspLimit - this->queue[maxB-1].pLot); t++){
			double altEPC = 0.0;
			for(unsigned h = 0; h < this->queue[maxB-1].pLot; h++) {
				altEPC += (double) ECost[t+h-1];
			}
			if(altEPC <= bestEPC) {
				bestStart = t;
				bestEPC = altEPC;
			}
		}
		if(bestStart > currentStart) {
			this->queue[maxB-1].setStart(bestStart);	
			// TODO: update the freeTimeSlots
		}
	} // otherwise this batch remains in its place
	for(int i = (maxB - 2); i >= 0; i--) {
		// Other batches: shift right if epc is reduced
		int cLimit = this->queue[i+1].getStart();
		if(cLimit <= this->queue[i].getC()) {
			continue;	// leave this batch in its position, continue with next batches	
		}
		int currentStart = this->queue[i].getStart();
		int bestStart = currentStart;
		double bestEPC = this->queue[i].getEPC(ECost);
		for(unsigned t = (currentStart + 1); t <= (cLimit - this->queue[i].pLot); t++){
			double altEPC = 0.0;
			for(unsigned h = 0; h < this->queue[i].pLot; h++) {
				altEPC += (double) ECost[t+h-1];
			}
			if(altEPC <= bestEPC) {
				bestStart = t;
				bestEPC = altEPC;
			}
		}
		if(bestStart > currentStart) {
			this->queue[i].setStart(bestStart);
		}
	}
	return this->getEPC(ECost);
}
double Machine::applyLOCT(const vector<int>& ECost) {
	// Local optimization of EPC conserving a given tardiness
	int maxT = ECost.size()+1;
	int maxB = this->queue.size();
	this->queue.sortBy_Start();
	int cLimit = min<int>(this->queue[maxB-1].earliestD, maxT);
	// consider moving the last batch right, if its c < d 
	if(this->queue[maxB-1].getC() < cLimit) {
		int currentStart = this->queue[maxB-1].getStart();
		int bestStart = currentStart;
		double bestEPC = this->queue[maxB-1].getEPC(ECost);
		for(unsigned t = (currentStart + 1); t <= (cLimit - this->queue[maxB-1].pLot); t++){
			double altEPC = 0.0;
			for(unsigned h = 0; h < this->queue[maxB-1].pLot; h++) {
				altEPC += (double) ECost[t+h-1];
			}
			if(altEPC <= bestEPC) {
				bestStart = t;
				bestEPC = altEPC;
			}
		}
		if(bestStart > currentStart) {
			this->queue[maxB-1].setStart(bestStart);
		}
	}
	
	// consider moving the other batches right
	for(int i = (maxB-2); i >= 0; i--) {
		int currentStart = this->queue[i].getStart();
		int bestStart = currentStart;
		int cLimit = min<int>(this->queue[i].earliestD, this->queue[i+1].getStart());
		if(bestStart < cLimit) {
			double bestEPC = this->getEPC(ECost);
			for(unsigned t = (currentStart + 1); t <= (cLimit - this->queue[i].pLot); t++) {
				double altEPC = 0.0;
				for(unsigned h = 0; h < this->queue[i].pLot; h++) {
					altEPC += (double) ECost[t+h-1];
				}
				if(altEPC <= bestEPC) {
					bestStart = t;
					bestEPC = altEPC;
				}
			}
			if(bestStart > currentStart) {
				this->queue[i].setStart(bestStart);
			}
		}
	}
	// TODO: update freeTimeSlots
	return this->getEPC(ECost);
}
double Machine::improveTWT(){
	// consider batches from left to right, shift batch left if both EPC and TWT are not increased
	int maxB;
	double epcImprovement;
	double twtImprovement;
	double sumTwtImprovement = 0;
	maxB = this->queue.size(); 
	for(unsigned j = 0; j < maxB; j++) {
		if(this->isShiftBatchLeftPossible(j)) {
			if(Global::gv_eRates <= 1) {
				epcImprovement = this->epcFromBatchLeftShift(j, Global::problem->e1);
			}
			else {
				epcImprovement = this->epcFromBatchLeftShift(j, Global::problem->e2);
			}
			twtImprovement = this->twtFromBatchLeftShift(j);
			while(this->isShiftBatchLeftPossible(j) && twtImprovement <= 0 && epcImprovement <= 0) {
				sumTwtImprovement += twtImprovement;
				this->shiftBatchLeft(j);
				if(this->isShiftBatchLeftPossible(j)) {
					if(Global::gv_eRates <= 1) {
						epcImprovement = this->epcFromBatchLeftShift(j, Global::problem->e1);
					}
					else {
						epcImprovement = this->epcFromBatchLeftShift(j, Global::problem->e2);
					}
					twtImprovement = this->twtFromBatchLeftShift(j);
				}
			}
		}
	}
	return sumTwtImprovement;
}
double Machine::improveEPC() {
	// consider batches from right to left, shift batch right if both EPC and TWT are not increased
	int maxB;
	double epcImprovement;
	double twtImprovement;
	double sumEpcImprovement = 0;
	maxB = this->queue.size();
	for(int j = (maxB - 1); j >= 0; j--) {
		if(this->isShiftBatchRightPossible(j)) {
			if(Global::gv_eRates <= 1) {
				epcImprovement = this->epcFromBatchRightShift(j, Global::problem->e1);
			}
			else {
				epcImprovement = this->epcFromBatchRightShift(j, Global::problem->e2);
			}
			twtImprovement = this->twtFromBatchRightShift(j);
			while(this->isShiftBatchRightPossible(j) && twtImprovement <= 0 && epcImprovement <= 0) {
				sumEpcImprovement += epcImprovement;
				this->shiftBatchRight(j);
				if(this->isShiftBatchRightPossible(j)) {
					if(Global::gv_eRates <= 1) {
						epcImprovement = this->epcFromBatchRightShift(j, Global::problem->e1);
					}
					else {
						epcImprovement = this->epcFromBatchRightShift(j, Global::problem->e2);
					}
					twtImprovement = this->twtFromBatchRightShift(j);
				}
			}
		}
	}
	return sumEpcImprovement;
}
double Machine::improveTWC(){
	// consider batches from left to right, shift batch left if both EPC and TWC are not increased
	int maxB;
	double epcImprovement;
	double twcImprovement;
	double sumTwcImprovement = 0;
	maxB = this->queue.size(); 
	for(unsigned j = 0; j < maxB; j++) {
		if(this->isShiftBatchLeftPossible(j)) {
			if(Global::gv_eRates <= 1) {
				epcImprovement = this->epcFromBatchLeftShift(j, Global::problem->e1);
			}
			else {
				epcImprovement = this->epcFromBatchLeftShift(j, Global::problem->e2);
			}
			twcImprovement = this->twcFromBatchLeftShift(j);
			while(this->isShiftBatchLeftPossible(j) && twcImprovement <= 0 && epcImprovement <= 0) {
				sumTwcImprovement += twcImprovement;
				this->shiftBatchLeft(j);
				if(this->isShiftBatchLeftPossible(j)) {
					if(Global::gv_eRates <= 1) {
						epcImprovement = this->epcFromBatchLeftShift(j, Global::problem->e1);
					}
					else {
						epcImprovement = this->epcFromBatchLeftShift(j, Global::problem->e2);
					}
					twcImprovement = this->twcFromBatchLeftShift(j);
				}
			}
		}
	}
	return sumTwcImprovement;
}
double Machine::improveCombinedGreedy(double ratio) {
	// step 1: consider batches from left to right, shift batch left if ratio * TWT + (1-ratio) * EPC is not increased
	int maxB = this->queue.size();
	double epcImprovement;
	double twtImprovement;
	double sumEpcImprovement = 0;
	double sumTwtImprovement = 0;
	double newEPC;
	for(unsigned j = 0; j < maxB; j++) {
		if(this->isShiftBatchLeftPossible(j)) {
			if(Global::gv_eRates <= 1) {
				epcImprovement = this->epcFromBatchLeftShift(j, Global::problem->e1);
			}
			else {
				epcImprovement = this->epcFromBatchLeftShift(j, Global::problem->e2);
			}
			twtImprovement = this->twtFromBatchLeftShift(j);
			while(this->isShiftBatchLeftPossible(j) && (ratio * twtImprovement + (1-ratio) * epcImprovement) <= 0 ) {
				sumEpcImprovement += epcImprovement;
				sumTwtImprovement += twtImprovement;
				this->shiftBatchLeft(j);
				if(Global::gv_eRates <= 1) {
					epcImprovement = this->epcFromBatchLeftShift(j, Global::problem->e1);
				}
				else {
					epcImprovement = this->epcFromBatchLeftShift(j, Global::problem->e2);
				}
				twtImprovement = this->twtFromBatchLeftShift(j);
			}
		}
	}
	// step 2: consider batches from right to left, shift batch left if ratio * TWT + (1-ratio) * EPC is not increased
	for(int j = (maxB - 1); j >= 0; j--) {
		if(this->isShiftBatchRightPossible(j)) {
			if(Global::gv_eRates <= 1) {
				epcImprovement = this->epcFromBatchRightShift(j, Global::problem->e1);
			}
			else {
				epcImprovement = this->epcFromBatchRightShift(j, Global::problem->e2);
			}
			twtImprovement = this->twtFromBatchRightShift(j);
			while(this->isShiftBatchRightPossible(j) && (ratio * twtImprovement + (1-ratio) * epcImprovement) <= 0) {
				sumEpcImprovement += epcImprovement;
				sumTwtImprovement += twtImprovement;
				this->shiftBatchRight(j);
				if(Global::gv_eRates <= 1) {
					epcImprovement = this->epcFromBatchRightShift(j, Global::problem->e1);
				}
				else {
					epcImprovement = this->epcFromBatchRightShift(j, Global::problem->e2);
				}
				twtImprovement = this->twtFromBatchRightShift(j);
			}
		}
	}
	return (ratio * sumTwtImprovement + (1-ratio) * sumEpcImprovement);
}
double Machine::improveCombined(double ratio) {
	int maxB = this->queue.size();
	double epcImprovement;
	double twtImprovement;
	double sumCombined = 0;
	double newEPC;
	int bestShifts;
	int possShifts;				
	double bestCombined;
	double tempCombined;
	// step 1: consider batches from right to left, shift batch right so that best value of ratio * TWT + (1-ratio) * EPC is achieved
	for(int j = maxB-1; j >= 0; j--) {
		bestShifts = 0;
		bestCombined = 0.0;
		tempCombined = 0.0;
		possShifts = this->possRightShifts(j);
		for(unsigned i = 1; i <= possShifts; i++) {
			if(Global::gv_eRates <= 1) {
				epcImprovement = this->epcFromBatchRightShifts(j, Global::problem->e1, i);
			}
			else {
				epcImprovement = this->epcFromBatchRightShifts(j, Global::problem->e2, i);
			}
			twtImprovement = this->twtFromBatchRightShifts(j, i);
			tempCombined = ratio * twtImprovement + (1-ratio) * epcImprovement;
			if(tempCombined <= bestCombined) {
				bestCombined = tempCombined;
				bestShifts = i;
			}
		}
		sumCombined += bestCombined;
		this->shiftBatchRight(j, bestShifts);
	}
	// step 2: consider batches from left to right, shift batch left so that best value of ratio* TWT + (1-ratio) * EPC is achieved
	for(unsigned j = 0; j < maxB; j++) {
		bestShifts = 0;
		bestCombined = 0.0;
		tempCombined = 0.0;
		possShifts = this->possLeftShifts(j);
		for(unsigned k = 1; k <= possShifts; k++) {
			if(Global::gv_eRates <= 1) {
				epcImprovement = this->epcFromBatchLeftShifts(j, Global::problem->e1, k);
			}
			else {
				epcImprovement = this->epcFromBatchLeftShifts(j, Global::problem->e2, k);
			}
			twtImprovement = this->twtFromBatchLeftShifts(j, k);
			tempCombined = ratio * twtImprovement + (1-ratio) * epcImprovement;
			if(tempCombined <= bestCombined) {
				bestCombined = tempCombined;
				bestShifts = k;
			}
		}
		sumCombined += bestCombined;
		this->shiftBatchLeft(j, bestShifts);
	}
	return sumCombined;
}

bool Machine::isEpcSavingPossible(const vector<int>& Ecost) {
	int iMax = this->queue.size();
	for(int i = iMax-1; i >= 0; i--) {
		int lShifts = this->possLeftShifts(i);
		int rShifts = this->possRightShifts(i);
		// consider single and/or multiple batch right shifts
		if(rShifts > 0) {
			for(int n = 0; n <= i; n++) {			// number of batches to be shifted	
				for(int r = 1; r <= rShifts; r++) {	// number of right shifts
					double cumEpcSavings = 0.0;
					cumEpcSavings += this->epcFromBatchRightShifts(i, Ecost, r);
					for(int m = 1; m <= n; m++) {	// current batch to be shifted
						cumEpcSavings += this->epcFromBatchRightShifts(i - m, Ecost, this->possRightShifts(i-m) + r);
						if(cumEpcSavings < 0) {
							return true;
						}
					}
				}
			}
		}
		// consider single batch left shifts
		for(int l = 1; l <= lShifts; l++) {
			if(this->epcFromBatchLeftShifts(i, Ecost, l) < 0) {
				return true;
			}
		}
	}
	// TODO: consider simultaneous Left-shift of two or more batches, BEWARE: r must be respected
	return false;
}
double Machine::minTwtIncreaseByEpcSaving(const vector<int>& Ecost){
	double minTwtIncrease = 999999;
	if(this->isEpcSavingPossible(Ecost)) {
		int iMax = this->queue.size();
		for(int i = 0; i < iMax; i++) {
			int lShifts = this->possLeftShifts(i);
			for(int l = 1; l <= lShifts; l++) {
			// check left shifts
				if(this->epcFromBatchLeftShifts(i, Ecost, l) < 0) {
					// left shift decreases EPC
					if(this->twtFromBatchLeftShifts(i, l) < minTwtIncrease) {
						// best option found so far
						minTwtIncrease = this->twtFromBatchLeftShifts(i, l);
					}
				}
			}
		}
		for(int i = iMax - 1; i >= 0; i--) {
			// consider multiple batch right shifts (only consider n batches if n-1 batches cannot save EPC)
			int rShifts = this->possRightShifts(i);
			if(rShifts > 0) {
				for(int n = 0; n <= i; n++) {			// number of batches to be shifted
					bool epcSavingPossible = false;
					for(int r = 1; r <= rShifts; r++) {	// number of right shifts
						double cumEpcSavings = 0.0;
						double cumTwtIncrease = 0.0;
						cumEpcSavings += this->epcFromBatchRightShifts(i, Ecost, r);
						cumTwtIncrease += this->twtFromBatchRightShifts(i, r);
						for(int m = 1; m <= n; m++) {	// current batch to be shifted
							cumEpcSavings += this->epcFromBatchRightShifts(i - m, Ecost, this->possRightShifts(i-m) + r);
							cumTwtIncrease += this->twtFromBatchRightShifts(i - m, this->possRightShifts(i-m) + r);
							if(cumEpcSavings < 0) {
								// right shift decreases EPC
								epcSavingPossible = true;
								if(cumTwtIncrease < minTwtIncrease) {
										// best option found so far
										minTwtIncrease = cumTwtIncrease;	
								}
							}
						}		
						if(epcSavingPossible) break; // no need to consider additional right shifts 
					}
					if(epcSavingPossible) break;	 // no need to consider additional batches
				}
			}
		}
	} 
	return minTwtIncrease;
}
double Machine::minTwcIncreaseByEpcSaving(const vector<int>& Ecost){
	double minTwcIncrease = 999999;
	if(this->isEpcSavingPossible(Ecost)) {
		int iMax = this->queue.size();
		for(int i = 0; i < iMax; i++) {
			int lShifts = this->possLeftShifts(i);
			for(int l = 1; l <= lShifts; l++) {
			// check left shifts
				if(this->epcFromBatchLeftShifts(i, Ecost, l) < 0) {
					// left shift decreases EPC
					if(this->twcFromBatchLeftShifts(i, l) < minTwcIncrease) {
						// best option found so far
						minTwcIncrease = this->twcFromBatchLeftShifts(i, l);
					}
				}
			}
		}
		for(int i = iMax - 1; i >= 0; i--) {
			// consider multiple batch right shifts (only consider n batches if n-1 batches cannot save EPC)
			int rShifts = this->possRightShifts(i);
			if(rShifts > 0) {
				for(int n = 0; n <= i; n++) {			// number of batches to be shifted
					bool epcSavingPossible = false;
					for(int r = 1; r <= rShifts; r++) {	// number of right shifts
						double cumEpcSavings = 0.0;
						double cumTwcIncrease = 0.0;
						cumEpcSavings += this->epcFromBatchRightShifts(i, Ecost, r);
						cumTwcIncrease += this->twcFromBatchRightShifts(i, r);
						for(int m = 1; m <= n; m++) {	// current batch to be shifted
							cumEpcSavings += this->epcFromBatchRightShifts(i - m, Ecost, this->possRightShifts(i-m) + r);
							cumTwcIncrease += this->twcFromBatchRightShifts(i - m, this->possRightShifts(i-m) + r);
							if(cumEpcSavings < 0) {
								// right shift decreases EPC
								epcSavingPossible = true;
								if(cumTwcIncrease < minTwcIncrease) {
										// best option found so far
										minTwcIncrease = cumTwcIncrease;	
								}
							}
						}		
						if(epcSavingPossible) break; // no need to consider additional right shifts 
					}
					if(epcSavingPossible) break;	 // no need to consider additional batches
				}
			}
		}
	} 
	return minTwcIncrease;
}
void Machine::ImproveEpcWithMinTwtImpact(const vector<int>& Ecost){
	int iMax = this->queue.size();
	
	// EPC decrease w/ minimal TWT increase is achieved if <batchToBeShifted> is shifted <directionLeft> by <numberOfShifts> positions.
	int batchToBeShifted = 0;	
	int neighboursToBeShifted = 0;
	int numberOfShifts = 0;
	bool directionLeft = false;	

	double minTwtIncrease = 999999;
	for(int i = 0; i < iMax; i++) {
		int lShifts = this->possLeftShifts(i);
		for(int l = 1; l <= lShifts; l++) {
			// check left shifts
			if(this->epcFromBatchLeftShifts(i, Ecost, l) < 0) {
				// left shift decreases EPC
				if(this->twtFromBatchLeftShifts(i, l) < minTwtIncrease) {
					// best option found so far
					minTwtIncrease = this->twtFromBatchLeftShifts(i, l);
					batchToBeShifted = i;
					numberOfShifts = l;
					directionLeft = true;
				}
			}
		}
	}
	for(int i = iMax-1; i > 0; i--) {
		int rShifts = this->possRightShifts(i);
		if(rShifts > 0) {
			for(int n = 0; n <= i; n++) {						// number of batches to be shifted
				bool epcSavingPossible = false;
				for(int r = 1; r <= rShifts; r++) {				// number of right shifts
					double cumEpcSavings = 0.0;
					double cumTwtIncrease = 0.0;
					cumEpcSavings += this->epcFromBatchRightShifts(i, Ecost, r);
					cumTwtIncrease += this->twtFromBatchRightShifts(i, r);
					for(int m = 1; m <= n; m++) {				// current batch to be shifted
						cumEpcSavings += this->epcFromBatchRightShifts(i - m, Ecost, this->possRightShifts(i-m) + r);
						cumTwtIncrease += this->twtFromBatchRightShifts(i - m, this->possRightShifts(i-m) + r);
						if(cumEpcSavings < 0) {
							// right shift decreases EPC
							epcSavingPossible = true;
							if(cumTwtIncrease < minTwtIncrease) {
								// best option found so far
								minTwtIncrease = cumTwtIncrease;
								batchToBeShifted = i;
								neighboursToBeShifted = n;
								numberOfShifts = r;
								directionLeft = false;
							}
						}
					}
				if(epcSavingPossible) break; // no need to consider additional right shifts 
				}
			if(epcSavingPossible) break;	 // no need to consider additional batches
			}
		}
	}
	// actually shift batch(es)
	if(directionLeft) {
		this->shiftBatchLeft(batchToBeShifted, numberOfShifts);
	}
	else {
		this->shiftBatchRight(batchToBeShifted, numberOfShifts);
		for(int d = 1; d <= neighboursToBeShifted; d++) {
			this->shiftBatchRight(batchToBeShifted - d, this->possRightShifts(batchToBeShifted - d));
		}
	}
}
void Machine::ImproveEpcWithMinTwcImpact(const vector<int>& Ecost){
	int iMax = this->queue.size();
	
	// EPC decrease w/ minimal TWT increase is achieved if <batchToBeShifted> is shifted <directionLeft> by <numberOfShifts> positions.
	int batchToBeShifted = 0;	
	int neighboursToBeShifted = 0;
	int numberOfShifts = 0;
	bool directionLeft = false;	

	double minTwcIncrease = 999999;
	for(int i = 0; i < iMax; i++) {
		int lShifts = this->possLeftShifts(i);
		for(int l = 1; l <= lShifts; l++) {
			// check left shifts
			if(this->epcFromBatchLeftShifts(i, Ecost, l) < 0) {
				// left shift decreases EPC
				if(this->twcFromBatchLeftShifts(i, l) < minTwcIncrease) {
					// best option found so far
					minTwcIncrease = this->twcFromBatchLeftShifts(i, l);
					batchToBeShifted = i;
					numberOfShifts = l;
					directionLeft = true;
				}
			}
		}
	}
	for(int i = iMax-1; i > 0; i--) {
		int rShifts = this->possRightShifts(i);
		if(rShifts > 0) {
			for(int n = 0; n <= i; n++) {						// number of batches to be shifted
				bool epcSavingPossible = false;
				for(int r = 1; r <= rShifts; r++) {				// number of right shifts
					double cumEpcSavings = 0.0;
					double cumTwcIncrease = 0.0;
					cumEpcSavings += this->epcFromBatchRightShifts(i, Ecost, r);
					cumTwcIncrease += this->twcFromBatchRightShifts(i, r);
					for(int m = 1; m <= n; m++) {				// current batch to be shifted
						cumEpcSavings += this->epcFromBatchRightShifts(i - m, Ecost, this->possRightShifts(i-m) + r);
						cumTwcIncrease += this->twcFromBatchRightShifts(i - m, this->possRightShifts(i-m) + r);
						if(cumEpcSavings < 0) {
							// right shift decreases EPC
							epcSavingPossible = true;
							if(cumTwcIncrease < minTwcIncrease) {
								// best option found so far
								minTwcIncrease = cumTwcIncrease;
								batchToBeShifted = i;
								neighboursToBeShifted = n;
								numberOfShifts = r;
								directionLeft = false;
							}
						}
					}
				if(epcSavingPossible) break; // no need to consider additional right shifts 
				}
			if(epcSavingPossible) break;	 // no need to consider additional batches
			}
		}
	}
	// actually shift batch(es)
	if(directionLeft) {
		this->shiftBatchLeft(batchToBeShifted, numberOfShifts);
	}
	else {
		this->shiftBatchRight(batchToBeShifted, numberOfShifts);
		for(int d = 1; d <= neighboursToBeShifted; d++) {
			this->shiftBatchRight(batchToBeShifted - d, this->possRightShifts(batchToBeShifted - d));
		}
	}
}
/// objective function values
double Machine::getTCT() {			
	// total completion time = sum of jobs´ c
	double tct = 0.0;
	int maxB = this->queue.size();
	if(maxB > 0) {
		for(unsigned i = 0; i < maxB; i++) {
			int maxJ = this->queue[i].numJobs;
			for(unsigned j = 0; j < maxJ; j++) {
				tct += (double) this->queue[i].getJob(j).getC();
			}
		}
	}
	return tct;
}

double Machine::getTWC() {			
	// total weighted completion time = weighted sum of jobs´ c
	double twct = 0.0;
	int maxB = this->queue.size();
	for(unsigned i = 0; i < maxB; i++) {
		twct += this->queue[i].getTWC();
	}

	return twct;
}
double Machine::getTT() {			
	// total tardiness = sum of batches´ TT
	double tt = 0.0;
	int maxB = this->queue.size();
	for(unsigned i = 0; i < maxB; i++) {
		tt += this->queue[i].getTT();
	}
	return tt;
}
double Machine::getTWT() {			
	// total weighted tardiness = weighted sum of jobs´ (c-d)+
	double twt = 0.0;
	int maxB = this->queue.size();
	for(unsigned i = 0; i < maxB; i++) {
		twt += this->queue[i].getTWT();
	}
	return twt;
}
double Machine::getMSP() {			
	// makespan = completion time of the last batch
	// TODO: consider gaps in the schedule: MSP = the maximum completion, done in BatchSet::getMSP 30/07/2018, tbc
	return this->queue.getMSP();
}


int Machine::getLoad(){
	int load = 0;
	int maxB = this->queue.size();
	for(int i = 0; i < maxB; i++) {
		load += this->queue[i].pLot;
	}
	return load;
}
double Machine::getEPC(const std::vector<int> &Ecost) {
	// electricity power cost = power cost of the processing intervals of all batches
	double epc = 0.0;
	int maxB = this->queue.size();
	for(unsigned i = 0; i < maxB; i++) {
		epc += this->queue[i].getEPC(Ecost);
	}
	return epc;
}