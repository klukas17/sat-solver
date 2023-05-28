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
    void select_unit_clause_variables(std::vector<int> &unit_clause_variables, std::vector<int> &possible_assignments, int &variable, bool &status);
    void select_random_variable(std::vector<int> &possible_assignments, int &variable);
};


#endif //SAT_SOLVER_DPLL_H
