//
// Created by mihael on 15/05/23.
//

#include "Clause.h"

#include <utility>

Clause::Clause(std::set<int> literals, std::set<int> negated_literals) {
    this->literals = std::move(literals);
    this->negated_literals = std::move(negated_literals);
}