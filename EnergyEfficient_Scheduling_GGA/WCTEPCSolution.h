#ifndef WCTEPCSOLUTION_H
#define WCTEPCSOLUTION_H

#include "momhsolution.h"
#include "MachineSet.h"
#include "GroupingGenome.h"

class WCTEPCSolution : public TMOMHSolution {
public:
	WCTEPCSolution();							// standard constructor => random initialization of one individual
	WCTEPCSolution(double wct, double epc);		// obsolete
	WCTEPCSolution(WCTEPCSolution & parent1, WCTEPCSolution & parent2);		// Recombination (Crossover)
	WCTEPCSolution(WCTEPCSolution& solution1);	// copy construcctor
	~WCTEPCSolution();						

	double getWCT();
	double getEPC();

	

private:
	// Objective Values
	double _wct;
	double _epc;

	// Chromosome
	GroupingGenome _chromosome; 	
	

};

#endif