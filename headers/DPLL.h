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
    bool assign_next_variable();
    bool assign_unit_clause_variables(std::vector<int> &unit_clause_variables, bool &continue_searching_for_unit_clause_variables);
    void restore_assignments(std::vector<int> &variables);
    void restore_assignment(int variable);
    void assign_variable(int variable, int value);
};


#endif //SAT_SOLVER_DPLL_H
