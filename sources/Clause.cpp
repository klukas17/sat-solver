//
// Created by mihael on 15/05/23.
//

#include "Clause.h"

#include <utility>

Clause::Clause(std::set<int> literals) {
    this->literals = std::move(literals);
    this->clause_eliminated = false;
    this->last_evaluation = -1;
}

void Clause::check_satisfiability(Assignment *assignment, bool &satisfied, bool &contradiction) {
    satisfied = false;
    contradiction = false;
    for (auto literal : literals) {
        if (literal < 0 && assignment->variable_assignment[-literal] == 0 || literal > 0 && assignment->variable_assignment[literal] == 1) {
            satisfied = true;
            return;
        }
    }
    contradiction = true;
    for (auto literal : literals) {
        if (!(literal > 0 && assignment->variable_assignment[literal] == 0 || literal < 0 && assignment->variable_assignment[-literal] == 1)) {
            contradiction = false;
            break;
        }
    }
}

void Clause::evaluate(Assignment *assignment) {
    for (auto literal : literals) {
        if (literal > 0 && assignment->variable_assignment[literal] == 1) {
            last_evaluation = true;
            return;
        }
        if (literal < 0 && assignment->variable_assignment[-literal] == 0) {
            last_evaluation = true;
            return;
        }
    }
    last_evaluation = false;
}