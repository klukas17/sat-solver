//
// Created by mihael on 15/05/23.
//

#include "DPLL.h"
#include "vector"
#include "iostream"
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
    fix_next_variable();
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

void DPLL::fix_next_variable() {

    std::vector<int> unit_clause_variables;
    cnf->evaluate(assignment);

    for (auto clause : cnf->clauses) {
        if (clause->last_evaluation == 1)
            continue;
        int free_literals = 0;
        int free_literal = 0;
        for (auto literal : clause->literals) {
            int literal_abs = literal;
            if (literal_abs < 0)
                literal_abs = -literal_abs;
            if (assignment->variable_assignment[literal_abs] == -1) {
                free_literal = literal;
                free_literals++;
                if (free_literals > 1)
                    break;
            }
        }
        if (free_literals == 1)
            unit_clause_variables.push_back(free_literal);
    }

    std::vector<int> possible_assignments;
    int variable;

    if (unit_clause_variables.empty()) {
        auto it = assignment->unassigned_variables.begin();
        std::advance(it, dis(gen) % assignment->unassigned_variables.size());
        variable = *it;
        possible_assignments = {0, 1};
    }

    else {
        variable = unit_clause_variables[dis(gen) % unit_clause_variables.size()];
        if (variable > 0) {
            possible_assignments = {1};
        }
        else {
            variable = -variable;
            possible_assignments = {0};
        }
    }

    for (auto value : possible_assignments) {
        assignment->variable_assignment[variable] = value;
        assignment->unassigned_variables.erase(variable);
        if (cnf->check_satisfiability(assignment)) {
            solution_found = true;
            break;
        }
        else if (assignment->unassigned_variables.empty()) {
            assignment->variable_assignment[variable] = -1;
            assignment->unassigned_variables.insert(variable);
        }
        else {
            fix_next_variable();
            if (solution_found)
                break;
            assignment->variable_assignment[variable] = -1;
            assignment->unassigned_variables.insert(variable);
        }
    }
}