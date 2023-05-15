//
// Created by mihael on 15/05/23.
//

#include "CNF.h"

#include <utility>

CNF::CNF(std::vector<Clause *> clauses) {
    this->clauses = std::move(clauses);
}