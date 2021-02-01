#include "nondominatedset.h"
#include "tlistset.h"
#include "WCTEPCSolution.h"
#include "TWTEPCSolution.h"
#include "TWTEPC_2Solution.h"
#include "TWTEPC_3Solution.h"
#include "TWTEPC_4Solution.h"
#include "TWCEPCSolution.h"
#include "TWCEPC_2Solution.h"
#include "TWCEPC_4Solution.h"
#include "Hyb1_Solution.h"
#include "Hyb1TWC_Solution.h"
#include "nsgaiic.h"

int scheduleEEBS() {
	// set global variables of MOMH lib
	NumberOfObjectives = 2;
	Objectives.resize(NumberOfObjectives);

	Objectives[0].ObjectiveType = _Min;
	Objectives[0].bActive = true;

	Objectives[1].ObjectiveType = _Min;
	Objectives[1].bActive = true;
	
	//TNondominatedSet* pNondominatedSet0 = new TListSet < TWTEPCSolution >;

	srand(666); // time(NULL)
	TDisplayNSGAIIC< TWTEPCSolution > nsga2c;
	nsga2c.SetParameters(Global::params.popSize, Global::params.numGens, Global::params.probMut, Global::params.popGeomFactor, Global::params.scalarize);		// iPopulationSize, iNumberOfGenerations, dMutationProbability, dPopulationGeometricalFactor, bScalarize 
	nsga2c.Run();

	for(int i = 0; i < nsga2c.pNondominatedSet->size(); i++) {
		TWTEPCSolution* result0 = (TWTEPCSolution*) nsga2c.pNondominatedSet->at(i);
		std::cout << "Result - TWT = " << result0->ObjectiveValues[0] << ", EPC = " << result0->ObjectiveValues[1] << endl;
	}
	return 0;
}

BiObjectiveSolutionSet scheduleEEBS(int eRates, int timeLimit) {
	BiObjectiveSolutionSet sols;
	pair< BiObjectiveSolution, BiObjectiveSolution > point;

	// set global variables of MOMH lib
	NumberOfObjectives = 2;
	Objectives.resize(NumberOfObjectives);

	Objectives[0].ObjectiveType = _Min;
	Objectives[0].bActive = true;

	Objectives[1].ObjectiveType = _Min;
	Objectives[1].bActive = true;
	
	//cout << "BEWARE: seed fixed!" << endl;
	srand(time(NULL)); // TODO: 666 for debugging; time(NULL) for independent replications
	//TDisplayNSGAIIC< TWTEPCSolution > nsga2c;		// with console output
	TNSGAIIC< TWTEPC_4Solution > nsga2c;

	nsga2c.SetParameters(Global::params.popSize, Global::params.numGens, Global::params.probMut, Global::params.popGeomFactor, Global::params.scalarize);		// iPopulationSize, iNumberOfGenerations, dMutationProbability, dPopulationGeometricalFactor, bScalarize 
	nsga2c.Run(timeLimit);

	// TIME MEASUREMENTS FOR TRAVERSAL ALGORITHM PART 1/2
	ofstream out;
	out.open("TraversalTime.csv", std::ofstream::app);
	out << "n\t" << "T\t" << "ONVG\t" << "Runtime" << endl;
	out << Global::problem->n << "\t" << Global::problem->T << "\t" << nsga2c.pNondominatedSet->size() << "\t";
	time_t start = time(NULL);


	for(int i = 0; i < nsga2c.pNondominatedSet->size(); i++) {
		BiObjectiveSolution sol1;
		BiObjectiveSolution sol2;
		TWTEPC_4Solution* result0 = (TWTEPC_4Solution*) nsga2c.pNondominatedSet->at(i);
		sol1.objective = "TWT";
		sol1.eRates = eRates;
		sol1.value = result0->ObjectiveValues[0];
		sol2.objective = "EPC";
		sol2.eRates = eRates;
		sol2.value = result0->ObjectiveValues[1];
		std::cout << "Result - TWT = " << result0->ObjectiveValues[0] << ", EPC = " << result0->ObjectiveValues[1] << endl;
		point.first = sol1;
		point.second = sol2;
		sols.solutions.push_back(point);
		// traverse Pareto front
		// !!! TRAVERSAL DEACTIVATED !!!
		/*if(Global::gv_eRates == 1) {
		//sols.append(result0->getSchedule().getLowerEpcSolsTwc(Global::problem->e1, 1));	// TWC	ACHTUNG
			sols.append(result0->getSchedule().getLowerEpcSols(Global::problem->e1, 1));		// TWT
		} else {
			//sols.append(result0->getSchedule().getLowerEpcSolsTwc(Global::problem->e2, 2));	// TWC
			sols.append(result0->getSchedule().getLowerEpcSols(Global::problem->e2, 2));		// TWT
		} 
		*/
		
	}

	// TIME MEASUREMENTS FOR TRAVERSAL ALGORITHM PART 2/2
	out << time(NULL) - start;
	out << endl;
	out.close();
	
	sols.deleteDominated();
	return sols;
}

