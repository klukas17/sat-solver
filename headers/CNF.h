//
// Created by mihael on 15/05/23.
//

#ifndef SAT_SOLVER_CNF_H
#define SAT_SOLVER_CNF_H

#include "extensions.h"
#include "Clause.h"
#include "vector"
#include "Assignment.h"

class CNF {
public:
    std::vector<Clause*> clauses;
    int number_of_variables;
    int evaluations_for_decay;

    CNF(std::vector<Clause*> clauses, int number_of_variables);
    void check_satisfiability(Assignment* assignment, bool &satisfied, bool &contradiction, int &conflict_level, Clause* &contradiction_clause);
    void evaluate_clauses(Assignment* assignment);

#ifdef VARIABLE_STATE_INDEPENDENT_DECAYING_SUM
    constexpr static double DECAY_FACTOR = 0.9;
    constexpr static double CONTRADICTION_SCORE_BOOST = 1;
    const int DECAY_PERIOD = 50;
    void reset_variable_scores(Assignment* assignment);
    void decay_variable_scores(Assignment *assignment);
#endif
};


#endif //SAT_SOLVER_CNF_H
