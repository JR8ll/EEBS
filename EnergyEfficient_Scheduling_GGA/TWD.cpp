#include "MachineSet.h"
#include "Functions.h"

using namespace std;

BiObjectiveSolutionSet scheduleTWD(int eRates){
	BiObjectiveSolutionSet sols;
	pair< BiObjectiveSolution, BiObjectiveSolution > point;
	int deltaT;
	int thresh = 2 * Global::problem->k * Global::problem->l;

	// copy problem instance data
	MachineSet machines;
	JobSet jobs;										// jobs to be scheduled
	JobSet jobsInTw;									// jobs in time window
	BatchSet batchesInTw;								// batches composed of jobs in time window
	vector<int> scheduledIds;							// indizes of the jobs already scheduled
	int jMax = Global::problem->jobs.size();
	int mMax = Global::problem->machines.size();
	int t = 0;											// current t
	int extendDeltaT;
	int nextMachine = 0;

	// set deltaT depending on the problem instance data
	deltaT = 0;
	for(int i = 0; i < Global::problem->n; i++) {
		deltaT += Global::problem->jobs[i].p;	
	}
	deltaT = (int) (ceil ((double) deltaT / (double) Global::problem->n) / 4.0); 
	
	// try different values for parameter kappa
	for(double kappa = 0.5; kappa <= 5.0; kappa += 0.5) {
		// I. LIST SCHEDULING
		jobs = Global::problem->jobs;	
		machines = Global::problem->machines;
		scheduledIds.clear();
		extendDeltaT = 0;

		// 1. consider jobs with r <= t+deltaT, sort by ATC non-increasing
		while(scheduledIds.size() < jMax) {		// repeat until all jobs are scheduled
			// update t and nextMachine
			nextMachine = machines.getEarliestAvailableMachine();
			t = machines[nextMachine].getMSP();
			jobsInTw.clear();

			// consider the first thresh jobs within time window
			mMax = min(jobs.size(), thresh);
			for(int m = 0; m < mMax; m++) {
				if(jobs[m].r <= t + deltaT + extendDeltaT) {
					jobsInTw.addJob(&jobs[m]);
				}
			}
			jobsInTw.sortBy_ATC(t, kappa, false);
			// TODO: consider thresh jobs and form all(?) possible batches
			batchesInTw = BatchSet();
			batchesInTw.form_FF(jobsInTw, Global::problem->k);
			batchesInTw.sortBy_BATCII(t, kappa, false); // TODO true oder false?
			
			
			// schedule batch with highest BATC-II index
			if(!batchesInTw.empty()) {
				machines.listSched_ASAP(batchesInTw[0]);
				int nMax = batchesInTw[0].numJobs;
				for(int n = 0; n < nMax; n++) {
					scheduledIds.push_back(batchesInTw[0].getJob(n).id);
					jobs.erase(batchesInTw[0].getJob(n).id);
				}
			} else {
				extendDeltaT += deltaT;
			}



		} //end while
		
		// roam pareto front by right shifting batches
		if(eRates == 1) {
			sols.append(machines.getLowerEpcSols(Global::problem->e1, 1));
		} else {
			sols.append(machines.getLowerEpcSols(Global::problem->e2, 2));
		}
		
		// 2. form batches

		// 3. schedule batch with largest BATC-II

		// 4. Stop if all jobs are scheduled

		

	}

	// III. delete dominated solutions
		sols.deleteDominated();
		
	
	return sols;
}