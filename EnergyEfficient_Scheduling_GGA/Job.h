#ifndef JOB_H
#define JOB_H

#include "General.h"

using namespace std;

class Job {
private:
	int c;		// completion time
	int start;	// actual starting time

public:
	// attributes´ declaration
	int id;		
	int p;		// processing time
	int d;		// due date
	int r;		// release date
	int s;		// size
	int w;		// weight
	int f;		// familie

	static int JobCounter;

	// constructors´ declaration
	Job();
	Job(int in_id, int in_p, int in_d, int in_r, int in_s, int in_w, int in_f);
	Job(const Job* other);

	// methods´ declaration
	/// scheduling
	void setC(int time);
	int getC();
	bool setStart(int time);
	int getStart();
	void copy(const Job* other);

	/// objective
	double getTCT();				// total completion time
	double getTWCT();				// total weighted completion time
	double getTT();					// total tardiness
	double getTWT();				// total weighted tardiness


};

#endif