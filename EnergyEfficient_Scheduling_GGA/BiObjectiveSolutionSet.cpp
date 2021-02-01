#include "BiObjectiveSolutionSet.h"

pair<BiObjectiveSolution, BiObjectiveSolution >& BiObjectiveSolutionSet::operator[](int index){
	return this->solutions[index];
}
int BiObjectiveSolutionSet::size(){
	return this->solutions.size();
}
void BiObjectiveSolutionSet::push_back(pair< BiObjectiveSolution, BiObjectiveSolution > biSol){
	this->solutions.push_back(biSol);
}
void BiObjectiveSolutionSet::append(BiObjectiveSolutionSet& appSet){
	int iMax = appSet.size();
	for(int i = 0; i < iMax; i++) {
		this->push_back(appSet[i]);
	}
}
void BiObjectiveSolutionSet::save(char* filename, int timeLimit) {
	
	// open file
	string file = "HEEBS_solutions.csv";
	ofstream out;
	out.open(file.c_str(), ios::app);

	if (solutions.empty()) {
		out << "Problem\t" << filename << "\t" << "no solutions" << endl;
	}
	else {
		// write summary
		out << "Problem\t" << filename << "\t" << solutions.size() << "\t" << solutions[0].first.eRates << "\t" << timeLimit << endl;

		// write headings
		out << solutions[0].first.objective << "\t" << solutions[0].second.objective << "\t" << endl;

		// write solutions
		for (unsigned i = 0; i < solutions.size(); i++) {
			out << solutions[i].first.value << "\t" << solutions[i].second.value << endl;
		}

	}
	// close file
	out.close();
}

void BiObjectiveSolutionSet::deleteDominated() {
	vector<pair<BiObjectiveSolution, BiObjectiveSolution >>::iterator a_it;
	vector<pair<BiObjectiveSolution, BiObjectiveSolution >>::iterator b_it;
	vector<pair<BiObjectiveSolution, BiObjectiveSolution >>::iterator c_it;
	
	// copy the set of solutions
	BiObjectiveSolutionSet copySols;
	int sMax = solutions.size();
	for(int s = 0; s < sMax; s++) {
		copySols.push_back(solutions[s]);
	}
	solutions.clear();	

	int myObj1, myObj2;

	for(a_it = copySols.solutions.begin(); a_it != copySols.solutions.end(); ) {
		myObj1 = a_it->first.value;
		myObj2 = a_it->second.value;
		bool aDominated = false;
		for(b_it = copySols.solutions.begin(); b_it != copySols.solutions.end(); ) {	
			if( (b_it->first.value < myObj1 && b_it->second.value <= myObj2)|| (b_it->first.value <= myObj1 && b_it->second.value < myObj2)) {
				aDominated = true;
			}
			b_it++;
		}
		
		bool alreadyPresent = false;
		for(c_it = this->solutions.begin(); c_it != this->solutions.end();) {
			if( c_it->first.value == myObj1 && c_it->second.value) {
				alreadyPresent = true;
			}
			c_it++;
		}

		if(!aDominated && !alreadyPresent) {
			this->solutions.push_back(*a_it);
		}
		a_it++;
	}



}