#include "Hyb1_Solution.h"

Hyb1_Solution::Hyb1_Solution() {
	ObjectiveValues.resize(2);	
	this->_key = ((double) rand() / ((double) RAND_MAX + 1.0));
	if(this->_key < 0.5) {
		this->_sol = new TWTEPCSolution();
	} else {
		this->_sol = new TWTEPC_4Solution();
	}
	this->setObjectiveValues();
	this->_valid = this->_sol->isValid();
} 

Hyb1_Solution::Hyb1_Solution(Hyb1_Solution& p1, Hyb1_Solution& p2){
// Recombination (Crossover)
	ObjectiveValues.resize(2);
	this->_key = (p1._key + p2._key) / 2.0;	// provisional, TODO: implement actual crossover
	if(this->_key < 0.5) {
		// create new TWTEPCSolution
		TWTEPCSolution* par1 = new TWTEPCSolution(p1._sol->getChromosome(), p1._sol->getSchedule(), p1._sol->getTwtPref(), p1._sol->getValid(), p1._sol->getDelayT());
		TWTEPCSolution* par2 = new TWTEPCSolution(p2._sol->getChromosome(), p2._sol->getSchedule(), p2._sol->getTwtPref(), p2._sol->getValid(), p2._sol->getDelayT());
		this->_sol = new TWTEPCSolution(*par1, *par2);
		delete par1;
		delete par2;
	} else {
		// create new TWTEPC_4Solution
		TWTEPC_4Solution* par1 = new TWTEPC_4Solution(p1._sol->getChromosome(), p1._sol->getSchedule(), p1._sol->getTwtPref(), p1._sol->getValid(), p1._sol->getDelayT());
		TWTEPC_4Solution* par2 = new TWTEPC_4Solution(p2._sol->getChromosome(), p2._sol->getSchedule(), p2._sol->getTwtPref(), p2._sol->getValid(), p2._sol->getDelayT());
		this->_sol = new TWTEPC_4Solution(*par1, *par2);
		delete par1;
		delete par2;
	}
	this->setObjectiveValues();
	this->_valid = this->_sol->isValid();
}					
Hyb1_Solution::Hyb1_Solution(Hyb1_Solution& p1, Hyb1_Solution& p2, bool nonDelay){
// Recombination (Crossover)
	ObjectiveValues.resize(2);
	this->_key = (p1._key + p2._key) / 2.0;	// provisional, TODO: implement actual crossover
	if(this->_key < 0.5) {
		// create new TWTEPCSolution
		TWTEPCSolution* par1 = new TWTEPCSolution(p1._sol->getChromosome(), p1._sol->getSchedule(), p1._sol->getTwtPref(), p1._sol->getValid(), p1._sol->getDelayT());
		TWTEPCSolution* par2 = new TWTEPCSolution(p2._sol->getChromosome(), p2._sol->getSchedule(), p2._sol->getTwtPref(), p2._sol->getValid(), p2._sol->getDelayT());
		this->_sol = new TWTEPCSolution(*par1, *par2, nonDelay);
		delete par1;
		delete par2;
	} else {
		// create new TWTEPC_4Solution
		TWTEPC_4Solution* par1 = new TWTEPC_4Solution(p1._sol->getChromosome(), p1._sol->getSchedule(), p1._sol->getTwtPref(), p1._sol->getValid(), p1._sol->getDelayT());
		TWTEPC_4Solution* par2 = new TWTEPC_4Solution(p2._sol->getChromosome(), p2._sol->getSchedule(), p2._sol->getTwtPref(), p2._sol->getValid(), p2._sol->getDelayT());
		this->_sol = new TWTEPC_4Solution(*par1, *par2, nonDelay);
		delete par1;
		delete par2;
	}
	this->setObjectiveValues();
	this->_valid = this->_sol->isValid();
}	
Hyb1_Solution::Hyb1_Solution(Hyb1_Solution& solution1){
// Copy constructor
	ObjectiveValues.resize(2);
	// copy solution attributes
	GroupingGenome chr = solution1._sol->getChromosome();
	MachineSet sched = solution1._sol->getSchedule();
	float pref = solution1._sol->getTwtPref();
	bool val = solution1._sol->getValid();
	vector<double> delays = solution1._sol->getDelayT();

	this->_key = solution1._key;
	// create new (copy) solution object
	if(this->_key < 0.5) {
		this->_sol = new TWTEPCSolution(chr, sched, pref, val, delays);
	} else {
		this->_sol = new TWTEPC_4Solution(chr, sched, pref, val, delays);
	}

	//this->setObjectiveValues();
	this->ObjectiveValues[0] = solution1.ObjectiveValues[0];
	this->ObjectiveValues[1] = solution1.ObjectiveValues[1];
	this->_valid = this->_sol->isValid();
}										
Hyb1_Solution::~Hyb1_Solution(){
	delete this->_sol;
}

void Hyb1_Solution::Mutate(){
	
	GroupingGenome chr = this->_sol->getChromosome();
	MachineSet sched = this->_sol->getSchedule();
	float pref = this->_sol->getTwtPref();
	bool val = this->_sol->getValid();
	vector<double> delays = this->_sol->getDelayT();
	delete this->_sol;

	if(this->_key < 0.5) {
		this->_key += 0.5;
		this->_sol = new TWTEPC_4Solution(chr, sched, pref, val, delays);
	} else {
		this->_key -= 0.5;
		this->_sol = new TWTEPCSolution(chr, sched, pref, val, delays);
	}
	this->setObjectiveValues();
	this->_valid = this->_sol->isValid();
}



bool Hyb1_Solution::isValid() {
	return this->_sol->isValid();
}



void Hyb1_Solution::setObjectiveValues(){
	this->ObjectiveValues[0] = this->_sol->ObjectiveValues[0];
	this->ObjectiveValues[1] = this->_sol->ObjectiveValues[1];
}
MachineSet Hyb1_Solution::getSchedule() {
	return this->_sol->getSchedule();
}