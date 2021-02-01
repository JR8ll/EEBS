
#include "WCTEPCSolution.h"
#include "TWTEPCSolution.h"

using namespace std;

// global variables
Problem* Global::problem;
GAParameters Global::params;
int Global::gv_eRates;

int main ( int argc, char* argv[]) {

		// open output file
		char* inputFile;	
		int eRates = 1;			// 1: Winter Rates, 2: Summer Rates
		int timeLimit = 10;		// seconds
		int inPopSize = 300;
		float in_pMut = 0.01;
		BiObjectiveSolutionSet solutions;
	

		// initialize problem instance from input file (demo, if no input file available)
		Problem problem;
		if(argc > 1) {
			problem.initializeFromFile(argv[1]);
			inputFile = argv[1];
			if(argc > 2) {
				eRates = atoi(argv[2]);
				if(argc > 3) {
					timeLimit = atoi(argv[3]);
					if(argc > 4) {
						inPopSize = atoi(argv[4]);
						if(argc > 5) {
							in_pMut = atof(argv[5]);
						}
					}
				}
			}
		}
		else {
			problem.initializeFromFile("EEBS_demoProblem.dat");
			inputFile = "EEBS_demoProblem.dat";
		}
		
		// Special Case, "flattenEnergyCostProfile"
		// problem.flattenEnergyCostProfile();

		Global::problem = &problem;
		Global::gv_eRates = eRates;

		// Special Case, uniform job sizes and zero ready times: >>OPTIMAL BATCH FORMATION<<
		//Problem specialCase = problem;
		//problem.convertForSpecialCase();


		// set parameters for NSGA-II
		setParameters(inPopSize, 100, timeLimit, in_pMut, 0.5, false);
		
		// perform NSGA-II
		solutions = scheduleEEBS(eRates, timeLimit);

		// perform simple heuristic for special case of (TWC, EPC)
		//solutions = Global::problem->solveSpecialCaseTwcEpcDeterministically(eRates);

		// perform TWD
		// solutions = scheduleTWD(eRates);
		
		solutions.save(inputFile, timeLimit);
		
		return 0;
}