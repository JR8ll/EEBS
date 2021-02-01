/* 
 * File:   IFGGATestInst.h
 * Author: DrSobik
 *
 * Created on March 2, 2011
 */

#ifndef _AIFGGATESTINST_H
#    define	_AIFGGATESTINST_H

//#    include "Debug.h"
#    include "AGATestInst.h"
#    include "AIFGGAGenome.h"
#    include "Container.h"
#    include "Item.h"
#    include "DataIOItem.h"
//#include "TestInstIO.h"

typedef Item<double> Order;

using namespace Common;
using namespace Common::Scheduling;

class AIFGGAGenome;

class AIFGGATestInst : public AGATestInst {
	friend class AIFGGAGenome;
public:
	AIFGGAGenome	*genome;

	double cont_cap; // Capacity of one container ~ maximal batch size (each item has size 1)
	int n_orders; // Number of orders
	int n_containers; // Number of available containers = number of orders, since we have batch machining and each batch may contain one order
	int n_families; // Number of incompatible order families
	int n_ord_per_family; // Number of orders per family

	vector<Order> orders; // Orders to be processed

	enum Format {
		DEFAULT
	} informat, outformat; // Format, in which the test instances are kept

public:
	AIFGGATestInst();
	AIFGGATestInst(const AIFGGATestInst& orig);
	virtual ~AIFGGATestInst();

	/** Find the approximate solution with the genetic algorithm. */
	AIFGGAGenome solve();

	virtual void init();

	virtual void copy(const AGATestInst *other);

	virtual void read(istream &in);

	virtual void write(ostream &out);

	/** Read test instance data in default format. */
	virtual void readDefault(istream &in);

	/** Write test instance data in default format. */
	virtual void writeDefault(ostream &out);


};

#endif	/* _AIFGGATESTINST_H */

