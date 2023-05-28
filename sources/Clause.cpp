//
// Created by mihael on 15/05/23.
//

#include "Clause.h"

#include <utility>

Clause::Clause(std::set<int> literals) {
    this->literals = std::move(literals);
}

bool Clause::check_satisfiability(Assignment *assignment) {
    for (int literal : literals) {
        if (literal < 0 && assignment->variable_assignment[-literal] == 0)
            return true;
        if (literal > 0 && assignment->variable_assignment[literal] == 1)
            return true;
    }
    return false;
}