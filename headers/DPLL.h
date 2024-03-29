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
    int last_conflict_level;

    DPLL(Assignment* assignment, CNF* cnf);
    void solve();
    bool assign_next_variable(int level);
    bool assign_unit_clause_variables(std::vector<int> &unit_clause_variables, bool &continue_searching_for_unit_clause_variables, int level) const;
    bool assign_pure_literal_variables(std::vector<int> &pure_literal_variables, bool &continue_searching_for_pure_literal_variables, int level) const;
    void restore_assignments(std::vector<int> &variables) const;
    void restore_assignment(int variable) const;
    void assign_variable(int variable, int value, int level) const;

#ifdef SUBSUMPTION
    void subsumption() const;
#endif

#ifdef BOUNDED_VARIABLE_ELIMINATION
    bool bounded_variable_elimination() const;
#endif
};


#endif //SAT_SOLVER_DPLL_H
