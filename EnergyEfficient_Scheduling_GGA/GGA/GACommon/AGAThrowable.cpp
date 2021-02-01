/* 
 * File:   AGAThrowable.cpp
 * Author: drsobik
 * 
 * Created on January 11, 2011, 2:49 PM
 */

#include "AGAThrowable.h"

AGAThrowable::AGAThrowable() {
}

AGAThrowable::~AGAThrowable() {
}

void AGAThrowable::msg(ostream& stream){
    switch (_description){
        case    AGAThrowable::NO_FEASIBLE_SOLUTION_FOUND : Debugger::wDebug("No feasible solution found!");
    }
}

void AGAThrowable::description(Description dscr){
    _description = dscr;
}

AGAThrowable::Description AGAThrowable::description(){
    return _description;
}