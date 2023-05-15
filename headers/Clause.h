//
// Created by mihael on 15/05/23.
//

#ifndef SAT_SOLVER_CLAUSE_H
#define SAT_SOLVER_CLAUSE_H

#include "set"

class Clause {
public:
    std::set<int> literals;
    std::set<int> negated_literals;
    Clause(std::set<int> literals, std::set<int> negated_literals);
};


#endif //SAT_SOLVER_CLAUSE_H
