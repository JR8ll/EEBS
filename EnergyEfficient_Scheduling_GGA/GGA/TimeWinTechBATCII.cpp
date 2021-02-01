/* 
 * File:   TimeWinTechBATCII.cpp
 * Author: DrSobik
 * 
 * Created on May 3, 2011, 10:06 AM
 */

#include "TimeWinTechBATCII.h"

TimeWinTechBATCII::TimeWinTechBATCII() {
}

TimeWinTechBATCII::~TimeWinTechBATCII() {
}

void TimeWinTechBATCII::buildBatches(const double kappa, const double delta, const int thres, vector<Item<double> > &orders, vector<Container<double> > &batches) {
	tdelta = delta;

	scheduled_orders.clear();
	orders_to_schedule.clear();

	batch_cap = batches[0].cap();

	do {
		// Select orders to schedule considering their ready times
		getMoreOrdersToSchedule(tdelta, thres, orders);

		// Build all possible batches from the orders to be scheduled
		buildAllPossibleBatches();

		// Select the best built batch using the BATCII-rule

		// Insert the best found batch into the result

		// Update the time window
		tdelta += delta;

	} while (scheduled_orders.size() < orders.size());

}

vector<Item<double> > TimeWinTechBATCII::getMoreOrdersToSchedule(const double t, const int thres, vector<Item<double> > &orders) {
	for (int i = 0; i < orders.size(); i++) {
		if (scheduled_orders.count(orders[i].id()) > 0) continue;
		if (orders[i].r() > t) continue;
		if (orders_to_schedule.size() < thres) {
			orders_to_schedule[orders[i].id()] = orders[i];
		}
	}
}

void TimeWinTechBATCII::buildAllPossibleBatches() {
	all_possible_batches.resize(0);

	// Decompose the orders to schedule into families
	map<int, vector<Item<double> > > fml_orders;

	for (map<int, Item<double> >::iterator it = orders_to_schedule.begin(); it != orders_to_schedule.end(); it++) {
		fml_orders[it->second.familiesV()[0]].push_back(it->second);
	}

	// For each bamily build all possible batches
	vector<int> indices;
	double cur_batch_cap = 0.0;

	do {
		// Select capcacity of batches to be built
		cur_batch_cap += 1.0;

		indices.resize(int(cur_batch_cap));
		for (int cur_idx = 0; cur_idx < indices.size(); cur_idx++) {
			
		}

	} while (cur_batch_cap < batch_cap);

}