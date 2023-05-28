//
// Created by mihael on 15/05/23.
//

#ifndef SAT_SOLVER_CLAUSE_H
#define SAT_SOLVER_CLAUSE_H

#include "set"
#include "Assignment.h"

class Clause {
public:
    std::set<int> literals;
    explicit Clause(std::set<int> literals);
    bool check_satisfiability(Assignment* assignment);
};


#endif //SAT_SOLVER_CLAUSE_H
