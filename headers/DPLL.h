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
    bool solution_found;
    DPLL(Assignment* assignment, CNF* cnf);
    void solve();
    void fix_next_variable();
};


#endif //SAT_SOLVER_DPLL_H
