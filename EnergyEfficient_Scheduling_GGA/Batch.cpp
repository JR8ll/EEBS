#include "Batch.h"

using namespace std;

int Batch::BatchCounter = 0;

// constructor definitions
Batch::Batch() {
	this->id = ++Batch::BatchCounter;
	this->capacity = 1;
	this->freeCapacity = this->capacity;
	this->pLot = 0;
	this->pItem = 0;
	this->r = 0;
	this->earliestD = 0;
	this->latestD = 0;
	this->c = 0;
	this->f = 0;			// neutral state: any family can be included	
	this->w = 0;
	this->numJobs = 0;
	this->start = 0;
	this->c = 0;
	this->key = 0.0;
}

Batch::Batch(int in_capacity){
	this->id = ++Batch::BatchCounter;
	this->capacity = in_capacity;
	this->freeCapacity = this->capacity;
	this->pLot = 0;
	this->pItem = 0;
	this->r = 0;
	this->earliestD = 0;
	this->latestD = 0;
	this->c = 0;
	this->f = 0;			// neutral state: any family can be included
	this->w = 0;
	this->numJobs = 0;
	this->start = 0;
	this->key = 0.0;
}

Batch::Batch(const Batch* other){
	this->id = ++Batch::BatchCounter;
	this->capacity = other->capacity;
	this->freeCapacity = other->freeCapacity;
	this->pLot = other->pLot;
	this->pItem = other->pItem;
	this->r = other->r;
	this->earliestD = other->earliestD;
	this->latestD = other->latestD;
	this->c = other->getC();
	this->f = other->f;			// neutral state: any family can be included
	this->w = other->w;
	this->numJobs = other->numJobs;
	this->start = other->getStart();
	this->content.copy( &other->content );
	this->key = other->key;
}

Batch::~Batch() {
}

// method definitions
void Batch::copy(const Batch *other) {
	if (this == other) return;
	Batch &oth = *((Batch*) (other));
	// id = oth.id;
	capacity = oth.capacity;
	freeCapacity = oth.freeCapacity;
	pLot = oth.pLot;
	pItem = oth.pItem;
	r = oth.r;
	earliestD = oth.earliestD;
	latestD = oth.latestD;
	f = oth.f;
	w = oth.w;
	numJobs = oth.numJobs;
	content.copy( ( &(*other).content) );
	start = other->getStart();
	this->setC(other->getC());
	this->key = oth.key;
}



void Batch::clear() {
	freeCapacity = capacity;
	pLot = 0;
	pItem = 0;
	r = 0;
	earliestD = 0;
	latestD = 0;
	f = 0;
	w = 0;
	numJobs = 0;
	content.clear();
	this->setStart(0);
	//this->key = 0.0;
}

void Batch::destroy() {
	if(!this->content.empty()) {
		this->content.destroy();		// TODO 23.07. crashes here somewhen, Nullpointer abfragen??
	}
}
Job& Batch::getJob(int i) const {
	return this->content.getJob(i);
}
bool Batch::addJob(Job* in_Job) {
	if(this->f == 0 || this->f == in_Job->f) {
		// matching family
		if(this->freeCapacity >= in_Job->s) {
			// sufficient capacity
			// REMARK insert job von hier verschoben 
			this->f = in_Job->f;					// update batch family
			this->freeCapacity -= in_Job->s;		// update free capacity
			this->numJobs++;					// update number of jobs included
			this->w += in_Job->w;				// update weight
			// update release time
			if(this->content.empty() || this->r < in_Job->r) {				
				this->r = in_Job->r;
			}
			// update earliestStart 18092018
			if(this->r > this->earliestStart) {
				this->earliestStart = this->r;
			}

			// update earliest due date
			if(in_Job->d < this->earliestD || this->earliestD == 0) {
				this->earliestD = in_Job->d;
			}
			// update latest due date
			if(in_Job->d > this->latestD || this->latestD == 0) {
				this->latestD = in_Job->d;
			}
			// update processing time for lot processing machines
			if(this->pLot < in_Job->p){
				this->pLot = in_Job->p;
			}
			// update processing time for item processing machines
			this->pItem += in_Job->p * in_Job->s;
			
			// update jobs´ start + completion time
			in_Job->setStart(this->getStart());
			in_Job->setC(this->getC());

			// insert Job
			this->content.addJob(in_Job);		

			
			

			return true;
		}
		else {
			// insufficient free capacity
			return false;
		}
	}
	else {
		// family mismatch
		return false;
	}
}




bool Batch::addJob(Job in_Job) {
	if(this->f == 0 || this->f == in_Job.f) {
		// matching family
		if(this->freeCapacity >= in_Job.s) {
			// sufficient capacity
			// REMARK insert job von hier verschoben 
			this->f = in_Job.f;					// update batch family
			this->freeCapacity -= in_Job.s;		// update free capacity
			this->numJobs++;					// update number of jobs included
			this->w += in_Job.w;				// update weight
			// update release time
			if(this->content.empty() || this->r < in_Job.r) {				
				this->r = in_Job.r;
			}
			// update earliestStart 18092018
			if(this->r > this->earliestStart) {
				this->earliestStart = this->r;
			}

			// update earliest due date
			if(in_Job.d < this->earliestD || this->earliestD == 0) {
				this->earliestD = in_Job.d;
			}
			// update latest due date
			if(in_Job.d > this->latestD || this->latestD == 0) {
				this->latestD = in_Job.d;
			}
			// update processing time for lot processing machines
			if(this->pLot < in_Job.p){
				this->pLot = in_Job.p;
			}
			// update processing time for item processing machines
			this->pItem += in_Job.p * in_Job.s;
			
			// update jobs´ start + completion time
			in_Job.setStart(this->getStart());
			in_Job.setC(this->getC());

			// insert Job
			this->content.addJob(in_Job);		

			
			

			return true;
		}
		else {
			// insufficient free capacity
			return false;
		}
	}
	else {
		// family mismatch
		return false;
	}
}




void Batch::unSchedule() {
	this->c = 0;
	this->start = 0;
	int jMax = this->numJobs;
	if(jMax > 0) {
		// reset the jobs´ completion to zero
		for(unsigned i = 0; i < jMax; i++) {
			this->content.getJob(i).setC(0);
		}
	}
}
void Batch::setC(int time) {
	// set the completion time of the batch and all jobs included
	this->c = time;
	int maxJ = this->content.size();
	for(unsigned i = 0; i < maxJ; i++) {
		content.getJob(i).setC(this->c);
	}
}
void Batch::setKey(double in_key) {
	this->key = in_key;
}
int Batch::getC() const {
	return this->c;
}

bool Batch::setStart(int time) {
	if(this->r <= time) {								// batch is supposed to start after it is available
		this->start = time;								// set batches start
		this->setC(this->start + this->pLot);			// update completion time
		int maxJ = this->content.size();
		for(unsigned i = 0; i < maxJ; i++) {
			if(!this->content.getJob(i).setStart(this->start)){	// set jobs´ start
				cout << "Job " << this->content.getJob(i).id << " mit r=" << this->content.getJob(i).r << " soll starten in " << this->start << "." << endl;
				cout << "Error: A job´s release time is disrespected" << endl;
				int stop;
				cin >> stop;
				return false;
			};	
		}
		return true;
	}
	else {												// batch is supposed to start before it is available
		return false;
	}
}
int Batch::getStart() const {
	return this->start;
}

bool Batch::intersects(const set<int> &other){
	int jobsInThisBatch = this->content.size();
	for(unsigned i = 0; i < jobsInThisBatch; i++) {
		if(other.find(this->content[i].id) != other.end()) return true;
	}
	return false;
}
bool Batch::contains(const int id) {
	if(!this->content.empty()){
		int jobsInThisBatch = this->content.size();
		for(unsigned i = 0; i < jobsInThisBatch; i++) {
			if(this->content[i].id == id) return true;
		}
	}
	return false;
}
bool Batch::erase(const int id) {
	int rMax = 0;
	int dMin = 999999;
	int dMax = 0;
	int jobsP = this->content.getJobByID(id)->p;
	int jobsS = this->content.getJobByID(id)->s;

	if(this->content.erase(id)) {
		this->numJobs--;
		// if empty reset the batch
		if(this->content.empty()) {
			this->clear();
		}
		else {
			// update release time, earliest due date, latest due date, pItem and free capacity
			int iMax = this->content.size();
			for(unsigned i = 0; i < iMax; i++) {
				if(this->content[i].r > rMax) rMax = this->content[i].r;
				if(this->content[i].d < dMin) dMin = this->content[i].d;
				if(this->content[i].d > dMax) dMax = this->content[i].d;
			}
			this->r = rMax;
			this->earliestD = dMin;
			this->latestD = dMax;
			this->pItem -= jobsP;
			this->freeCapacity += jobsS;
		}
		return true;
	}
	return false;

}

bool Batch::empty() {
	return this->content.empty();
}
/// objective function values
double Batch::getTCT() {									// total completion time
	return this->content.getTCT();
}
double Batch::getTWC(){								// total weighted completion time
	int jMax = this->content.size();
	double wSum = 0;
	for(int j = 0; j < jMax; j++) {
		wSum += (double) this->content[j].w;
	}
	return wSum * this->getC();
	//return this->content.getTWCT();
}
double Batch::getTT(){										// total tardiness
	return this->content.getTT();
}
double Batch::getTWT(){									// total weighted tardiness
	return this->content.getTWT(this->c);
}
double Batch::getMSP(){									// makespan
	return this->c;
}
double Batch::getEPC(const vector<int>& Eprice) {
	if(this->c > Eprice.size()) {
			// batch completes after the last available time bucket => infeasible schedule
			return 999999.0;
	}

	double epc = 0.0;
	for(unsigned t = this->start; t < this->c; t++) {	
		epc += (double) Eprice[t];
	}
	return epc;
}