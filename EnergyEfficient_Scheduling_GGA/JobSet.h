#ifndef JOBSET_H
#define JOBSET_H

#include "Job.h"

using namespace std;

class JobSet {
private:
	// attributes declaration
	vector<Job *> jobs;

public:
	// attributes declaration
	

	// constructor declarations
	JobSet();
	~JobSet();

	// operator overloading
	Job& operator[](int index);

	// method declarations
	int size();
	vector<Job*>::iterator begin();

	void copy(const JobSet *other);
	void clear();
	Job& getJob(int i) const;
	bool addJob(Job* in_Job);
	bool addJob(Job in_Job);
	void print();
	bool empty();
	double getAvgPLot();
	double getAvgPItem();
	bool erase(const int id);		// erase a job with the given id
	Job* getJobByID(int i);
	void destroy();

	/// sorting
	void sortBy_p(bool asc);
	void sortBy_d(bool asc);
	void sortBy_r(bool asc);
	void sortBy_s(bool asc);
	void sortBy_w(bool asc);
	void sortBy_f(bool asc);
	void sortBy_wp(bool asc);
	void sortBy_ATC(int time, double kappa, bool asc);	// usually ATC implies a non-increasing order => call w/ asc = false
	
	/// compare ascending order
	static bool compareJobsBy_p(const Job* a, const Job* b);
	static bool compareJobsBy_d(const Job* a, const Job* b);
	static bool compareJobsBy_r(const Job* a, const Job* b);
	static bool compareJobsBy_s(const Job* a, const Job* b);
	static bool compareJobsBy_w(const Job* a, const Job* b);
	static bool compareJobsBy_f(const Job* a, const Job* b);
	static bool compareJobsBy_wp(const Job* a, const Job* b);
	
	/// compare descending order
	static bool compareJobsBy_p_desc(const Job* a, const Job* b);
	static bool compareJobsBy_d_desc(const Job* a, const Job* b);
	static bool compareJobsBy_r_desc(const Job* a, const Job* b);
	static bool compareJobsBy_s_desc(const Job* a, const Job* b);
	static bool compareJobsBy_w_desc(const Job* a, const Job* b);
	static bool compareJobsBy_f_desc(const Job* a, const Job* b);
	static bool compareJobsBy_wp_desc(const Job* a, const Job* b);
	/// objective
	double getTCT();				// total completion time
	double getTWCT();				// total weighted completion time
	double getTT();					// total tardiness
	double getTWT(int batchCompletion);	// total weighted tardiness
};

class CompareJobsBy_ATC {
public:
	double avgP;
	double t;
	double kappa;
	CompareJobsBy_ATC(double avgP, double t, double kappa) { 
		this->avgP = avgP;
		this->t = (double)t;
		this->kappa = kappa;
	};
	bool operator () (const Job *a, const Job *b) {
		double wpA = (double) a->w / (double) a->p;
		double wpB = (double) b->w / (double) b->p;
		double atc_a = wpA * exp(- (max<double>((double)a->d - (double)a->p + ((double)a->r - t), 0)) / (kappa * avgP));
		double atc_b = wpB * exp(- (max<double>((double)b->d - (double)b->p + ((double)b->r - t), 0)) / (kappa * avgP));
		if(atc_a == atc_b){
			return a->id < b->id;
		}
		else {
			return atc_a < atc_b;
		}
	}
};

class CompareJobsBy_ATC_desc {
public:
	double avgP;
	double t;
	double kappa;
	CompareJobsBy_ATC_desc(double avgP, double t, double kappa) { 
		this->avgP = avgP;
		this->t = t;
		this->kappa = kappa;
	};
	bool operator () (const Job* a, const Job* b) {
		double wpA = (double) a->w / (double) a->p;
		double wpB = (double) b->w / (double) b->p;
		double atc_a = wpA * exp(- (max<double>((double)a->d - (double)a->p + ((double)a->r - t), 0)) / (kappa * avgP));
		double atc_b = wpB * exp(- (max<double>((double)b->d - (double)b->p + ((double)b->r - t), 0)) / (kappa * avgP));
		if(atc_a == atc_b){
			return a->id < b->id;
		}
		else {
			return atc_a > atc_b;
		}
	}
};




#endif