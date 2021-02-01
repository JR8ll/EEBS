#ifndef GROUPINGGENOME_H
#define GROUPINGGENOME_H

//GALib
#include "MachineSet.h"
#include "Functions.h"

class GroupingGenome : public vector<Batch *> {

public:
	GroupingGenome();
	GroupingGenome(const GroupingGenome &other);
	~GroupingGenome();

	void init();
	void destroy();
	void copy(const GroupingGenome *other);
	void clean();									// Initialize all Genes (NOT clear()
	void operator=(const GroupingGenome &other);
	int firstEmpty(const bool from_start);
	int predecessorOnMachine(int batchIdx);

	// Initialization is done in ***Solution classes
	void initializeRandom();
	void initializeEDD();
	void initializeTWD();

	// Mutation
	void mutShift(const float prob);
	void mutSwap(const float prob);

	// Refinement
	bool shiftJobsForDominance(const int if_scr, const int if_dst);
	bool shiftJobsTWT(const int srcBatchIdx, const int dstBatchIdx);		// shift jobs from src to dst ordered by weight, only shift if batch´s r is not increased
	bool shiftJobsEPC(const int srcBatchIdx, const int dstBatchIdx);		// shift jobs from src to dst ordered by size
	bool shiftJobsTWC(const int srcBatchIdx, const int dstBatchIdx);		// shift jobs from src to dst ordered by weight, only shift if batch´s r is not increased

	bool swapJobsPossible(const int fidx1, const int jobId1, const int fidx2, const int jobId2);

	/// sorting
	void sortBy_r(bool asc);
	void sortBy_pLot(bool asc);
	void sortBy_pItem(bool asc);
	void sortBy_w(bool asc);
	void sortBy_wpLot(bool asc);
	void sortBy_wpItem(bool asc);
	void sortBy_BATCII(int time, double kappa, bool asc);
	void moveNonEmptyBatchesToFront();
	bool reinsert(vector<int> &missingJobs);				// reinsert first-fit
	bool reinsertTWTEPC_2(vector<int> &missingJobs);		// reinsertion suited to TWTEPC_2Solution
	bool reinsertTWTEPC_3(vector<int> &missingJobs);		// reinsertion suited to TWTEPC_3Solution
	bool reinsertTWCEPC_2(vector<int> &missingJobs);		// reinsertion suited to TWCEPC_2Solution
	bool reinsertReady(vector<int> &missingJobs);			// reinsert considering ready times
	bool reinsertDue(vector<int> &missingJobs);				// reinsert considering due dates
	bool reinsertReadyDue(vector<int> &missingJobs);		// reinsert considering ready times and due dates
	bool reinsertReadyDueWeight(vector<int> &missingJobs);	// reinsert considering ready times, due dates and weight
	bool reinsertMinDeltaTWT(vector<int> &missingJobs);		// reinsert increasing TWT as little as possible
	bool reinsertTWT(vector<int> &missingJobs);				// reinsert so that TWT of accepting batch does not increase
	bool reinsertTWC(vector<int> &missingJobs);				// reinsert so that TWC of accepting batch does not increase
	bool reinsertBATC(vector<int> &missingJobs);			// reinsert so the BATC values of utilized batches are not increased


	/// compare ascending
	static bool compareBy_r(const Batch* a, const Batch* b);
	static bool compareBy_pLot(const Batch* a, const Batch* b);
	static bool compareBy_pItem(const Batch* a, const Batch* b);
	static bool compareBy_w(const Batch* a, const Batch* b);
	static bool compareBy_wpLot(const Batch* a, const Batch* b);
	static bool compareBy_wpItem(const Batch* a, const Batch* b);
	static bool compareEmpty(const Batch* a, const Batch* b);
	
	/// compare descending
	static bool compareBy_r_desc(const Batch* a, const Batch* b);
	static bool compareBy_pLot_desc(const Batch* a, const Batch* b);
	static bool compareBy_pItem_desc(const Batch* a, const Batch* b);
	static bool compareBy_w_desc(const Batch* a, const Batch* b);
	static bool compareBy_wpLot_desc(const Batch* a, const Batch* b);
	static bool compareBy_wpItem_desc(const Batch* a, const Batch* b);
};

class CompareBy_BATCII {
	public:
		double avgP;
		double t;
		double kappa;
		CompareBy_BATCII(double avgP, int t, double kappa) {
			this->avgP = avgP;
			this->t = (double) t;
			this->kappa = kappa;
		};
		bool operator() (const Batch* a, const Batch* b) {
			// compute BATC-II for batch a
			double batcA = 0;
			int jMax = a->numJobs;
			for(unsigned j = 0; j < jMax; j++) {
				batcA += ((double) a->getJob(j).w / (double) a->pLot) * exp(-1.0* ((max<double>(((double) a->getJob(j).d - (double) a->pLot - t + (max<double>((double) a->r - t, 0))), 0)) / (kappa * avgP)));
			}
			batcA = batcA * ((double)a->w / (double)a->capacity);
			// compute BATC-II for batch b
			double batcB = 0;
			int kMax = b->numJobs;
			for(unsigned k = 0; k < kMax; k++) {
				batcB += ((double) b->getJob(k).w / (double) b->pLot) * exp(-1.0* ((max<double>(((double) b->getJob(k).d - (double) b->pLot - t + (max<double>((double) b->r - t, 0))), 0)) / (kappa * avgP)));
			}
			batcB = batcB * ((double)b->w / (double)b->capacity);
			// actually compare
			if(batcA == batcB) {
				return a->id < b->id;
			}
			else {
				return batcA < batcB;
			}
		};
	};
class CompareBy_BATCII_desc {
		public:
		double avgP;
		double t;
		double kappa;
		CompareBy_BATCII_desc(double avgP, int t, double kappa) {
			this->avgP = avgP;
			this->t = (double) t;
			this->kappa = kappa;
		};
		bool operator() (const Batch* a, const Batch* b) {
			// compute BATC-II for batch a
			double batcA = 0;
			int jMax = a->numJobs;
			for(unsigned j = 0; j < jMax; j++) {
				batcA += ((double) a->getJob(j).w / (double) a->pLot) * exp(-1* ((max<double>(((double) a->getJob(j).d - (double) a->pLot - t + (max<double>((double) a->r - t, 0))), 0)) / (kappa * avgP)));
			}
			batcA = batcA * ((double)a->w / (double)a->capacity);
			// compute BATC-II for batch b
			double batcB = 0;
			int kMax = b->numJobs;
			for(unsigned k = 0; k < kMax; k++) {
				batcB += ((double) b->getJob(k).w / (double) b->pLot) * exp(-1* ((max<double>(((double) b->getJob(k).d - (double) b->pLot - t + (max<double>((double) b->r - t, 0))), 0)) / (kappa * avgP)));
			}
			batcB = batcB * ((double)b->w / (double)b->capacity);
			if(batcA == batcB) {
				return a->id < b->id;
			}
			else {
				return batcA > batcB;
			}
		};
	};



void sortBy_BATCII(int time, double kappa, bool asc);





bool compareJobWeight(const int JobIdx1, const int JobIdx2);
bool compareJobSize(const int JobIdx1, const int JobIdx2);

#endif // GROUPINGGENOME_H