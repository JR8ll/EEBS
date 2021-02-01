#include "TWTEPC_2Solution.h"

TWTEPC_2Solution::TWTEPC_2Solution() {			// standard constructor => random initialization of one individual
	_valid = true;
	_twtPref = (float) rand() / RAND_MAX;	// preference for TWT minimization is set U~[0, 1]
	ObjectiveValues.resize(2);	// TODO put 2 into Global::parameters
	int cur_batch;

	// initialize a machineset/schedule for evaluation of the chromosome
	this->_schedule.clear();
	for(unsigned i = 0; i < Global::problem->m; i++) {
		Machine tempM;
		this->_schedule.addMachine(tempM);
	}

	ObjectiveValues.resize(2);				// TODO put 2 into Global::parameters
	
	if(_chromosome.size() != Global::problem->n) {
		// if chromosome not empty, then delete
		for(unsigned i = 0; i < _chromosome.size(); i++) {
			delete _chromosome[i];
			_chromosome[i] = NULL;
		}
		// create new chromosome
		_chromosome.resize(Global::problem->n);
		for(unsigned i = 0; i < Global::problem->n; i++) {
			_chromosome[i] = new Batch(Global::problem->k);
		}
	}
	else {
		// clean chromosome
		_chromosome.clean();
		for(unsigned i = 0; i < Global::problem->n; i++) {
			_chromosome[i]->capacity = Global::problem->k;
		}
	}	

	// set batches r, earliestStart and latestC
	int bMax = Global::problem->n;
	int mMax = Global::problem->m;
	int tInterval = ceil ( (double) Global::problem->T / ceil( (double) bMax / (double) mMax));		// 14/08/2018 ceil(... replaces floor(...
	for(int i = 0; i <  bMax; i++) {
			this->_chromosome[i]->r = tInterval * floor((double) i / (double) mMax);
			this->_chromosome[i]->earliestStart = this->_chromosome[i]->r;
			this->_chromosome[i]->latestC = Global::problem->T;
	}

	// distribute jobs between batches in random manner
	for(unsigned i = 0; i < bMax; i++){
		int timesTried = 0;
		bool assigned = false;
		while(!assigned) {
			cur_batch = rand() % bMax;
			// bis 21092018: if(Global::problem->jobs[i].r <= this->_chromosome[cur_batch]->earliestStart && Global::problem->jobs[i].p <= (this->_chromosome[cur_batch]->latestC - this->_chromosome[cur_batch]->earliestStart)) {
			int nextBatchesR = 0;
			if(cur_batch < bMax - mMax) {
				nextBatchesR = this->_chromosome[cur_batch + mMax]->earliestStart;
			}
			else {
				nextBatchesR = this->_chromosome[cur_batch]->latestC - Global::problem->jobs[i].p;
			}
			
			if(Global::problem->jobs[i].r < nextBatchesR && Global::problem->jobs[i].p <= (this->_chromosome[cur_batch]->latestC - max(this->_chromosome[cur_batch]->earliestStart, Global::problem->jobs[i].r))) {

				// try to add the job
				if(this->_chromosome[cur_batch]->addJob(&Global::problem->jobs[i])) {
					assigned = true;
					// set latestC for batches processed earlier
					for(int p = (cur_batch-mMax); p >= 0; p = p - mMax) {
						this->_chromosome[p]->latestC = min(this->_chromosome[cur_batch]->earliestStart, this->_chromosome[p]->latestC);
					}
					// set earliestStart for batches processed afterwards
					for(int q = cur_batch+mMax; q < bMax; q = q + mMax) {
						this->_chromosome[q]->earliestStart = max(this->_chromosome[cur_batch]->earliestStart + this->_chromosome[cur_batch]->pLot, this->_chromosome[q]->earliestStart);
					}
				}
			} 
			timesTried++;
			if(timesTried > (bMax * 10000)) {		// one job cannot be assigned to a batch 
				this->_valid = false;
				break;
			}
		}
	}
	
	//this->_chromosome.moveNonEmptyBatchesToFront();
	// decode the chromosome and set objective values
	//this->setObjectiveValues();

	// VARIATION
	//this->consolidateBatches();

	// VARIATION
	//this->setObjectiveValues_ASAP();
	this->setObjectiveValues();
}							

TWTEPC_2Solution::TWTEPC_2Solution(TWTEPC_2Solution & p1, TWTEPC_2Solution & p2) {	// Recombination (Crossover)
	_valid = true;
	_twtPref = p2._twtPref;							// inherit Objective function preference from p2
	
	// initialize chromosome
	this->_chromosome.resize(Global::problem->n);	// TODO: check
	for(unsigned i = 0; i < Global::problem->n; i++) {
		this->_chromosome[i] = new Batch(Global::problem->k);
	}

	// set batches r, earliestStart and latestC
	int bMax = Global::problem->n;
	int mMax = Global::problem->m;
	int tInterval = ceil ( (double) Global::problem->T / ceil( (double) bMax / (double) mMax));		// 14/08/2018 ceil(... replaces floor(...
	for(int i = 0; i <  bMax; i++) {
			this->_chromosome[i]->r = tInterval * floor((double) i / (double) mMax);
			this->_chromosome[i]->earliestStart = this->_chromosome[i]->r;
			this->_chromosome[i]->latestC = Global::problem->T;
	}

	ObjectiveValues.resize(2);						// TODO put 2 into Global::parameters

	// initialize a machineset/schedule for evaluation of the chromosome
	this->_schedule.clear();
	for(unsigned i = 0; i < Global::problem->m; i++) {
		Machine tempM;
		this->_schedule.addMachine(tempM);
	}


	/**
	 * Algorithm:
	 * 1. Select randomly two crossing points in the first parent.
	 * 2. Select randomly two crossing points in the second parent.
	 * 3. Replace the crossing site of the first parent with the crossing site
	 *		of the second one.
	 * 4. Remove the repeated orders from the part of the second parent.
	 * 5. Reinsert the uninserted orders.
	 */

	

	int cs11, cs12, cs21, cs22;
	set<int> contained; // IDs of the orders already present in the first parent
	vector<int> missing;
	set<int> all;
	set<int> ids_tmp;
	int cs1_end;
	int cs2_end;

	// Define orders which are present int the parents
	all.clear();
	for (int i = 0; i < p1._chromosome.size(); i++) {
		int jobsInThisBatch = p1._chromosome[i]->numJobs;
		for (unsigned j = 0; j < jobsInThisBatch; j++) {
			all.insert(p1._chromosome[i]->getJob(j).id);
		}
	}

	// 1. Select two crossing sites of the first parent
	//Debugger::iDebug("Selecting crossing sites for p1 ...");
	//p1.write(cout);

	cs1_end = p1._chromosome.size() - 1;	

	cs11 = rand() % (cs1_end + 1); 
	if (cs1_end > 0) {
		do {
			cs12 = rand() % (cs1_end + 1);
		} while (cs12 == cs11);
		if (cs11 > cs12) swap(cs11, cs12);
	} else {
		cs12 = cs11;
	}

	// 2. Select two crossing sites of the second parent

	cs2_end = p2._chromosome.size() - 1;

	cs21 = rand() % (cs2_end + 1);
	if (cs2_end > 0) {
		do {
			cs22 = rand() % (cs2_end + 1); 
		} while (cs22 == cs21);
		if (cs21 > cs22) swap(cs21, cs22);
	} else {
		cs22 = cs21;
	}

	// Collect order IDs present in the p2's part
	for (int i = cs21; i <= cs22; i++) {
		int jobsInThisBatch = p2._chromosome[i]->numJobs;
		for (unsigned j = 0; j < jobsInThisBatch; j++) {
			contained.insert(p2._chromosome[i]->getJob(j).id);
		}
	}

	// Copy p2's batches
	for (int i = cs21; i <= cs22; i++) {
		this->_chromosome[i]->copy(p2._chromosome[i]); // = new Batch(p2._chromosome[i]); 
		// set latestC for batches processed earlier
		for(int p = (i-mMax); p >= 0; p = p - mMax) {
			this->_chromosome[p]->latestC = min(this->_chromosome[i]->earliestStart, this->_chromosome[p]->latestC);
		}
		
		// set earliestStart for batch processed afterwards
		for(int q = i + mMax; q < bMax; q = q + mMax) {
			this->_chromosome[q]->earliestStart = max(this->_chromosome[i]->earliestStart + this->_chromosome[i]->pLot, this->_chromosome[q]->earliestStart);
		}
	}

	// Copy p1's batches eliminating the elements from contained : first part
	for (int i = 0; i < cs11; i++) {
		// do not copy empty batches, JR 13/08/2018
		if( !p1._chromosome[i]->empty() ) {
			// check if earliestStart and latestC can be respected
			if( (this->_chromosome[i]->latestC - this->_chromosome[i]->earliestStart) >= p1._chromosome[i]->pLot ) {
				int actualStart = this->_chromosome[i]->earliestStart;
				this->_chromosome[i]->copy(p1._chromosome[i]); 	
				this->_chromosome[i]->setStart(actualStart);

				// set latestC for batches processed before
				if(i >= mMax) {
					for(int p = (i-mMax); p >= 0; p = p - mMax) {
						this->_chromosome[p]->latestC = min(this->_chromosome[i]->earliestStart, this->_chromosome[p]->latestC);
					}
				}
				// set earliestStart for batch processed afterwards
				if(i < bMax - mMax) {
					this->_chromosome[i+mMax]->earliestStart = max(this->_chromosome[i]->earliestStart + this->_chromosome[i]->pLot, this->_chromosome[i+mMax]->earliestStart);
				}

				if (this->_chromosome[i]->intersects(contained)) { // ch.batches[i]->intersects(contained)) {
					// Eliminate some jobs
					for (set<int>::iterator iter = contained.begin(); iter != contained.end(); iter++) {
						if (this->_chromosome[i]->contains(*iter)) {
							this->_chromosome[i]->erase(*iter);
						}
					}

					// if the batch is empty, expand earliestStart of i+maxM and latestC of i-maxM if necessary
					if(this->_chromosome[i]->numJobs <= 0) {
						// expand latestC for batch processed before
						bool hasSuccessor = false;
						if(i >= mMax) {				// not the first job
							if(i < bMax - mMax) {	// not the last job
								for(int s = i + mMax; s < bMax; s = s+mMax) {
									if(this->_chromosome[s]->numJobs > 0) {	// successor
										hasSuccessor = true;
										this->_chromosome[i-mMax]->latestC = this->_chromosome[s]->earliestStart;
										break;
									}
								}
								if(!hasSuccessor) {
									this->_chromosome[i-mMax]->latestC = Global::problem->T;
								}
							}
							this->_chromosome[i-mMax]->latestC = this->_chromosome[i]->earliestStart;	
						}
						// expand earliestStart for batch processed afterwards
						if(i < bMax - mMax) {
							this->_chromosome[i+mMax]->earliestStart = tInterval * floor((double) (i+mMax) / (double) mMax);
						}
					}
				}
			} // else, batch cannot be placed because available timeSlot is too short
		} // else, batch is empty and is not copied
	}

	// Copy p1's batches eliminating the elements from contained : second part
	for (int i = cs12 + 1; i <= cs1_end; i++) {
		// do not copy empty batches, JR 13/08/2018
		if( !p1._chromosome[i]->empty() ) {
			// TODO: check if earliestStart and latestC can be respected
			if( (this->_chromosome[i]->latestC - this->_chromosome[i]->earliestStart) >= p1._chromosome[i]->pLot ) {
				int actualStart = this->_chromosome[i]->earliestStart;
				this->_chromosome[i]->copy(p1._chromosome[i]); // = new Batch(p1._chromosome[i]);
				this->_chromosome[i]->setStart(actualStart);
				
				// set latestC for batch processed before
				if(i >= mMax) {
					for(int p = (i-mMax); p >= 0; p = p - mMax) {
						this->_chromosome[p]->latestC = min(this->_chromosome[i]->earliestStart, this->_chromosome[p]->latestC);
					}
				}
				// set earliestStart for batch processed afterwards
				if(i < bMax - mMax) {
					this->_chromosome[i+mMax]->earliestStart = max(this->_chromosome[i]->earliestStart + this->_chromosome[i]->pLot, this->_chromosome[i+mMax]->earliestStart);
				}

				if (this->_chromosome[i]->intersects(contained)) {
					// Eliminate some jobs
					for (set<int>::iterator iter = contained.begin(); iter != contained.end(); iter++) {
						if (this->_chromosome[i]->contains(*iter)) {
							this->_chromosome[i]->erase(*iter);
						}
					}

					// if the batch is empty, expand earliestStart of i+maxM and latestC of i-maxM if necessary
					if(this->_chromosome[i]->numJobs <= 0) {
						// expand latestC for batch processed before
						bool hasSuccessor = false;
						if(i >= mMax) {				// not the first job
							if(i < bMax - mMax) {	// not the last job
								for(int s = i + mMax; s < bMax; s = s+mMax) {
									if(this->_chromosome[s]->numJobs > 0) {	// successor
										hasSuccessor = true;
										this->_chromosome[i-mMax]->latestC = this->_chromosome[s]->earliestStart;
										break;
									}
								}
								if(!hasSuccessor) {
									this->_chromosome[i-mMax]->latestC = Global::problem->T;
								}
							}
							this->_chromosome[i-mMax]->latestC = this->_chromosome[i]->earliestStart;	
						}
						// expand earliestStart for batch processed afterwards
						if(i < bMax - mMax) {
							this->_chromosome[i+mMax]->earliestStart = tInterval * floor((double) (i+mMax) / (double) mMax);
						}
					}
				}
			} // else, batch cannot be placed because available timeSlot is too short
		} // else, batch is empty and will not be copied
	}

	// Define orders which are contained in the child
	contained.clear();
	for (int i = 0; i < this->_chromosome.size(); i++) {
		int jobsInThisBatch = this->_chromosome[i]->numJobs;
		for (unsigned j = 0; j < jobsInThisBatch; j++) {
			
				contained.insert(this->_chromosome[i]->getJob(j).id);
		}
	}

	// Collect the missing orders (missing from the orders in both parents)
	missing.clear();
	for (set<int>::iterator iter = all.begin(); iter != all.end(); iter++) {
		if (contained.count(*iter) == 0) missing.push_back(*iter);
	}

	// reinsert missing jobs, if reinsertion unsuccessfull, set _valid to false
	if (!this->_chromosome.reinsertTWTEPC_2(missing)) {
		this->_valid = false;
	}
	

	// VARIATION this->_chromosome.reinsertReady(missing);

	// VARIATION
	//this->consolidateBatches();

	// decode the chromosome and set objective values
	// VARIATION
	//this->setObjectiveValues();		// simpler version (list-sched asap)
	this->setObjectiveValues();
	//this->setObjectiveValues_ASAP();

	// move mon-empty batches to the front => variable length chromosome includes all non-empty batches only
	//this->_chromosome.moveNonEmptyBatchesToFront();
	

}		
TWTEPC_2Solution::TWTEPC_2Solution(TWTEPC_2Solution& solution1){								// copy constructor
	_twtPref = solution1._twtPref; // preference for TWT minimization is copied
	ObjectiveValues.resize(2);	// TODO put 2 into Global::parameters
	_schedule.copy(&solution1._schedule);
	this->_chromosome.copy(&solution1._chromosome);
	// decode the chromosome and set objective values
	this->ObjectiveValues[0] = solution1.ObjectiveValues[0];
	this->ObjectiveValues[1] = solution1.ObjectiveValues[1];
	
}	


TWTEPC_2Solution::~TWTEPC_2Solution() {
}
void TWTEPC_2Solution::Mutate() {
	//this->_twtPref = (float) rand() / RAND_MAX;
	//this->setObjectiveValues_Pref();
	//this->consolidateBatches();
	return;
}



bool TWTEPC_2Solution::decodeChromosome() {
	// clear machines
	int iMax = this->_schedule.size();
	for(unsigned i = 0; i < iMax; i++) {
		this->_schedule[i].clear();
	}
	/// schedule batches on machines at their respective earliestStart
	int bMax = this->_chromosome.size();
	for(unsigned b = 0; b < bMax; b++) {
		if( !this->_chromosome[b]->empty()) { // do not place empty batches
			if(!this->_schedule[(b+iMax) % iMax ].addBatch(*this->_chromosome[b], this->_chromosome[b]->earliestStart)) {
				return false;
			}
		}
	}

	// TODO maybe move this check to MachineSet::listSched
	for(unsigned i = 0; i < iMax; i++) {
		if(this->_schedule[i].getMSP() > Global::problem->e1.size()){
			return false;
		}
	}
	// TODO maybe apply local optimization

	this->improveTWT();
	this->improveEPC();

	return true;
}


bool TWTEPC_2Solution::decodeChromosome_VLIST() {
	// get number of non-empty batches
	this->_chromosome.moveNonEmptyBatchesToFront();
	int bMax = this->_chromosome.firstEmpty(true);	
	if(bMax == -1) {
		bMax = this->_chromosome.size();
	}
	int seperatorTWT = (int) floor( (this->_twtPref * (double)bMax) + 0.5);		// batches 0 - seperatorTWT are schedule for a low TWT, seperatorTWT - (bMax-1) are scheduled for a low EPC
	// clear machines
	int iMax = this->_schedule.size();
	for(unsigned i = 0; i < iMax; i++) {
		this->_schedule[i].clear();
	}
	/// schedule batches on machines
	// batches from 0 - seperatorTWT are scheduled for a low TWT
	for(int b = 0; b < seperatorTWT; b++) {
		if(!this->_schedule.listSched_TWT(*this->_chromosome[b])) {		// VARIATION: listSched_ASAP
			return false;
		}
	}
	// batches from seperatorTWT - (bMax-1) are scheduled for a low EPC
	for(int b = seperatorTWT; b < bMax; b++) {
		if(Global::gv_eRates <= 1) {
			if(!this->_schedule.listSched_EPC(*this->_chromosome[b], Global::problem->e1)) {
				return false;
			};
		}
		else {
			if(!this->_schedule.listSched_EPC(*this->_chromosome[b], Global::problem->e2)) {
				return false;
			};
		}
	}

	for(unsigned i = 0; i < iMax; i++) {
		if(this->_schedule[i].getMSP() > Global::problem->e1.size()){
			return false;
		}
	}
	
	// VARIATION: local search, shift batches to improve either TWT or EPC not increasing the other objective
	this->improveTWT();
	this->improveEPC();

	// TODO maybe apply local optimization
	return true;
}
bool TWTEPC_2Solution::decodeChromosome_ASAP() {
	// get number of non-empty batches
	this->_chromosome.moveNonEmptyBatchesToFront();
	int bMax = this->_chromosome.firstEmpty(true);	
	if(bMax == -1) {
		bMax = this->_chromosome.size();
	}
	// clear machines
	int iMax = this->_schedule.size();
	for(unsigned i = 0; i < iMax; i++) {
		this->_schedule[i].clear();
	}
	/// schedule batches on machines
	// batches from 0 - seperatorTWT are scheduled for a low TWT
	for(int b = 0; b < bMax; b++) {
		if(!this->_schedule.listSched_ASAP(*this->_chromosome[b])) {
			return false;
		}
	}
	// batches from seperatorTWT - (bMax-1) are scheduled for a low EPC

	for(unsigned i = 0; i < iMax; i++) {
		if(this->_schedule[i].getMSP() > Global::problem->e1.size()){
			return false;
		}
	}
	
	// VARIATION: local search, shift batches to improve either TWT or EPC not increasing the other objective
	this->improveTWT();
	this->improveEPC();

	// TODO maybe apply local optimization
	return true;
}
void TWTEPC_2Solution::setObjectiveValues() {
	if(!this->decodeChromosome()) {
		this->ObjectiveValues[0] = 999999;
		this->ObjectiveValues[1] = 999999;	
	} else {
		this->ObjectiveValues[0] = this->getTWT();
		this->ObjectiveValues[1] = this->getEPC();
	}
}
void TWTEPC_2Solution::setObjectiveValues_VLIST(){			// derives a schedule considering the solution´s TWT preference
	if(!this->decodeChromosome_VLIST()) {
		this->ObjectiveValues[0] = 999999;
		this->ObjectiveValues[1] = 999999;	
	} else {
		// TODO: local search
		//this->_schedule.improveTWT();
		//this->_schedule.improveEPC();
		this->ObjectiveValues[0] = this->getTWT();
		this->ObjectiveValues[1] = this->getEPC();
	}
}
void TWTEPC_2Solution::setObjectiveValues_ASAP(){			// derives a schedule considering the solution´s TWT preference
	if(!this->decodeChromosome_ASAP()) {
		this->ObjectiveValues[0] = 999999;
		this->ObjectiveValues[1] = 999999;	
	} else {
		// TODO: local search
		this->_schedule.improveTWT();
		this->_schedule.improveEPC();
		this->ObjectiveValues[0] = this->getTWT();
		this->ObjectiveValues[1] = this->getEPC();
	}
}
double TWTEPC_2Solution::getTWT() {
	return this->_schedule.getTWT();
}

double TWTEPC_2Solution::getEPC() {
	if(Global::gv_eRates <= 1) {
		return this->_schedule.getEPC(Global::problem->e1);
	}
	return this->_schedule.getEPC(Global::problem->e2);
}
bool TWTEPC_2Solution::isValid() {
	return this->_valid;
}

MachineSet TWTEPC_2Solution::getSchedule() {
	return this->_schedule;
}
double TWTEPC_2Solution::improveTWT() {		// locally improve TWT, not accepting increase of EPC
	// step 1: consider batches from left to right, shift batch left if both EPC and TWT are not increased
	double twtImprove = this->_schedule.improveTWT();
	this->ObjectiveValues[0] = this->_schedule.getTWT();
	if(Global::gv_eRates <= 1) {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e1);
	}
	else {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e2);
	}
	return twtImprove;
}						
double TWTEPC_2Solution::improveEPC() {		// locally improve EPC, not accepting increase of TWT
	// step 1: consider batches from right to left, shift batch right if both EPC and TWT are not increased
	double epcImprove = this->_schedule.improveEPC();
	this->ObjectiveValues[0] = this->_schedule.getTWT();
	if(Global::gv_eRates <= 1) {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e1);
	}
	else {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e2);
	}
	return epcImprove;
}					
double TWTEPC_2Solution::improveCombined(double ratio) {		// locally improve ratio * TWT + (1-ratio) * EPC
	// step 1a: consider batches from left to right, shift batch left if ratio * TWT + (1-ratio) * EPC is not increased
	// step 1b: consider batches from right to left, shift batch right if ratio * TWT + (1-ratio) * EPC is not increased
	double combined = this->_schedule.improveCombined(ratio);
	this->ObjectiveValues[0] = this->_schedule.getTWT();
		if(Global::gv_eRates <= 1) {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e1);
	}
	else {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e2);
	}
	return combined;
}
void TWTEPC_2Solution::consolidateBatches() {
	int bMax = this->_chromosome.size();
	// pass through chromosome from right to left
	for(int src = (bMax-1); src > 0; src--) {
		// try to shift jobs to batches from left to right
		if(!this->_chromosome[src]->empty()) {
			for(unsigned dst = 0; dst < src; dst++) {
				this->_chromosome.shiftJobsTWT(src, dst);		// TODO: choose most suitable shiftJobs version (see GroupingGenome.h)
				//this->_chromosome.shiftJobsEPC(src, dst);
			}
		}
	}
	this->_chromosome.moveNonEmptyBatchesToFront();
}