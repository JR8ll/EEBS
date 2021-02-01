/* 
 * File:   AGAGenome.h
 * Author: DrSobik
 *
 * Created on March 11, 2010, 10:38 AM
 */

#ifndef _AGAGENOME_H
#    define	_AGAGENOME_H

#    include <ga/GAGenome.h>

#    include "AGATestInst.h"
#    include "AGAThrowable.h"

#    include <MathExt>

#    include <iostream>

using namespace Common;
using namespace std;

class AGATestInst;

template <class VT> class AGAGenome : public GAGenome {
protected:
	bool _valid; // Identifies wheather the genome is valid in sense that all of the sequenced orders could be distributed.
	VT _cur_object; // Current value of the objective
public:

	/** Types of objective functions. */

	enum Cost {
		COST_ANY, COST_TWC, COST_TWT, COST_TWU, COST_TU, COST_TT
	};

	enum Cross {
		CROSS_ONE_POINT, CROSS_TWO_POINT
	};

	enum Mutation {
		MUTATION_COMBINED, MUTATION_SWAP, MUTATION_SHIFT
	};

	static /*Cross*/ int _cross; // Crossover type
	static /*Mutation*/ int _mutation; // Mutation type

	static /*Cost*/ int _cost; // Cost function type

	// How to identify
	GADefineIdentity("AGAGenome", 201);

	AGAGenome();
	AGAGenome(const AGAGenome& orig);
	virtual ~AGAGenome();

	// Copying the other object
	virtual void copy(const GAGenome &orig);
	AGAGenome & operator=(const GAGenome &orig);

	// Cloning myself
	virtual GAGenome* clone(CloneMethod) const;

	// Output
	virtual int write(ostream &) const;

	/** Check if the orders could be correctly distributed beween FOUPs. */
	virtual bool valid();

	// ############################  Initializer   #############################
	/** Initializer specific to the genome. */
	virtual void initialize();

	/** Static initializer for the GA. */
	static void init(GAGenome &);

	// ############################  Crossover #################################
	/** Crossover specific to the genome. This pairs with the mate. */
	virtual void mate(AGAGenome &mate, AGAGenome *, AGAGenome *);

	/** Static crossover for the genetic algorithm. */
	static int cross(const GAGenome &, const GAGenome &, GAGenome *, GAGenome *);

	//#############################  Mutator   #################################
	/** Mutator specific to the genome. */
	virtual int mut(const float prob);

	/** Static genome mutator for the GA. */
	static int mutate(GAGenome &, float);

	//#############################  Comparator  ###############################
	/** Compare this genome to the other one. */
	virtual float compare(const GAGenome &other);

	/** Static comparator of the genomes for the genetic algorithm. */
	static float compare(const GAGenome &, const GAGenome &);

	/** Total Weighted Completion time for the genome. */
	virtual VT TWC();

	/** Total Weighted Tardidness for the genome. */
	virtual VT TWT();

	/** Total weighted number of tardy jobs. */
	virtual VT TWU();

	/** Total number of tardy jobs. */
	virtual VT TU();

	/** Total number of tardy jobs. */
	virtual VT TT();

	/** Objective of the genome. */
	virtual VT object();

	/** Static objective for the GA. */
	static float objective(GAGenome &);

	/** Update the genome. Meaning of the update function must be defined.
	 *  it could be left empty or used for updating for example the completion
	 *  times. */
	virtual void update();

	/** Just for testing. */
	virtual void refine(){};
};

template <class VT> int AGAGenome<VT>::_cross;
template <class VT> int AGAGenome<VT>::_mutation;
template <class VT> int AGAGenome<VT>::_cost;

template <class VT> AGAGenome<VT>::AGAGenome() {
	initializer(init);
	mutator(mutate);
	comparator(compare);
	crossover(cross);
	evaluator(objective);
}

template <class VT> AGAGenome<VT>::AGAGenome(const AGAGenome<VT>& orig) {
	copy(orig);
}

template <class VT> AGAGenome<VT>::~AGAGenome() {
	//Debugger::iDebug("AGAGenome<VT>::~AGAGenome()");
}

template <class VT> void AGAGenome<VT>::copy(const GAGenome &orig) {
	//cout << "AGAGenome :: copying" << endl;
	GAGenome::copy(orig);

	AGAGenome & o = (AGAGenome &) orig;

	_valid = o._valid;
	_cur_object = o._cur_object;

	//write(cout);
	//cout << "AGAGenome :: copying finished" << endl;
}

template <class VT> AGAGenome<VT> & AGAGenome<VT>::operator=(const GAGenome &orig) {
	if (&orig != this) copy(orig);
	return *this;
}

template <class VT> GAGenome* AGAGenome<VT>::clone(CloneMethod) const {
	//cout<<"Genome:: Cloning"<<endl;
	return new AGAGenome<VT > (*this);
}

template <class VT> bool AGAGenome<VT>::valid() {
	Debugger::eDebug("AGAGenome::valid() : Not implemented!");
}

template <class VT> int AGAGenome<VT>::write(ostream & stream) const {
	Debugger::eDebug("AGAGenome::write(ostream & stream) const : Not Implemented!");
}

template <class VT> void AGAGenome<VT>::initialize() {
	Debugger::eDebug("AGAGenome::initialize() : Not implemented!");
}

template <class VT> void AGAGenome<VT>::init(GAGenome &gen) {
	AGAGenome<VT> &g = (AGAGenome<VT> &) gen;

	g.initialize();

	//cout<<"Objective init: "<<g._cur_object<<endl;;

}

template <class VT> void AGAGenome<VT>::mate(AGAGenome<VT> &mate, AGAGenome<VT> *sis, AGAGenome<VT> *bro) {
	Debugger::eDebug("AGAGenome::crossover(const AGAGenome &mate, AGAGenome *sis, AGAGenome *bro) not implemented!");
}

template <class VT> int AGAGenome<VT>::cross(const GAGenome &a, const GAGenome &b, GAGenome *c, GAGenome *d) {
	//cout << "Genome:: crossing" << endl;
	AGAGenome<VT> &dad = (AGAGenome<VT> &) a;
	AGAGenome<VT> &mom = (AGAGenome<VT> &) b;
	AGAGenome<VT> &sis = (AGAGenome<VT> &) * c;
	AGAGenome<VT> &bro = (AGAGenome<VT> &) * d;

	int n_crossovers = 0; // Amount of the crossovers

	if (!dad.valid() || !mom.valid()) {
		mom.write(cout);
		dad.write(cout);
		Debugger::eDebug("AGAGenome::cross(const GAGenome & a, const GAGenome & b, GAGenome * c, GAGenome * d) invalid parents!");
	}

	// Copy parents in order to initialize
	if (c) sis.copy(mom);
	if (d) bro.copy(dad);

	if (c && d) { // Make sister and brother

		mom.mate(dad, &sis, &bro);

		//sis.evaluate(gaTrue);
		//bro.evaluate(gaTrue);
		n_crossovers = 2;
	} else {
		if (c) { // Only sister

			mom.mate(dad, &sis, 0);

			//sis.evaluate(gaTrue);
			n_crossovers = 1;
		} else {
			if (d) { // Only brother

				dad.mate(mom, 0, &bro);

				//bro.evaluate(gaTrue);
				n_crossovers = 1;
			}
		}
	}

	//Update the genomes after we obtain feasible representations
	//Debugger::iDebug("AGAGenome<VT>::cross(const GAGenome &a, const GAGenome &b, GAGenome *c, GAGenome *d) : Updating after mating...");
	if (c) sis.update();
	if (d) bro.update();
	//Debugger::iDebug("AGAGenome<VT>::cross(const GAGenome &a, const GAGenome &b, GAGenome *c, GAGenome *d) : Updated after mating.");

	// Evaluate objectives of the children
	if (c) sis.evaluate(gaTrue);
	if (d) bro.evaluate(gaTrue);

	//cout<< "n_crosses : "<<n_crossovers<<endl;
	//cout << "Cross finished" << endl;

	return n_crossovers;
}

template <class VT> int AGAGenome<VT>::mut(const float prob) {
	Debugger::eDebug("AGAGenome::mut() not implemented!");
}

template <class VT> int AGAGenome<VT>::mutate(GAGenome &gen, float prob) {
	AGAGenome<VT> &g = (AGAGenome<VT> &) gen;
	int res = g.mut(prob);

	//Update the genome after obtaining its feasible representation
	g.update();

	// Important !!! Tell GALib to reevaluate the genome in order not to use the cached  value
	g.evaluate(gaTrue);

	return res;
}

template <class VT> float AGAGenome<VT>::compare(const GAGenome &other) {
	Debugger::eDebug("AGAGenome::compare(const GAGenome &other) : Not implemented!");
}

template <class VT> float AGAGenome<VT>::compare(const GAGenome &g1, const GAGenome &g2) {
	return g1.compare(g2);
}

template <class VT> VT AGAGenome<VT>::TWC() {
	Debugger::eDebug("AGAGenome::TWC() : Not implemented!");
}

template <class VT> VT AGAGenome<VT>::TWT() {
	Debugger::eDebug("AGAGenome::TWT() : Not implemented!");
}

template <class VT> VT AGAGenome<VT>::TWU() {
	Debugger::eDebug("AGAGenome::TWU() : Not implemented!");
}

template <class VT> VT AGAGenome<VT>::TU() {
	Debugger::eDebug("AGAGenome::TU() : Not implemented!");
}

template <class VT> VT AGAGenome<VT>::TT() {
	Debugger::eDebug("AGAGenome::TT() : Not implemented!");
}

template <class VT> VT AGAGenome<VT>::object() {
	switch (_cost) {
		case AGAGenome::COST_TWC: _cur_object = TWC();
			break;

		case AGAGenome::COST_TWT: _cur_object = TWT();
			break;

		case AGAGenome::COST_TWU: _cur_object = TWU();
			break;

		case AGAGenome::COST_TU: _cur_object = TU();
			break;

		case AGAGenome::COST_TT: _cur_object = TT();
			break;
	}

	return _cur_object;
}

template <class VT> float AGAGenome<VT>::objective(GAGenome & gen) {
	//cout << "Genome:: calculating objective" << endl;
	AGAGenome<VT> &g = (AGAGenome<VT> &) gen;

	return float(g.object());
}

template <class VT> void AGAGenome<VT>::update() {
	Debugger::eDebug("AGAGenome::update() : Not implemented!");
}

#endif	/* _AGAGENOME_H */

