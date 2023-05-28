//
// Created by mihael on 15/05/23.
//

#ifndef SAT_SOLVER_DPLL_H
#define SAT_SOLVER_DPLL_H

#include "Assignment.h"
#include "CNF.h"

class DPLL {
public:
    Assignment* assignment;
    CNF* cnf;
    bool solution_found;
    DPLL(Assignment* assignment, CNF* cnf);
    void solve();
    void fix_variable(int variable, int value);
    [[nodiscard]] int choose_next_variable() const;
};


#endif //SAT_SOLVER_DPLL_H
