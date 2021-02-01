#ifndef BATCH_H
#define BATCH_H

#include "JobSet.h"

using namespace std;

class Batch {
private:
	int c;					// completion time
	int start;				// actual starting time
	JobSet content;			// jobs included in the batch

public:
	// attributes´ declaration
	static int BatchCounter;
	int id;
	int capacity;			// total capacity
	int freeCapacity;		// available capacity
	int pLot;				// processing time at lot processing machines
	int pItem;				// processing time at item processing machines 
	int r;					// earliest starting time
	int earliestD;			// earliest due date of the jobs included
	int latestD;			// latest due date of the jobs included
	int latestC;			// latest C depending on following batches (TWTEPC_2Solution)
	int earliestStart;		// earliest Start depending on the batches processed before (TWTEPC_2Solution)
	int f;					// family of orders which can be included in this batch
	int w;					// sum of the weights of the jobs included
	int numJobs;			// number of jobs included
	double key;				// random key

	// constructors´ declaration
	Batch();
	Batch(int in_capacity);
	Batch(const Batch* other);
	~Batch();

	// methods´ declaration
	void copy(const Batch *other);
	void clear();
	void destroy();
	Batch copy();


	bool addJob(Job* in_Job);
	bool addJob(Job in_Job);
	Job& getJob(int i) const;
	void unSchedule();								// reset start and c to 0;
	void setC(int time);
	void setKey(double in_key);
	int getC() const;
	bool setStart(int time);
	int getStart() const;
	bool intersects(const set<int> &other);			// returns true, if there is at least one job inside the batch with an id contained in the set
	bool contains(const int id);					// returns true, if a job with the given id is part of the batch
	bool erase(const int id);						// erases the jobs with the given id
	bool empty();

	/// objective function values
	double getTCT();									// total completion time
	double getTWC();									// total weighted completion time
	double getTT();										// total tardiness
	double getTWT();									// total weighted tardiness
	double getMSP();									// makespan
	double getEPC(const vector<int>& Eprice);			// electric power cost

};

#endif