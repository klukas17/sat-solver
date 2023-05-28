//
// Created by mihael on 15/05/23.
//

#ifndef SAT_SOLVER_DPLL_H
#define SAT_SOLVER_DPLL_H

#include "Assignment.h"
#include "CNF.h"
#include "random"

class DPLL {
public:
    Assignment* assignment;
    CNF* cnf;
    DPLL(Assignment* assignment, CNF* cnf);
    void solve();
};


#endif //SAT_SOLVER_DPLL_H
