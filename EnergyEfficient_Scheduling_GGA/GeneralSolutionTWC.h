// provides an interface for solutions used in hybrid solution

#ifndef GENERALSOLUTIONTWC_H
#define GENERALSOLUTIONTWC_H

#include "momhsolution.h"
#include "GroupingGenome.h"

class GeneralSolutionTWC : public TMOMHSolution {

public: 

	virtual bool isValid() = 0;	// pure virtual (abstract) function 
	virtual double getTWC() = 0;
	virtual double getEPC() = 0;
	virtual bool decodeChromosome() = 0;

	// getters for member attributes
	virtual GroupingGenome getChromosome() = 0;
	virtual MachineSet getSchedule() = 0;
	virtual float getTwcPref() = 0;
	virtual bool getValid() = 0;
	virtual vector<double> getDelayT() = 0;

	// setters for member attributesb
	virtual void setChromosome(GroupingGenome chr) = 0;
	virtual void setSchedule(MachineSet sched) = 0;
	virtual void setTwcPref(float pref) = 0;
	virtual void setValid(bool val) = 0;
	virtual void setDelayT(vector<double> delays) = 0;
};


#endif