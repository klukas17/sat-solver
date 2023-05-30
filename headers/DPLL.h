//
// Created by mihael on 15/05/23.
//

#ifndef SAT_SOLVER_DPLL_H
#define SAT_SOLVER_DPLL_H

#include "extensions.h"
#include "Assignment.h"
#include "CNF.h"
#include "random"

class DPLL {
public:
    Assignment* assignment;
    CNF* cnf;
    int recursion_level;

    DPLL(Assignment* assignment, CNF* cnf);
    void solve();
    bool assign_next_variable();
    bool assign_unit_clause_variables(std::vector<int> &unit_clause_variables, bool &continue_searching_for_unit_clause_variables) const;
    bool assign_pure_literal_variables(std::vector<int> &pure_literal_variables, bool &continue_searching_for_pure_literal_variables) const;
    void restore_assignments(std::vector<int> &variables) const;
    void restore_assignment(int variable) const;
    void assign_variable(int variable, int value) const;

#ifdef SUBSUMPTION
    void subsumption() const;
#endif
};


#endif //SAT_SOLVER_DPLL_H
