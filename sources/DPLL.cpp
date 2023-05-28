//
// Created by mihael on 15/05/23.
//

#include "DPLL.h"
#include "vector"
#include "iostream"
#include <random>
#include "climits"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, INT_MAX);

DPLL::DPLL(Assignment *assignment, CNF *cnf) {
    this->assignment = assignment;
    this->cnf = cnf;
    this->solution_found = false;
}

void DPLL::solve() {
    int next_variable = choose_next_variable();
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
        int next_variable = choose_next_variable();
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

int DPLL::choose_next_variable() const {
    auto it = assignment->unassigned_variables.begin();
    std::advance(it, dis(gen) % assignment->unassigned_variables.size());
    return *it;
}