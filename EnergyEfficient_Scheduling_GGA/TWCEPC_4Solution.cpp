#include "TWCEPC_4Solution.h"


TWCEPC_4Solution::TWCEPC_4Solution() {			// standard constructor => random initialization of one individual
	_valid = true;
	_twcPref = (float) rand() / RAND_MAX;
	ObjectiveValues.resize(2);				// TODO put 2 into Global::parameters
	int cur_batch;

	// initialize a machineset/schedule for evaluation of the chromosome
	this->_schedule.clear();
	this->_delayT.clear();
	for(unsigned i = 0; i < Global::problem->m; i++) {
		Machine tempM;
		this->_schedule.addMachine(tempM);
	}
	
	if(_chromosome.size() != Global::problem->n) {
		// if chromosome not empty, then delete
		for(unsigned i = 0; i < _chromosome.size(); i++) {
			delete _chromosome[i];
			_chromosome[i] = NULL;
		}
		// create new chromosome
		_chromosome.resize(Global::problem->n);
		_delayT.resize(Global::problem->m);
		for(unsigned i = 0; i < Global::problem->n; i++) {
			_chromosome[i] = new Batch(Global::problem->k);
		}
		for(unsigned i = 0; i < Global::problem->m; i++) {
			_delayT[i] = 0.0;
		}

	}
	else {
		// clean chromosome
		_chromosome.clean();
		for(unsigned i = 0; i < Global::problem->n; i++) {
			_chromosome[i]->capacity = Global::problem->k;
		}
		for(unsigned i = 0; i < Global::problem->m; i++) {
			_delayT[i] = 0.0;
		}
	}	

	vector<int> batchIndices;
	for(int b = 0; b < Global::problem->n; b++) {
		batchIndices.push_back(b);
	}

	int bMax = Global::problem->n;
	int mMax = Global::problem->m;
	// distribute jobs between batches in random manner: SPECIAL CASE => one job per batch
	for(unsigned i = 0; i < bMax; i++){
		bool assigned = false;
		while(!assigned) {
			cur_batch = rand() % batchIndices.size(); // bMax;
			// try to add the job
			//if(this->_chromosome[cur_batch]->addJob(&Global::problem->jobs[i])) {
			if(this->_chromosome[batchIndices[cur_batch]]->addJob(&Global::problem->jobs[i])) {		
				assigned = true;
				batchIndices.erase(batchIndices.begin()+cur_batch);
			}
		}
	}

	// set random keys and delays after the last batches
	for(unsigned i = 0; i < Global::problem->n; i++) {
		_chromosome[i]->key = ((double) rand() / ((double) RAND_MAX + 1.0)) * mMax;		// Random Value between 0 and mMax	
	}
	for(unsigned i = 0; i < Global::problem->m; i++) {
		_delayT[i] = i + ((double) rand() / ((double) RAND_MAX + 1.0));					// Random Value between i and i+1			
	}
	
	this->_chromosome.moveNonEmptyBatchesToFront();
	
	this->setObjectiveValues();
}							



TWCEPC_4Solution::TWCEPC_4Solution(TWCEPC_4Solution & p1, TWCEPC_4Solution & p2) {	// Recombination (Crossover)
/*
	// ACHTUNG: Variante ZUFALL (statt Rekombination wird ein neues Chromosom zufällig initialisiert
	_valid = true;
	ObjectiveValues.resize(2);				// TODO put 2 into Global::parameters
	int cur_batch;

	// initialize a machineset/schedule for evaluation of the chromosome
	this->_schedule.clear();
	this->_delayT.clear();
	for(unsigned i = 0; i < Global::problem->m; i++) {
		Machine tempM;
		this->_schedule.addMachine(tempM);
	}

	
	
	if(_chromosome.size() != Global::problem->n) {
		// if chromosome not empty, then delete
		for(unsigned i = 0; i < _chromosome.size(); i++) {
			delete _chromosome[i];
			_chromosome[i] = NULL;
		}
		// create new chromosome
		_chromosome.resize(Global::problem->n);
		_delayT.resize(Global::problem->m);
		for(unsigned i = 0; i < Global::problem->n; i++) {
			_chromosome[i] = new Batch(Global::problem->k);
		}
		for(unsigned i = 0; i < Global::problem->m; i++) {
			_delayT[i] = 0.0;
		}

	}
	else {
		// clean chromosome
		_chromosome.clean();
		for(unsigned i = 0; i < Global::problem->n; i++) {
			_chromosome[i]->capacity = Global::problem->k;
		}
		for(unsigned i = 0; i < Global::problem->m; i++) {
			_delayT[i] = 0.0;
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

	// set random keys and delays after the last batches
	for(unsigned i = 0; i < Global::problem->n; i++) {
		_chromosome[i]->key = ((double) rand() / ((double) RAND_MAX + 1.0)) * mMax;		// Random Value between 0 and mMax	
	}
	for(unsigned i = 0; i < Global::problem->m; i++) {
		_delayT[i] = i + ((double) rand() / ((double) RAND_MAX + 1.0));					// Random Value between i and i+1			
	}
	
	this->_chromosome.moveNonEmptyBatchesToFront();
	
	this->setObjectiveValues();
*/


	_valid = true;
	_twcPref = p2._twcPref;
	
	// initialize chromosome
	this->_chromosome.resize(Global::problem->n);
	this->_delayT.resize(Global::problem->m);
	for(int i = 0; i < Global::problem->n; i++) {
		this->_chromosome[i] = new Batch(Global::problem->k);
	}
	for(int i = 0; i < Global::problem->m; i++) {
		this->_delayT[i] = 0.0;
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

	// move mon-empty batches to the front => variable length chromosome includes all non-empty batches only
	p1._chromosome.moveNonEmptyBatchesToFront();
	p2._chromosome.moveNonEmptyBatchesToFront();

	// 1. Select two crossing sites of the first parent
	//Debugger::iDebug("Selecting crossing sites for p1 ...");
	//p1.write(cout);

	cs1_end = p1._chromosome.firstEmpty(true) - 1;
	if (cs1_end < 0) cs1_end = p1._chromosome.size() - 1;

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

	cs2_end = p2._chromosome.firstEmpty(true) - 1;
	if (cs2_end < 0) cs2_end = p2._chromosome.size() - 1;

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
		this->_chromosome[i]->copy(p2._chromosome[i]);	
	}

	// Copy p1's batches eliminating the elements from contained : first part
	for (int i = 0; i < cs11; i++) {
		this->_chromosome[i]->copy(p1._chromosome[i]); 	
		if (this->_chromosome[i]->intersects(contained)) { // ch.batches[i]->intersects(contained)) {
			// Eliminate some jobs
			for (set<int>::iterator iter = contained.begin(); iter != contained.end(); iter++) {
				if (this->_chromosome[i]->contains(*iter)) {
					this->_chromosome[i]->erase(*iter);
				}
			}
		}
	}

	// Copy p1's batches eliminating the elements from contained : second part
	for (int i = cs12 + 1; i <= cs1_end; i++) {
		this->_chromosome[i]->copy(p1._chromosome[i]); // = new Batch(p1._chromosome[i]);
		if (this->_chromosome[i]->intersects(contained)) {
			// Eliminate some jobs
			for (set<int>::iterator iter = contained.begin(); iter != contained.end(); iter++) {
				if (this->_chromosome[i]->contains(*iter)) {
					this->_chromosome[i]->erase(*iter);
				}
			}
		}	
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
	if (!this->_chromosome.reinsert(missing)) {		// TODO: define reinsertion strategy
		this->_valid = false;
	}

	// copy final idle time per machine from first parent
	for(unsigned i = 0; i < Global::problem->m; i++) {
		_delayT[i] = p1._delayT[i];					// Random Value between i and i+1			
	}


	this->_chromosome.moveNonEmptyBatchesToFront();
	this->setObjectiveValues();
	
}		
TWCEPC_4Solution::TWCEPC_4Solution(TWCEPC_4Solution & p1, TWCEPC_4Solution & p2, bool nonDelay) {	// Recombination (Crossover)
	if(nonDelay){
			_valid = true;
			_twcPref = p2._twcPref;
		
			// initialize chromosome
			this->_chromosome.resize(Global::problem->n);
			this->_delayT.resize(Global::problem->m);
			for(int i = 0; i < Global::problem->n; i++) {
				this->_chromosome[i] = new Batch(Global::problem->k);
			}
			for(int i = 0; i < Global::problem->m; i++) {
				this->_delayT[i] = 0.0;
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

			// move mon-empty batches to the front => variable length chromosome includes all non-empty batches only
			p1._chromosome.moveNonEmptyBatchesToFront();
			p2._chromosome.moveNonEmptyBatchesToFront();

		// 1. Select two crossing sites of the first parent
		//Debugger::iDebug("Selecting crossing sites for p1 ...");
		//p1.write(cout);

			cs1_end = p1._chromosome.firstEmpty(true) - 1;
			if (cs1_end < 0) cs1_end = p1._chromosome.size() - 1;

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

			cs2_end = p2._chromosome.firstEmpty(true) - 1;
			if (cs2_end < 0) cs2_end = p2._chromosome.size() - 1;

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
				this->_chromosome[i]->copy(p2._chromosome[i]);	
			}

			// Copy p1's batches eliminating the elements from contained : first part
			for (int i = 0; i < cs11; i++) {
				this->_chromosome[i]->copy(p1._chromosome[i]); 	
				if (this->_chromosome[i]->intersects(contained)) { // ch.batches[i]->intersects(contained)) {
					// Eliminate some jobs
					for (set<int>::iterator iter = contained.begin(); iter != contained.end(); iter++) {
						if (this->_chromosome[i]->contains(*iter)) {
							this->_chromosome[i]->erase(*iter);
						}
					}
				}
			}

			// Copy p1's batches eliminating the elements from contained : second part
			for (int i = cs12 + 1; i <= cs1_end; i++) {
				this->_chromosome[i]->copy(p1._chromosome[i]); // = new Batch(p1._chromosome[i]);
				if (this->_chromosome[i]->intersects(contained)) {
					// Eliminate some jobs
					for (set<int>::iterator iter = contained.begin(); iter != contained.end(); iter++) {
						if (this->_chromosome[i]->contains(*iter)) {
							this->_chromosome[i]->erase(*iter);
						}
					}
				}	
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
			if (!this->_chromosome.reinsert(missing)) {		// TODO: define reinsertion strategy
				this->_valid = false;
			}

			this->_chromosome.moveNonEmptyBatchesToFront();

			int iMax = this->_chromosome.size();
			for(int i = 0; i < iMax; i++) {
				this->_chromosome[i]->setKey(floor(this->_chromosome[i]->key));
			}

			for(unsigned i = 0; i < Global::problem->m; i++) {
				_delayT[i] = (double) i + 0.999;					// Random Value between i and i+1			
			}


			this->setObjectiveValues();

		} else {	// nonDelay == false
			TWCEPC_4Solution::TWCEPC_4Solution(p1, p2);
		}

}		
TWCEPC_4Solution::TWCEPC_4Solution(TWCEPC_4Solution& solution1){								// copy constructor
	ObjectiveValues.resize(2);	// TODO put 2 into Global::parameters
	_schedule.copy(&solution1._schedule);
	_twcPref = solution1._twcPref;
	_valid = solution1._valid;
	this->_chromosome.copy(&solution1._chromosome);
	// decode the chromosome and set objective values
	this->ObjectiveValues[0] = solution1.ObjectiveValues[0];
	this->ObjectiveValues[1] = solution1.ObjectiveValues[1];
}	

TWCEPC_4Solution::TWCEPC_4Solution(GroupingGenome chr, MachineSet sched, float pref, bool val, vector<double> delays){
	this->_chromosome = chr;
	this->_schedule = sched;
	this->_twcPref = pref;
	this->_valid = val;
	this->_delayT = delays;
	this->setObjectiveValues();
}
TWCEPC_4Solution::~TWCEPC_4Solution() {
}
void TWCEPC_4Solution::Mutate() {
	//this->_TWCPref = (float) rand() / RAND_MAX;
	//this->setObjectiveValues_Pref();
	//this->consolidateBatches();
	return;
}



bool TWCEPC_4Solution::decodeChromosome() {
	int iMax = this->_schedule.size();
	int bMax = this->_chromosome.size();
	
	//vector< BatchSet > BatchesPerMachine;
	//vector<int> SumDelayPerMachine;
	//vector<double> SumPiPerMachine;

	//for(int i = 0; i < iMax; i++) {
	//	BatchesPerMachine.push_back(BatchSet());
	//}
	// assign Batches to machines
	/*for(int i = 0; i < bMax; i++) {
		if(!this->_chromosome[i]->empty()) {
			BatchesPerMachine[floor(this->_chromosome[i]->key)].addBatch(*(this->_chromosome[i]));
		}
	}

	// compute sumPi + sumDelay per machine 
	for(int i = 0; i < iMax; i++) {
		double sumPi = 0.0;
		int sumDelay = 0;
		int bPerMachine = BatchesPerMachine[i].size();
		for(int b = 0; b < bPerMachine; b++) {
			sumPi += BatchesPerMachine[i].getBatch(b).key - floor(BatchesPerMachine[i].getBatch(b).key);
			if(b > 0) {
				BatchesPerMachine[i].getBatch(b).setStart(max(BatchesPerMachine[i].getBatch(b-1).getC(), BatchesPerMachine[i].getBatch(b).r));
				
			} else {
				BatchesPerMachine[i].getBatch(b).setStart(max(0, BatchesPerMachine[i].getBatch(b).r));
			}
		}
		sumPi += this->_delayT[i] - floor(this->_delayT[i]);
		SumPiPerMachine.push_back(sumPi);
		sumDelay = Global::problem->T - BatchesPerMachine[i].getBatch(bPerMachine-1).getC();
		SumDelayPerMachine.push_back(sumDelay);
	}*/

	this->updateSchedule();
	this->locSshiftJobs();
	this->locSswapJobs();
	this->balanceLoad();			
	this->updateSchedule();

	// TODO maybe move this check to MachineSet::listSched
	if(!this->hasFeasibleSchedule()){
		return false;
	}

	this->improveTWC();
	this->improveEPC();
	
	return true;
}



void TWCEPC_4Solution::setObjectiveValues() {
	if(!this->decodeChromosome()) {
		this->ObjectiveValues[0] = 999999;
		this->ObjectiveValues[1] = 999999;	
	} else {
		this->ObjectiveValues[0] = this->getTWC();
		this->ObjectiveValues[1] = this->getEPC();
	}
}

MachineSet TWCEPC_4Solution::getSchedule() {
	return this->_schedule;
}
int TWCEPC_4Solution::getLastNonemptyBatch(int machine) {
	// get the _chromosome´s index of the last non-empty batch at machine
	int bMax = this->_chromosome.size();
	for(int b = bMax-1; b >= 0; b--) {
		if((int) floor(this->_chromosome[b]->key) == machine) {
			if(!this->_chromosome[b]->empty()){
				return b;
			}
		}
	}
	return -1;
}
int TWCEPC_4Solution::getLastEmptyBatch(int machine) {
	// get the _chromosome´s index of the last empty batch at machine
	int bMax = this->_chromosome.size();
	for(int b = bMax-1; b >= 0; b--) {
		if((int) floor(this->_chromosome[b]->key) == machine) {
			if(this->_chromosome[b]->empty()){
				return b;
			}
		}
	}
	return -1;
}

vector<int> TWCEPC_4Solution::getPausesPerMachine(){
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
double TWCEPC_4Solution::getTWC() {
	return this->_schedule.getTWC();
}

double TWCEPC_4Solution::getEPC() {
	if(Global::gv_eRates <= 1) {
		return this->_schedule.getEPC(Global::problem->e1);
	}
	return this->_schedule.getEPC(Global::problem->e2);
}
bool TWCEPC_4Solution::isValid() {
	return this->_valid;
}

double TWCEPC_4Solution::improveTWC() {		// locally improve TWC, not accepting increase of EPC
	// step 1: consider batches from left to right, shift batch left if both EPC and TWC are not increased
	double TWCImprove = this->_schedule.improveTWC();
	this->ObjectiveValues[0] = this->_schedule.getTWC();
	if(Global::gv_eRates <= 1) {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e1);
	}
	else {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e2);
	}
	return TWCImprove;
}						
double TWCEPC_4Solution::improveEPC() {		// locally improve EPC, not accepting increase of TWC
	// step 1: consider batches from right to left, shift batch right if both EPC and TWC are not increased
	double epcImprove = this->_schedule.improveEPC();
	this->ObjectiveValues[0] = this->_schedule.getTWC();
	if(Global::gv_eRates <= 1) {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e1);
	}
	else {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e2);
	}
	return epcImprove;
}					
double TWCEPC_4Solution::improveCombined(double ratio) {		// locally improve ratio * TWC + (1-ratio) * EPC
	// step 1a: consider batches from left to right, shift batch left if ratio * TWC + (1-ratio) * EPC is not increased
	// step 1b: consider batches from right to left, shift batch right if ratio * TWC + (1-ratio) * EPC is not increased
	double combined = this->_schedule.improveCombined(ratio);
	this->ObjectiveValues[0] = this->_schedule.getTWC();
		if(Global::gv_eRates <= 1) {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e1);
	}
	else {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e2);
	}
	return combined;
}
void TWCEPC_4Solution::locSswapJobs() {							// local search: pairwise swap jobs between two batches
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
void TWCEPC_4Solution::locSswapJobsAdj() {						// local search: pairwise swap jobs between two adjacent batches
	cout << "TWCEPC_4Solution::locSswapJobsAdj() not implemented" << endl;
}
void TWCEPC_4Solution::locSshiftJobs() {						// local search: shift jobs to another batch
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
void TWCEPC_4Solution::locSshiftJobsAdj() {						// local search: shift jobs to another adjacent batch
	cout << "TWCEPC_4Solution::locSshiftJobsAdj() not implemented" << endl;
}
void TWCEPC_4Solution::locSsplitBatch() {						// local search: split a single batch into two batches
	cout << "TWCEPC_4Solution::locSsplitBatch() not implemented" << endl;
}
MachineSet& TWCEPC_4Solution::updateSchedule(){
	int iMax = this->_schedule.size();
	int bMax = this->_chromosome.size();
	vector< BatchSet > BatchesPerMachine;
	vector<int> SumDelayPerMachine;
	vector<double> SumPiPerMachine;

	// clear machines
	for(unsigned i = 0; i < iMax; i++) {
		this->_schedule[i].clear();
	}

	for(int i = 0; i < iMax; i++) {
		BatchesPerMachine.push_back(BatchSet());
	}

	for(int i = 0; i < bMax; i++) {
		if(!this->_chromosome[i]->empty()) {
			int machineIdx = (int)floor(this->_chromosome[i]->key);
			BatchesPerMachine[machineIdx].addBatch(*(this->_chromosome[i]));
		}
	}

	// compute sumPi + sumDelay per machine 
	for(int i = 0; i < iMax; i++) {
		double sumPi = 0.0;
		int sumDelay = 0;
		int bPerMachine = BatchesPerMachine[i].size();
		for(int b = 0; b < bPerMachine; b++) {
			sumPi += BatchesPerMachine[i].getBatch(b).key - floor(BatchesPerMachine[i].getBatch(b).key);
			if(b > 0) {
				BatchesPerMachine[i].getBatch(b).setStart(max(BatchesPerMachine[i].getBatch(b-1).getC(), BatchesPerMachine[i].getBatch(b).r));
				
			} else {
				BatchesPerMachine[i].getBatch(b).setStart(max(0, BatchesPerMachine[i].getBatch(b).r));
			}
		}
		sumPi += this->_delayT[i] - floor(this->_delayT[i]);
		SumPiPerMachine.push_back(sumPi);
		if(bPerMachine > 0) {
			sumDelay = Global::problem->T - BatchesPerMachine[i].getBatch(bPerMachine-1).getC();
		} else {
			sumDelay = Global::problem->T;
		}
		SumDelayPerMachine.push_back(sumDelay);
	}

	// schedule batches on machines
	for(unsigned b = 0; b < bMax; b++) {
		if(!this->_chromosome[b]->empty()) {
			int machineIdx = (int) floor(this->_chromosome[b]->key);
			double piFraction = this->_chromosome[b]->key - floor(this->_chromosome[b]->key);
			this->_schedule[machineIdx].addBatch(*this->_chromosome[b]);
			int delay = max<int>(0, piFraction / SumPiPerMachine[machineIdx] * SumDelayPerMachine[machineIdx]);
			this->_schedule[machineIdx].getBatch(this->_schedule[machineIdx].getQueueSize()-1).setStart(this->_schedule[machineIdx].getBatch(this->_schedule[machineIdx].getQueueSize()-1).getStart() + delay);
		}
	}
	return this->_schedule;
}
void TWCEPC_4Solution::consolidateBatches() {
	int bMax = this->_chromosome.size();
	// pass through chromosome from right to left
	for(int src = (bMax-1); src > 0; src--) {
		// try to shift jobs to batches from left to right
		if(!this->_chromosome[src]->empty()) {
			for(unsigned dst = 0; dst < src; dst++) {
				this->_chromosome.shiftJobsTWC(src, dst);		// TODO: choose most suitable shiftJobs version (see GroupingGenome.h)
				//this->_chromosome.shiftJobsEPC(src, dst);
			}
		}
	}
	this->_chromosome.moveNonEmptyBatchesToFront();
}


bool TWCEPC_4Solution::balanceLoad() {
	bool loadIsBalanced = false;
	this->updateSchedule();
	int iMax = this->_schedule.size();
	int bMax = this->_chromosome.size();
	int breakout = 0;
	int minPause, maxPause, minPauseMachine, maxPauseMachine;

	vector<int> pausesPerMachine = this->getPausesPerMachine();

	if(!this->_schedule.isEmpty()){
		while(!loadIsBalanced) {
			breakout++;
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
					if(minPause < maxPause) {
						
						int srcBatch = this->getLastNonemptyBatch(minPauseMachine);
						int addDelay = 0;

						if(pausesPerMachine[maxPauseMachine] - this->_chromosome[srcBatch]->pLot > pausesPerMachine[minPauseMachine] + this->_chromosome[srcBatch]->pLot + addDelay) {
							// reassigning the batch does not reduce the (new) pauses of machine with more pauses below the (new) level of the other one
							this->_chromosome[srcBatch]->key = this->_chromosome[srcBatch]->key - floor(this->_chromosome[srcBatch]->key) + maxPauseMachine;
							this->updateSchedule();
							pausesPerMachine = this->getPausesPerMachine();
							if(breakout > bMax) {
								loadIsBalanced = true;
							}
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

bool TWCEPC_4Solution::hasFeasibleSchedule(){
	int iMax = this->_schedule.size();
	for(int i = 0; i < iMax; i++) {
		if(this->_schedule[i].getMSP() > Global::problem->T) {
			return false;
		}
	}
	return true;
}


void TWCEPC_4Solution::applyLocalSearch(double weightTWC, double weightEPC) {
	// TODO implement local search
	// cout << "TWCEPCSolution::applyLocalSearch(double, double) called." << endl;
	this->improveCombined(weightTWC / (weightTWC + weightEPC));

	// TODO swap jobs between batches as long as integrared objective function value decreases

}

GroupingGenome TWCEPC_4Solution::getChromosome() {
	return this->_chromosome;
}

float TWCEPC_4Solution::getTwcPref(){
	return this->_twcPref;
}
bool TWCEPC_4Solution::getValid(){
	return this->_valid;
}
vector<double> TWCEPC_4Solution::getDelayT(){
	return this->_delayT;
}

void TWCEPC_4Solution::setChromosome(GroupingGenome chr){
	this->_chromosome = chr;
}
void TWCEPC_4Solution::setSchedule(MachineSet sched){
	this->_schedule = sched;
}
void TWCEPC_4Solution::setTwcPref(float pref){
	this->_twcPref = pref;
}
void TWCEPC_4Solution::setValid(bool val){
	this->_valid = val;
}
void TWCEPC_4Solution::setDelayT(vector<double> delays){
	this->_delayT = delays;
}