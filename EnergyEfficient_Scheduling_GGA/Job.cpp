#include "Job.h"

using namespace std;

int Job::JobCounter = 0;

// constructor definitions
Job::Job() {
	this->id = ++Job::JobCounter;
	this->p = 1;
	this->d = 1;
	this->r = 1;
	this->start = r;
	this->c = 0;
	this->s = 1;
	this->w = 1;
	this->f = 1;
}

Job::Job(int in_id, int in_p, int in_d, int in_r, int in_s, int in_w, int in_f){
	this->id = ++Job::JobCounter;
	this->p = in_p;
	this->d = in_d;
	this->r = in_r;
	this->start = in_r;
	this->c = 0;
	this->s = in_s;
	this->w = in_w;
	this->f = in_f;
}

Job::Job(const Job* other){
	this->id = other->id;
	this->p = other->p;
	this->d = other->d;
	this->r = other->r;
	this->start = other->r;
	this->c = 0;
	this->s = other->s;
	this->w = other->w;
	this->f = other->f;
}

// method definitions
void Job::setC(int time) {
	this->c = time;
}

int Job::getC() {
	return this->c;
}

bool Job::setStart(int time) {
	if(this->r <= time) {
		this->start = time;
		return true;
	}
	else  {
		return false;
	}
}
int Job::getStart() {
	return this->start;
}

void Job::copy(const Job* other) {
	if (this == other) return;
	Job &oth = *((Job*) (other));
	id = oth.id;
	p = oth.p;
	d = oth.d;
	r = oth.r;
	s = oth.s;
	w = oth.w;
	f = oth.f;
}

/// objectives
double Job::getTCT() {				// total completion time
	return this->c;
}				
double Job::getTWCT() {					// total weighted completion time
	return (this->c * this->w);
}				
double Job::getTT() {					// total tardiness
	return max<double>(this->c - this->d, 0);
}					
double Job::getTWT() {					// total weighted tardiness
	return max<double>((this->c - this->d) * this->w, 0);
}				
		