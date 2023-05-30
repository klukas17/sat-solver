//
// Created by mihael on 15/05/23.
//

#ifndef SAT_SOLVER_CNF_H
#define SAT_SOLVER_CNF_H

#include "extensions.h"
#include "Clause.h"
#include "vector"
#include "Assignment.h"

class CNF {
public:
    std::vector<Clause*> clauses;
    int number_of_variables;

    CNF(std::vector<Clause*> clauses, int number_of_variables);
    void check_satisfiability(Assignment* assignment, bool &satisfied, bool &contradiction, int &conflict_level);
    void evaluate_clauses(Assignment* assignment);
};


#endif //SAT_SOLVER_CNF_H
