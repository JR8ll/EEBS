#include "TWTEPC_3Solution.h"

TWTEPC_3Solution::TWTEPC_3Solution() {			// standard constructor => random initialization of one individual
	_valid = true;
	ObjectiveValues.resize(2);	// TODO put 2 into Global::parameters
	int cur_batch;

	// initialize a machineset/schedule for evaluation of the chromosome
	this->_schedule.clear();
	this->_priority.clear();
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
		_priority.resize(Global::problem->n);
		for(unsigned i = 0; i < Global::problem->n; i++) {
			_chromosome[i] = new Batch(Global::problem->k);
			_priority[i] = pair<int, float>(i, (float) rand() / RAND_MAX);
		}
	}
	else {
		// clean chromosome
		_chromosome.clean();
		for(unsigned i = 0; i < Global::problem->n; i++) {
			_chromosome[i]->capacity = Global::problem->k;
		}
	}	

	int bMax = Global::problem->n;
	int mMax = Global::problem->m;
	// distribute jobs between batches in random manner
	for(unsigned i = 0; i < bMax; i++){
		bool assigned = false;
		while(!assigned) {
			cur_batch = rand() % bMax;
			// try to add the job
			if(this->_chromosome[cur_batch]->addJob(&Global::problem->jobs[i])) {
				assigned = true;
			}
		}
	}
	
	// VARIATION
	//this->setObjectiveValues_ASAP();
	this->setObjectiveValues();
}							

TWTEPC_3Solution::TWTEPC_3Solution(TWTEPC_3Solution & p1, TWTEPC_3Solution & p2) {	// Recombination (Crossover)
	_valid = true;
	
	// initialize chromosome
	this->_chromosome.resize(Global::problem->n);
	this->_priority.resize(Global::problem->n);
	for(int i = 0; i < Global::problem->n; i++) {
		this->_chromosome[i] = new Batch(Global::problem->k);
		this->_priority[i] = pair<int, float>(i, 0);
	}

	// set batches r, earliestStart and latestC
	int bMax = Global::problem->n;
	int mMax = Global::problem->m;
	
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
		this->_priority[i] = p2._priority[i];	
	}

	// Copy p1's batches eliminating the elements from contained : first part
	for (int i = 0; i < cs11; i++) {
		// do not copy empty batches, JR 13/08/2018, TODO: verify if this is a good idea in case of TWTEPC3
		if( !p1._chromosome[i]->empty() ) {
			this->_chromosome[i]->copy(p1._chromosome[i]); 	
			this->_priority[i] = p1._priority[i];

			if (this->_chromosome[i]->intersects(contained)) { // ch.batches[i]->intersects(contained)) {
				// Eliminate some jobs
				for (set<int>::iterator iter = contained.begin(); iter != contained.end(); iter++) {
					if (this->_chromosome[i]->contains(*iter)) {
						this->_chromosome[i]->erase(*iter);
					}
				}
			}
		} // else, batch is empty and is not copied
	}

	// Copy p1's batches eliminating the elements from contained : second part
	for (int i = cs12 + 1; i <= cs1_end; i++) {
		// do not copy empty batches, JR 13/08/2018, TODO: check if this makes sense for TWTEPC3
		if( !p1._chromosome[i]->empty() ) {
			int actualStart = this->_chromosome[i]->earliestStart;
			this->_chromosome[i]->copy(p1._chromosome[i]); // = new Batch(p1._chromosome[i]);
			this->_chromosome[i]->setStart(actualStart);

			if (this->_chromosome[i]->intersects(contained)) {
				// Eliminate some jobs
				for (set<int>::iterator iter = contained.begin(); iter != contained.end(); iter++) {
					if (this->_chromosome[i]->contains(*iter)) {
						this->_chromosome[i]->erase(*iter);
					}
				}
			}	
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
	if (!this->_chromosome.reinsertTWTEPC_3(missing)) {
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
TWTEPC_3Solution::TWTEPC_3Solution(TWTEPC_3Solution& solution1){								// copy constructor
	ObjectiveValues.resize(2);	// TODO put 2 into Global::parameters
	_schedule.copy(&solution1._schedule);
	_priority = solution1._priority;
	this->_chromosome.copy(&solution1._chromosome);
	// decode the chromosome and set objective values
	this->ObjectiveValues[0] = solution1.ObjectiveValues[0];
	this->ObjectiveValues[1] = solution1.ObjectiveValues[1];
	
}	


TWTEPC_3Solution::~TWTEPC_3Solution() {
}
void TWTEPC_3Solution::Mutate() {
	//this->_twtPref = (float) rand() / RAND_MAX;
	//this->setObjectiveValues_Pref();
	//this->consolidateBatches();
	return;
}



bool TWTEPC_3Solution::decodeChromosome() {
	int iMax = this->_schedule.size();
	int bMax = this->_chromosome.size();
	
	// derive raw schedule from chromosome
	this->updateSchedule();

	
	/* this is done by updateSchedule();
	// clear machines
	int iMax = this->_schedule.size();
	for(unsigned i = 0; i < iMax; i++) {
		this->_schedule[i].clear();
	}

	/// schedule batches on machines at their respective earliestStart
	int bMax = this->_chromosome.size();
	for(unsigned b = 0; b < bMax; b++) {
		if(!this->_schedule[(b+iMax) % iMax ].addBatch(*this->_chromosome[b])) {
			return false;
		}	
	}
	*/

	// calculate the maximal number of 1-period-long pauses allowing a feasible schedule
	// TODO: this has to be done AFTER the batches are assigned to machines, maybe balanceLoad(?)
	vector<int> pausesPerMachine = this->getPausesPerMachine();
	vector<int> emptyBatchesPerMachine = this->getEmptyBatchesPerMachine();		// 11.11.2018 define the pause length by possible pause divided by empty batches
	
	// set pauses to p = 1
	this->sortPrioByRK();

	/* Versuch 13.11.2018
	this->balanceLoad();
	// TODO 

	// set pauses´ length
	bMax = this->_chromosome.size();
	for(int p = 0; p < bMax; p++) {
		int chrIndex = this->_priority[p].first;
		int mchIndex = (chrIndex+iMax)%iMax;
		int btcIndex = this->_priority[p].first / iMax;
		if( pausesPerMachine[mchIndex] > 0 && this->_schedule[mchIndex].getBatch(btcIndex).empty() ) {
			pausesPerMachine[mchIndex]--;									// TODO: 121118 sub the pauseLength
			this->_schedule[mchIndex].getBatch(btcIndex).pLot = 1;			// define the length of a pause
		}
	}
	*/



	// TODO: if pausesPerMachine < 0, then consolidate batches _chromosome must be changed!
	if(!this->hasFeasibleSchedule()){
		this->consolidateBatchesKeepPauses();		// TODO maybe use this->consolidateBatchesSameMachineKeepPauses()
		this->updateSchedule();
		this->balanceLoad();			// TODO: only if not 'consolidateBatchesSameMachineKeepPauses()' two line above 
	}

	this->setPausesAvgLength();			// 13.11.2018
	// TODO set start and c of all batches
	for(int q = 0; q < iMax; q++) {
		this->_schedule[q].updateCompletionTimes();
	}
	this->resetPauses();

	// TODO: set pauses` length again?!
	

	// TODO maybe move this check to MachineSet::listSched
	if(!this->hasFeasibleSchedule()){
		return false;
	}

	this->improveTWT();
	this->improveEPC();
	this->locSshiftJobs();
	this->locSswapJobs();

	return true;
}


bool TWTEPC_3Solution::decodeChromosome_ASAP() {
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
void TWTEPC_3Solution::setObjectiveValues() {
	if(!this->decodeChromosome()) {
		this->ObjectiveValues[0] = 999999;
		this->ObjectiveValues[1] = 999999;	
	} else {
		this->ObjectiveValues[0] = this->getTWT();
		this->ObjectiveValues[1] = this->getEPC();
	}
}
void TWTEPC_3Solution::setObjectiveValues_ASAP(){			// derives a schedule considering the solution´s TWT preference
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
MachineSet TWTEPC_3Solution::getSchedule() {
	return this->_schedule;
}
int TWTEPC_3Solution::getLastNonemptyBatch(int machine) {
	// get the _chromosome´s index of the last non-empty batch at machine
	int iMax = this->_schedule.size();
	int bMax = this->_chromosome.size();
	if(this->_schedule[machine].getQueueSize() > 0) {
		for (int i = this->_schedule[machine].getQueueSize() - 1; i >= 0; i--) {
			if(!this->_schedule[machine].getBatch(i).empty()) {
				return (i * iMax) + machine;
			}
		}
	}
	return -1;
}
int TWTEPC_3Solution::getLastEmptyBatch(int machine) {
	// get the _chromosome´s index of the last empty batch at machine
	int iMax = this->_schedule.size();
	int bMax = this->_chromosome.size();
	if(this->_schedule[machine].getQueueSize() > 0) {
		for (int i = this->_schedule[machine].getQueueSize() - 1; i >= 0; i--) {
			if(this->_schedule[machine].getBatch(i).empty()) {
				return (i * iMax) + machine;
			}
		}
	}
	return -1;
}
vector<int> TWTEPC_3Solution::getPausesPerMachine() {
	vector<int> pausesPerMachine;
	int iMax = this->_schedule.size();
	int bMax;
	for(int i = 0; i < iMax; i++) {
		bMax = this->_schedule[i].getQueueSize();
		int myPauses = 0;
		int pSum = 0;
		int tempMSP = 0;	// 'temporary' makespan of the batches considered so far
		int delaySum = 0;	// inevitable delays due to batch availability
		if(!this->_schedule[i].isEmpty()) {
			pSum += this->_schedule[i].getBatch(0).pLot;
			delaySum += this->_schedule[i].getBatch(0).r;
			tempMSP += this->_schedule[i].getBatch(0).r + this->_schedule[i].getBatch(0).pLot;
		}

		for(int j = 1; j < bMax; j++) {
			pSum += this->_schedule[i].getBatch(j).pLot;
			delaySum += max(0, this->_schedule[i].getBatch(j).r - tempMSP);
			tempMSP = max(tempMSP, this->_schedule[i].getBatch(j).r) + this->_schedule[i].getBatch(j).pLot;
		}
		// calculate pauses for machine i
		myPauses = Global::problem->T - pSum - delaySum;
		pausesPerMachine.push_back(myPauses);
	}
	return pausesPerMachine;
}
vector<int> TWTEPC_3Solution::getEmptyBatchesPerMachine() {
	vector<int> emptyBatchesPerMachine;
	int iMax = this->_schedule.size();
	int bMax;
	int emptyBatches;
	for(int i = 0; i < iMax; i++) {
		bMax = this->_schedule[i].getQueueSize();
		emptyBatches = 0;
		for(int j = 0; j < bMax; j++) {
			if(this->_schedule[i].getBatch(j).empty()) {
				emptyBatches++;
			}
		}
		emptyBatchesPerMachine.push_back(emptyBatches);
	}
	return emptyBatchesPerMachine;
}
double TWTEPC_3Solution::getTWT() {
	return this->_schedule.getTWT();
}

double TWTEPC_3Solution::getEPC() {
	if(Global::gv_eRates <= 1) {
		return this->_schedule.getEPC(Global::problem->e1);
	}
	return this->_schedule.getEPC(Global::problem->e2);
}
bool TWTEPC_3Solution::isValid() {
	return this->_valid;
}

double TWTEPC_3Solution::improveTWT() {		// locally improve TWT, not accepting increase of EPC
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
double TWTEPC_3Solution::improveEPC() {		// locally improve EPC, not accepting increase of TWT
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
double TWTEPC_3Solution::improveCombined(double ratio) {		// locally improve ratio * TWT + (1-ratio) * EPC
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
void TWTEPC_3Solution::locSswapJobs() {							// local search: pairwise swap jobs between two batches
	int bMax = this->_chromosome.size();
	int mMax = this->_schedule.size();
	int jMax = 0;
	int srcFam = 0;		// job family of the source batch
	Job* srcJob;
	Job* trgJob;
	
	// consider all non-empty batches as source
	for(int srcIdx = 0; srcIdx < bMax; srcIdx++) {
		if(!this->_chromosome[srcIdx]->empty()) {			
			srcFam = this->_chromosome[srcIdx]->f;
			// consider all non-empty batches of the same family as target, target job has the smaller completion time
			for(int trgIdx = 0; trgIdx < bMax; trgIdx++) {
				if(srcIdx != trgIdx && (!this->_chromosome[trgIdx]->empty()) && srcFam == this->_chromosome[trgIdx]->f && this->_chromosome[trgIdx]->getC() < this->_chromosome[srcIdx]->getC()) {
					// consider all jobs in the source batch
					int bestSwapJobSrc = 0;
					int bestSwapJobTrg = 0;
					double bestImprovement = 0;
					// consider all jobs in source Batch
					for(int srcJobIdx = 0; srcJobIdx < this->_chromosome[srcIdx]->numJobs; srcJobIdx++) {
						// consider all jobs in target Batch
						for(int trgJobIdx = 0; trgJobIdx < this->_chromosome[trgIdx]->numJobs; trgJobIdx++) {
							bool capTrgOK = this->_chromosome[trgIdx]->freeCapacity + this->_chromosome[trgIdx]->getJob(trgJobIdx).s - this->_chromosome[srcIdx]->getJob(srcJobIdx).s >= 0; 
							bool capSrcOK = this->_chromosome[srcIdx]->freeCapacity + this->_chromosome[srcIdx]->getJob(srcJobIdx).s - this->_chromosome[trgIdx]->getJob(trgJobIdx).s >= 0;
							bool rTrgOK = this->_chromosome[trgIdx]->r >= this->_chromosome[srcIdx]->getJob(srcJobIdx).r;
							bool rSrcOK = this->_chromosome[srcIdx]->r >= this->_chromosome[trgIdx]->getJob(trgJobIdx).r;
							if(capTrgOK && capSrcOK && rTrgOK && rSrcOK){
								// assess improvement achieved by swapping these jobs
								double thisImprovement = this->_chromosome[srcIdx]->getJob(srcJobIdx).w - this->_chromosome[trgIdx]->getJob(trgJobIdx).w;
								if( thisImprovement > bestImprovement ) {
									bestImprovement = thisImprovement;
									bestSwapJobSrc = srcJobIdx;
									bestSwapJobTrg = trgJobIdx;			
								}
							}
						}
					}
					// swap jobs with the best improvement 
					if(bestImprovement > 0) {			
						srcJob = &this->_chromosome[srcIdx]->getJob(bestSwapJobSrc);
						trgJob = &this->_chromosome[trgIdx]->getJob(bestSwapJobTrg);
						
						//srcJob.copy( &this->_chromosome[srcIdx]->getJob(bestSwapJobSrc) );
						//trgJob.copy( &this->_chromosome[trgIdx]->getJob(bestSwapJobTrg) );
						this->_chromosome[srcIdx]->erase( srcJob->id );
						this->_chromosome[trgIdx]->erase( trgJob->id );
						this->_chromosome[srcIdx]->addJob(trgJob);
						this->_chromosome[trgIdx]->addJob(srcJob);	
					}
				}
			}		
		}
	}
	this->updateSchedule();
}
void TWTEPC_3Solution::locSswapJobsAdj() {						// local search: pairwise swap jobs between two adjacent batches
	cout << "TWTEPC_3Solution::locSswapJobsAdj() not implemented" << endl;
}
void TWTEPC_3Solution::locSshiftJobs() {						// local search: shift jobs to another batch
	int bMax = this->_chromosome.size();
	int mMax = this->_schedule.size();
	int jMax = 0;
	int srcFam = 0;		// job family of the source batch
	//Job swapJob = Job();
	
	// consider all non-empty batches as source
	for(int srcIdx = 0; srcIdx < bMax; srcIdx++) {
		if(!this->_chromosome[srcIdx]->empty()) {			
			srcFam = this->_chromosome[srcIdx]->f;
			// consider all non-empty batches of the same family as target
			for(int trgIdx = 0; trgIdx < bMax; trgIdx++) {
				if(srcIdx != trgIdx && (!this->_chromosome[trgIdx]->empty()) && srcFam == this->_chromosome[trgIdx]->f && this->_chromosome[trgIdx]->getC() < this->_chromosome[srcIdx]->getC()) {
					// consider all jobs in the source batch
					int srcJobIdx = 0;
					while(srcJobIdx < this->_chromosome[srcIdx]->numJobs) {
						if(this->_chromosome[trgIdx]->freeCapacity >= this->_chromosome[srcIdx]->getJob(srcJobIdx).s && this->_chromosome[trgIdx]->r >= this->_chromosome[srcIdx]->getJob(srcJobIdx).r ){
							// shift job from source to target
							Job* swapJob = &this->_chromosome[srcIdx]->getJob(srcJobIdx);
							if( this->_chromosome[trgIdx]->addJob(swapJob) ) {
								this->_chromosome[srcIdx]->erase( swapJob->id );
							} else {
								srcJobIdx++;
							}
						} else {
							srcJobIdx++;
						}
					}	
				}
			}		
		}
	}
	this->updateSchedule();
}
void TWTEPC_3Solution::locSshiftJobsAdj() {						// local search: shift jobs to another adjacent batch
	cout << "TWTEPC_3Solution::locSshiftJobsAdj() not implemented" << endl;
}
void TWTEPC_3Solution::locSsplitBatch() {						// local search: split a single batch into two batches
	cout << "TWTEPC_3Solution::locSsplitBatch() not implemented" << endl;
}
MachineSet& TWTEPC_3Solution::updateSchedule(){
	// clear machines
	int iMax = this->_schedule.size();
	for(unsigned i = 0; i < iMax; i++) {
		this->_schedule[i].clear();
	}

	/// schedule batches on machines
	int bMax = this->_chromosome.size();
	for(unsigned b = 0; b < bMax; b++) {
		this->_schedule[(b+iMax) % iMax ].addBatch(*this->_chromosome[b]);
	}

	for(unsigned i = 0; i < iMax; i++) {
		this->_schedule[i].updateCompletionTimes();
	}
	return this->_schedule;
}
void TWTEPC_3Solution::consolidateBatches() {
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
void TWTEPC_3Solution::consolidateBatchesKeepPauses(){
	// keeps empty batches empty and consolidates batches on the same machine only
	int bMax = this->_chromosome.size();
	int iMax = this->_schedule.size();
	// for each machine
	for(int i = 0; i < iMax; i++) {
		// pass through chromosome from right to left
		for(int src = (bMax -1); src > 0; src--) {
			// try to shift jobs to batches from left to right
			if(!this->_chromosome[src]->empty()) {
				for(unsigned dst = 0; dst < src; dst++) {
					if(!this->_chromosome[dst]->empty()) {
						// do not fill empty batches (pauses should remain)
						this->_chromosome.shiftJobsTWT(src, dst);		// TODO: choose most suitable shiftJobs version (see GroupingGenome.h)
						//this->_chromosome.shiftJobsEPC(src, dst);
					}
				}
			}
		}
	}
}
void TWTEPC_3Solution::consolidateBatchesSameMachineKeepPauses(){
	// keeps empty batches empty and consolidates batches on the same machine only
	int bMax = this->_chromosome.size();
	int iMax = this->_schedule.size();
	// for each machine
	for(int i = 0; i < iMax; i++) {
		// pass through chromosome from right to left
		for(int src = (bMax-iMax+i); src > 0; src = src - iMax) {
			// try to shift jobs to batches from left to right
			if(!this->_chromosome[src]->empty()) {
				for(unsigned dst = 0 + i; dst < src; dst = dst + iMax) {
					if(!this->_chromosome[dst]->empty()) {
						// do not fill empty batches (pauses should remain)
						this->_chromosome.shiftJobsTWT(src, dst);		// TODO: choose most suitable shiftJobs version (see GroupingGenome.h)
						//this->_chromosome.shiftJobsEPC(src, dst);
					}
				}
			}
		}
	}
}
bool TWTEPC_3Solution::balanceLoad() {
	bool loadIsBalanced = false;
	this->updateSchedule();
	int iMax = this->_schedule.size();
	int bMax;
	int minPause, maxPause, minPauseMachine, maxPauseMachine;

	vector<int> pausesPerMachine = this->getPausesPerMachine();

	if(!this->_schedule.isEmpty()){
		while(!loadIsBalanced) {
			minPause = pausesPerMachine[0];
			minPauseMachine = 0;
			maxPause = pausesPerMachine[0];
			maxPauseMachine = 0;
		
			for(int m = 1; m < iMax; m++) {
				int tempPause = pausesPerMachine[m];
				if(tempPause < minPause) {
					minPause = tempPause;
					minPauseMachine = m;
				}
				if(tempPause > maxPause) {
					maxPause = tempPause;
					maxPauseMachine = m;
				}
			}
			if(minPauseMachine == maxPauseMachine) {
				loadIsBalanced = true;
			} else {
				if(this->_schedule[minPauseMachine].getQueueSize() > 0) {
					// move last batch from machine with highest load to machine with lowest load
					if(minPause < maxPause && this->getLastNonemptyBatch(minPauseMachine) != -1 && this->getLastEmptyBatch(maxPauseMachine) != -1) {
						// TODO condition 
						int srcBatch = this->getLastNonemptyBatch(minPauseMachine);
						int dstBatch = this->getLastEmptyBatch(maxPauseMachine);

						int addDelay = 0;
						Batch src = Batch();
						src.copy(this->_chromosome[srcBatch]);
						if(dstBatch >= iMax){
							// dstBatch has a predecessor
							addDelay += max<int>(0, src.r - this->_chromosome[dstBatch-iMax]->getMSP());
						} else {
							addDelay += src.r;
						}
						
						if(pausesPerMachine[maxPauseMachine] - this->_chromosome[srcBatch]->pLot > pausesPerMachine[minPauseMachine] + this->_chromosome[srcBatch]->pLot + addDelay) {
							// reassigning the batch does not reduce the (new) pauses of machine with more pauses below the (new) level of the other one
							this->_chromosome[dstBatch]->copy(&src);
							this->_chromosome[srcBatch]->clear();
							this->updateSchedule();
							pausesPerMachine = this->getPausesPerMachine();
						} else {
							loadIsBalanced = true;
						}
					} else {
						loadIsBalanced = true;
					}
				} else {
					loadIsBalanced = true;
				}
			}
		}
	}
	return this->hasFeasibleSchedule();
}

void TWTEPC_3Solution::setPauses(int length) {
	vector<int> pausesPerMachine = this->getPausesPerMachine();
	int iMax = this->_schedule.size();
	int bMax = this->_chromosome.size();
	for(int p = 0; p < bMax; p++) {
		int chrIndex = this->_priority[p].first;
		int mchIndex = (chrIndex+iMax)%iMax;
		int btcIndex = this->_priority[p].first / iMax;
		if( pausesPerMachine[mchIndex] > 0 && this->_schedule[mchIndex].getBatch(btcIndex).empty() ) {
			pausesPerMachine[mchIndex] -= length;									// TODO: 121118 sub the pauseLength
			this->_schedule[mchIndex].getBatch(btcIndex).pLot = length;				// define the length of a pause
		}
	}
	// TODO: set pauses on chromosome or schedule?!
}
void TWTEPC_3Solution::setPausesAvgLength() {
	vector<int> pausesPerMachine = this->getPausesPerMachine();
	vector<int> emptyBatchesPerMachine = this->getEmptyBatchesPerMachine();
	vector<int> pauseLengthPerMachine;
	int xMax = pausesPerMachine.size();
	for(int x = 0; x < xMax; x++) {
		if(emptyBatchesPerMachine[x] != 0) {
			pauseLengthPerMachine.push_back(max(pausesPerMachine[x] / emptyBatchesPerMachine[x], 1));
		} else {
			pauseLengthPerMachine.push_back(0);
		}
	}
	int iMax = this->_schedule.size();
	int bMax = this->_chromosome.size();
	for(int p = 0; p < bMax; p++) {
		int chrIndex = this->_priority[p].first;
		int mchIndex = (chrIndex+iMax)%iMax;
		int btcIndex = this->_priority[p].first / iMax;
		if( pausesPerMachine[mchIndex] > 0 && this->_schedule[mchIndex].getBatch(btcIndex).empty() ) {
			pausesPerMachine[mchIndex] -= pauseLengthPerMachine[mchIndex];									// TODO: 121118 sub the pauseLength
			this->_schedule[mchIndex].getBatch(btcIndex).pLot = pauseLengthPerMachine[mchIndex];				// define the length of a pause
		}
	}
	// TODO: set pauses on chromosome or schedule?!
}

void TWTEPC_3Solution::resetPauses() {
	// TODO: implement
	int iMax = this->_schedule.size();
	int bMax;
	for(int i = 0; i < iMax; i++) {
		bMax = this->_schedule[i].getQueueSize();
		for(int b = 0; b < bMax; b++) {
			if(this->_schedule[i].getBatch(b).empty()) {
				this->_schedule[i].getBatch(b).pLot = 0;
				this->_schedule[i].getBatch(b).setC(this->_schedule[i].getBatch(b).getStart());
			}
		}
	}
	
}
bool TWTEPC_3Solution::hasFeasibleSchedule(){
	int iMax = this->_schedule.size();
	for(int i = 0; i < iMax; i++) {
		if(this->_schedule[i].getMSP() > Global::problem->T) {
			return false;
		}
	}
	return true;
}

bool TWTEPC_3Solution::compByRK(const pair<int, float>& a, const pair<int, float>& b){
	return a.second < b.second;
}
void TWTEPC_3Solution::sortPrioByRK() {
	sort(this->_priority.begin(), this->_priority.end(), TWTEPC_3Solution::compByRK);
}
void TWTEPC_3Solution::applyLocalSearch(double weightTWT, double weightEPC) {
	// TODO implement local search
	// cout << "TWTEPCSolution::applyLocalSearch(double, double) called." << endl;
	this->improveCombined(weightTWT / (weightTWT + weightEPC));

	// TODO swap jobs between batches as long as integrared objective function value decreases

}