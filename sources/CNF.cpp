//
// Created by mihael on 15/05/23.
//

#include "CNF.h"
#include <utility>

CNF::CNF(std::vector<Clause *> clauses, int number_of_variables) {
    this->clauses = std::move(clauses);
    this->number_of_variables = number_of_variables;
}

void CNF::check_satisfiability(Assignment *assignment, bool &satisfied, bool &contradiction, int &conflict_level) {
#ifdef IMPLICATION_GRAPH_WITH_BACKJUMPING
    bool contradiction_found = false;
    bool not_satisfied = false;
#endif
    satisfied = false;
    contradiction = false;
    for (auto clause : clauses) {
        clause->check_satisfiability(assignment, satisfied, contradiction);
#ifndef IMPLICATION_GRAPH_WITH_BACKJUMPING
        if (!satisfied || contradiction)
            return;
#else
        if (!satisfied)
            not_satisfied = true;
        if (contradiction) {
            contradiction_found = true;
            int max_conflict_level = 0;
            for (auto literal : clause->literals)
                if (assignment->variable_assignment_level[literal] > max_conflict_level)
                    max_conflict_level = assignment->variable_assignment_level[literal];
            if (conflict_level == 0 || conflict_level > max_conflict_level)
                conflict_level = max_conflict_level;
        }
#endif
    }
#ifdef IMPLICATION_GRAPH_WITH_BACKJUMPING
    if (contradiction_found)
        contradiction = true;
    if (not_satisfied)
        satisfied = false;
#endif
}

void CNF::evaluate_clauses(Assignment *assignment) {
    for (auto clause : clauses)
        clause->evaluate(assignment);
}