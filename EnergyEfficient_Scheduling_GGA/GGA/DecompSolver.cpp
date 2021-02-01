/* 
 * File:   DecompSolver.cpp
 * Author: drsobik
 * 
 * Created on April 8, 2011, 9:20 AM
 */

#include <complex>

#include "DecompSolver.h"

DecompSolver::DecompSolver() {
	init();
}

DecompSolver::~DecompSolver() {
}

void DecompSolver::init() {
	_cur_t = 0.0;
	_delta_t = -1.0;
	_max_lots = 0;

	_scheduled_lots.clear();
	_unsched_lots_time_win.clear();

	_cur_start_batch = 0;
}

void DecompSolver::readTestInstance(const string & file_name) {
	Debugger::info << "DecompSolver: Reading test instance form file: " << file_name << Common::ENDL;

	_ti.informat = IFGGATestInst::DEFAULT;
	_ti.outformat = IFGGATestInst::DEFAULT;

	ifstream infile;

	infile.open(file_name.c_str(), ios_base::in);
	if (infile.is_open()) {
		_ti.read(infile);
	} else {
		Debugger::err << "File " << file_name << " not found!" << Common::ENDL;
	}
	infile.close();

	/*
	_ti.genome = &_initgenome;
	_ti.n_genomes = 30;
	_ti.n_cycles = 100000;
	_ti.n_secs = 600; //100000000;
	_ti.p_mut = 0.15;
	 */

	Debugger::info << "DecompSolver: Test instance successfully read." << Common::ENDL;
}

void DecompSolver::solve() {
	Debugger::info << "Solving the sequnse of subproblems ..." << Common::ENDL;
	// Check time window correktness 
	if (_delta_t <= 0) Debugger::err << "DecompSolver::solve(): Time window size is negative!" << Common::ENDL;
	if (_ti.orders.size() == 0) Debugger::err << "DecompSolver::solve(): No orders in the testinstance!" << Common::ENDL;
	if (_max_lots <= 0) Debugger::err << "DecompSolver::solve(): Maximum lots per test instance must be positive!" << Common::ENDL;

	do {
		// Solve subproblem and update the BIG genome with the new batches
		solveSubProblem(_cur_start_batch, _num_bathes_per_subproblem);

		_cur_start_batch += _batch_step;

	} while (_cur_start_batch + _batch_step < _cur_big_genome.batches.firstEmpty());

	_cur_big_genome.update();
	Debugger::info<<"Calculating the final TWT."<<Common::ENDL;
	double final_TWT = _cur_big_genome.object();
	Debugger::info<<"Calculated the final TWT."<<Common::ENDL;

	//Debugger::info << "TWT= " << TWT << Common::ENDL;
	Debugger::info << "Final TWT= " << final_TWT << Common::ENDL;

	Debugger::info << "The problem has been solved!" << Common::ENDL;
}

void DecompSolver::solveSubProblem(const int start_batch, const int num_batches) {
	Debugger::info << "Solving a subproblem ..." << Common::ENDL;
	// Check time window correktness
	if (_delta_t <= 0) Debugger::err << "DecompSolver::solve(): Time window size is negative!" << Common::ENDL;
	if (_ti.orders.size() == 0) Debugger::err << "DecompSolver::solve(): No orders in the testinstance!" << Common::ENDL;
	if (_max_lots <= 0) Debugger::err << "DecompSolver::solve(): Maximum lots per test instance must be positive!" << Common::ENDL;

	vector<Order> cur_orders;
	set<int> cur_ids;

	AIFGGAGenome cur_res;
	double TWT = 0.0;

	// Get order ids of the orders in the selected batches
	cur_ids.clear();
	for (int cur_batch = start_batch; cur_batch <= start_batch + num_batches; cur_batch++) {
		set<int> tmp_ids = _cur_big_genome.batches[cur_batch]->idsS();
		cur_ids.insert(tmp_ids.begin(), tmp_ids.end());
	}

	cout << "Solving test instance with orders:" << endl;
	for (set<int>::iterator i = cur_ids.begin(); i != cur_ids.end(); i++) {
		cout << *i << " ";
	}
	cout << endl;


	// Prepare the corresponding subproblem

	cur_orders.resize(0);
	for (set<int>::iterator i = cur_ids.begin(); i != cur_ids.end(); i++) {
		cur_orders.push_back(_ti.orders[*i]);
	}

	// Update the ready times of the local orders
	double start_time = 0.0;
	if (start_batch > 0) start_time = _cur_big_genome.CTs()[start_batch - 1];

	for (int i = 0; i < cur_orders.size(); i++) {
		cur_orders[i].r(Math::max(start_time, cur_orders[i].r()));
	}

	// Adjust an solve the GA test instance for the current orders
	_cur_sti.init();
	_cur_sti.orders = cur_orders;
	_cur_sti.n_orders = cur_orders.size();
	_cur_sti.n_families = numOfFamilies(cur_orders);
	_cur_sti.n_containers = cur_orders.size();
	_cur_sti.cont_cap = _ti.cont_cap;
	_cur_sti.genome = &_initgenome;
	_cur_sti.n_genomes = 50;
	_cur_sti.n_cycles = 1000;
	_cur_sti.n_secs = 2; //100000000;
	_cur_sti.p_mut = 0.15;

	// Preserve the BIG test instance
	_saver = AIFGGAGenome::_ti;

	cur_res = _cur_sti.solve();

	// Restore the static pointer to the big test instance
	AIFGGAGenome::_ti = _saver;

	Debugger::info << "Current WT = " << (double) cur_res.score() << Common::ENDL;
	TWT += cur_res.score();
	Debugger::info << "Current TWT = " << (double) TWT << Common::ENDL;
	cur_res.write(cout);

	// Increase the _cur_t. At this time the machine will be available again.
	//_cur_t = cur_res.CTs()[cur_res.batches.firstEmpty() - 1];
	//Debugger::info << "New _cur_t = " << _cur_t << Common::ENDL;

	// Update ready times of the unscheduled orders
	//for (int i = 0; i < _ti.orders.size(); i++) {
	//	if (_scheduled_lots.count(_ti.orders[i].id()) == 0) {
	//		_ti.orders[i].r(Math::max(_cur_t, _ti.orders[i].r()));
	//	}
	//}

	Debugger::info << "The subproblem has been solved!" << Common::ENDL;

	Debugger::info << "Updating the BIG genome." << Common::ENDL;

	IFGGAGenome tmp_big_genome;
	tmp_big_genome.copy(_cur_big_genome);

	Debugger::info << "Copying the batches." << Common::ENDL;
	tmp_big_genome.batches.clean();
	int idx = 0;

	// Copy previous untouched batches of the BIG genome
	Debugger::info << "Copying previous untouched baches of the BIG genome." << Common::ENDL;
	for (int i = 0; i < start_batch; i++) {
		tmp_big_genome.batches[idx]->copy(_cur_big_genome.batches[i]);
		idx++;
	}
	Debugger::info << "Previous untouched baches of the BIG genome have been copied." << Common::ENDL;

	// Copy the batches from the subgenome of the just solved subproblem
	Debugger::info << "Copying baches of the subgenome." << Common::ENDL;
	for (int i = 0; i < cur_res.batches.firstEmpty(true); i++) {
		tmp_big_genome.batches[idx]->copy(cur_res.batches[i]);
		idx++;
	}
	Debugger::info << "Baches of the subgenome have been copied." << Common::ENDL;

	// Copy the rest of the batches of BIG genome
	Debugger::info << "Copying rest baches of the BIG genome." << Common::ENDL;
	for (int i = start_batch + num_batches + 1; i < _cur_big_genome.batches.size(); i++) {
		if (idx == _cur_big_genome.batches.size()) break;
		tmp_big_genome.batches[idx]->copy(_cur_big_genome.batches[i]);
		idx++;
	}
	Debugger::info << "Rest batches of the big genome have been copied have been copied." << Common::ENDL;

	// Copy the batches from the tmp_big_genome back to the _cur_big_genome
	for (int i = 0; i < tmp_big_genome.batches.size(); i++) {
		_cur_big_genome.batches[i]->copy(tmp_big_genome.batches[i]);
	}

	//_cur_big_genome.refine();
	Debugger::info << "New num of batches: " << (int) _cur_big_genome.batches.size() << Common::ENDL;
	Debugger::info << "New num of _cts: " << (int) _cur_big_genome._cts.size() << Common::ENDL;

	Debugger::info << "Updating the big genome." << Common::ENDL;
	//_cur_big_genome.utilizedContToFront();
	//_cur_big_genome.updateFamBatIdx();
	_cur_big_genome.update();
	Debugger::info << "The big genome has been updated." << Common::ENDL;

	Debugger::info << "The BIG genome has been updated." << Common::ENDL;
}

// ########################  Help functions  ###################################

bool cmpOrdDue(Order o1, Order o2) {
	return o1.d() < o2.d();
}

set<int> DecompSolver::findOrdersWithinCurrentTimeWindow(vector<Order> &ords, const double twin_start, const double twin_end) {
	set<int> res;
	vector<Order> ords_tmp = ords;

	// Sort ords using some rule
	sort_heap(ords_tmp.begin(), ords_tmp.end(), cmpOrdDue);

	res.clear();
	for (int i = 0; i < ords_tmp.size(); i++) {
		if (res.size() >= _max_lots) break;
		if (_scheduled_lots.count(ords_tmp[i].id()) == 0) { // Check whether it has been scheduled
			//if (ords[i].r() <= twin_end) {
			res.insert(ords_tmp[i].id());
			//}
		}
	}

	return res;
}

set<int> DecompSolver::findOrdersForFollovingIteration() {


}

int DecompSolver::numOfFamilies(vector<Order> &ords) {
	set<int> fmls;
	set<int> cur_fmls;

	for (int i = 0; i < ords.size(); i++) {
		cur_fmls = ords[i].familiesS();
		fmls.insert(cur_fmls.begin(), cur_fmls.end());
	}

	return fmls.size();
}