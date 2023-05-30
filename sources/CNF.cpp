//
// Created by mihael on 15/05/23.
//

#include "CNF.h"
#include <utility>

CNF::CNF(std::vector<Clause *> clauses, int number_of_variables) {
    this->clauses = std::move(clauses);
    this->number_of_variables = number_of_variables;
    this->evaluations_for_decay = 0;
}

void CNF::check_satisfiability(Assignment *assignment, bool &satisfied, bool &contradiction, int &conflict_level, Clause* &contradiction_clause) {
#ifdef VARIABLE_STATE_INDEPENDENT_DECAYING_SUM
    evaluations_for_decay++;
    if (evaluations_for_decay >= DECAY_PERIOD) {
        evaluations_for_decay = 0;
        decay_variable_scores(assignment);
    }
#endif
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
            if (conflict_level == 0 || conflict_level > max_conflict_level) {
                conflict_level = max_conflict_level;
                contradiction_clause = clause;
            }
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

#ifdef VARIABLE_STATE_INDEPENDENT_DECAYING_SUM

void CNF::reset_variable_scores(Assignment *assignment) {
    for (int i = 1; i <= assignment->number_of_literals; i++)
        assignment->variable_scores[i] = 0.0;
    for (auto clause : clauses)
        for (auto literal : clause->literals)
            assignment->variable_scores[literal > 0 ? literal : -literal]++;
}

void CNF::decay_variable_scores(Assignment *assignment) {
    for (int i = 1; i <= assignment->number_of_literals; i++)
        assignment->variable_scores[i] *= DECAY_FACTOR;
}

#endif