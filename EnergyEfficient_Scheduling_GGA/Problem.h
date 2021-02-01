#ifndef PROBLEM_H
#define PROBLEM_H

#include "MachineSet.h"

using namespace std;

class Problem {
	
public:
	static int count;
	static int seed;


	// attributes´ declaration
	MachineSet machines;
	BatchSet batches;
	JobSet jobs;
	//int seed;			// seed used for creation
	int n;				// number of jobs
	int m;				// number of machines
	int l;				// number of families
	int k;				// batch capacity
	int T;				// time horizon
	int q;				// penalty term (large integer)
	
	float rAlpha;		// alpha factor for computation of r
	float dBeta;		// beta factor for computation of d

	vector<int> p;		// job processing times
	vector<int> d;		// job due dates
	vector<int> r;		// job release dates
	vector<int> s;		// job sizes
	vector<int> w;		// job weights
	vector<int> f;		// job families

	vector<int> pt;		// batch processing times
	vector<int> st;		// batch starting times

	vector<int> e1;
	vector<int> e2;

	static void generateInstances(int seed, vector<int> nPerF, vector<int> numFamilies, vector<int> numMachines, vector<int> batchSizes, vector<float> alphas, vector<float> betas, int replications);

	Problem();
	Problem(int numJobsPerFamily, int batchSize, int numFamilies, int numMachines, float alpha, float beta);


	// methods´ declaration
	/// mics
	void printObjectives();									// write all objective values in console
	bool hasUniformS();
	bool hasUniformW();
	bool hasUniformP();
	bool hasSingleF();
	bool hasUniformR();

	/// Initialization
	bool initializeFromFile (char* file);
	bool initializeMachineSet();
	bool initializeMachineSet(int T);
	bool initializeBatchSet();								// initialize a set of N batches (one for each job)
	bool initializeBatchSet(int numBatches);				// initialize a set of numBatches batches
	bool initializeJobSet();
	void saveToFile(int replication);
	void convertForSpecialCase();
	void flattenEnergyCostProfile();						// sets the energy prices to 1 for all periods
	
	

	/// Job2Batch assignment
	BatchSet& formBatches_FFDn();							// from the problem´s own jobset
	BatchSet& formBatches_FFDn(vector<Job>& jobs);
	BatchSet& formBatches_FFDn(JobSet& jobs);

	BatchSet& formBatches_FFD1();							// from the problem´s own jobset
	BatchSet& formBatches_FFD1(vector<Job>& jobs);
	BatchSet& formBatches_FFD1(JobSet& jobs);

	BatchSet& formBatches_FFBF();							// from the problem´s own jobset
	BatchSet& formBatches_FFBF(vector<Job>& jobs);
	BatchSet& formBatches_FFBF(JobSet& jobs);
	BatchSet& formBatches_TWD(int deltaT);
	BatchSet& formBatches_TWD(int deltaT, double kappa);
	BatchSet& formBatches_TWD(JobSet& jobs, int deltaT, double kappa);	// time window decomposition heuristic

	/// Batch2Machine assignment
	void listSched();

	/// Objective function values
	double getTCT();									// total completion time
	double getTWCT();									// total weighted completion time
	double getTT();										// total tardiness
	double getTWT();									// total weighted tardiness
	double getMSP();									// makespan
	double getEPC();									// electricity power cost
	int getLowerBoundEPC();

	/// Solver
	bool solveTWD(int deltaT, double kappa);									// time window decomposition
	bool solveTWD(JobSet& jobs, int deltaT, double kappa);
	BiObjectiveSolutionSet solveSpecialCaseTwcEpcDeterministically(int eRates);
};

#endif