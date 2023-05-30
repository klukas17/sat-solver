//
// Created by mihael on 15/05/23.
//

#include "DPLL.h"
#include "vector"
#include "iostream"
#include "climits"
#include "algorithm"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, INT_MAX);

DPLL::DPLL(Assignment *assignment, CNF *cnf) {
    this->assignment = assignment;
    this->cnf = cnf;
}

void DPLL::solve() {

#ifdef SUBSUMPTION
    subsumption();
#endif
    
    if (!assign_next_variable()) {
        std::cout << "UNSAT" << std::endl;
    }

    else {
        std::cout << "SAT:" << std::endl;
        for (auto &it : assignment->variable_assignment)
            if (assignment->variable_assignment[it.first] != -1)
                std::cout << "\tx" << it.first << " = " << it.second << std::endl;
    }
}

bool DPLL::assign_next_variable() {

    bool satisfied = false, contradiction = false;

    std::vector<int> unit_clause_variables;
    bool continue_searching_for_unit_clause_variables = true;

    do {
        if (!assign_unit_clause_variables(unit_clause_variables, continue_searching_for_unit_clause_variables)) {
            restore_assignments(unit_clause_variables);
            return false;
        }
        cnf->check_satisfiability(assignment, satisfied, contradiction);
        if (satisfied) return true;
        if (contradiction) {
            restore_assignments(unit_clause_variables);
            return false;
        }
    } while (continue_searching_for_unit_clause_variables);

    std::vector<int> pure_literal_variables;
    bool continue_searching_for_pure_literal_variables = true;

    do {
        if (!assign_pure_literal_variables(pure_literal_variables, continue_searching_for_pure_literal_variables)) {
            restore_assignments(unit_clause_variables);
            restore_assignments(pure_literal_variables);
            return false;
        }
        cnf->check_satisfiability(assignment, satisfied, contradiction);
        if (satisfied) return true;
        if (contradiction) {
            restore_assignments(unit_clause_variables);
            restore_assignments(pure_literal_variables);
            return false;
        }
    } while (continue_searching_for_pure_literal_variables);

    if (assignment->unassigned_variables.empty()){
        restore_assignments(unit_clause_variables);
        restore_assignments(pure_literal_variables);
        return false;
    }

    int variable;
    std::vector<int> possible_assignments;

    auto it = assignment->unassigned_variables.begin();
    std::advance(it, dis(gen) % assignment->unassigned_variables.size());
    variable = *it;
    possible_assignments.push_back(dis(gen) % 2);
    if (possible_assignments[0] == 0)
        possible_assignments.push_back(1);
    else
        possible_assignments.push_back(0);

    for (auto value : possible_assignments) {
        assign_variable(variable, value);
        cnf->check_satisfiability(assignment, satisfied, contradiction);
        if (satisfied) return true;
        if (contradiction) {
            restore_assignments(unit_clause_variables);
            restore_assignments(pure_literal_variables);
            return false;
        }
        else {
            if (assign_next_variable())
                return true;
            restore_assignment(variable);
        }
    }

    restore_assignments(unit_clause_variables);
    restore_assignments(pure_literal_variables);
    return false;
}

bool DPLL::assign_unit_clause_variables(std::vector<int> &unit_clause_variables, bool &continue_searching_for_unit_clause_variables) {

    std::set<int> found_unit_clause_variables;

    cnf->evaluate_clauses(assignment);

    for (auto clause : cnf->clauses) {
        if (clause->last_evaluation)
            continue;
        int found_free_literal_count = 0;
        int found_free_literal = 0;
        for (auto literal : clause->literals) {
            if (assignment->variable_assignment[literal > 0 ? literal : -literal] == -1) {
                found_free_literal_count++;
                if (found_free_literal_count > 1)
                    break;
                found_free_literal = literal;
            }
        }
        if (found_free_literal_count == 1) {
            if (found_unit_clause_variables.find(-found_free_literal) != found_unit_clause_variables.end())
                return false;
            found_unit_clause_variables.insert(found_free_literal);
        }
    }

    if (found_unit_clause_variables.empty())
        continue_searching_for_unit_clause_variables = false;

    for (auto literal_found : found_unit_clause_variables) {
        int var = literal_found > 0 ? literal_found : -literal_found;
        int val = literal_found > 0 ? 1 : 0;
        unit_clause_variables.push_back(var);
        assign_variable(var, val);
    }

    return true;
}

bool DPLL::assign_pure_literal_variables(std::vector<int> &pure_literal_variables, bool &continue_searching_for_pure_literal_variables) {

    cnf->evaluate_clauses(assignment);

    std::map<int, int> literal_occurrences;
    std::map<int, int> blacklisted_literals;
    for (int i = 1; i <= cnf->number_of_variables; i++) {
        literal_occurrences[i] = literal_occurrences[-i] = 0;
        blacklisted_literals[i] = 0;
    }

    for (auto clause : cnf->clauses) {
        if (clause->last_evaluation)
            continue;
        for (auto literal : clause->literals) {
            if (!blacklisted_literals[literal > 0 ? literal : -literal]) {
                if (literal_occurrences[-literal] == 0)
                    literal_occurrences[literal]++;
                else
                    blacklisted_literals[literal > 0 ? literal : -literal] = 1;
            }
        }
    }

    int found_pure_literals = 0;

    for (int i = 1; i <= cnf->number_of_variables; i++) {
        if (blacklisted_literals[i] || assignment->variable_assignment[i] != -1)
            continue;
        if (literal_occurrences[i] > 0 && literal_occurrences[-i] == 0) {
            if (assignment->variable_assignment[i] == 0)
                return false;
            pure_literal_variables.push_back(i);
            assign_variable(i, 1);
            found_pure_literals++;
        }
        else if (literal_occurrences[-i] > 0 && literal_occurrences[i] == 0) {
            if (assignment->variable_assignment[i] == 1)
                return false;
            pure_literal_variables.push_back(i);
            assign_variable(i, 0);
            found_pure_literals++;
        }
    }

    if (found_pure_literals == 0)
        continue_searching_for_pure_literal_variables = false;

    return true;
}

void DPLL::restore_assignments(std::vector<int> &variables) {
    for (auto variable : variables)
        restore_assignment(variable);
}

void DPLL::restore_assignment(int variable) {
    assignment->variable_assignment[variable] = -1;
    assignment->unassigned_variables.insert(variable);
}

void DPLL::assign_variable(int variable, int value) {
    assignment->variable_assignment[variable] = value;
    assignment->unassigned_variables.erase(variable);
}

#ifdef SUBSUMPTION

bool inline is_subset(const std::set<int>& s1, const std::set<int>& s2) {
    return std::all_of(s1.begin(), s1.end(), [&s2](const int& elem) {
        return s2.find(elem) != s2.end();
    });
}

void DPLL::subsumption() {
    std::vector<Clause*> one_literal_clauses, other_clauses;
    for (auto clause : cnf->clauses)
        if (clause->literals.size() == 1)
            one_literal_clauses.push_back(clause);
        else
            other_clauses.push_back(clause);
#ifdef SUBSUMPTION_EXTENSION
    std::vector<Clause*> new_one_literal_clauses, current_one_literal_clauses;
    do {
        if (new_one_literal_clauses.empty())
            current_one_literal_clauses = one_literal_clauses;
        else
            current_one_literal_clauses = new_one_literal_clauses;
        new_one_literal_clauses.clear();
#else
        std::vector<Clause*> current_one_literal_clauses(one_literal_clauses);
#endif
        for (auto clause1 : current_one_literal_clauses) {
            for (auto clause2 : other_clauses) {
                if (clause2->clause_eliminated)
                    continue;
                if (is_subset(clause1->literals, clause2->literals))
                    clause2->clause_eliminated = true;
#ifdef SUBSUMPTION_EXTENSION
                else {
                    int literal = *clause1->literals.begin();
                    if (clause2->literals.find(-literal) != clause2->literals.end()) {
                        clause2->literals.erase(-literal);
                        if (clause2->literals.size() == 1)
                            new_one_literal_clauses.push_back(clause2);
                    }
                }
#endif
            }
        }
#ifdef SUBSUMPTION_EXTENSION
    } while (!new_one_literal_clauses.empty());
#endif
    std::vector<Clause*> remaining_clauses;
    for (auto clause : one_literal_clauses)
        remaining_clauses.push_back(clause);
    for (auto clause : other_clauses)
        if (!clause->clause_eliminated)
            remaining_clauses.push_back(clause);

    std::cout << "Clauses before subsumption: " << cnf->clauses.size() << std::endl;
    std::cout << "Clauses after subsumption: " << remaining_clauses.size() << std::endl;
    cnf->clauses = remaining_clauses;
}

#endif