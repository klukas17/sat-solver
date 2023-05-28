//
// Created by mihael on 15/05/23.
//

#include "DPLL.h"
#include "vector"
#include "iostream"

static std::vector<int> possible_assignments = {0, 1};

DPLL::DPLL(Assignment *assignment, CNF *cnf) {
    this->assignment = assignment;
    this->cnf = cnf;
    this->solution_found = false;
}

void DPLL::solve() {
    int next_variable = *assignment->unassigned_variables.begin();
    fix_variable(next_variable, 0);
    if (!solution_found)
        fix_variable(next_variable, 1);
    if (!solution_found) {
        std::cout << "UNSAT" << std::endl;
    }
    else {
        std::cout << "SAT:" << std::endl;
        for (auto &it : assignment->variable_assignment)
            if (assignment->variable_assignment[it.first] != -1)
                std::cout << "\tx" << it.first << " = " << it.second << std::endl;
    }
}

void DPLL::fix_variable(int variable, int value) {
    assignment->variable_assignment[variable] = value;
    assignment->unassigned_variables.erase(variable);
    if (cnf->check_satisfiability(assignment)) {
        solution_found = true;
        return;
    }
    else if (assignment->unassigned_variables.empty()) {
        assignment->variable_assignment[variable] = -1;
        assignment->unassigned_variables.insert(variable);
        return;
    }
    else {
        int next_variable = *assignment->unassigned_variables.begin();
        fix_variable(next_variable, 0);
        if (solution_found)
            return;
        fix_variable(next_variable, 1);
        if (solution_found)
            return;
        assignment->variable_assignment[variable] = -1;
        assignment->unassigned_variables.insert(variable);
        return;
    }
}