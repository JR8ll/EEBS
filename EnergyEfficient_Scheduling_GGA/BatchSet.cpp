#include "BatchSet.h"

using namespace std;
// constructor definitions
BatchSet::BatchSet() {
	this->batches = vector<Batch>();
}

// operator overloading
Batch& BatchSet::operator[](int index) {
	return this->batches[index];
}


// method definitions
int BatchSet::size() {
	return this->batches.size();
}	
void BatchSet::clear() {
	this->batches.clear();
}

bool BatchSet::empty() {
	if(this->batches.empty()){
		return true;
	} else {
		return false;
	}
}

void BatchSet::copy(const BatchSet* other) {
	if (this == other) return;
	BatchSet &oth = *((BatchSet*) (other));
	this->clear();
	int bMax = oth.size();
	this->resize( bMax );
	for(unsigned i = 0; i < bMax; i++) {
		this->batches[i].copy(&oth.getBatch(i));
	}
}

Batch& BatchSet::getBatch(int i) {
	return this->batches[i];
}
bool BatchSet::addBatch(Batch& in_Batch){
	this->batches.push_back(in_Batch);
	return true;
	// TODO exception handling
}

bool BatchSet::addBatch(Batch& in_Batch, int start) {
	// get position
	int bMax = this->size();
	for(vector<Batch>::iterator it = this->batches.begin(); it != this->batches.end(); it++) {
		if( (start + in_Batch.pLot) <= it->getStart()) {
			in_Batch.setStart(start);
			this->batches.insert(it, in_Batch);
			return true;
		}
	}
	if( start > this->batches[bMax-1].getC() ) {
		in_Batch.setStart(start);
		this->batches.push_back(in_Batch);
		return true;
	}
	return false;
}

bool BatchSet::erase(int index) {
	if(index >= 0 && index < this->batches.size()) {
		this->batches.erase(this->batches.begin() + index);
		return true;
	}
	return false;
}
void BatchSet::print() {
	int bMax = this->size();
	if(bMax == 0) {
		cout << "This set of batches is empty." << endl;
	}
	else {
		for(unsigned i = 0; i < bMax; i++) {
			cout << "Batch " << this->batches[i].id << endl;
			cout <<"\t(K=" << this->batches[i].capacity << ",freeCap=" << this->batches[i].freeCapacity; 
			cout << ",f=" << this->batches[i].f << ",r=" << this->batches[i].r;
			cout << ",pLot=" << this->batches[i].pLot << ",pItem=" << this->batches[i].pItem;
			cout << ",earliestD=" << this->batches[i].earliestD;
			cout << ",r=" << this->batches[i].r << ",start=" << this->batches[i].getStart() << ",c=" << this->batches[i].getC() << ")" << endl;
			cout << "\tcontains " << this->batches[i].numJobs << " jobs ["; 
			int jMax = this->batches[i].numJobs;
			for(unsigned j = 0; j < jMax; j++) {
				cout << this->batches[i].getJob(j).id;
				if(j < jMax - 1) cout << ", ";
			}
			cout << "]" << endl;
		}
	}
}



void BatchSet::updateCompletionTimes(){
	if(!this->empty()){
		int bMax = this->size();
		this->batches[0].setStart(this->batches[0].r);		// 1st batch starts at r
		for(unsigned i = 1; i < bMax; i++) {
			// other batches start when they are available and their predecessors are finished
			this->batches[i].setStart(max<int>(this->batches[i].r, this->batches[i-1].getC()));
		}
	}
}
void BatchSet::resize(int newSize) {
	this->batches.resize(newSize);
}
// sort Batches

void BatchSet::sortBy_Start() {
	sort(this->batches.begin(), this->batches.end(), BatchSet::compareBatchesBy_Start);
}
void BatchSet::sortBy_r(bool asc) {
	if(asc) {
		sort(this->batches.begin(), this->batches.end(), BatchSet::compareBatchesBy_r);
	}
	else {
		sort(this->batches.begin(), this->batches.end(), BatchSet::compareBatchesBy_r_desc);
	}
	this->updateCompletionTimes();		// TAKE CARE
}

void BatchSet::sortBy_pLot(bool asc) {
	if(asc) {
		sort(this->batches.begin(), this->batches.end(), BatchSet::compareBatchesBy_pLot);
	}
	else {
		sort(this->batches.begin(), this->batches.end(), BatchSet::compareBatchesBy_pLot_desc);
	}
	this->updateCompletionTimes();
}

void BatchSet::sortBy_pItem(bool asc) {
	if(asc) {
		sort(this->batches.begin(), this->batches.end(), BatchSet::compareBatchesBy_pItem);
	}
	else {
		sort(this->batches.begin(), this->batches.end(), BatchSet::compareBatchesBy_pItem_desc);
	}
	this->updateCompletionTimes();
}

void BatchSet::sortBy_w(bool asc) {
	if(asc) {
		sort(this->batches.begin(), this->batches.end(), BatchSet::compareBatchesBy_w);
	}
	else {
		sort(this->batches.begin(), this->batches.end(), BatchSet::compareBatchesBy_w_desc);
	}
	this->updateCompletionTimes();
}

void BatchSet::sortBy_wpLot(bool asc) {
	if(asc) {
		sort(this->batches.begin(), this->batches.end(), BatchSet::compareBatchesBy_wpLot);
	}
	else {
		sort(this->batches.begin(), this->batches.end(), BatchSet::compareBatchesBy_wpLot_desc);
	}
	this->updateCompletionTimes();
}

void BatchSet::sortBy_wpItem(bool asc) {
	if(asc) {
		sort(this->batches.begin(), this->batches.end(), BatchSet::compareBatchesBy_wpItem);
	}
	else {
		sort(this->batches.begin(), this->batches.end(), BatchSet::compareBatchesBy_wpItem_desc);
	}
	this->updateCompletionTimes();
}
void BatchSet::sortBy_BATCII(int time, double kappa, bool asc){
	// get average p
	double avgP = 0;		// average processing time
	double numJ = 0;		// total number of jobs
	int bMax = this->batches.size();
	for(unsigned i = 0; i < bMax; i++) {
		int jMax = this->batches[i].numJobs;
		numJ += (double) jMax;
		for(unsigned j = 0; j < jMax; j++) {
			avgP += (double) this->batches[i].getJob(j).p;
		}
	}
	avgP = avgP / numJ;

	if(asc) {		// sort ascending
		sort(this->batches.begin(), this->batches.end(), CompareBatchesBy_BATCII(avgP, time, kappa));
	}
	else {			// sort descending
		sort(this->batches.begin(), this->batches.end(), CompareBatchesBy_BATCII_desc(avgP, time, kappa));
	}
	this->updateCompletionTimes();
}


// compare Batches
/// ascending order
bool BatchSet::compareBatchesBy_Start(const Batch& a, const Batch& b) {
	if(a.getStart() == b.getStart()) {
		return a.id < b.id;
	}
	else {
		return a.getStart() < b.getStart();
	}
}
bool BatchSet::compareBatchesBy_r(const Batch& a, const Batch& b) {
	if(a.r == b.r) {
		return a.id < b.id;
	}
	else {
		return a.r< b.r;
	}
}

bool BatchSet::compareBatchesBy_pLot(const Batch& a, const Batch& b) {
	if(a.pLot == b.pLot) {
		return a.id < b.id;
	}
	else {
		return a.pLot < b.pLot;
	}
}

bool BatchSet::compareBatchesBy_pItem(const Batch& a, const Batch& b) {
	if(a.pItem == b.pItem) {
		return a.id < b.id;
	}
	else {
		return a.pItem < b.pItem;
	}
}

bool BatchSet::compareBatchesBy_w(const Batch& a, const Batch& b) {
	if(a.w == b.w) {
		return a.id < b.id;
	}
	else {
		return a.w < b.w;
	}
}

bool BatchSet::compareBatchesBy_wpLot(const Batch& a, const Batch& b) {
	if(((double)a.w / (double)a.pLot) == ((double)b.w / (double)b.pLot)) {
		return a.id < b.id;
	}
	else {
		return ((double)a.w / (double)a.pLot) < ((double)b.w / (double)b.pLot);
	}
}

bool BatchSet::compareBatchesBy_wpItem(const Batch& a, const Batch& b) {
	if(((double)a.w / (double)a.pItem) == ((double)b.w / (double)b.pItem)) {
		return a.id < b.id;
	}
	else {
		return ((double)a.w / (double)a.pItem) < ((double)b.w / (double)b.pItem);
	}
}

/// descending order
bool BatchSet::compareBatchesBy_r_desc(const Batch& a, const Batch& b) {
	if(a.r == b.r) {
		return a.id < b.id;
	}
	else {
		return a.r > b.r;
	}
}

bool BatchSet::compareBatchesBy_pLot_desc(const Batch& a, const Batch& b) {
	if(a.pLot == b.pLot) {
		return a.id < b.id;
	}
	else {
		return a.pLot > b.pLot;
	}
}

bool BatchSet::compareBatchesBy_pItem_desc(const Batch& a, const Batch& b) {
	if(a.pItem == b.pItem) {
		return a.id < b.id;
	}
	else {
		return a.pItem > b.pItem;
	}
}

bool BatchSet::compareBatchesBy_w_desc(const Batch& a, const Batch& b) {
	if(a.w == b.w) {
		return a.id < b.id;
	}
	else {
		return a.w > b.w;
	}
}

bool BatchSet::compareBatchesBy_wpLot_desc(const Batch& a, const Batch& b) {
	if(((double)a.w / (double)a.pLot) == ((double)b.w / (double)b.pLot)) {
		return a.id < b.id;
	}
	else {
		return ((double)a.w / (double)a.pLot) > ((double)b.w / (double)b.pLot);
	}
}

bool BatchSet::compareBatchesBy_wpItem_desc(const Batch& a, const Batch& b) {
	if(((double)a.w / (double)a.pItem) == ((double)b.w / (double)b.pItem)) {
		return a.id < b.id;
	}
	else {
		return ((double)a.w / (double)a.pItem) > ((double)b.w / (double)b.pItem);
	}
}


// Job2Batch assignment
bool BatchSet::assign_FFDn(vector<Job>& jobs){
	unsigned jMax = jobs.size();
	unsigned bMax = this->batches.size();
	for(unsigned j = 0; j < jMax; j++) {
		unsigned b = 0;
		while(!this->batches[b].addJob(&jobs[j])) {
				b++;
				if(b >= bMax) {
					return false;				// at least one job could not be assigned to any batch
			}
		}	
	}
	return true;
}
bool BatchSet::assign_FFDn(JobSet& jobs){
	unsigned jMax = jobs.size();
	unsigned bMax = this->batches.size();
	for(unsigned j = 0; j < jMax; j++) {
		unsigned b = 0;
		while(!this->batches[b].addJob(&jobs[j])) {
				b++;
				if(b >= bMax) {
					return false;				// at least one job could not be assigned to any batch
			}
		}	
	}
	return true;
}

void BatchSet::form_FF(JobSet& jobs, int batchCap) {
	// TODO: finish + debug implementation 
	unsigned jMax = jobs.size();
	JobSet tempJobs;
	tempJobs.copy(&jobs);
	int jobsAssigned = 0;
	int currentBatch = 0;
	// add new batch
	while(jobsAssigned < jMax) {
		// add new batch
		this->addBatch(Batch(batchCap));
		currentBatch++;
		// assign as many jobs as possible
		
		for(int i = 0; i < tempJobs.size(); i++) {
			if(this->batches[currentBatch-1].addJob(&tempJobs[i])) {
				jobsAssigned++;
				tempJobs.erase(tempJobs[i].id);
				i--;
			}
		}
		
	}
}

bool BatchSet::assign_FFDnb(vector<Job>& jobs) {
	// TODO implement
	return false;
}

bool BatchSet::assign_FFD1(vector<Job>& jobs) {
	unsigned jMax = jobs.size();
	unsigned bMax = this->batches.size();
	list<Job*> jobPtrList;
	for(unsigned j = 0; j < jMax; j++) {
		vector<Job>::pointer ptr = &(jobs[j]);
		jobPtrList.push_back(ptr);
	}
	for(unsigned b = 0; b < bMax; b++) {
		for(list<Job*>::iterator it = jobPtrList.begin(); it != jobPtrList.end(); ) {
			if(this->batches[b].addJob(&(*(*it)))) {	// TODO: wenn das mal gut geht...
				it = jobPtrList.erase(it);
			}
			else {
				++it;
			}
		}
	}
	if(!jobPtrList.empty()){
		return false;
	}
	return true;
}
bool BatchSet::assign_FFD1(JobSet& jobs) {
	unsigned jMax = jobs.size();
	unsigned bMax = this->batches.size();
	list<Job*> jobPtrList;
	for(unsigned j = 0; j < jMax; j++) {
		vector<Job>::pointer ptr = &(jobs[j]);
		jobPtrList.push_back(ptr);
	}
	for(unsigned b = 0; b < bMax; b++) {
		for(list<Job*>::iterator it = jobPtrList.begin(); it != jobPtrList.end(); ) {
			if(this->batches[b].addJob(*it)) {
				it = jobPtrList.erase(it);
			}
			else {
				++it;
			}
		}
	}
	if(!jobPtrList.empty()){
		return false;
	}
	return true;
}
bool BatchSet::assign_FFD1b(vector<Job>& jobs, int globalF){
	vector<int> numJobs;		// number of jobs per family (=index)
	vector<int> numBatches;		// number of batches per family (=index)
	int jMax = jobs.size();
	int bMax = this->batches.size();
	// count number of Jobs/Batches
	for(unsigned f = 1; f <= globalF; f++) {
		int nJ = 0;
		int nB = 0;
		for(unsigned j = 0; j < jMax; j++){
			if(jobs[j].f == f){
				nJ++;
			}
		}
		for(unsigned b = 0; b < bMax; b++){
			if(this->batches[b].f == f){
				nB++;
			}
		}
		numJobs.push_back(nJ);
		numBatches.push_back(nB);
	}

	//build list of Job-Pointers
	list<Job*> jobPtrList;
	for(unsigned j = 0; j < jMax; j++)
	{
		vector<Job>::pointer ptr = &(jobs[j]);
		jobPtrList.push_back(ptr);
	}	

	// actual Job2Batch assignment
	for(unsigned f = 1; f <= globalF; f++) {
		int batchesNotFilled = numBatches[f-1];
		int jobsToBeAssigned = numJobs[f-1];
		unsigned b = 0;		// for iterating through the batches
		while(batchesNotFilled < jobsToBeAssigned && b < bMax) {
			// FFD1
			if(this->batches[b].f == f){
				//iterate through jobPtrList
				for(list<Job*>::iterator it = jobPtrList.begin(); it != jobPtrList.end(); ){
					if(this->batches[b].addJob(*it)) {
						it = jobPtrList.erase(it);
						jobsToBeAssigned--;
						if(this->batches[b].freeCapacity == 0){
							break;
						}
					}
					else {
						it++;
					}
					if(batchesNotFilled > jobsToBeAssigned){
						break;
					}
				}
				batchesNotFilled--;
				b++;
			}
			else {
				b++;
			}
		}
		//assign one job to each remaining batch
		while(b < bMax && jobsToBeAssigned > 0){
			for(list<Job*>::iterator it = jobPtrList.begin(); it != jobPtrList.end(); ){
				if(this->batches[b].f == f){
					if(batches[b].addJob(*it)) {
						it = jobPtrList.erase(it);
						jobsToBeAssigned--;
						batchesNotFilled--;
						b++;
						break;
					}
					else{
						it++;
					}
				}
				else {
					b++;
				}
			}
		}
	}
	if(!jobPtrList.empty()){
		return false;
	}
	return true;
}

double BatchSet::getMSP() {
	// TODO: consider gaps in the schedule => MSP is the maximum completion, done 30/07/2018 tbc
	int bMax = this->size();
	if(bMax > 0) {
		double cMax = 0.0;
		for(unsigned i = 0; i < bMax; i++) {
			if(this->batches[i].getC() > cMax) {
				cMax = this->batches[i].getC();
			}
		}
		return cMax;
	}
	else {
		return 0.0;
	}
}