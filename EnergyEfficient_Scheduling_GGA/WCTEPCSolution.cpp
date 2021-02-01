#include "WCTEPCSolution.h"

WCTEPCSolution::WCTEPCSolution() {
	// TODO: implement random initialization
	_chromosome.resize(Global::problem->n);
	for(unsigned i = 0; i < Global::problem->n; i++) {
		//_chromosome[i] = new Batch(Global::cap);
	}

	_wct = rand();
	_epc = rand();
	std::cout << "_wct: " << _wct << ", _epc: " << _epc << endl;
}

WCTEPCSolution::WCTEPCSolution(double wct, double epc){		// TODO delete 
	// TODO: implement or delete
	_wct = wct;
	_epc = epc;
	this->ObjectiveValues[0] = _wct;
	this->ObjectiveValues[1] = _epc;
}

WCTEPCSolution::WCTEPCSolution(WCTEPCSolution & parent1, WCTEPCSolution & parent2){
	// TODO: implement crossover
	cout << "+++ Crossover +++" << endl;
	_wct = parent1._wct;
	_epc = parent1._epc;
	this->ObjectiveValues[0] = _wct;
	this->ObjectiveValues[1] = _epc;
}

WCTEPCSolution::WCTEPCSolution(WCTEPCSolution & solution1){
	// TODO: implement copy-constructor
	_wct = solution1._wct;
	_epc = solution1._epc;
	this->ObjectiveValues[0] = _wct;
	this->ObjectiveValues[1] = _epc;
}

WCTEPCSolution::~WCTEPCSolution(){} 

double WCTEPCSolution::getWCT(){
	return _wct;
}

double WCTEPCSolution::getEPC(){
	return _epc;	
}