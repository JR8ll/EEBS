#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Problem.h"

struct GAParameters {
	int popSize;			// iPopulationSize
	int numGens;			// iNumberOfGenerations
	int timeLimit;			// timeLimit
	double probMut;			// dMutationProbability
	double popGeomFactor;	// dPopulationGeometricalFactor
	bool scalarize;			// bScalarize
};

// global attributes
namespace Global {
	extern Problem *problem;
	extern GAParameters params;
	extern int gv_eRates;
}

// global function declarations
int scheduleEEBS();
BiObjectiveSolutionSet scheduleEEBS(int eRates, int timeLimit);
BiObjectiveSolutionSet scheduleTWD(int eRates);
GAParameters& setParameters(int popSize, int numGens, int timeLimit, double probMut, double popGeomFactor, bool scalarize);


// sort Batches
void sortBatchesBy_r(vector<Batch>& batches, bool asc);
void sortBatchesBy_pLot(vector<Batch>& batches, bool asc);
void sortBatchesBy_pItem(vector<Batch>& batches, bool asc);
void sortBatchesBy_w(vector<Batch>& batches, bool asc);
void sortBatchesBy_wpLot(vector<Batch>& batches, bool asc);
void sortBatchesBy_wpItem(vector<Batch>& batches, bool asc);
void sortBatchesBy_BATCII(vector<Batch>& batches, int time, double kappa, bool asc);

// sort Jobs
void sortJobsBy_p(vector<Job>& jobs, bool asc);
void sortJobsBy_d(vector<Job>& jobs, bool asc);
void sortJobsBy_r(vector<Job>& jobs, bool asc);
void sortJobsBy_s(vector<Job>& jobs, bool asc);
void sortJobsBy_w(vector<Job>& jobs, bool asc);
void sortJobsBy_f(vector<Job>& jobs, bool asc);
void sortJobsBy_wp(vector<Job>& jobs, bool asc);
void sortJobsBy_ATC(vector<Job>& jobs, int time, double kappa, bool asc);	// usually ATC implies a non-increasing order => call w/ asc = false

void sortJobsBy_f();
void sortJobsBy_r();
void sortJobsBy_ATC();

// Job2Batch Assignment (with a given number of batches, see MOJ)
bool assign_FFDn(vector<Job>& jobs, vector<Batch>& batches);
bool assign_FFDnb(vector<Job>& jobs, vector<Batch>& batches);
bool assign_FFD1(vector<Job>& jobs, vector<Batch>& batches);
bool assign_FFD1b(vector<Job>& jobs, vector<Batch>& batches, Problem& problem);
bool assign_FFDAJS(vector<Job>& jobs, vector<Batch>& batches);
bool assign_FFBF(vector<Job>& jobs, vector<Batch>& batches);
bool assign_poker_clockwise(vector<Job>& jobs, vector<Batch>& batches);
bool assign_poker_oscillating(vector<Job>& jobs, vector<Batch>& batches);

// batch formation (with an arbitrary number of batches)
vector<Batch> formBatches_FFDn(vector<Job>& jobs, Problem& problem);
vector<Batch> formBatches_FFD1(vector<Job>& jobs);
vector<Batch> formBatches_FFBF(vector<Job>& jobs);

// compare Batches
/// ascending order
bool compareBatchesBy_r(const Batch& a, const Batch& b);
bool compareBatchesBy_pLot(const Batch& a, const Batch& b);
bool compareBatchesBy_pItem(const Batch& a, const Batch& b);
bool compareBatchesBy_w(const Batch& a, const Batch& b);
bool compareBatchesBy_wpLot(const Batch& a, const Batch& b);
bool compareBatchesBy_wpItem(const Batch& a, const Batch& b);
// remark: class CompareBatchesBy_BATCII was moved to BatchSet.h
/// descending order
bool compareBatchesBy_r_desc(const Batch& a, const Batch& b);
bool compareBatchesBy_pLot_desc(const Batch& a, const Batch& b);
bool compareBatchesBy_pItem_desc(const Batch& a, const Batch& b);
bool compareBatchesBy_w_desc(const Batch& a, const Batch& b);
bool compareBatchesBy_wpLot_desc(const Batch& a, const Batch& b);
bool compareBatchesBy_wpItem_desc(const Batch& a, const Batch& b);
// remark: class CompareBatchesBy_BATCII_desc was moved to BatchSet.h
// compare Jobs
/// ascending order
bool compareJobsBy_p(const Job& a, const Job& b);
bool compareJobsBy_d(const Job& a, const Job& b);
bool compareJobsBy_r(const Job& a, const Job& b);
bool compareJobsBy_s(const Job& a, const Job& b);
bool compareJobsBy_w(const Job& a, const Job& b);
bool compareJobsBy_f(const Job& a, const Job& b);
bool compareJobsBy_wp(const Job& a, const Job& b);
/*
class CompareJobsBy_ATC {
public:
	double avgP;
	double t;
	double kappa;
	CompareJobsBy_ATC(double avgP, int t, double kappa) { 
		this->avgP = avgP;
		this->t = (double)t;
		this->kappa = kappa;
	};
	bool operator () (const Job& a, const Job& b) {
		double wpA = (double) a.w / (double) a.p;
		double wpB = (double) b.w / (double) b.p;
		double atc_a = wpA * exp(- (max<double>((double)a.d - (double)a.p + ((double)a.r - t), 0)) / (kappa * avgP));
		double atc_b = wpB * exp(- (max<double>((double)b.d - (double)b.p + ((double)b.r - t), 0)) / (kappa * avgP));
		if(atc_a == atc_b){
			return a.id < b.id;
		}
		else {
			return atc_a < atc_b;
		}
	}
};
*/

/// descending order
bool compareJobsBy_p_desc(const Job& a, const Job& b);
bool compareJobsBy_d_desc(const Job& a, const Job& b);
bool compareJobsBy_r_desc(const Job& a, const Job& b);
bool compareJobsBy_s_desc(const Job& a, const Job& b);
bool compareJobsBy_w_desc(const Job& a, const Job& b);
bool compareJobsBy_f_desc(const Job& a, const Job& b);
bool compareJobsBy_wp_desc(const Job& a, const Job& b);
/*
class CompareJobsBy_ATC_desc {
public:
	double avgP;
	int t;
	double kappa;
	CompareJobsBy_ATC_desc(double avgP, int t, double kappa) { 
		this->avgP = avgP;
		this->t = t;
		this->kappa = kappa;
	};
	bool operator () (const Job& a, const Job& b) {
		double wpA = (double) a.w / (double) a.p;
		double wpB = (double) b.w / (double) b.p;
		double atc_a = wpA * exp(- (max<double>((double)a.d - (double)a.p + ((double)a.r - t), 0)) / (kappa * avgP));
		double atc_b = wpB * exp(- (max<double>((double)b.d - (double)b.p + ((double)b.r - t), 0)) / (kappa * avgP));
		if(atc_a == atc_b){
			return a.id < b.id;
		}
		else {
			return atc_a > atc_b;
		}
	}
};
*/

#endif