#include "Functions.h"

using namespace std;

// intialize global attributes
GAParameters& setParameters(int popSize, int numGens, int timeLimit, double probMut, double popGeomFactor, bool scalarize) {
	Global::params.popSize = popSize;
	Global::params.numGens = numGens;
	Global::params.timeLimit = timeLimit;
	Global::params.probMut = probMut;
	Global::params.popGeomFactor = popGeomFactor;
	Global::params.scalarize = scalarize;
	return Global::params;
}


// sort Batches

void sortBatchesBy_r(vector<Batch>& batches, bool asc) {
	if(asc) {
		sort(batches.begin(), batches.end(), compareBatchesBy_r);
	}
	else {
		sort(batches.begin(), batches.end(), compareBatchesBy_r_desc);
	}
}

void sortBatchesBy_pLot(vector<Batch>& batches, bool asc) {
	if(asc) {
		sort(batches.begin(), batches.end(), compareBatchesBy_pLot);
	}
	else {
		sort(batches.begin(), batches.end(), compareBatchesBy_pLot_desc);
	}
}

void sortBatchesBy_pItem(vector<Batch>& batches, bool asc) {
	if(asc) {
		sort(batches.begin(), batches.end(), compareBatchesBy_pItem);
	}
	else {
		sort(batches.begin(), batches.end(), compareBatchesBy_pItem_desc);
	}
}

void sortBatchesBy_w(vector<Batch>& batches, bool asc) {
	if(asc) {
		sort(batches.begin(), batches.end(), compareBatchesBy_w);
	}
	else {
		sort(batches.begin(), batches.end(), compareBatchesBy_w_desc);
	}
}

void sortBatchesBy_wpLot(vector<Batch>& batches, bool asc) {
	if(asc) {
		sort(batches.begin(), batches.end(), compareBatchesBy_wpLot);
	}
	else {
		sort(batches.begin(), batches.end(), compareBatchesBy_wpLot_desc);
	}
}

void sortBatchesBy_wpItem(vector<Batch>& batches, bool asc) {
	if(asc) {
		sort(batches.begin(), batches.end(), compareBatchesBy_wpItem);
	}
	else {
		sort(batches.begin(), batches.end(), compareBatchesBy_wpItem_desc);
	}
}
void sortBatchesBy_BATCII(vector<Batch>& batches, int time, double kappa, bool asc){
	// get average p
	double avgP = 0;		// average processing time
	double numJ = 0;		// total number of jobs
	int bMax = batches.size();
	for(unsigned i = 0; i < bMax; i++) {
		int jMax = batches[i].numJobs;
		numJ += (double) jMax;
		for(unsigned j = 0; j < jMax; j++) {
			avgP += (double) batches[i].getJob(j).p;
		}
	}
	avgP = avgP / numJ;

	if(asc) {		// sort ascending
		sort(batches.begin(), batches.end(), CompareBatchesBy_BATCII(avgP, time, kappa));
	}
	else {			// sort descending
		sort(batches.begin(), batches.end(), CompareBatchesBy_BATCII_desc(avgP, time, kappa));
	}
}

// sort Jobs
void sortJobsBy_p(vector<Job>& jobs, bool asc){
	if(asc) {
	sort(jobs.begin(), jobs.end(), compareJobsBy_p);
	}
	else {
		sort(jobs.begin(), jobs.end(), compareJobsBy_p_desc);
	}
}
void sortJobsBy_d(vector<Job>& jobs, bool asc){
	if(asc) {
	sort(jobs.begin(), jobs.end(), compareJobsBy_d);
	}
	else {
		sort(jobs.begin(), jobs.end(), compareJobsBy_d_desc);
	}
}
void sortJobsBy_r(vector<Job>& jobs, bool asc){
	if(asc) {
	sort(jobs.begin(), jobs.end(), compareJobsBy_r);
	}
	else {
		sort(jobs.begin(), jobs.end(), compareJobsBy_r_desc);
	}
}
void sortJobsBy_s(vector<Job>& jobs, bool asc){
	if(asc) {
	sort(jobs.begin(), jobs.end(), compareJobsBy_s);
	}
	else {
		sort(jobs.begin(), jobs.end(), compareJobsBy_s_desc);
	}
}
void sortJobsBy_w(vector<Job>& jobs, bool asc){
	if(asc) {
	sort(jobs.begin(), jobs.end(), compareJobsBy_w);
	}
	else {
		sort(jobs.begin(), jobs.end(), compareJobsBy_w_desc);
	}
}
void sortJobsBy_f(vector<Job>& jobs, bool asc){
	if(asc) {
	sort(jobs.begin(), jobs.end(), compareJobsBy_f);
	}
	else {
		sort(jobs.begin(), jobs.end(), compareJobsBy_f_desc);
	}
}
void sortJobsBy_wp(vector<Job>& jobs, bool asc){
	if(asc) {	// sort ascending
	sort(jobs.begin(), jobs.end(), compareJobsBy_wp);
	}
	else {		// sort descending
		sort(jobs.begin(), jobs.end(), compareJobsBy_wp_desc);
	}
}

void sortJobsBy_ATC(vector<Job *>& jobs, int time, double kappa, bool asc) {
	// usually ATC implies a non-increasing order => call w/ asc = false
	// get average p
	double avgP = 0;
	int jMax = jobs.size();
	for(unsigned i = 0; i < jMax; i++){
		avgP += (double)jobs[i]->p;
	}
	avgP = avgP / (double)jMax;
	if(asc) {	// sort ascending
		sort(jobs.begin(), jobs.end(), CompareJobsBy_ATC(avgP, time, kappa));
	}
	else {		// sort descending
		sort(jobs.begin(), jobs.end(), CompareJobsBy_ATC_desc(avgP, time, kappa));
	}
}

// TODO: umstellen auf vector<Job *>
// Job2Batch assignment
bool assign_FFDn(vector<Job>& jobs, vector<Batch>& batches){
	unsigned jMax = jobs.size();
	unsigned bMax = batches.size();
	for(unsigned j = 0; j < jMax; j++) {
		unsigned b = 0;
		while(!batches[b].addJob(&jobs[j])) {
				b++;
				if(b >= bMax) {
					return false;				// at least one job could not be assigned to any batch
			}
		}
		return true;
	}
}
bool assign_FFDnb(vector<Job>& jobs, vector<Batch>& batches);
bool assign_FFD1(vector<Job>& jobs, vector<Batch>& batches) {
	unsigned jMax = jobs.size();
	unsigned bMax = batches.size();
	list<Job*> jobPtrList;
	for(unsigned j = 0; j < jMax; j++) {
		vector<Job>::pointer ptr = &(jobs[j]);
		jobPtrList.push_back(ptr);
	}
	for(unsigned b = 0; b < bMax; b++) {
		for(list<Job*>::iterator it = jobPtrList.begin(); it != jobPtrList.end(); ) {
			if(batches[b].addJob(*it)) {
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
bool assign_FFD1b(vector<Job>& jobs, vector<Batch>& batches, Problem& problem){
	vector<int> numJobs;		// number of jobs per family (=index)
	vector<int> numBatches;		// number of batches per family (=index)
	int jMax = jobs.size();
	int bMax = batches.size();
	// count number of Jobs/Batches
	for(unsigned f = 1; f <= problem.l; f++) {
		int nJ = 0;
		int nB = 0;
		for(unsigned j = 0; j < jMax; j++){
			if(jobs[j].f == f){
				nJ++;
			}
		}
		for(unsigned b = 0; b < bMax; b++){
			if(batches[b].f == f){
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
	for(unsigned f = 1; f <= problem.l; f++) {
		int batchesNotFilled = numBatches[f-1];
		int jobsToBeAssigned = numJobs[f-1];
		unsigned b = 0;		// for iterating through the batches
		while(batchesNotFilled < jobsToBeAssigned && b < bMax) {
			// FFD1
			if(batches[b].f == f){
				//iterate through jobPtrList
				for(list<Job*>::iterator it = jobPtrList.begin(); it != jobPtrList.end(); ){
					if(batches[b].addJob(*it)) {
						it = jobPtrList.erase(it);
						jobsToBeAssigned--;
						if(batches[b].freeCapacity == 0){
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
				if(batches[b].f == f){
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
bool assign_FFDAJS(vector<Job>& jobs, vector<Batch>& batches);
bool assign_FFBF(vector<Job>& jobs, vector<Batch>& batches);
bool assign_PokerClockwise(vector<Job>& jobs, vector<Batch>& batches);
bool assign_PokerOscillating(vector<Job>& jobs, vector<Batch>& batches);

// form Batches
/// single family
vector<Batch> formBatches_FFDn(vector<Job>& jobs, Problem& problem){
	vector<Batch> batches;
	Batch thisBatch = Batch(problem.k);
	int jMax = jobs.size();
	for(unsigned i = 0; i < jMax; i++) {
		if(!thisBatch.addJob(&jobs[i])){
			batches.push_back(thisBatch);
			thisBatch = Batch(problem.k);
			thisBatch.addJob(&jobs[i]);
		}
	}
	batches.push_back(thisBatch);
	return batches;
}


// compare Batches
/// ascending order
bool compareBatchesBy_r(const Batch& a, const Batch& b) {
	if(a.r == b.r) {
		return a.id < b.id;
	}
	else {
		return a.r< b.r;
	}
}

bool compareBatchesBy_pLot(const Batch& a, const Batch& b) {
	if(a.pLot == b.pLot) {
		return a.id < b.id;
	}
	else {
		return a.pLot < b.pLot;
	}
}

bool compareBatchesBy_pItem(const Batch& a, const Batch& b) {
	if(a.pItem == b.pItem) {
		return a.id < b.id;
	}
	else {
		return a.pItem < b.pItem;
	}
}

bool compareBatchesBy_w(const Batch& a, const Batch& b) {
	if(a.w == b.w) {
		return a.id < b.id;
	}
	else {
		return a.w < b.w;
	}
}

bool compareBatchesBy_wpLot(const Batch& a, const Batch& b) {
	if((a.w / a.pLot) == (b.w / b.pLot)) {
		return a.id < b.id;
	}
	else {
		return (a.w / a.pLot) < (b.w / b.pLot);
	}
}

bool compareBatchesBy_wpItem(const Batch& a, const Batch& b) {
	if((a.w / a.pItem) == (b.w / b.pItem)) {
		return a.id < b.id;
	}
	else {
		return (a.w / a.pItem) < (b.w / b.pItem);
	}
}

/// descending order
bool compareBatchesBy_r_desc(const Batch& a, const Batch& b) {
	if(a.r == b.r) {
		return a.id < b.id;
	}
	else {
		return a.r > b.r;
	}
}

bool compareBatchesBy_pLot_desc(const Batch& a, const Batch& b) {
	if(a.pLot == b.pLot) {
		return a.id < b.id;
	}
	else {
		return a.pLot > b.pLot;
	}
}

bool compareBatchesBy_pItem_desc(const Batch& a, const Batch& b) {
	if(a.pItem == b.pItem) {
		return a.id < b.id;
	}
	else {
		return a.pItem > b.pItem;
	}
}

bool compareBatchesBy_w_desc(const Batch& a, const Batch& b) {
	if(a.w == b.w) {
		return a.id < b.id;
	}
	else {
		return a.w > b.w;
	}
}

bool compareBatchesBy_wpLot_desc(const Batch& a, const Batch& b) {
	if((a.w / a.pLot) == (b.w / b.pLot)) {
		return a.id < b.id;
	}
	else {
		return (a.w / a.pLot) > (b.w / b.pLot);
	}
}

bool compareBatchesBy_wpItem_desc(const Batch& a, const Batch& b) {
	if((a.w / a.pItem) == (b.w / b.pItem)) {
		return a.id < b.id;
	}
	else {
		return (a.w / a.pItem) > (b.w / b.pItem);
	}
}


// compare Jobs
/// ascending order
bool compareJobsBy_p(const Job& a, const Job& b) {
	if(a.p == b.p) {
		return a.id < b.id;
	}
	else {
		return a.p < b.p;
	}
}
bool compareJobsBy_d(const Job& a, const Job& b) {
	if(a.d == b.d) {
		return a.id < b.id;
	}
	else {
		return a.d < b.d;
	}
}
bool compareJobsBy_r(const Job& a, const Job& b) {
	if(a.r == b.r) {
		return a.id < b.id;
	}
	else {
		return a.r < b.r;
	}
}

bool compareJobsBy_s(const Job& a, const Job& b) {
	if(a.s == b.s) {
		return a.id < b.id;
	}
	else {
		return a.s < b.s;
	}
}
bool compareJobsBy_w(const Job& a, const Job& b) {
	if(a.w == b.w) {
		return a.id < b.id;
	}
	else {
		return a.w < b.w;
	}
}
bool compareJobsBy_f(const Job& a, const Job& b) {
	if(a.f == b.f) {
		return a.id < b.id;
	}
	else {
		return a.f < b.f;
	}
}
bool compareJobsBy_wp(const Job& a, const Job& b) {
	double wpA = (double) a.w / (double) a.p;
	double wpB = (double) b.w / (double) b.p;
	if( wpA == wpB) {
		return a.id <= b.id;
	}
	else {
		return wpA < wpB;
	}
}
/// descending order
bool compareJobsBy_p_desc(const Job& a, const Job& b) {
	if(a.p == b.p) {
		return a.id < b.id;
	}
	else {
		return a.p > b.p;
	}
}
bool compareJobsBy_d_desc(const Job& a, const Job& b) {
	if(a.d == b.d) {
		return a.id < b.id;
	}
	else {
		return a.d > b.d;
	}
}
bool compareJobsBy_r_desc(const Job& a, const Job& b) {
	if(a.r == b.r) {
		return a.id < b.id;
	}
	else {
		return a.r > b.r;
	}
}

bool compareJobsBy_s_desc(const Job& a, const Job& b) {
	if(a.s == b.s) {
		return a.id < b.id;
	}
	else {
		return a.s > b.s;
	}
}
bool compareJobsBy_w_desc(const Job& a, const Job& b) {
	if(a.w == b.w) {
		return a.id < b.id;
	}
	else {
		return a.w > b.w;
	}
}
bool compareJobsBy_f_desc(const Job& a, const Job& b) {
	if(a.f == b.f) {
		return a.id < b.id;
	}
	else {
		return a.f > b.f;
	}
}
bool compareJobsBy_wp_desc(const Job& a, const Job& b) {
	double wpA = (double) a.w / (double) a.p;
	double wpB = (double) b.w / (double) b.p;
	if( wpA == wpB) {
		return a.id <= b.id;
	}
	else {
		return wpA > wpB;
	}
}