/* 
 * File:   GGATestInst.cpp
 * Author: DrSobik
 * 
 * Created on March 10, 2010, 2:25 PM
 */

#include "AIFGGATestInst.h"
#include "IFGGAGenome.h"

#include <stdio.h>
#include <stdlib.h>

class IFGGAGenome;

AIFGGATestInst::AIFGGATestInst() {
	init();
}

AIFGGATestInst::AIFGGATestInst(const AIFGGATestInst& orig) {
	copy((AGATestInst*) & orig);
}

AIFGGATestInst::~AIFGGATestInst() {
}

AIFGGAGenome AIFGGATestInst::solve() {
	//AIFGGAGenome::_ti = this;

	//AIFGGAGenome res;
	//AIFGGAGenome genome;

	cout << "Creating population ..." << endl;
	// Create a population of the specific genomes which "know" the concrete static specific operators.
	GAPopulation population;
	population.size(0);

	int n_batcii = 4;
	int n_edd = 0;

	// Add genomes with random initialization
	for (int i = 0; i < n_genomes - n_edd - n_batcii; i++) {
		genome->_ti = this;
		genome->_init_rule = AIFGGAGenome::IR_RANDOM;
		genome->_init_max_batch_size = cont_cap;
		population.add(*genome);
	}

	// Add genomes with EDD-initialization
	for (int i = 0; i < n_edd; i++) {
		genome->_ti = this;
		genome->_init_rule = AIFGGAGenome::IR_EDD;
		genome->_init_max_batch_size = Math::max(2.0, cont_cap - i);
		population.add(*genome);
	}

	// Add genomes with BATCII-initialization
	for (int i = 0; i < n_batcii; i++) {
		genome->_ti = this;
		genome->_init_rule = AIFGGAGenome::IR_BATCII;
		genome->cur_init_sol_str = char(i + 1 + 48);
		population.add(*genome);
	}

	cout << "Population created." << endl;

	// Run the genetic algorithm
	cout << "Solving ..." << endl;
	//AGATestInst::solve(/*&res*/genome, population);


	try {

		//Debugger::wDebug("AGATestInst::solve(AGAGenome *res, GAPopulation &population) setting statics...");

		//AGAGenome::_cross = AGAGenome::CROSS_TWO_POINT;
		//AGAGenome::_ti = this; // Give the genome pointer to the test instance

		//Debugger::wDebug("AGATestInst::solve(AGAGenome *res, GAPopulation &population) statics set.");

		//Debugger::wDebug("AGATestInst::solve(AGAGenome *res, GAPopulation &population) creating GA...");

		GASteadyStateGA ga(population);

		//Debugger::wDebug("AGATestInst::solve(AGAGenome *res, GAPopulation &population) GA created.");


		/*
		for (int i = 0; i < population.size(); i++) {
			((AGAGenome&) population.individual(i)).write(cout);
		}
		return;
		 */


		//Debugger::iDebug("AGATestInst::solve(AGAGenome *res, GAPopulation &population) adjusting GA...");
		// Perform minimization
		ga.minimize();

		// Make the appropriate adjustments for the algorithm
		//ga.populationSize(n_genes);
		ga.nGenerations(n_cycles);
		ga.pReplacement(p_repl);
		ga.pMutation(p_mut);
		ga.pCrossover(p_cross);
		ga.initialize(370368);
		ga.scoreFrequency(0.1);
		ga.flushFrequency(0.1);
		ga.selectScores(GAStatistics::Deviation);
		//Debugger::iDebug("AGATestInst::solve(AGAGenome *res, GAPopulation &population) GA adjusted.");

		//for (int i = 0; i < population.size(); i++) {
		//    if (!((AGAGenome&) population.individual(i)).valid())// population.remove(i);
		//    Debugger::wDebug("Removed invalid genome during initialization!");
		//}

		double best_fitness;

		//Debugger::wDebug("AGATestInst::solve(AGAGenome *res, GAPopulation &population) initial scores...");

		cout << "Best initial Obj : " << ga.statistics().bestIndividual().score() << endl;

		//Debugger::wDebug("AGATestInst::solve(AGAGenome *res, GAPopulation &population) initial scores done.");

		cout << "Starting evolution ... " << endl;
		time_t start_t;
		time_t t;

		start_t = time(NULL);
		while (!ga.done()) {
			//cout<<"Entered main loop.";
			//cout.flush();

			//for (int i = 0; i < 20; i++) {
			//	((AGAGenome<double> *)&(ga.population().best(i)))->refine();
			//}



			//if (ga.generation() < 20) {
			//cout<<"Objective before frefinement: "<<ga.population().best().score()<<endl;
			IFGGAGenome *tmp_gen;

			ga.step();

			/*
			if (ga.generation() < 25) {

				for (int i = 0; i < ga.population().size(); i++) {
					tmp_gen = (IFGGAGenome*) (&(ga.population().best(i)));
					tmp_gen->invert();
					//((IFGGAGenome&) ga.population().best()).update();
					//tmp_gen->refine();
					//tmp_gen->score(tmp_gen->object());
					//cout<<"Objective after frefinement: "<<ga.population().best().score()<<endl;
				}

			}
			 */



			//tmp_gen = (IFGGAGenome*) (&(ga.population().worst()));
			//tmp_gen->invert();
			//((IFGGAGenome&) ga.population().best()).update();
			//tmp_gen->refine();
			//tmp_gen->score(tmp_gen->object());
			//tmp_gen = 0;
			//}

			//if (ga.population().dev() < 0.02 * ga.statistics().bestIndividual().score()) {
			//	//Debugger::info << "Population deviation is too small => increasing mutation rate." << Common::ENDL;
			//	p_mut = Math::min(1.0, p_mut + 0.1);
			//	ga.pMutation(p_mut);
			//}

			//cout<<"First step performed.";
			//cout.flush();
			if (ga.statistics().generation() % 50 == 0) { // Every 50-th generation
				if (ga.population().dev() < 0.02 * ga.statistics().bestIndividual().score()) {
					Debugger::info << "Population deviation is too small => increasing mutation rate." << Common::ENDL;
					p_mut = Math::min(1.0, p_mut + 0.1);
					ga.pMutation(p_mut);
				}
				cout << setfill(' ') << setw(100) << '\r';
				cout << "Iter: " << setw(5) << ga.statistics().generation() << '\t';
				cout << "dev : " << ga.population().dev() << ", Obj : " << ga.statistics().bestIndividual().score() << endl;
			} else {
				cout << setfill(' ') << setw(100) << '\r';
				cout << "Iter: " << setw(5) << ga.statistics().generation() << '\t';
				cout << "dev : " << ga.population().dev() << ", Obj : " << ga.statistics().bestIndividual().score() << '\r';
			}
			cout.flush();




			t = time(NULL);
			if (t >= start_t + n_secs) {
				cout << endl << "Time out for the test instance. Terminating ..." << endl;
				break;
			}

			if (ga.statistics().bestIndividual().score() == 0) {
				cout << endl << "Best solution found. Terminating ..." << endl;
				break;
			}

		}

		best_fitness = ga.statistics().bestIndividual().score();

		cout << "Number of iterations => " << ga.statistics().generation() << endl;
		cout << "Best Obj => " << best_fitness << endl << endl;

		//ga.statistics().bestIndividual().write(cout);

		//Debugger::wDebug("Copying best genome ...");
		genome->copy(ga.population().best());
		//Debugger::wDebug("Best genome copied.");

	} catch (AGAThrowable &error) {
		// How do we interprete result in case of the exception
		error.msg();

		AGAThrowable::Description d = error.description();

		switch (d) {
			case AGAThrowable::NO_FEASIBLE_SOLUTION_FOUND:
			{
				Debugger::wDebug("Handling unfeasible solution!");
				//res->foups.clean();
				genome->score(-1);
			}
		}
	}


	cout << "Solution found." << endl;

	return /*res*/*genome;
}

void AIFGGATestInst::init() {
	cont_cap = 0;
	n_orders = 0;
	n_containers = 0;

	n_families = 0;
	n_ord_per_family = 0;

	orders.resize(0);

	informat = DEFAULT;
	outformat = DEFAULT;
}

void AIFGGATestInst::copy(const AGATestInst * other) {
	if (this == other) return;

	AIFGGATestInst &oth = *((AIFGGATestInst*) other);

	n_containers = oth.n_containers;
	cont_cap = oth.cont_cap;
	n_orders = oth.n_orders;

	n_families = oth.n_families;
	n_ord_per_family = oth.n_ord_per_family;

	orders = oth.orders;

	informat = oth.informat;
	outformat = oth.outformat;
}

string extractValue(const string & str) {
	int num_start;
	int num_end;

	num_start = str.find('=') + 1;
	num_end = str.find(';');

	return str.substr(num_start, num_end - num_start);
}

void AIFGGATestInst::readDefault(istream & in) {
	string cur_string;
	string num;
	Order ord;

	Debugger::iDebug("Reading batch capacity ...");
	getline(in, cur_string); // # Batch capacity
	getline(in, cur_string);
	cont_cap = atof(cur_string.c_str());
	Debugger::info << "Batch capacity:" << " " << cont_cap << Common::ENDL;
	getline(in, cur_string);
	Debugger::iDebug("Batch capacity read.");

	Debugger::iDebug("Reading orders from stream ...");

	n_orders = 0;
	orders.resize(0);
	// The read line format: <family>;<ord_id>;<due_date>;<weight>;<proc_time>;<ready_time>
	while (!in.eof()) {
		getline(in, cur_string, ';'); // Get Family
		//if (cur_string == "\r\n"){
		//	Debugger::iDebug("Found empty string: skipping the following file read.");
		//	break;
		//}
		//cout<<cur_string.size()<<" "<<(int)(cur_string.data()[0])<<" ";
		ord.f((int) (cur_string.data()[0]) - 64, true);
		//cout << cur_string << " ";
		//cout << ord.familiesV()[0] << " ";
		//cout << ord.familiesV()[0] << " ";


		getline(in, cur_string, ';'); // Order ID
		ord.id(n_orders);
		//cout << cur_string << " ";

		getline(in, cur_string, ';'); // Order due date
		ord.d(atof(cur_string.c_str()));
		//cout << cur_string << " ";
		//cout << ord.d() << " ";

		getline(in, cur_string, ';'); // Order weight
		ord.w(atof(cur_string.c_str()));
		//cout << ord.w() << " ";

		getline(in, cur_string, ';'); // Order processing time
		ord.p(0, atof(cur_string.c_str()));
		//cout << ord.p(0) << " ";

		getline(in, cur_string, ';'); // Get ready time
		ord.r(atof(cur_string.c_str()));
		//cout << cur_string << " "<<endl;

		// Set size of each order = 1
		ord.s(1);

		in.get(); // '0x0D'
		in.get(); // '0x0A'

		in.get();
		if (!in.eof()) in.unget();

		/*
		cout<<"Just read order: ";
		ord.print(cout);
		cout<<endl;
		 */

		orders.push_back(ord);

		n_orders++;
	}

	Debugger::iDebug("Orders read! ");

	Debugger::info << "Total orders read: " << (int) orders.size() << Common::ENDL;

#ifdef DEBUG
	/*
		for (int i = 0; i < orders.size(); i++) {
			cout << orders[i].familiesV()[0] << " ";
			cout << i << " ";
			cout << orders[i].s() << " ";
			cout << orders[i].d() << " ";
			cout << orders[i].w() << " ";
			cout << orders[i].p(0) << " ";
			cout << orders[i].r() << endl;
		}
	 */
#endif

	// Number of containers = number of orders
	n_containers = n_orders;

	// Calculate number of families
	set<int> fmls;

	fmls.clear();
	for (int i = 0; i < n_orders; i++) {
		fmls.insert(orders[i].familiesV()[0]);
	}
	n_families = fmls.size();
	Debugger::info << "Number of families: " << n_families << Common::ENDL;

	// Calculate number of orders per family (it is assumed that each family contains the same number of orders)
	n_ord_per_family = n_orders / n_families;
	Debugger::info << "Number of orders per family: " << n_ord_per_family << Common::ENDL;

}

void AIFGGATestInst::writeDefault(ostream & out) {
	out << "#n_orders" << endl;
	out << orders.size() << endl;
	out << "#cont_cap" << endl;
	out << cont_cap << endl;
	out << "#cont_amount" << endl;

	for (int i = 0; i < orders.size(); i++) {
		out << i << " ";
		out << orders[i].s() << " ";
		out << orders[i].w() << " ";
		out << orders[i].p() << " ";
		out << orders[i].r() << " ";
		out << orders[i].d();
		out << endl;
	}
}

void AIFGGATestInst::read(istream & in) {

	switch (informat) {
		case AIFGGATestInst::DEFAULT:
		{
			readDefault(in);
		}

	} // switch
}

void AIFGGATestInst::write(ostream & out) {
	Debugger::eDebug("IFGGATestInst::write(ostream &out) : Must be reimplemented!");

	switch (outformat) {
		case AIFGGATestInst::DEFAULT:
		{
			writeDefault(out);
		}
	}

}