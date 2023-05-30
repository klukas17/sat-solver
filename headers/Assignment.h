//
// Created by mihael on 15/05/23.
//

#ifndef SAT_SOLVER_ASSIGNMENT_H
#define SAT_SOLVER_ASSIGNMENT_H

#include "extensions.h"
#include "map"
#include "set"

class Assignment {
public:
    int number_of_literals;
    std::map<int, int> variable_assignment;
    std::map<int, int> variable_assignment_level;
    std::set<int> unassigned_variables;
    std::map<int, double> variable_scores;

    explicit Assignment(int number_of_literals);
};


#endif //SAT_SOLVER_ASSIGNMENT_H
