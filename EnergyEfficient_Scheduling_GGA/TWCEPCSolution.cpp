#include "TWCEPCSolution.h"

TWCEPCSolution::TWCEPCSolution() {			// standard constructor => random initialization of one individual
	_valid = true;
	_twcPref = (float) rand() / RAND_MAX;	// preference for TWC minimization is set U~[0, 1]
	ObjectiveValues.resize(2);	// TODO put 2 into Global::parameters
	int cur_batch;

	// initialize a machineset/schedule for evaluation of the chromosome
	this->_schedule.clear();
	this->_delayT.clear();
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

	// distribute jobs between batches in random manner
	// Original Version: Random distribution
	for(unsigned i = 0; i < Global::problem->n; i++){
		do {
			cur_batch = rand() % Global::problem->n;
		} while(!_chromosome[cur_batch]->addJob(&Global::problem->jobs[i]));
	}

	this->_chromosome.moveNonEmptyBatchesToFront();
	// decode the chromosome and set objective values

	// Special Case version: One-to-one distribution
	/* vector<int> batchIndices;
	for(int b = 0; b < Global::problem->n; b++) {
		batchIndices.push_back(b);
	}
	int bMax = Global::problem->n;
	int mMax = Global::problem->m;
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
	} */
	// End of Special Case version
	

	// VARIATION
	this->_chromosome.sortBy_wpLot(false);	// Sort WSPT
	
	// VARIATION
	this->consolidateBatches();

	// set random keys and delays after the last batches
	for(unsigned i = 0; i < Global::problem->n; i++) {
		_chromosome[i]->key = ((double) rand() / ((double) RAND_MAX + 1.0)) * Global::problem->m;		// Random Value between 0 and mMax	
	}
	for(unsigned i = 0; i < Global::problem->m; i++) {
		_delayT[i] = i + ((double) rand() / ((double) RAND_MAX + 1.0));					// Random Value between i and i+1			
	}
	
	// VARIATION
	//this->setObjectiveValues_ASAP();
	this->setObjectiveValues_VLIST();
}							

TWCEPCSolution::TWCEPCSolution(TWCEPCSolution & p1, TWCEPCSolution & p2) {	// Recombination (Crossover)
	_valid = true;
	_twcPref = p2._twcPref;							// inherit Objective function preference from p2
	_delayT = p2._delayT;							// inherit delayT from p2

	this->_chromosome.resize(Global::problem->n);	// TODO: check
	for(unsigned i = 0; i < Global::problem->n; i++) {
		this->_chromosome[i] = new Batch(Global::problem->k);
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

	// move mon-empty batches to the front => variable length chromosome includes all non-empty batches only
	p1._chromosome.moveNonEmptyBatchesToFront();	// TODO: maybe WSPT instead?!
	p2._chromosome.moveNonEmptyBatchesToFront();	// TODO: maybe WSPT instead?!

	// 1. Select two crossing sites of the first parent
	//Debugger::iDebug("Selecting crossing sites for p1 ...");
	//p1.write(cout);

	cs1_end = p1._chromosome.firstEmpty(true) - 1;
	if (cs1_end < 0) cs1_end = p1._chromosome.size() - 1;

	cs11 = rand() % (cs1_end + 1); //GARandomInt(0, cs1_end/* - 1*/);
	//cout << "Selecting cs1x from [" << 0 << "," << cs1_end << "]" << endl;
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
		this->_chromosome[i]->copy(p2._chromosome[i]); // = new Batch(p2._chromosome[i]); 
	}

	// Copy p1's batches eliminating the elements from contained : first part
	for (int i = 0; i < cs11; i++) {
		this->_chromosome[i]->copy(p1._chromosome[i]); // = new Batch(p1._chromosome[i]);				
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
	for (int i = cs12 + 1; i <= cs1_end/*p1_first_empty*/; i++) {
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
	
	// TODO mit leere batches auffüllen
	//this->_chromosome.moveNonEmptyBatchesToFront();
	//int batchesInChild = this->_chromosome.size();
	//for(unsigned i = batchesInChild; i < Global::problem->n; i++) {
	//	this->_chromosome.push_back(new Batch(Global::problem->k));
	//}
	
	this->_chromosome.reinsert(missing);
	// VARIATION this->_chromosome.reinsertReady(missing);

	// VARIATION
	//this->_chromosome.moveNonEmptyBatchesToFront();
	
	// VARIATION
	this->_chromosome.sortBy_wpLot(false);
	this->consolidateBatches();
	// decode the chromosome and set objective values
	// VARIATION
	//this->setObjectiveValues();		// simpler version (list-sched asap)
	this->setObjectiveValues_VLIST();
	//this->setObjectiveValues_ASAP();

	// move mon-empty batches to the front => variable length chromosome includes all non-empty batches only
	
	

}		
TWCEPCSolution::TWCEPCSolution(TWCEPCSolution & p1, TWCEPCSolution & p2, bool nonDelay) {	// Recombination (Crossover)
	if(nonDelay){
			_valid = true;
			_twcPref = p2._twcPref;							// inherit Objective function preference from p2
			_delayT = p2._delayT;							// inherit delayT from p2
		
			// initialize chromosome
			this->_chromosome.resize(Global::problem->n);
			for(int i = 0; i < Global::problem->n; i++) {
				this->_chromosome[i] = new Batch(Global::problem->k);
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
			TWCEPCSolution::TWCEPCSolution(p1, p2);
		}

}		
TWCEPCSolution::TWCEPCSolution(TWCEPCSolution& solution1){								// copy constructor
	_valid = solution1._valid;
	_twcPref = solution1._twcPref; // preference for TWC minimization is copied
	ObjectiveValues.resize(2);	// TODO put 2 into Global::parameters
	_schedule.copy(&solution1._schedule);
	this->_chromosome.copy(&solution1._chromosome);
	// decode the chromosome and set objective values
	this->ObjectiveValues[0] = solution1.ObjectiveValues[0];
	this->ObjectiveValues[1] = solution1.ObjectiveValues[1];
	
}	


TWCEPCSolution::TWCEPCSolution(GroupingGenome chr, MachineSet sched, float pref, bool val, vector<double> delays){
	this->_chromosome = chr;
	this->_schedule = sched;
	this->_twcPref = pref;
	this->_valid = val;
	this->_delayT = delays;
	this->setObjectiveValues_VLIST();
}
TWCEPCSolution::~TWCEPCSolution() {
}
void TWCEPCSolution::Mutate() {
	//this->_twcPref = (float) rand() / RAND_MAX;
	//this->setObjectiveValues_Pref();
	//this->consolidateBatches();
	return;
}



bool TWCEPCSolution::decodeChromosome() {
	// clear machines
	int iMax = this->_schedule.size();
	for(unsigned i = 0; i < iMax; i++) {
		this->_schedule[i].clear();
	}
	/// schedule batches on machines
	this->_schedule.listSched(this->_chromosome);

	// TODO maybe move this check to MachineSet::listSched
	for(unsigned i = 0; i < iMax; i++) {
		if(this->_schedule[i].getMSP() > Global::problem->e1.size()){
			return false;
		}
	}

	// TODO maybe apply local optimization

	return true;
}


bool TWCEPCSolution::decodeChromosome_VLIST() {
	// get number of non-empty batches
	//this->_chromosome.moveNonEmptyBatchesToFront();
	//this->_chromosome.sortBy_wpLot(false);
	int bMax = this->_chromosome.firstEmpty(true);	
	if(bMax == -1) {
		bMax = this->_chromosome.size();
	}
	int seperatorTWC = (int) floor( (this->_twcPref * (double)bMax) + 0.5);		// batches 0 - seperatorTWC are schedule for a low TWC, seperatorTWC - (bMax-1) are scheduled for a low EPC
	// clear machines
	int iMax = this->_schedule.size();
	for(unsigned i = 0; i < iMax; i++) {
		this->_schedule[i].clear();
	}
	/// schedule batches on machines
	// batches from 0 - seperatorTWC are scheduled for a low TWC
	for(int b = 0; b < seperatorTWC; b++) {
		if(!this->_schedule.listSched_ASAP(*this->_chromosome[b])) {		// VARIATION: listSched_TWC
			return false;
		}
	}
	// batches from seperatorTWC - (bMax-1) are scheduled for a low EPC
	for(int b = seperatorTWC; b < bMax; b++) {
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

	this->updateSchedule();
	this->locSshiftJobs();
	this->locSswapJobs();			
	this->updateSchedule();

	for(unsigned i = 0; i < iMax; i++) {
		if(this->_schedule[i].getMSP() > Global::problem->e1.size()){
			return false;
		}
	}
	
	// VARIATION: local search, shift batches to improve either TWC or EPC not increasing the other objective
	this->improveTWC();
	this->improveEPC();

	// TODO maybe apply local optimization
	return true;
}
bool TWCEPCSolution::decodeChromosome_ASAP() {
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
	// batches from 0 - seperatorTWC are scheduled for a low TWC
	for(int b = 0; b < bMax; b++) {
		if(!this->_schedule.listSched_ASAP(*this->_chromosome[b])) {
			return false;
		}
	}
	// batches from seperatorTWC - (bMax-1) are scheduled for a low EPC

	for(unsigned i = 0; i < iMax; i++) {
		if(this->_schedule[i].getMSP() > Global::problem->e1.size()){
			return false;
		}
	}
	
	// VARIATION: local search, shift batches to improve either TWC or EPC not increasing the other objective
	this->improveTWC();
	this->improveEPC();

	// TODO maybe apply local optimization
	return true;
}
void TWCEPCSolution::setObjectiveValues() {
	if(!this->decodeChromosome()) {
		this->ObjectiveValues[0] = 999999;
		this->ObjectiveValues[1] = 999999;	
	} else {
		this->ObjectiveValues[0] = this->getTWC();
		this->ObjectiveValues[1] = this->getEPC();
	}
}
void TWCEPCSolution::setObjectiveValues_VLIST(){			// derives a schedule considering the solution´s TWC preference
	if(!this->decodeChromosome_VLIST()) {
		this->ObjectiveValues[0] = 999999;
		this->ObjectiveValues[1] = 999999;	
	} else {
		// TODO: local search
		//this->_schedule.improveTWC();
		//this->_schedule.improveEPC();
		this->ObjectiveValues[0] = this->getTWC();
		this->ObjectiveValues[1] = this->getEPC();
	}
}
void TWCEPCSolution::setObjectiveValues_ASAP(){			// derives a schedule considering the solution´s TWC preference
	if(!this->decodeChromosome_ASAP()) {
		this->ObjectiveValues[0] = 999999;
		this->ObjectiveValues[1] = 999999;	
	} else {
		// TODO: local search
		this->_schedule.improveTWC();
		this->_schedule.improveEPC();
		this->ObjectiveValues[0] = this->getTWC();
		this->ObjectiveValues[1] = this->getEPC();
	}
}
void TWCEPCSolution::applyLocalSearch(double weightTWC, double weightEPC) {
	// TODO implement local search
	// cout << "TWCEPCSolution::applyLocalSearch(double, double) called." << endl;
	this->improveCombined(weightTWC / (weightTWC + weightEPC));

	// TODO swap jobs between batches as long as integrared objective function value decreases

}

double TWCEPCSolution::getTWC() {
	return this->_schedule.getTWC();
}

double TWCEPCSolution::getEPC() {
	if(Global::gv_eRates <= 1) {
		return this->_schedule.getEPC(Global::problem->e1);
	}
	return this->_schedule.getEPC(Global::problem->e2);
}
double TWCEPCSolution::getCombinedObjective(double twcWeight, double epcWeight) {
	double epc = 0;
	double twc = 0;
	if(Global::gv_eRates <= 1) {
		epc = this->_schedule.getEPC(Global::problem->e1);
	} else {
		epc = this->_schedule.getEPC(Global::problem->e2);
	}
	twc = this->_schedule.getTWC();
	return ((twcWeight / (twcWeight + epcWeight)) * twc) + ((epcWeight / (twcWeight + epcWeight)) * epc);
}




MachineSet TWCEPCSolution::getSchedule() {
	return this->_schedule;
}
bool TWCEPCSolution::isValid() {
	return this->_valid;
}
double TWCEPCSolution::improveTWC() {		// locally improve TWC, not accepting increase of EPC
	// step 1: consider batches from left to right, shift batch left if both EPC and TWC are not increased
	double twcImprove = this->_schedule.improveTWC();
	this->ObjectiveValues[0] = this->_schedule.getTWC();
	if(Global::gv_eRates <= 1) {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e1);
	}
	else {
		this->ObjectiveValues[1] = this->_schedule.getEPC(Global::problem->e2);
	}
	return twcImprove;
}						
double TWCEPCSolution::improveEPC() {		// locally improve EPC, not accepting increase of TWC
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
double TWCEPCSolution::improveCombined(double ratio) {		// locally improve ratio * TWC + (1-ratio) * EPC
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
void TWCEPCSolution::consolidateBatches() {
	int bMax = this->_chromosome.size();
	// pass through chromosome from right to left
	for(int src = (bMax-1); src > 0; src--) {
		// try to shift jobs to batches from left to right
		if(!this->_chromosome[src]->empty()) {
			for(unsigned dst = 0; dst < src; dst++) {
				//this->_chromosome.shiftJobsTWC(src, dst);		// TODO: choose most suitable shiftJobs version (see GroupingGenome.h)
				this->_chromosome.shiftJobsEPC(src, dst);
			}
		}
	}
	this->_chromosome.moveNonEmptyBatchesToFront();
}
void TWCEPCSolution::locSswapJobs(){		// local search: pairwise swap jobs between two batches
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
void TWCEPCSolution::locSswapJobsAdj(){		// local search: pairwise swap jobs between two adjacent batches
	cout << "TWCEPCSolution::locSswapJobsAdj() not implemented." << endl;
}						
void TWCEPCSolution::locSshiftJobs(){		// local search: shift jobs to another batch
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
void TWCEPCSolution::locSshiftJobsAdj(){	// local search: shift jobs to another adjacent batch
	cout << "TWCEPCSolution::locSshiftJobsAdj() not implemented." << endl;
}						
void TWCEPCSolution::locSsplitBatch(){		// local search: split a single batch into two batches
	cout << "TWCEPCSolution::locSsplitBatch() not implemented." << endl;
}						
bool TWCEPCSolution::updateSchedule(){	// derive the schedule from the current chromosome
	int bMax = this->_chromosome.firstEmpty(true);	
	if(bMax == -1) {
		bMax = this->_chromosome.size();
	}
	int seperatorTWC = (int) floor( (this->_twcPref * (double)bMax) + 0.5);		// batches 0 - seperatorTWC are schedule for a low TWC, seperatorTWC - (bMax-1) are scheduled for a low EPC
	// clear machines
	int iMax = this->_schedule.size();
	for(unsigned i = 0; i < iMax; i++) {
		this->_schedule[i].clear();
	}
	/// schedule batches on machines
	// batches from 0 - seperatorTWC are scheduled for a low TWC
	for(int b = 0; b < seperatorTWC; b++) {
		if(!this->_schedule.listSched_ASAP(*this->_chromosome[b])) {
			return false;
		}
	}
	// batches from seperatorTWC - (bMax-1) are scheduled for a low EPC
	for(int b = seperatorTWC; b < bMax; b++) {
		if(Global::gv_eRates <= 1) {
			if(!this->_schedule.listSched_EPC(*this->_chromosome[b], Global::problem->e1)){
				return false;
			}
		}
		else {
			if(!this->_schedule.listSched_EPC(*this->_chromosome[b], Global::problem->e2)) {
				return false;
			}
		}
	}
	return true;
}				
						
GroupingGenome TWCEPCSolution::getChromosome() {
	return this->_chromosome;
}

float TWCEPCSolution::getTwcPref(){
	return this->_twcPref;
}
bool TWCEPCSolution::getValid(){
	return this->_valid;
}
vector<double> TWCEPCSolution::getDelayT(){
	return this->_delayT;
}

void TWCEPCSolution::setChromosome(GroupingGenome chr){
	this->_chromosome = chr;
}
void TWCEPCSolution::setSchedule(MachineSet sched){
	this->_schedule = sched;
}
void TWCEPCSolution::setTwcPref(float pref){
	this->_twcPref = pref;
}
void TWCEPCSolution::setValid(bool val){
	this->_valid = val;
}
void TWCEPCSolution::setDelayT(vector<double> delays){
	this->_delayT = delays;
}