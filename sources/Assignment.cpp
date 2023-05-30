//
// Created by mihael on 15/05/23.
//

#include "Assignment.h"

Assignment::Assignment(int number_of_literals) {
    for (int i = 1; i <= number_of_literals; i++) {
        unassigned_variables.insert(i);
        variable_assignment[i] = -1;
        variable_assignment_level[i] = 0;
    }
}