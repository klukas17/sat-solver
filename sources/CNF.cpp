//
// Created by mihael on 15/05/23.
//

#include "CNF.h"
#include <utility>

CNF::CNF(std::vector<Clause *> clauses, int number_of_literals) {
    this->clauses = std::move(clauses);
    this->number_of_variables = number_of_literals;
}

bool CNF::check_satisfiability(Assignment *assignment) {
    bool result = true;
    for (Clause* clause : clauses) {
        result = clause->check_satisfiability(assignment);
        if (!result)
            break;
    }
    return result;
}