//
// Created by mihael on 15/05/23.
//

#ifndef SAT_SOLVER_ASSIGNMENT_H
#define SAT_SOLVER_ASSIGNMENT_H

#include "map"
#include "set"

class Assignment {
public:
    std::map<int, int> variable_assignment;
    std::set<int> unassigned_variables;
    explicit Assignment(int number_of_literals);
};


#endif //SAT_SOLVER_ASSIGNMENT_H
