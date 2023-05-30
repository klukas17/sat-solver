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
    this->last_conflict_level = 0;
}

void DPLL::solve() {

    // 155232014

//    auto x = 155232014;
//    std::cout << x << std::endl;
//    gen.seed(x);

#ifdef SUBSUMPTION
    subsumption();
#endif
    
    if (!assign_next_variable(1)) {
        std::cout << "UNSAT" << std::endl;
    }

    else {
        std::cout << "SAT:" << std::endl;
        for (auto &it : assignment->variable_assignment)
            if (assignment->variable_assignment[it.first] != -1)
                std::cout << "\tx" << it.first << " = " << it.second << std::endl;
    }
}

bool DPLL::assign_next_variable(int level) {

    bool satisfied = false, contradiction = false;
    int conflict_level = 0;

    std::vector<int> unit_clause_variables;
    bool continue_searching_for_unit_clause_variables = true;

    do {
        if (!assign_unit_clause_variables(unit_clause_variables, continue_searching_for_unit_clause_variables, level)) {
            restore_assignments(unit_clause_variables);
            return false;
        }
        cnf->check_satisfiability(assignment, satisfied, contradiction, conflict_level);
        if (satisfied) return true;
        if (contradiction) {
            restore_assignments(unit_clause_variables);
            last_conflict_level = conflict_level;
            return false;
        }
    } while (continue_searching_for_unit_clause_variables);

    std::vector<int> pure_literal_variables;
    bool continue_searching_for_pure_literal_variables = true;

    do {
        if (!assign_pure_literal_variables(pure_literal_variables, continue_searching_for_pure_literal_variables, level)) {
            restore_assignments(unit_clause_variables);
            restore_assignments(pure_literal_variables);
            last_conflict_level = conflict_level;
            return false;
        }
        cnf->check_satisfiability(assignment, satisfied, contradiction, conflict_level);
        if (satisfied) return true;
        if (contradiction) {
            restore_assignments(unit_clause_variables);
            restore_assignments(pure_literal_variables);
            last_conflict_level = conflict_level;
            return false;
        }
    } while (continue_searching_for_pure_literal_variables);

    if (assignment->unassigned_variables.empty()){
        restore_assignments(unit_clause_variables);
        restore_assignments(pure_literal_variables);
        last_conflict_level = conflict_level;
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
        assign_variable(variable, value, level);
        cnf->check_satisfiability(assignment, satisfied, contradiction, conflict_level);
        if (satisfied) return true;
        if (contradiction) {
            restore_assignments(unit_clause_variables);
            restore_assignments(pure_literal_variables);
            last_conflict_level = conflict_level;
            return false;
        }
        else {
            if (assign_next_variable(level + 1)) {
                return true;
            }
#ifdef IMPLICATION_GRAPH_WITH_BACKJUMPING
            restore_assignment(variable);
            if (last_conflict_level > 0 && last_conflict_level < level)
                break;
            if (last_conflict_level == level)
                last_conflict_level = 0;
#endif
        }
    }

    restore_assignments(unit_clause_variables);
    restore_assignments(pure_literal_variables);
    return false;
}

bool DPLL::assign_unit_clause_variables(std::vector<int> &unit_clause_variables, bool &continue_searching_for_unit_clause_variables, int level) const {

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
        assign_variable(var, val, level);
    }

    return true;
}

bool DPLL::assign_pure_literal_variables(std::vector<int> &pure_literal_variables, bool &continue_searching_for_pure_literal_variables, int level) const {

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
            assign_variable(i, 1, level);
            found_pure_literals++;
        }
        else if (literal_occurrences[-i] > 0 && literal_occurrences[i] == 0) {
            if (assignment->variable_assignment[i] == 1)
                return false;
            pure_literal_variables.push_back(i);
            assign_variable(i, 0, level);
            found_pure_literals++;
        }
    }

    if (found_pure_literals == 0)
        continue_searching_for_pure_literal_variables = false;

    return true;
}

void DPLL::restore_assignments(std::vector<int> &variables) const {
    for (auto variable : variables)
        restore_assignment(variable);
}

void DPLL::restore_assignment(int variable) const {
    assignment->variable_assignment[variable] = -1;
    assignment->unassigned_variables.insert(variable);
}

void DPLL::assign_variable(int variable, int value, int level) const {
    assignment->variable_assignment[variable] = value;
    assignment->variable_assignment_level[variable] = level;
    assignment->unassigned_variables.erase(variable);
}

#ifdef SUBSUMPTION

bool inline is_subset(const std::set<int>& s1, const std::set<int>& s2) {
    return std::all_of(s1.begin(), s1.end(), [&s2](const int& elem) {
        return s2.find(elem) != s2.end();
    });
}

void DPLL::subsumption() const {
    std::vector<Clause*> one_literal_clauses, other_clauses;
    for (auto clause : cnf->clauses)
        if (clause->literals.size() == 1)
            one_literal_clauses.push_back(clause);
        else
            other_clauses.push_back(clause);
    std::vector<Clause*> new_one_literal_clauses, current_one_literal_clauses;
    do {
        if (new_one_literal_clauses.empty())
            current_one_literal_clauses = one_literal_clauses;
        else
            current_one_literal_clauses = new_one_literal_clauses;
        new_one_literal_clauses.clear();
        for (auto clause1 : current_one_literal_clauses) {
            for (auto clause2 : other_clauses) {
                if (clause2->clause_eliminated || clause2->literals.size() == 1)
                    continue;
                if (is_subset(clause1->literals, clause2->literals))
                    clause2->clause_eliminated = true;
                else {
                    int literal = *clause1->literals.begin();
                    if (clause2->literals.find(-literal) != clause2->literals.end()) {
                        clause2->literals.erase(-literal);
                        if (clause2->literals.size() == 1)
                            new_one_literal_clauses.push_back(clause2);
                    }
                }
            }
        }
    } while (!new_one_literal_clauses.empty());
    std::vector<Clause*> remaining_clauses;
    remaining_clauses.reserve(one_literal_clauses.size());
    for (auto clause : one_literal_clauses)
            remaining_clauses.push_back(clause);
    for (auto clause : other_clauses)
        if (!clause->clause_eliminated)
            remaining_clauses.push_back(clause);

    for (auto clause1 : remaining_clauses) {
        if (clause1->clause_eliminated)
            continue;
        for (auto clause2 : remaining_clauses) {
            if (clause2->clause_eliminated || clause1 == clause2)
                continue;
            if (is_subset(clause1->literals, clause2->literals))
                clause2->clause_eliminated = true;
        }
    }

    std::vector<Clause*> final_clauses;
    for (auto clause : remaining_clauses)
        if (!clause->clause_eliminated)
            final_clauses.push_back(clause);

    std::cout << "Clauses before subsumption: " << cnf->clauses.size() << std::endl;
    std::cout << "Clauses after subsumption: " << final_clauses.size() << std::endl;
    cnf->clauses = final_clauses;
}

#endif