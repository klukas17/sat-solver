//
// Created by mihael on 15/05/23.
//

#ifndef SAT_SOLVER_CLAUSE_H
#define SAT_SOLVER_CLAUSE_H

#include "extensions.h"
#include "set"
#include "Assignment.h"

class Clause {
public:
    std::set<int> literals;
    bool last_evaluation;
    bool clause_eliminated;

    explicit Clause(std::set<int> literals);
    void check_satisfiability(Assignment* assignment, bool &satisfied, bool &contradiction);
    void evaluate(Assignment* assignment);
};


#endif //SAT_SOLVER_CLAUSE_H
