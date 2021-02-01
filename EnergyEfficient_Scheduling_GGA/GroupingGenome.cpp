
#include "GroupingGenome.h"

GroupingGenome::GroupingGenome(){}
GroupingGenome::GroupingGenome(const GroupingGenome &other) {
	this->copy((const GroupingGenome *) &other);
}
GroupingGenome::~GroupingGenome(){
	int n = this->size();
	for(int i = 0; i < n; i++) {
		delete this->at(i);
	}
}

void GroupingGenome::init() {
	destroy();
	this->resize(0);
}
void GroupingGenome::destroy() {
	int n = this->size();
	// Destroys all of the containers
	for (int i = 0; i < n; i++) {
		if ((*this)[i] != NULL) {
			// TODO delete all jobs 23.07.2018
			for(unsigned j = 0; j < (*this)[i]->numJobs; j++) {
				(*this)[i]->destroy();
			}
			delete (*this)[i];
		}
		(*this)[i] = NULL;
	}
}

void GroupingGenome::copy(const GroupingGenome *other) {
    if (this == other) return;
    this->destroy();
    int n = other->size();
    this->resize(n);

    for (int i = 0; i < n; i++) {
		(*this)[i] = new Batch(); //new Batch();		
		(*this)[i]->copy((*other)[i]);
    }
}
void GroupingGenome::clean() {			// Initialize all Genes (NOT clear()
    int n = this->size();
    for (int i = 0; i < n; i++) {
		(*this)[i]->clear();
	}
}	
int GroupingGenome::firstEmpty(const bool from_start) {
	int n = this->size();
	if(from_start) {
		for (unsigned i = 0; i < n; i++) {
			if(this->at(i)->numJobs == 0) {
				return i;
			}
		}
	} else {
		for (int i = n-1; i >= 0; i--) {
			if(this->at(i)->numJobs == 0) {
				return i;
			}
		}
	}
	return -1;
}
int GroupingGenome::predecessorOnMachine(int batchIdx){
	int n = this->size();
	int assignedM = 0;
	if(batchIdx > 0 && batchIdx < n) {
		assignedM = (int) floor(this->at(batchIdx)->key);
		for(int i = batchIdx - 1; i >= 0; i--) {
			if(floor(this->at(i)->key) == assignedM) {
				return i;
			}
		}
	}
	return -1; // no predecessor
}
void GroupingGenome::operator=(const GroupingGenome &other) {
	copy((const GroupingGenome *) & other);
}







// Initialization
void GroupingGenome::initializeRandom() {
	// ACTUAL INITIALIZATION IS DONE IN ***SOLUTION CLASSES (MOMHLib)
	this->clear();
	this->resize(Global::problem->n);
	for(unsigned i = 0; i < Global::problem->n; i++) {
//		do {
// TODO:			cur_batch = GARandomInt(0, Global::nJobs - 1);
//		} while (!batches[cur_batch].addOrder(&order[i]));
	}

}
void GroupingGenome::initializeEDD() {
	// ACTUAL INITIALIZATION IS DONE IN ***SOLUTION CLASSES (MOMHLib)
	cout << "GroupingGenome::initializeEDD() not implemented!" << endl;
};
void GroupingGenome::initializeTWD(){
	// ACTUAL INITIALIZATION IS DONE IN ***SOLUTION CLASSES (MOMHLib)
	cout << "GroupingGenome::initializeTWD() not implemented!" << endl;
};

// Mutation
void GroupingGenome::mutShift(const float prob){
	cout << "GroupingGenome::mutShift(const float) not implemented." << endl;
}
void GroupingGenome::mutSwap(const float prob){
	cout << "GroupingGenome::mutSwap(const float) not implemented." << endl;
}

// Refinement
bool GroupingGenome::shiftJobsForDominance(const int srcBatchIdx, const int dstBatchIdx) {
	cout << "GroupingGenome::shiftJobsForDominance is not yet implemented." << endl;
	if(srcBatchIdx >= 0 && srcBatchIdx < this->size() && dstBatchIdx >= 0 && dstBatchIdx < this->size() && srcBatchIdx != dstBatchIdx) {
		if(this->at(srcBatchIdx)->numJobs <= 0) {
			// no Jobs contained in source
			return false;
		}


	}
	return false;
}
bool GroupingGenome::shiftJobsTWT(const int srcBatchIdx, const int dstBatchIdx) {
	// shift a job from source to destination batch if (assume that dst큦 completion <= scr큦 completion)
	if(srcBatchIdx >= 0 && srcBatchIdx < this->size() && dstBatchIdx >= 0 && dstBatchIdx < this->size() && srcBatchIdx != dstBatchIdx) {
		if(this->at(srcBatchIdx)->numJobs > 0 && this->at(srcBatchIdx)->f == (this->at(dstBatchIdx)->f || this->at(dstBatchIdx)->f == 0  && this->at(dstBatchIdx)->freeCapacity > 0)) {
			vector<int> jobIds2bShifted;
			for(unsigned i = 0; i < this->at(srcBatchIdx)->numJobs; i++) {
				jobIds2bShifted.push_back(this->at(srcBatchIdx)->getJob(i).id);
			}
			// sort jobs by weight
			// make_heap(jobIds2bShifted.begin(), jobIds2bShifted.end());
			// sort_heap(jobIds2bShifted.begin(), jobIds2bShifted.end(), compareJobWeight);
			sort(jobIds2bShifted.begin(), jobIds2bShifted.end(), compareJobWeight);
			bool jobsShifted = false;
			for(unsigned i = 0; i < jobIds2bShifted.size(); i++) {
				if(Global::problem->jobs.getJobByID(jobIds2bShifted[i])->s <= this->at(dstBatchIdx)->freeCapacity) {
					if(Global::problem->jobs.getJobByID(jobIds2bShifted[i])->r <= this->at(dstBatchIdx)->r)	{		// IMPORTANT
						// job will not increase the batch큦 release time
						// capacity available, addJob to dst, eraseJob from src
						if(this->at(dstBatchIdx)->addJob(Global::problem->jobs.getJobByID(jobIds2bShifted[i]))) {
							this->at(srcBatchIdx)->erase(jobIds2bShifted[i]);
							jobsShifted = true;
							if(this->at(dstBatchIdx)->freeCapacity <= 0) {
								break;
							}
						}
					}
				} 
			}
			return jobsShifted;
		}
		// no Jobs contained in source, incompatible families or no free capacity
		return false;
	}
	// index out of bounds
	return false;
}
bool GroupingGenome::shiftJobsEPC(const int srcBatchIdx, const int dstBatchIdx) {
	// shift jobs aiming at a decreased EPC => try to shift as many jobs into the destination batch so eventually the source batch can be cleared
	if(srcBatchIdx >= 0 && srcBatchIdx < this->size() && dstBatchIdx >= 0 && dstBatchIdx < this->size() && srcBatchIdx != dstBatchIdx) {
		if(this->at(srcBatchIdx)->numJobs > 0 && this->at(dstBatchIdx)->numJobs > 0 && this->at(srcBatchIdx)->f == this->at(dstBatchIdx)->f && this->at(dstBatchIdx)->freeCapacity > 0) {
			vector<int> jobIds2bShifted;
			for(unsigned i = 0; i < this->at(srcBatchIdx)->numJobs; i++) {
				jobIds2bShifted.push_back(this->at(srcBatchIdx)->getJob(i).id);
			}
			// sort jobs by size
			// make_heap(jobIds2bShifted.begin(), jobIds2bShifted.end());
			//sort_heap(jobIds2bShifted.begin(), jobIds2bShifted.end(), compareJobSize); 
			sort(jobIds2bShifted.begin(), jobIds2bShifted.end(), compareJobSize);
			// shift jobs
			bool jobsShifted = false;
			for(unsigned i = 0; i < jobIds2bShifted.size(); i++) {
				if(Global::problem->jobs.getJobByID(jobIds2bShifted[i])->s <= this->at(dstBatchIdx)->freeCapacity) {
					// TODO: conditions to shift a job
					// capacity available, addJob to dst, eraseJob from src
					if(this->at(dstBatchIdx)->addJob(Global::problem->jobs.getJobByID(jobIds2bShifted[i]))) {
						this->at(srcBatchIdx)->erase(jobIds2bShifted[i]);
						jobsShifted = true;
						if(this->at(dstBatchIdx)->freeCapacity <= 0) {
							break;
						}
					}
				} 
			}
			return jobsShifted;
		}
		// else, no jobs contained in either src or dst, incompatible families or no free capacity
	}
	// index out of bounds or src eq dst
	return false;
}
bool GroupingGenome::shiftJobsTWC(const int srcBatchIdx, const int dstBatchIdx) {
	// shift a job from source to destination batch if (assume that dst큦 completion <= scr큦 completion)
	if(srcBatchIdx >= 0 && srcBatchIdx < this->size() && dstBatchIdx >= 0 && dstBatchIdx < this->size() && srcBatchIdx != dstBatchIdx) {
		if(this->at(srcBatchIdx)->numJobs > 0 && this->at(srcBatchIdx)->f == (this->at(dstBatchIdx)->f || this->at(dstBatchIdx)->f == 0  && this->at(dstBatchIdx)->freeCapacity > 0)) {
			vector<int> jobIds2bShifted;
			for(unsigned i = 0; i < this->at(srcBatchIdx)->numJobs; i++) {
				jobIds2bShifted.push_back(this->at(srcBatchIdx)->getJob(i).id);
			}
			// sort jobs by weight
			// make_heap(jobIds2bShifted.begin(), jobIds2bShifted.end());
			// sort_heap(jobIds2bShifted.begin(), jobIds2bShifted.end(), compareJobWeight);
			sort(jobIds2bShifted.begin(), jobIds2bShifted.end(), compareJobWeight);
			bool jobsShifted = false;
			for(unsigned i = 0; i < jobIds2bShifted.size(); i++) {
				if(Global::problem->jobs.getJobByID(jobIds2bShifted[i])->s <= this->at(dstBatchIdx)->freeCapacity) {
					if(Global::problem->jobs.getJobByID(jobIds2bShifted[i])->r <= this->at(dstBatchIdx)->r)	{		// IMPORTANT
						// job will not increase the batch큦 release time
						// capacity available, addJob to dst, eraseJob from src
						if(this->at(dstBatchIdx)->addJob(Global::problem->jobs.getJobByID(jobIds2bShifted[i]))) {
							this->at(srcBatchIdx)->erase(jobIds2bShifted[i]);
							jobsShifted = true;
							if(this->at(dstBatchIdx)->freeCapacity <= 0) {
								break;
							}
						}
					}
				} 
			}
			return jobsShifted;
		}
		// no Jobs contained in source, incompatible families or no free capacity
		return false;
	}
	// index out of bounds
	return false;
}
bool GroupingGenome::swapJobsPossible(const int batchId1, const int jobId1, const int batchId2, const int jobId2) {
	if( batchId1 >= 0 && batchId1 <= this->size() && batchId2 >= 0 && batchId2 <= this->size() ) {
		if(!this->at(batchId1)->contains(jobId1) || !this->at(batchId2)->contains(jobId2)) {
			// batch does not contain the job
			return false;
		}
		else {
			// check for matching families and sufficient capacity
			return (this->at(batchId1)->f = this->at(batchId2)->f) 
				&& ( (this->at(batchId1)->freeCapacity + Global::problem->jobs.getJobByID(jobId1)->s) >= Global::problem->jobs.getJobByID(jobId2)->s ) 
				&& ( (this->at(batchId2)->freeCapacity + Global::problem->jobs.getJobByID(jobId2)->s) >= Global::problem->jobs.getJobByID(jobId1)->s );
		}
	}
	// else: index out of bounds
	return false;
}

/// sorting
void GroupingGenome::sortBy_r(bool asc) {
	if(asc) {
		sort(this->begin(), this->end(), GroupingGenome::compareBy_r);
	}
	else {
		sort(this->begin(), this->end(), GroupingGenome::compareBy_r_desc);
	}
	// TODO update completion times ?
}
void GroupingGenome::sortBy_pLot(bool asc) {
	if(asc) {
		sort(this->begin(), this->end(), GroupingGenome::compareBy_pLot);
	}
	else {
		sort(this->begin(), this->end(), GroupingGenome::compareBy_pLot_desc);
	}
	// TODO update completion times ?
}
void GroupingGenome::sortBy_pItem(bool asc){
	if(asc) {
		sort(this->begin(), this->end(), GroupingGenome::compareBy_pItem);
	}
	else {
		sort(this->begin(), this->end(), GroupingGenome::compareBy_pItem_desc);
	}
	// TODO update completion times ?
}
void GroupingGenome::sortBy_w(bool asc){
	if(asc) {
		sort(this->begin(), this->end(), GroupingGenome::compareBy_w);
	}
	else {
		sort(this->begin(), this->end(), GroupingGenome::compareBy_w_desc);
	}
	// TODO update completion times ?
}
void GroupingGenome::sortBy_wpLot(bool asc) {
	if(asc) {
		sort(this->begin(), this->end(), GroupingGenome::compareBy_wpLot);
	}
	else {
		sort(this->begin(), this->end(), GroupingGenome::compareBy_wpLot_desc);
	}
	// TODO update completion times ?
}
void GroupingGenome::sortBy_wpItem(bool asc) {
	if(asc) {
		sort(this->begin(), this->end(), GroupingGenome::compareBy_wpItem);
	}
	else {
		sort(this->begin(), this->end(), GroupingGenome::compareBy_wpItem_desc);
	}
	// TODO update completion times ?
}
void GroupingGenome::sortBy_BATCII(int time, double kappa, bool asc) {
	// get average p
	double avgP = 0;		// average processing time
	double numJ = 0;		// total number of jobs
	int bMax = this->size();
	for(unsigned i = 0; i < bMax; i++) {
		int jMax = this->at(i)->numJobs;
		numJ += (double) jMax;
		for(unsigned j = 0; j < jMax; j++) {
			avgP += (double) this->at(i)->getJob(j).p;
		}
	}
	avgP = avgP / numJ;

	if(asc) {		// sort ascending
		sort(this->begin(), this->end(), CompareBy_BATCII(avgP, time, kappa));
	}
	else {			// sort descending
		sort(this->begin(), this->end(), CompareBy_BATCII_desc(avgP, time, kappa));
	}
	// TODO this->updateCompletionTimes(); ??
}

void GroupingGenome::moveNonEmptyBatchesToFront() {
	sort(this->begin(), this->end(), GroupingGenome::compareEmpty);
}
bool GroupingGenome::reinsert(vector<int> &missingJobs) {
	set<int> assigned;
	vector<int> unins = missingJobs;
	assigned.clear();

	int iMax = unins.size();
	int bMax = this->size();
	for(unsigned i = 0; i < iMax; i++) {
	// insert first-fit into existing batches 
		for(unsigned b = 0; b < bMax; b++) {
			if (this->at(b)->addJob(Global::problem->jobs.getJobByID(unins[i]))) {
				if(this->at(b)->numJobs <= 1) {
					this->at(b)->setKey((double) rand() / ((double) RAND_MAX + 1.0) * Global::problem->m);
				}
				assigned.insert(unins[i]);
				break;
			}
		}
	}
	return assigned.size() == unins.size();
}

bool GroupingGenome::reinsertTWTEPC_2(vector<int> &missingJobs) {
	// TODO: check earliestStart, latestC and update the batches� values
	set<int> assigned;
	vector<int> unins = missingJobs;
	assigned.clear();

	int iMax = unins.size();
	int bMax = this->size();
	for(unsigned i = 0; i < iMax; i++) {
	// insert first-fit into existing batches 
		for(unsigned b = 0; b < bMax; b++) {
			// TODO: check if job.r <= batch.earliestD und job.p <= batch.latestC-batch.earliestStart
			if(Global::problem->jobs.getJobByID(unins[i])->r <= this->at(b)->earliestStart && Global::problem->jobs.getJobByID(unins[i])->p <= (this->at(b)->latestC - this->at(b)->earliestStart)) {
				if (this->at(b)->addJob(Global::problem->jobs.getJobByID(unins[i]))) {
					assigned.insert(unins[i]);
					break;
				}
			}
		}
	}
	return assigned.size() == unins.size();
}

bool GroupingGenome::reinsertTWTEPC_3(vector<int> &missingJobs) {
	set<int> assigned;
	vector<int> unins = missingJobs;
	assigned.clear();

	int iMax = unins.size();
	int bMax = this->size();
	for(unsigned i = 0; i < iMax; i++) {
	// insert first-fit into existing batches 
		for(unsigned b = 0; b < bMax; b++) {
			// check if job.r <= batch.earliestD und job.p <= batch.latestC-batch.earliestStart
			if(Global::problem->jobs.getJobByID(unins[i])->r <= this->at(b)->r) {
				// job큦 r does not affect batch큦 r
				if (this->at(b)->addJob(Global::problem->jobs.getJobByID(unins[i]))) {
					assigned.insert(unins[i]);
					break;
				}
			}
		}
	}
	return assigned.size() == unins.size();
}

bool GroupingGenome::reinsertTWCEPC_2(vector<int> &missingJobs) {
	// TODO: check earliestStart, latestC and update the batches� values
	set<int> assigned;
	vector<int> unins = missingJobs;
	assigned.clear();

	int iMax = unins.size();
	int bMax = this->size();
	for(unsigned i = 0; i < iMax; i++) {
	// insert first-fit into existing batches 
		for(unsigned b = 0; b < bMax; b++) {
			// TODO: check if job.r <= batch.earliestD und job.p <= batch.latestC-batch.earliestStart
			if(Global::problem->jobs.getJobByID(unins[i])->r <= this->at(b)->earliestStart && Global::problem->jobs.getJobByID(unins[i])->p <= (this->at(b)->latestC - this->at(b)->earliestStart)) {
				if (this->at(b)->addJob(Global::problem->jobs.getJobByID(unins[i]))) {
					assigned.insert(unins[i]);
					break;
				}
			}
		}
	}
	return assigned.size() == unins.size();
}

bool GroupingGenome::reinsertReady(vector<int> &missingJobs){			// reinsert considering ready times
	set<int> assigned;
	vector<int> unins = missingJobs;
	
	assigned.clear();

	int jMax = unins.size();
	int bMax = this->size();

	// sort job ids by decreasing weight 
	sort(missingJobs.begin(), missingJobs.end(), compareJobWeight);
	
	for(unsigned i = 0; i < bMax; i++) {
		for(unsigned j = 0; j < jMax; j++) {
			if(assigned.count(unins[j]) == 0 && this->at(i)->r >= Global::problem->jobs.getJobByID(unins[j])->r || this->at(i)->numJobs == 0) {
				// Try to assign a job if 1) it is not yet assigned AND 2) its r is not larger than the batch큦 r OR 3) the batch is empty
				if(this->at(i)->addJob(Global::problem->jobs.getJobByID(j))) {
					// The job is only assigned if the batch큦 capacity restriction is met and the families match
					assigned.insert(unins[j]);
				}
			}
		}
	}
	return assigned.size() == unins.size();
}			
bool GroupingGenome::reinsertDue(vector<int> &missingJobs){				// reinsert considering due dates
	set<int> assigned;
	vector<int> unins = missingJobs;
	
	assigned.clear();

	int iMax = unins.size();
	int bMax = this->size();
	int dRestriction;

	// sort job ids by decreasing weight 
	sort(missingJobs.begin(), missingJobs.end(), compareJobWeight);
	
	for(unsigned j = 0; j < iMax; j++) {
		 for(unsigned i = 0; i < bMax; i++) {
			 if(this->at(i)->getC() <= 0) {
				dRestriction = this->at(i)->latestD;
			 }
			 else {
				dRestriction = this->at(i)->getC();
			 }
			 // at the time of reinsertion the batch is not yet scheduled, therefore put job in if its d is not larger than the latest d of jobs already assigned
			 if(assigned.count(unins[j]) == 0 && this->at(i)->latestD >= Global::problem->jobs.getJobByID(unins[j])->d || this->at(i)->numJobs == 0) {
				// Try to assign a job if 1) it is not yet assigned AND 2) its r is not larger than the batch큦 r OR 3) the batch is empty
				if(this->at(i)->addJob(Global::problem->jobs.getJobByID(j))) {
					// The job is only assigned if the batch큦 capacity restriction is met and the families match
					assigned.insert(unins[j]);
				}
			}
		}
	}
	return assigned.size() == unins.size();
}			
bool GroupingGenome::reinsertReadyDue(vector<int> &missingJobs){		// reinsert considering ready times and due dates
	return false;
}		
bool GroupingGenome::reinsertReadyDueWeight(vector<int> &missingJobs){	// reinsert considering ready times, due dates and weight
	return false;
}	
bool GroupingGenome::reinsertMinDeltaTWT(vector<int> &missingJobs){		// reinsert increasing TWT as little as possible
	return false;
}	
bool GroupingGenome::reinsertTWT(vector<int> &missingJobs){				// reinsert so that TWT of accepting batch does not increase
	std::cout << "GroupingGenome::reinsterTWT(vector<int>) not implemented." << endl;
	return false;
}			
bool GroupingGenome::reinsertTWC(vector<int> &missingJobs){				// reinsert so that TWT of accepting batch does not increase
	std::cout << "GroupingGenome::reinsterTWC(vector<int>) not implemented." << endl;
	return false;
}			
bool GroupingGenome::reinsertBATC(vector<int> &missingJobs){			// reinsert so the BATC values of utilized batches are not increased
	return false;
}			


// compare Batches
/// ascending order
bool GroupingGenome::compareBy_r(const Batch* a, const Batch* b) {
	if(a->r == b->r) {
		return a->id < b->id;
	}
	else {
		return a->r< b->r;
	}
}

bool GroupingGenome::compareBy_pLot(const Batch* a, const Batch* b) {
	if(a->pLot == b->pLot) {
		return a->id < b->id;
	}
	else {
		return a->pLot < b->pLot;
	}
}

bool GroupingGenome::compareBy_pItem(const Batch* a, const Batch* b) {
	if(a->pItem == b->pItem) {
		return a->id < b->id;
	}
	else {
		return a->pItem < b->pItem;
	}
}

bool GroupingGenome::compareBy_w(const Batch* a, const Batch* b) {
	if(a->w == b->w) {
		return a->id < b->id;
	}
	else {
		return a->w < b->w;
	}
}

bool GroupingGenome::compareBy_wpLot(const Batch* a, const Batch* b) {
	// consider empty batches
	if(a->pLot == 0) {
		if(b->pLot == 0) {
			return a->id < b->id;
		} else {
			return false;	// TODO check
		}
	}
	if(b->pLot == 0) {
		return true;
	}
	
	if(((double)a->w / (double)a->pLot) == ((double)b->w / (double)b->pLot)) {
		return a->id < b->id;
	}
	else {
		return ((double)a->w / (double)a->pLot) < ((double)b->w / (double)b->pLot);
	}
}

bool GroupingGenome::compareBy_wpItem(const Batch* a, const Batch* b) {
	// consider empty batches
	if(a->pLot == 0) {
		if(b->pLot == 0) {
			return a->id < b->id;
		} else {
			return false;	// TODO check
		}
	}
	if(b->pLot == 0) {
		return true;
	}
	
	if(((double)a->w / (double)a->pItem) == ((double)b->w / (double)b->pItem)) {
		return a->id < b->id;
	}
	else {
		return ((double)a->w / (double)a->pItem) < ((double)b->w / (double)b->pItem);
	}
}

bool GroupingGenome::compareEmpty(const Batch* a, const Batch* b) {
	if( (a->numJobs > 0 && b->numJobs > 0) || (a->numJobs == 0 && b->numJobs == 0) ) {
		return a->id < b->id;
	} 
	else if( b->numJobs > 0) {
		return false;
	}
	return true;
	// return true if a is to be placed before b
}
/// descending order
bool GroupingGenome::compareBy_r_desc(const Batch* a, const Batch* b) {
	if(a->r == b->r) {
		return a->id < b->id;
	}
	else {
		return a->r > b->r;
	}
}

bool GroupingGenome::compareBy_pLot_desc(const Batch* a, const Batch* b) {
	if(a->pLot == b->pLot) {
		return a->id < b->id;
	}
	else {
		return a->pLot > b->pLot;
	}
}

bool GroupingGenome::compareBy_pItem_desc(const Batch* a, const Batch* b) {
	if(a->pItem == b->pItem) {
		return a->id < b->id;
	}
	else {
		return a->pItem > b->pItem;
	}
}

bool GroupingGenome::compareBy_w_desc(const Batch* a, const Batch* b) {
	if(a->w == b->w) {
		return a->id < b->id;
	}
	else {
		return a->w > b->w;
	}
}

bool GroupingGenome::compareBy_wpLot_desc(const Batch* a, const Batch* b) {
	// consider empty batches
	if(a->pLot == 0) {
		if(b->pLot == 0) {
			return a->id < b->id;
		} else {
			return false;	// TODO check
		}
	}
	if(b->pLot == 0) {
		return true;
	}

	if(((double)a->w / (double)a->pLot) == ((double)b->w / (double)b->pLot)) {
		return a->id < b->id;
	}
	else {
		return ((double)a->w / (double)a->pLot) > ((double)b->w / (double)b->pLot);
	}
}

bool GroupingGenome::compareBy_wpItem_desc(const Batch* a, const Batch* b) {
	// consider empty batches
	if(a->pLot == 0) {
		if(b->pLot == 0) {
			return a->id < b->id;
		} else {
			return false;	// TODO check
		}
	}
	if(b->pLot == 0) {
		return true;
	}
	
	
	if(((double)a->w / (double)a->pItem) == ((double)b->w / (double)b->pItem)) {
		return a->id < b->id;
	}
	else {
		return ((double)a->w / (double)a->pItem) > ((double)b->w / (double)b->pItem);
	}
}



bool compareJobWeight(const int JobIdx1, const int JobIdx2) {
	return Global::problem->jobs.getJobByID(JobIdx1)->w > Global::problem->jobs.getJobByID(JobIdx2)->w;
}
bool compareJobSize(const int JobIdx1, const int JobIdx2) {
	return Global::problem->jobs.getJobByID(JobIdx1)->s > Global::problem->jobs.getJobByID(JobIdx2)->s;
}