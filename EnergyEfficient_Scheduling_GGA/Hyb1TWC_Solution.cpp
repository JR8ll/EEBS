#include "Hyb1TWC_Solution.h"

Hyb1TWC_Solution::Hyb1TWC_Solution() {
	ObjectiveValues.resize(2);	
	this->_key = ((double) rand() / ((double) RAND_MAX + 1.0));
	if(this->_key < 0.5) {
		this->_sol = new TWCEPCSolution();
	} else {
		this->_sol = new TWCEPC_4Solution();
	}
	this->setObjectiveValues();
	this->_valid = this->_sol->isValid();
} 

Hyb1TWC_Solution::Hyb1TWC_Solution(Hyb1TWC_Solution& p1, Hyb1TWC_Solution& p2){
// Recombination (Crossover)
	ObjectiveValues.resize(2);
	this->_key = (p1._key + p2._key) / 2.0;	// provisional, TODO: implement actual crossover
	if(this->_key < 0.5) {
		// create new TWCEPCSolution
		TWCEPCSolution* par1 = new TWCEPCSolution(p1._sol->getChromosome(), p1._sol->getSchedule(), p1._sol->getTwcPref(), p1._sol->getValid(), p1._sol->getDelayT());
		TWCEPCSolution* par2 = new TWCEPCSolution(p2._sol->getChromosome(), p2._sol->getSchedule(), p2._sol->getTwcPref(), p2._sol->getValid(), p2._sol->getDelayT());
		this->_sol = new TWCEPCSolution(*par1, *par2);
		delete par1;
		delete par2;
	} else {
		// create new TWCEPC_4Solution
		TWCEPC_4Solution* par1 = new TWCEPC_4Solution(p1._sol->getChromosome(), p1._sol->getSchedule(), p1._sol->getTwcPref(), p1._sol->getValid(), p1._sol->getDelayT());
		TWCEPC_4Solution* par2 = new TWCEPC_4Solution(p2._sol->getChromosome(), p2._sol->getSchedule(), p2._sol->getTwcPref(), p2._sol->getValid(), p2._sol->getDelayT());
		this->_sol = new TWCEPC_4Solution(*par1, *par2);
		delete par1;
		delete par2;
	}
	this->setObjectiveValues();
	this->_valid = this->_sol->isValid();
}					
Hyb1TWC_Solution::Hyb1TWC_Solution(Hyb1TWC_Solution& p1, Hyb1TWC_Solution& p2, bool nonDelay){
// Recombination (Crossover)
	ObjectiveValues.resize(2);
	this->_key = (p1._key + p2._key) / 2.0;	// provisional, TODO: implement actual crossover
	if(this->_key < 0.5) {
		// create new TWCEPCSolution
		TWCEPCSolution* par1 = new TWCEPCSolution(p1._sol->getChromosome(), p1._sol->getSchedule(), p1._sol->getTwcPref(), p1._sol->getValid(), p1._sol->getDelayT());
		TWCEPCSolution* par2 = new TWCEPCSolution(p2._sol->getChromosome(), p2._sol->getSchedule(), p2._sol->getTwcPref(), p2._sol->getValid(), p2._sol->getDelayT());
		this->_sol = new TWCEPCSolution(*par1, *par2, nonDelay);
		delete par1;
		delete par2;
	} else {
		// create new TWCEPC_4Solution
		TWCEPC_4Solution* par1 = new TWCEPC_4Solution(p1._sol->getChromosome(), p1._sol->getSchedule(), p1._sol->getTwcPref(), p1._sol->getValid(), p1._sol->getDelayT());
		TWCEPC_4Solution* par2 = new TWCEPC_4Solution(p2._sol->getChromosome(), p2._sol->getSchedule(), p2._sol->getTwcPref(), p2._sol->getValid(), p2._sol->getDelayT());
		this->_sol = new TWCEPC_4Solution(*par1, *par2, nonDelay);
		delete par1;
		delete par2;
	}
	this->setObjectiveValues();
	this->_valid = this->_sol->isValid();
}	
Hyb1TWC_Solution::Hyb1TWC_Solution(Hyb1TWC_Solution& solution1){
// Copy constructor
	ObjectiveValues.resize(2);
	// copy solution attributes
	GroupingGenome chr = solution1._sol->getChromosome();
	MachineSet sched = solution1._sol->getSchedule();
	float pref = solution1._sol->getTwcPref();
	bool val = solution1._sol->getValid();
	vector<double> delays = solution1._sol->getDelayT();

	this->_key = solution1._key;
	// create new (copy) solution object
	if(this->_key < 0.5) {
		this->_sol = new TWCEPCSolution(chr, sched, pref, val, delays);
	} else {
		this->_sol = new TWCEPC_4Solution(chr, sched, pref, val, delays);
	}

	//this->setObjectiveValues();
	this->ObjectiveValues[0] = solution1.ObjectiveValues[0];
	this->ObjectiveValues[1] = solution1.ObjectiveValues[1];
	this->_valid = this->_sol->isValid();
}										
Hyb1TWC_Solution::~Hyb1TWC_Solution(){
	delete this->_sol;
}

void Hyb1TWC_Solution::Mutate(){
	
	GroupingGenome chr = this->_sol->getChromosome();
	MachineSet sched = this->_sol->getSchedule();
	float pref = this->_sol->getTwcPref();
	bool val = this->_sol->getValid();
	vector<double> delays = this->_sol->getDelayT();
	delete this->_sol;

	if(this->_key < 0.5) {
		this->_key += 0.5;
		this->_sol = new TWCEPC_4Solution(chr, sched, pref, val, delays);
	} else {
		this->_key -= 0.5;
		this->_sol = new TWCEPCSolution(chr, sched, pref, val, delays);
	}
	this->setObjectiveValues();
	this->_valid = this->_sol->isValid();
}



bool Hyb1TWC_Solution::isValid() {
	return this->_sol->isValid();
}



void Hyb1TWC_Solution::setObjectiveValues(){
	this->ObjectiveValues[0] = this->_sol->ObjectiveValues[0];
	this->ObjectiveValues[1] = this->_sol->ObjectiveValues[1];
}