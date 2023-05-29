//
// Created by mihael on 15/05/23.
//

#include "CNF.h"
#include <utility>

CNF::CNF(std::vector<Clause *> clauses, int number_of_variables) {
    this->clauses = std::move(clauses);
    this->number_of_variables = number_of_variables;
}

void CNF::check_satisfiability(Assignment *assignment, bool &satisfied, bool &contradiction) {
    satisfied = false;
    contradiction = false;
    for (auto clause : clauses) {
        clause->check_satisfiability(assignment, satisfied, contradiction);
        if (!satisfied || contradiction)
            return;
    }
}

void CNF::evaluate_clauses(Assignment *assignment) {
    for (auto clause : clauses)
        clause->evaluate(assignment);
}