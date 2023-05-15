//
// Created by mihael on 15/05/23.
//

#ifndef SAT_SOLVER_CNF_H
#define SAT_SOLVER_CNF_H

#include "Clause.h"
#include "vector"

class CNF {
public:
    std::vector<Clause*> clauses;
    explicit CNF(std::vector<Clause*> clauses);
};


#endif //SAT_SOLVER_CNF_H
