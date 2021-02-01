/* 
 * File:   AGAThrowable.h
 * Author: drsobik
 *
 * Created on January 11, 2011, 2:49 PM
 */

#ifndef AGATHROWABLE_H
#define	AGATHROWABLE_H

#include <iostream>

#include "AThrowable.h"

using namespace Common;
using namespace std;

class AGAThrowable {
public:
    enum Description
    {
        NO_FEASIBLE_SOLUTION_FOUND,
    };
private:
     Description _description;

public:
    AGAThrowable();
    virtual ~AGAThrowable();

    virtual void msg(ostream& stream=cout);

    /** Set description of the error. */
    virtual void description(Description dscr);

    /** Get description of the error. */
    virtual Description description();

};

#endif	/* AGATHROWABLE_H */

