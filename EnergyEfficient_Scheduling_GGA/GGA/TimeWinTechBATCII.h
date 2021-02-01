/* 
 * File:   TimeWinTechBATCII.h
 * Author: DrSobik
 *
 * Created on May 3, 2011, 10:06 AM
 */

#ifndef TIMEWINTECHBATCII_H
#define	TIMEWINTECHBATCII_H

#include "Debug.h"
#include "Math.h"
#include "BATCII.h"
#include "Item.h"
#include "Container.h"

#include <set>
#include <map>

using namespace std;
using namespace Common;
using namespace Common::Scheduling;

class TimeWinTechBATCII {
private:
	double tdelta;

	map<int, Item<double> > orders_to_schedule;
	set<int> scheduled_orders;

	vector<Container<double> > all_possible_batches;

	double batch_cap;

public:
	TimeWinTechBATCII();
	virtual ~TimeWinTechBATCII();

	vector<Item<double> > getMoreOrdersToSchedule(const double t, const int thres, vector<Item<double> > &orders);

	void buildAllPossibleBatches();

	/** Build batches using time-window thechnique with BATCII-rule. */
	void buildBatches(const double kappa, const double delta, const int thres, vector<Item<double> > &orders, vector<Container<double> > &batches);

private:

};

#endif	/* TIMEWINTECHBATCII_H */

