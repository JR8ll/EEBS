#include "JobSet.h"

using namespace std;

// constructor definitions
JobSet::JobSet() {
	this->jobs = vector<Job *>();
}

JobSet::~JobSet() {
}

// operator overloading
Job& JobSet::operator[](int index) {
	return *(this->jobs[index]);
}

// method definitions
int JobSet::size() {
	return this->jobs.size();	
}

vector<Job*>::iterator JobSet::begin() {
	return this->jobs.begin();
}

void JobSet::clear() {
	this->jobs.clear();
}

void JobSet::copy(const JobSet *other) {
	jobs.clear();
	jobs.resize(other->jobs.size());
	jobs = other->jobs;
}
Job& JobSet::getJob(int i) const {
	return *(this->jobs[i]);
}

bool JobSet::addJob(Job* in_Job){
	this->jobs.push_back(in_Job);
	return true;
	// TODO exception handling
}

bool JobSet::addJob(Job in_Job){
	this->jobs.push_back(&in_Job);
	return true;
	// TODO exception handling
}

void JobSet::print() {
	int jMax = this->size();
	if(jMax == 0) {
		cout << "This set of jobs is empty." << endl;
	}
	else {
		for(unsigned j = 0; j < jMax; j++) {
			cout << "Job " << this->jobs[j]->id << "\t(s=" << this->jobs[j]->s << ",\t p=" << this->jobs[j]->p;
			cout << ",\t d=" << this->jobs[j]->d << ",\t r=" << this->jobs[j]->r << ",\t f=" << this->jobs[j]->f << ",\t w=" << this->jobs[j]->w << ")";
			cout << endl;
		}
	}
}

bool JobSet::empty() {
	if(this->jobs.empty()){
		return true;
	}
	else {
		return false;
	}
}
double JobSet::getAvgPLot() {
	double avgP = 0.0;
	int jMax = this->size();
	for(unsigned i = 0; i < jMax; i++) {
		avgP += this->jobs[i]->p;
	}
	avgP = avgP / (double) jMax;
	return avgP;
}

double JobSet::getAvgPItem() {
	double avgP = 0.0;
	int jMax = this->size();
	for(unsigned i = 0; i < jMax; i++) {
		avgP += this->jobs[i]->p * this->jobs[i]->s;
	}
	avgP = avgP / (double) jMax;
	return avgP;
}
bool JobSet::erase(const int id) {
	if(!this->jobs.empty()) {
		for(vector<Job *>::iterator it = this->jobs.begin(); it != this->jobs.end(); it++) {
			if( (*it)->id == id) {
				this->jobs.erase(it);
				return true;
			}
		}
	}
	return false;
}
Job* JobSet::getJobByID(int in_id) {
	int setSize = this->jobs.size();
	for(unsigned i = 0; i < setSize; i++) {
		if(this->jobs[i]->id == in_id) {
			return this->jobs[i];
		}
	}
	return 0; 
}
void JobSet::destroy() {
	int iMax = this->jobs.size();
	for(unsigned i = 0; i < iMax; i++) {
		delete this->jobs[i];
		this->jobs[i] = NULL;
	}
}
/// sorting
void JobSet::sortBy_p(bool asc){
	if(asc) {
		sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_p);
	}
	else {
		sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_p_desc);
	}
}
void JobSet::sortBy_d(bool asc){
	if(asc) {
	sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_d);
	}
	else {
		sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_d_desc);
	}
}
void JobSet::sortBy_r(bool asc){
	if(asc) {
	sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_r);
	}
	else {
		sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_r_desc);
	}
}
void JobSet::sortBy_s(bool asc){
	if(asc) {
	sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_s);
	}
	else {
		sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_s_desc);
	}
}
void JobSet::sortBy_w(bool asc){
	if(asc) {
	sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_w);
	}
	else {
		sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_w_desc);
	}
}
void JobSet::sortBy_f(bool asc){
	if(asc) {
	sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_f);
	}
	else {
		sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_f_desc);
	}
}
void JobSet::sortBy_wp(bool asc){
	if(asc) {	// sort ascending
	sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_wp);
	}
	else {		// sort descending
		sort(this->jobs.begin(), this->jobs.end(), JobSet::compareJobsBy_wp_desc);
	}
}

void JobSet::sortBy_ATC(int time, double kappa, bool asc) {
	// usually ATC implies a non-increasing order => call w/ asc = false
	// get average p
	double avgP = 0;
	int jMax = this->jobs.size();
	for(unsigned i = 0; i < jMax; i++){
		avgP += (double)jobs[i]->p;
	}
	avgP = avgP / (double)jMax;
	if(asc) {	// sort ascending
		sort(this->jobs.begin(), this->jobs.end(), CompareJobsBy_ATC(avgP, time, kappa));
	}
	else {		// sort descending
		sort(this->jobs.begin(), this->jobs.end(), CompareJobsBy_ATC_desc(avgP, time, kappa));
	}
}

// compare Jobs
/// ascending order
bool JobSet::compareJobsBy_p(const Job* a, const Job* b) {
	if(a->p == b->p) {
		return a->id < b->id;
	}
	else {
		return a->p < b->p;
	}
}
bool JobSet::compareJobsBy_d(const Job* a, const Job* b) {
	if(a->d == b->d) {
		return a->id < b->id;
	}
	else {
		return a->d < b->d;
	}
}
bool JobSet::compareJobsBy_r(const Job* a, const Job* b) {
	if(a->r == b->r) {
		return a->id < b->id;
	}
	else {
		return a->r < b->r;
	}
}

bool JobSet::compareJobsBy_s(const Job* a, const Job* b) {
	if(a->s == b->s) {
		return a->id < b->id;
	}
	else {
		return a->s < b->s;
	}
}
bool JobSet::compareJobsBy_w(const Job* a, const Job* b) {
	if(a->w == b->w) {
		return a->id < b->id;
	}
	else {
		return a->w < b->w;
	}
}
bool JobSet::compareJobsBy_f(const Job* a, const Job* b) {
	if(a->f == b->f) {
		return a->id < b->id;
	}
	else {
		return a->f < b->f;
	}
}
bool JobSet::compareJobsBy_wp(const Job* a, const Job* b) {
	double wpA = (double) a->w / (double) a->p;
	double wpB = (double) b->w / (double) b->p;
	if( wpA == wpB) {
		return a->id <= b->id;
	}
	else {
		return wpA < wpB;
	}
}
/// descending order
bool JobSet::compareJobsBy_p_desc(const Job* a, const Job* b) {
	if(a->p == b->p) {
		return a->id < b->id;
	}
	else {
		return a->p > b->p;
	}
}
bool JobSet::compareJobsBy_d_desc(const Job* a, const Job* b) {
	if(a->d == b->d) {
		return a->id < b->id;
	}
	else {
		return a->d > b->d;
	}
}
bool JobSet::compareJobsBy_r_desc(const Job* a, const Job* b) {
	if(a->r == b->r) {
		return a->id < b->id;
	}
	else {
		return a->r > b->r;
	}
}

bool JobSet::compareJobsBy_s_desc(const Job* a, const Job* b) {
	if(a->s == b->s) {
		return a->id < b->id;
	}
	else {
		return a->s > b->s;
	}
}
bool JobSet::compareJobsBy_w_desc(const Job* a, const Job* b) {
	if(a->w == b->w) {
		return a->id < b->id;
	}
	else {
		return a->w > b->w;
	}
}
bool JobSet::compareJobsBy_f_desc(const Job* a, const Job* b) {
	if(a->f == b->f) {
		return a->id < b->id;
	}
	else {
		return a->f > b->f;
	}
}
bool JobSet::compareJobsBy_wp_desc(const Job* a, const Job* b) {
	double wpA = (double) a->w / (double) a->p;
	double wpB = (double) b->w / (double) b->p;
	if( wpA == wpB) {
		return a->id <= b->id;
	}
	else {
		return wpA > wpB;
	}
}
/// objective
double JobSet::getTCT()	{			// total completion time
	double tct = 0.0;
	int maxJ = this->jobs.size();
	for(unsigned i = 0; i < maxJ; i++) {
		tct += this->jobs[i]->getC();
	}
	return tct;
}
double JobSet::getTWCT(){				// total weighted completion time
	double twct = 0.0;
	int maxJ = this->jobs.size();
	for(unsigned i = 0; i < maxJ; i++) {
		twct += this->jobs[i]->getC() * this->jobs[i]->w;
	}
	return twct;
}
double JobSet::getTT()	{				// total tardiness
	double tt = 0.0;
	int maxJ = this->jobs.size();
	for(unsigned i = 0; i < maxJ; i++) {
		tt += max<double>(this->jobs[i]->getC() - this->jobs[i]->d, 0);
	}
	return tt;
}
double JobSet::getTWT(int batchCompletion)	{			// total weighted tardiness
	double twt = 0.0;
	int maxJ = this->jobs.size();
	for(unsigned i = 0; i < maxJ; i++) {
		twt += max<double>(( batchCompletion - this->jobs[i]->d), 0) * this->jobs[i]->w;
	}
	return twt;
}