#ifndef BATCHSET_H
#define BATCHSET_H

#include "Batch.h"

using namespace std;

class BatchSet {
private: 
	// attributes declaration
	vector<Batch> batches;

public:
	// attributes declaration

	// constructor declarations
	BatchSet();

	// operator overloading
	Batch& operator[](int index);

	// method declarations
	int size();
	void clear();
	bool empty();
	void copy(const BatchSet* other);
	Batch& getBatch(int i);
	bool addBatch(Batch& in_Batch);
	bool addBatch(Batch& in_Batch, int start);
	bool erase(int index);
	void print();
	void updateCompletionTimes();
	void resize(int newSize);

	/// sorting
	void sortBy_Start();
	void sortBy_r(bool asc);
	void sortBy_pLot(bool asc);
	void sortBy_pItem(bool asc);
	void sortBy_w(bool asc);
	void sortBy_wpLot(bool asc);
	void sortBy_wpItem(bool asc);
	void sortBy_BATCII(int time, double kappa, bool asc);
	
	/// compare ascending
	static bool compareBatchesBy_Start(const Batch& a, const Batch& b);
	static bool compareBatchesBy_r(const Batch& a, const Batch& b);
	static bool compareBatchesBy_pLot(const Batch& a, const Batch& b);
	static bool compareBatchesBy_pItem(const Batch& a, const Batch& b);
	static bool compareBatchesBy_w(const Batch& a, const Batch& b);
	static bool compareBatchesBy_wpLot(const Batch& a, const Batch& b);
	static bool compareBatchesBy_wpItem(const Batch& a, const Batch& b);
	
	/// compare descending
	static bool compareBatchesBy_r_desc(const Batch& a, const Batch& b);
	static bool compareBatchesBy_pLot_desc(const Batch& a, const Batch& b);
	static bool compareBatchesBy_pItem_desc(const Batch& a, const Batch& b);
	static bool compareBatchesBy_w_desc(const Batch& a, const Batch& b);
	static bool compareBatchesBy_wpLot_desc(const Batch& a, const Batch& b);
	static bool compareBatchesBy_wpItem_desc(const Batch& a, const Batch& b);
	
	/// job assignment
	bool assign_FFDn(vector<Job>& jobs);
	bool assign_FFDn(JobSet& jobs);

	void form_FF(JobSet& jobs, int batchCap);

	bool assign_FFDnb(vector<Job>& jobs);
	bool assign_FFDnb(JobSet& jobs);

	bool assign_FFD1(vector<Job>& jobs);
	bool assign_FFD1(JobSet& jobs);

	bool assign_FFD1b(vector<Job>& jobs, int globalF);		// globalF = number of families given with the problem instance
	bool assign_FFD1b(JobSet& jobs, int globalF);		// globalF = number of families given with the problem instance
	
	bool assign_FFDAJS(vector<Job>& jobs);
	bool assign_FFDAJS(JobSet& jobs);
	
	bool assign_FFBF(vector<Job>& jobs);
	bool assign_FFBF(JobSet& jobs);
	
	bool assign_poker_clockwise(vector<Job>& jobs);
	bool assign_poker_clockwise(JobSet& jobs);
	
	bool assign_poker_oscillating(vector<Job>& jobs);
	bool assign_poker_oscillating(JobSet& jobs);

	double getMSP();
};

class CompareBatchesBy_BATCII {
	public:
		double avgP;
		double t;
		double kappa;
		CompareBatchesBy_BATCII(double avgP, int t, double kappa) {
			this->avgP = avgP;
			this->t = (double) t;
			this->kappa = kappa;
		};
		bool operator() (const Batch& a, const Batch& b) {
			// compute BATC-II for batch a
			double batcA = 0;
			int jMax = a.numJobs;
			for(unsigned j = 0; j < jMax; j++) {
				batcA += ((double) a.getJob(j).w / (double) a.pLot) * exp(-1.0* ((max<double>(((double) a.getJob(j).d - (double) a.pLot - t + (max<double>((double) a.r - t, 0))), 0)) / (kappa * avgP)));
			}
			batcA = batcA * ((double)a.w / (double)a.capacity);
			// compute BATC-II for batch b
			double batcB = 0;
			int kMax = b.numJobs;
			for(unsigned k = 0; k < kMax; k++) {
				batcB += ((double) b.getJob(k).w / (double) b.pLot) * exp(-1.0* ((max<double>(((double) b.getJob(k).d - (double) b.pLot - t + (max<double>((double) b.r - t, 0))), 0)) / (kappa * avgP)));
			}
			batcB = batcB * ((double)b.w / (double)b.capacity);
			// actually compare
			if(batcA == batcB) {
				return a.id < b.id;
			}
			else {
				return batcA < batcB;
			}
		};
	};
class CompareBatchesBy_BATCII_desc {
		public:
		double avgP;
		double t;
		double kappa;
		CompareBatchesBy_BATCII_desc(double avgP, int t, double kappa) {
			this->avgP = avgP;
			this->t = (double) t;
			this->kappa = kappa;
		};
		bool operator() (const Batch& a, const Batch& b) {
			// compute BATC-II for batch a
			double batcA = 0;
			int jMax = a.numJobs;
			for(unsigned j = 0; j < jMax; j++) {
				batcA += ((double) a.getJob(j).w / (double) a.pLot) * exp(-1* ((max<double>(((double) a.getJob(j).d - (double) a.pLot - t + (max<double>((double) a.r - t, 0))), 0)) / (kappa * avgP)));
			}
			batcA = batcA * ((double)a.w / (double)a.capacity);
			// compute BATC-II for batch b
			double batcB = 0;
			int kMax = b.numJobs;
			for(unsigned k = 0; k < kMax; k++) {
				batcB += ((double) b.getJob(k).w / (double) b.pLot) * exp(-1* ((max<double>(((double) b.getJob(k).d - (double) b.pLot - t + (max<double>((double) b.r - t, 0))), 0)) / (kappa * avgP)));
			}
			batcB = batcB * ((double)b.w / (double)b.capacity);
			if(batcA == batcB) {
				return a.id < b.id;
			}
			else {
				return batcA > batcB;
			}
		};
	};

#endif