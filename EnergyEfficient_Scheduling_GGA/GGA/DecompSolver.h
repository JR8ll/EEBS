/* 
 * File:   DecompSolver.h
 * Author: drsobik
 *
 * Created on April 8, 2011, 9:20 AM
 *
 * Description: DecompSolver class solves the one-machine batch problem with
 *				incompatible families by decomposing the problem into smaller
 *				subproblems using time-window technique, i.e. only those lots
 *				are considered, which have not yet been scheduled and are
 *				available within the current time window. Each subproblem is
 *				solved using the GGA.
 *
 */

#ifndef DECOMPSOLVER_H
#    define	DECOMPSOLVER_H

#    include <stdlib.h>

//#    include "Debug.h"
#    include "IFGGATestInst.h"
#    include "IFGGAGenome.h"

//#include "TestInstIO.h"
//#include "MOJKey.h"
//#include "MOJStatistics.h"

#    include <fstream>

using namespace std;

class DecompSolver {
private:
	double _cur_t; // Current time = start of the time window
	double _delta_t; // Size of the time window
	double _max_lots; // Maximum lots per test instance
	int   _n_overlaping_orders; // Number of orders which will overlap in two successive subproblem runs
	int   _num_bathes_per_subproblem; // Number of bathes from the initial solution for the subproblems
	int   _batch_step; // Step for shifting the start batch
	int	  _cur_start_batch; // Current start batch

	IFGGATestInst _ti; // Non-decomposed test instance (used for creating decomposed subinstances)
	IFGGATestInst _cur_sti; // Current test subinstance (within the time window)
	AIFGGATestInst *_saver; // For preserving pointer to the correct test instance

	vector<IFGGAGenome> _sub_results; // Vector of consequitive results of the subproblems

	IFGGAGenome _initgenome; // Genome used for initializing of the populations
	IFGGAGenome _cur_big_genome; // Genome from presolving

	set<int> _scheduled_lots; // Set of secheduled lot ids
	set<int> _unsched_lots_time_win; // Set of not yet scheduled lots within the current time window

	/** Returns set of IDs of those orders, which are ready within the current
	 *  time window. */
	set<int> findOrdersWithinCurrentTimeWindow(vector<Order> &ords, const double twin_start, const double twin_end);

	/** Returns set of IDs of the orders to be solved on the folloving iteration. */
	set<int> findOrdersForFollovingIteration();

	/** Returns number of different families of the orders. */
	int numOfFamilies(vector<Order> &ords);

public:
	DecompSolver();
	virtual ~DecompSolver();

	/** Initialize the solver. */
	void init();

	/** Read the desired test instance from the file. */
	void readTestInstance(const string & file_name);

	/** Solve the test instance using decomposition technique */
	void solve();

	/** Solve subproblems using the built batches from the initial solution. */
	void solveSubProblem(const int start_batch, const int num_batches);

	inline void timeWindowSize(const double delta_t) {
		_delta_t = delta_t;
	}

	inline double timeWindowSize() {
		return _delta_t;
	}

	inline void maxLotsPerTest(const int max_lots) {
		_max_lots = max_lots;
	}

	inline int maxLotsPerTest() {
		return _max_lots;
	}

	inline void nOverlapingOrders(const int n){
		_n_overlaping_orders = n;
	}

	inline void setStartGenome(IFGGAGenome &gnm) {
		_cur_big_genome = gnm;

		//Debugger::info << "Start Genome is:" << Common::ENDL;
		//_cur_big_genome.write(cout);
	}

	inline void nBatchesPerSubproblem(const int num){
		_num_bathes_per_subproblem = num;
	}

	inline void setBatchStep(const int step){
		_batch_step = step;
	}

};

#endif	/* DECOMPSOLVER_H */

