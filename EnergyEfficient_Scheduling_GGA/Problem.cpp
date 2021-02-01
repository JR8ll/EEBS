#include "Problem.h"
#include "Functions.h"

int Problem::count = 0;
int Problem::seed = time(NULL);

using namespace std;

// static class methods
void Problem::generateInstances(int in_seed, vector<int> nPerF, vector<int> numFamilies, vector<int> numMachines, vector<int> batchSizes, vector<float> alphas, vector<float> betas, int replications) {
	if (in_seed > 0) {
		Problem::seed = in_seed;
	}
	srand(Problem::seed);

	for (unsigned i = 0; i < nPerF.size(); i++) {
		for (unsigned j = 0; j < numFamilies.size(); j++) {
			for (unsigned k = 0; k < numMachines.size(); k++) {
				for (unsigned l = 0; l < batchSizes.size(); l++) {
					for (unsigned m = 0; m < alphas.size(); m++) {
						for (unsigned q = 0; q < betas.size(); q++) {
							for (unsigned n = 0; n < replications; n++) {
								Problem p = Problem(nPerF[i], batchSizes[l], numFamilies[j], numMachines[k], alphas[m], betas[q]);
								p.saveToFile(n + 1);
							}
						}
					}
				}
			}
		}
	}
}

// method definitions
// constructors
Problem::Problem() {}
Problem::Problem(int numJobsPerFamily, int batchSize, int numFamilies, int numMachines, float alpha, float beta) {
	
	++Problem::count;
	this->n = numJobsPerFamily * numFamilies;
	this->m = numMachines;
	this->l = numFamilies;
	this->k = batchSize;

	int pMax = 0;
	int pSum = 0;
	int rMax = 0;
	vector<int> pFamily;

	for(unsigned i = 0; i < numFamilies; i++){
		//set processing time per familiy randomly
		int pRandom = 1 + (rand() % 10);
		int myP = 0;
		if (pRandom <= 2) {
			myP = 2;
		}
		else if (pRandom <= 4) {
			myP = 4;
		}
		else if (pRandom <= 7) {
			myP = 10;
		}
		else if (pRandom <= 9) {
			myP = 16;
		}
		else if (pRandom <= 10) {
			myP = 20;
		}
		pFamily.push_back(myP);
		pSum += myP * numJobsPerFamily;
		if (myP > pMax) {
			pMax = myP;
		}
	}
	//for each job...
	for (unsigned i = 0; i < this->n; i++) {
		
		//set release date - pSum must be computed at this point
		int myR = 0;
		if (alpha > 0.01) {
			int ub_myR = (int)floor(alpha / this->k * pSum);
			myR = rand() % (ub_myR + 1);
			this->r.push_back(myR);
			if (myR > rMax) {
				rMax = myR;
			}
		}
		else {
			this->r.push_back(myR);
		}

		//set due date
		int myD = 0;
		if (beta > 0.01) {
			int ub_myD = myR + (int)floor(beta / this->k * pSum);
			myD = rand() % (ub_myD + 1);
			this->d.push_back(myD);
		}
		else {
			this->d.push_back(myD);
		}

		//set size
		this->s.push_back(1);

		//set weight randomly
		int wRandom = 1 + (rand() % 5);		// weight between 1 and 5
		this->w.push_back(wRandom);

		//set family
		int myF = (i / numJobsPerFamily) + 1;
		this->f.push_back(myF);		
		
		//set processing time per job dependent on the family
		this->p.push_back(pFamily[myF-1]);
	}

	//compute time horizon
	float tNumerator = 0;
	float tDenominator = 0;
	for (unsigned i = 0; i < numFamilies; i++) {
		tNumerator += ceil((float)numJobsPerFamily / (float) this->k) * (float) pFamily[i];
	}
	tDenominator = (float) this->m * (float) pMax;
	this->T = ceil(1.1 * (int)ceil(tNumerator / tDenominator)) * pMax + rMax;		// expand minimum time by 20%
	this->T = (this->T % 6 == 0) ? this->T : this->T + 6 -(this->T % 6);			// ceil to a multiple of six															

	//set e values for stepwise price functions
	int eOnPeak = 10;
	int ePartialPeak = 9;
	int eOffPeak = 8;
	
	//compute t values for stepwise price functions
	int tWinterOffPeak = this->T / 2;
	int tSummerPartialPeakOne = this->T / 3;
	int tSummerOffPeak = this->T / 2;
	int tSummerPartialPeakTwo = 5 * this->T / 6;

	//set second electricity price function (winter + summer rates)
	for (unsigned t = 0; t < this->T; t++) {
		//set first electricity price function (winter rates)
		if (t < tWinterOffPeak) {
			e1.push_back(eOnPeak);
		}
		else {
			e1.push_back(eOffPeak);
		}

		//set second electricity price function (summer rates)
		if (t < tSummerPartialPeakOne) {
			e2.push_back(eOnPeak);
		}
		else if (t < tSummerOffPeak) {
			e2.push_back(ePartialPeak);
		}
		else if (t < tSummerPartialPeakTwo) {
			e2.push_back(eOffPeak);
		}
		else {
			e2.push_back(ePartialPeak);
		}
	}



	
}


/// misc
void Problem::printObjectives() {
	cout << "TCT:  " << this->getTCT() << " \t(Total completion time)" << endl;
	cout << "TWCT: " << this->getTWCT()<< " \t(Total weighted completion time)" << endl;
	cout << "TT:   " << this->getTT()<< " \t(Total tardiness)" << endl;
	cout << "TWT:  " << this->getTWT()<< " \t(Total weighted tardiness)" << endl;
	cout << "MSP:  " << this->getMSP() << " \t(Makespan)" << endl;
	cout << "EPC:  " << this->getEPC() << " \t(Electric power cost)" << endl;
};
bool Problem::hasUniformS() {
	if (this->s.empty()) {
		return false;	// return false if there are no jobs
	}
	else {
		int firstS = this->s[0];
		for (unsigned i = 1; i < this->s.size(); i++) {
			if (this->s[i] != firstS) {
				return false;  // return false if there are two jobs with different sizes
			}
		}
		return true;	// return true if there are jobs, and all sizes are equal
	}
}
bool Problem::hasUniformW() {
	if (this->w.empty()) {
		return false;	// return false if there are no jobs
	}
	else {
		int firstW = this->w[0];
		for (unsigned i = 1; i < this->w.size(); i++) {
			if (this->w[i] != firstW) {
				return false;  // return false if there are two jobs with different weights
			}
		}
		return true;	// return true if there are jobs, and all weights are equal
	}
}
bool Problem::hasUniformP() {
	if (this->p.empty()) {
		return false;	// return false if there are no jobs
	}
	else {
		int firstP = this->p[0];
		for (unsigned i = 1; i < this->p.size(); i++) {
			if (this->p[i] != firstP) {
				return false;  // return false if there are two jobs with different processing times
			}
		}
		return true;	// return true if there are jobs, and all processing times are equal
	}
}
bool Problem::hasSingleF() {
	if (this->f.empty()) {
		return false;	// return false if there are no jobs
	}
	else {
		int firstF = this->f[0];
		for (unsigned i = 1; i < this->f.size(); i++) {
			if (this->f[i] != firstF) {
				return false;  // return false if there are two jobs with a different family
			}
		}
		return true;	// return true if there are jobs, and all belong to the same family
	}
}
bool Problem::hasUniformR(){
	if (this->r.empty()) {
		return false;	// return false if there are no jobs
	}
	else {
		int firstR = this->r[0];
		for (unsigned i = 1; i < this->r.size(); i++) {
			if (this->r[i] != firstR) {
				return false;  // return false if there are two jobs with a different family
			}
		}
		return true;	// return true if there are jobs, and all belong to the same family
	}
}
/// Initialization
bool Problem::initializeFromFile(char* file) {
	ifstream input(file);
	if(!input) {
		cerr << file << " not found." << endl;
		return 0;
	}
	else {
		string dummy;
		//set parameters
		input >> dummy;			// "seed="
		input >> this->seed;
		input >> dummy;			// ";"
		input >> dummy;			// "N="
		input >> this->n;
		input >> dummy;			// ";"
		input >> dummy;			// "M="
		input >> this->m;
		input >> dummy;			// ";"
		input >> dummy;			// "F="
		input >> this->l;
		input >> dummy;			// ";"
		input >> dummy;			// "K="
		input >> this->k;
		input >> dummy;			// ";"
		input >> dummy;			// "T="
		input >> this->T;
		input >> dummy;			// ";"

		getline(input, dummy);	// empty line

		// processing times
		input >> dummy;			// "p=["
		int p;
		int pSum = 0;
		for(int j = 0; j < this->n; j++) {
			input >> p;
			pSum += p;
			this->p.push_back(p);
			this->pt.push_back(p);
		}
		input >> dummy;			// "];"

		// ready times
		input >> dummy;			// "r=["
		int r;
		int rMax = 0;
		for(int j = 0; j < this->n; j++) {
			input >> r;
			this->r.push_back(r);
			this->st.push_back(r);
			if(r > rMax) rMax = r;
		}
		input >> dummy;			// "];"

		// due dates
		input >> dummy;			// "d=["
		int d;
		for(int j = 0; j < this->n; j++) {
			input >> d;
			this->d.push_back(d);
		}
		input >> dummy;			// "];"

		// sizes
		input >> dummy;			// "s=["
		int s;
		int sSum = 0;
		for(int j = 0; j < this->n; j++) {
			input >> s;
			sSum += s;
			this->s.push_back(s);
		}
		input >> dummy;			// "];"

		// weights
		input >> dummy;			// "w=["
		int w;
		for(int j = 0; j < this->n; j++) {
			input >> w;
			this->w.push_back(w);
		}
		input >> dummy;			// "];"

		// families
		input >> dummy;			// "f=["
		int f;
		for(int j = 0; j < this->n; j++) {
			input >> f;
			this->f.push_back(f);
		}
		input >> dummy;			// "];"

		getline(input, dummy);	// empty line

		// 1st electricity price
		input >> dummy;			// "e1=["
		
		int e;
		for(int t = 0; t < this->T; t++) {
			input >> e;
			this->e1.push_back(e);
		}
		input >> dummy;			// "];"

		// 2nd electricity price
		input >> dummy;			// "e2=["
		for (int t = 0; t < this->T; t++) {
			input >> e;
			this->e2.push_back(e);
		}
		input >> dummy;			// "];"

		this->q = pSum + rMax + this->T;	

		cout << "Problem instance from file " << file << " initialized." << endl << endl;
		// TODO initialize global variables


		this->initializeJobSet();
		this->initializeMachineSet(T);

		return 1;
	};	
}


bool Problem::initializeMachineSet() {
	this->machines.clear();
	for(unsigned i = 0; i < this->m; i++) {
		Machine newMachine;
		this->machines.addMachine(newMachine);
	}
	return true;
	// TODO exception handling
}

bool Problem::initializeMachineSet(int T) {
	this->machines.clear();
	for(unsigned i = 0; i < this->m; i++) {
		Machine newMachine = Machine(T);
		this->machines.addMachine(newMachine);
	}
	return true;
	// TODO exception handling
}
bool Problem::initializeBatchSet() {
	this->batches.clear();
	for(unsigned i = 0; i < this->n; i++) {
		Batch newBatch(this->k);
		this->batches.addBatch(newBatch);
	}
	return true;
	// TODO exception handling
}

bool Problem::initializeBatchSet(int numBatches) {
	this->batches.clear();
	for(unsigned i = 0; i < numBatches; i++) {
		Batch newBatch(this->k);
		this->batches.addBatch(newBatch);
	}
	return true;
	// TODO exception handling
}

bool Problem::initializeJobSet() {
	this->jobs.clear();
	for(unsigned i = 0; i < this->n; i++) {
		Job* newJob;
		newJob = new Job(i+1, this->p[i], this->d[i], this->r[i], this->s[i], this->w[i], this->f[i]);
		this->jobs.addJob(newJob);
	}
	return true;
	// TODO exception handling
}


void Problem::saveToFile(int replication) {

	string path = "pInstances//";
	string file = "EEBS";
	
	int alph = (int)(this->rAlpha * 100.0);
	int bet = (int)(this->dBeta * 100.0);

	ostringstream os;
	os << file << "_n" << this->n << "_F" << this->l << "_M" << this->m << "_B" << this->m << "_a0." << alph << "_b0." << bet << "-" << replication << ".dat";
	string filename = os.str();
	ofstream out;

	ostringstream fs;
	fs << path << filename;
	string fstr = fs.str();
	out.open(fstr.c_str());

	out << "seed= " << Problem::seed << " ;" << endl;
	out << "N= " << this->n << " ;" << endl;
	out << "M= " << this->m << " ;" << endl;
	out << "F= " << this->l << " ;" << endl;
	out << "K= " << this->k << " ;" << endl;
	out << "T= " << this->T << " ;" << endl;
	out << endl;
	out << "p=[";
	for (unsigned i = 0; i < this->n; i++) {
		out << " " << this->p[i];
	}
	out << " ];" << endl;
	out << "r=[";
	for (unsigned i = 0; i < this->n; i++) {
		out << " " << this->r[i];
	}
	out << " ];" << endl;
	out << "d=[";
	for (unsigned i = 0; i < this->n; i++) {
		out << " " << this->d[i];
	}
	out << " ];" << endl;
	out << "s=[";
	for (unsigned i = 0; i < this->n; i++) {
		out << " " << this->s[i];
	}
	out << " ];" << endl;
	out << "w=[";
	for (unsigned i = 0; i < this->n; i++) {
		out << " " << this->w[i];
	}
	out << " ];" << endl;
	out << "f=[";
	for (unsigned i = 0; i < this->n; i++) {
		out << " " << this->f[i];
	}
	out << " ];" << endl;
	out << endl;
	out << "e1=[";
	for (unsigned t = 0; t < this->T; t++) {
		out << " " << e1[t];
	}
	out << " ];" << endl;
	out << "e2=[";
	for (unsigned t = 0; t < this->T; t++) {
		out << " " << e2[t];
	}
	out << " ];";
	out.close();
}
void Problem::convertForSpecialCase(){
	JobSet myJobs = this->jobs;
	BatchSet myBatches;

	// Sort Jobs by non-increasing weight
	myJobs.sortBy_w(false);

	// Form Batches First-Fit
	myBatches.form_FF(myJobs, this->k);

	// Sort Batches by WSPT
	myBatches.sortBy_wpLot(false);

	this->n = myBatches.size();		// batches have become the entities to be scheduled
	
	this->k = 1;	// Batch formation subproblem has been optimally solved 

	this->p.clear();
	this->r.clear();
	this->d.clear();
	this->s.clear();
	this->w.clear();
	this->f.clear();

	this->jobs.clear();

	for(int i = 0; i < this->n; i++) {
		p.push_back(myBatches[i].pLot);
		r.push_back(0);
		d.push_back(0);			// TWC
		s.push_back(1);
		w.push_back(myBatches[i].w);
		f.push_back(myBatches[i].f);
	}
	
	this->initializeJobSet();
	
	


}
void Problem::flattenEnergyCostProfile() {
	for (int t = 0; t < this->T; t++) {
		this->e1[t] = 1;			
		this->e2[t] = 1;
		}
}
/// Job2Batch assignment
BatchSet& Problem::formBatches_FFDn() {
	this->formBatches_FFDn(this->jobs);
	return this->batches;
}
BatchSet& Problem::formBatches_FFDn(vector<Job>& in_jobs) {
	this->jobs.sortBy_f(true);
	this->batches.clear();
	Batch newBatch = Batch(this->k);
	int jMax = in_jobs.size();
	for(unsigned i = 0; i < jMax; i++) {
		if(!newBatch.addJob(&in_jobs[i])){
			this->batches.addBatch(newBatch);
			newBatch = Batch(this->k);			// initialize a new batch
			newBatch.addJob(&in_jobs[i]);
		}
	}
	this->batches.addBatch(newBatch);
	return this->batches;
}
BatchSet& Problem::formBatches_FFDn(JobSet& in_jobSet) {
	this->jobs.sortBy_f(true);
	this->batches.clear();
	Batch newBatch = Batch(this->k);
	int jMax = in_jobSet.size();
	for(unsigned i = 0; i < jMax; i++) {
		if(!newBatch.addJob(&in_jobSet[i])){
			this->batches.addBatch(newBatch);
			newBatch = Batch(this->k);			// initialize a new batch
			newBatch.addJob(&in_jobSet[i]);
		}
	}
	this->batches.addBatch(newBatch);
	return this->batches;
}

BatchSet& Problem::formBatches_FFD1(vector<Job>& jobs) {
	// TODO: implement
	return this->batches;
}
BatchSet& Problem::formBatches_FFD1(JobSet& jobs) {
	// TODO: implement
	return this->batches;
}

BatchSet& Problem::formBatches_FFBF(vector<Job>& jobs) {
	// TODO: implement
	return this->batches;
}
BatchSet& Problem::formBatches_FFBF(JobSet& jobs) {
	// TODO: implement
	return this->batches;
}

BatchSet& Problem::formBatches_TWD(int deltaT) {
	this->formBatches_TWD(this->jobs, deltaT, 1.5);
	return this->batches;
}
BatchSet& Problem::formBatches_TWD(int deltaT, double kappa) {
	this->formBatches_TWD(this->jobs, deltaT, kappa);
	return this->batches;
}
BatchSet& Problem::formBatches_TWD(JobSet& jobs, int deltaT, double kappa) {
	this->formBatches_TWD(jobs, deltaT, kappa);
	return this->batches;
}

/// Batch2Machine assignemnt
void Problem::listSched(){
}

/// Objective function values
double Problem::getTCT() {									// total completion time
	return this->machines.getTCT();
}
double Problem::getTWCT() {									// total weighted completion time
	return this->machines.getTWC();
}
double Problem::getTT() {									// total tardiness
	return this->machines.getTT();
}	
double Problem::getTWT() {									// total weighted tardiness
	return this->machines.getTWT();
}		
double Problem::getMSP() {									// makespan
	return this->machines.getMSP();
}			
double Problem::getEPC() {									// electricity power cost
	return this->machines.getEPC(this->e1);
}
int Problem::getLowerBoundEPC(){
	/* this lower bound is computed makin the assumptions that
	a. no batch capacity is spoiled and the minimum number of batches per family is used
	b. all off-peak price periods can be used
	*/
	vector<int> sumSizePerFam;
	vector<int> pPerFam;
	int sumP;
	int sumEPC;
	vector<int> prices;

	for(int i = 0; i < this->l; i++) {
		sumSizePerFam.push_back(0);
		pPerFam.push_back(0);
	}
	
	for(int j = 0; j < this->n; j++) {
		sumSizePerFam[this->f[j]-1] += this->s[j]; 
		pPerFam[this->f[j]-1] = this->p[j];
	}

	sumP = 0;
	for(int i = 0; i < this->l; i++) {
		sumP += (ceil( (double) sumSizePerFam[i] / (double) this->k) ) * pPerFam[i]; 
	}

	if(Global::gv_eRates == 1) {
		for(int t = 0; t < this->T; t++) {
			for(int m = 0; m < this->m; m++) {
				prices.push_back(this->e1[t]);
			}
		}
	} else {
		for(int t = 0; t < this->T; t++) {
			for(int m = 0; m < this->m; m++) {
				prices.push_back(this->e2[t]);
			}
		}
	}

	sort(prices.begin(), prices.end());

	sumEPC = 0;
	for(int t = 0; t < sumP; t++) {
		sumEPC += prices[t];
	}
	
	return sumEPC;

}
/// Solver
bool Problem::solveTWD(int deltaT, double kappa){
	return this->solveTWD(this->jobs, deltaT, kappa);
}

bool Problem::solveTWD(JobSet& jobs, int deltaT, double kappa){
	// local variables	
	vector<Job*> jobPtrList;

	this->jobs.sortBy_f(true);

	// TODO implement
	return false;
}
BiObjectiveSolutionSet Problem::solveSpecialCaseTwcEpcDeterministically(int eRates){
	BiObjectiveSolutionSet sols;
	JobSet myJobs = this->jobs;
	BatchSet myBatches;
	MachineSet myMachines = this->machines;


	// TODO: implement

	// Sort Jobs by non-increasing weight
	myJobs.sortBy_w(false);

	// Form Batches First-Fit
	myBatches.form_FF(myJobs, this->k);

	// Sort Batches by WSPT
	myBatches.sortBy_wpLot(false);

	// List-Schedule Batches
	int bMax = myBatches.size();
	for(int i = 0; i < bMax; i++) {
		myMachines.listSched_ASAP(myBatches[i]);
	}

	// traverse front of non-dominated solutions by right shifting batches
	if(eRates == 1) {
		sols = myMachines.getLowerEpcSolsTwc(this->e1, 1);
	} else {
		sols = myMachines.getLowerEpcSolsTwc(this->e2, 2);
	}

	return sols;
}