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

#ifdef BOUNDED_VARIABLE_ELIMINATION
    std::vector<Clause*> pre_bve_clauses = cnf->clauses;
#endif

#ifdef SUBSUMPTION
    subsumption();
#endif

#ifdef BOUNDED_VARIABLE_ELIMINATION
    bool success = true;
    while (success) {
        success = bounded_variable_elimination();
#ifdef SUBSUMPTION
        subsumption();
#endif
    }
#endif

    std::cout << "Clauses after preprocessing: " << std::endl;
    for (auto clause : cnf->clauses) {
        for (auto literal : clause->literals)
            std::cout << literal << " ";
        std::cout << std::endl;
    }

#ifdef VARIABLE_STATE_INDEPENDENT_DECAYING_SUM
    cnf->reset_variable_scores(assignment);
#endif

    bool sat = assign_next_variable(1);

#ifdef BOUNDED_VARIABLE_ELIMINATION

    if (sat) {

        std::cout << "Inserting found solutions as unit clauses to the original problem" << std::endl;

        for (auto it : assignment->variable_assignment) {
            int var = it.first;
            if (assignment->variable_assignment[var] != -1) {
                std::set<int> literals;
                if (assignment->variable_assignment[var] == 1)
                    literals.insert(var);
                else if (assignment->variable_assignment[var] == 0)
                    literals.insert(-var);
                pre_bve_clauses.push_back(new Clause(literals));
            }
        }

        for (auto clause : pre_bve_clauses)
            clause->clause_eliminated = false;

        cnf->clauses = pre_bve_clauses;
        subsumption();
        sat = assign_next_variable(1);
    }

#endif

    if (!sat) {
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

    std::cout << "Exploring new variable assignment on level " << level << std::endl;

    bool satisfied = false, contradiction = false;
    int conflict_level = 0;
    std::vector<Clause*> contradiction_clauses;

    std::vector<int> unit_clause_variables;
    bool continue_searching_for_unit_clause_variables = true;

    do {
        if (!assign_unit_clause_variables(unit_clause_variables, continue_searching_for_unit_clause_variables, level)) {
            restore_assignments(unit_clause_variables);
            std::cout << "Found contradiction at level " << level - 1 << std::endl;
            return false;
        }
        cnf->check_satisfiability(assignment, satisfied, contradiction, conflict_level, contradiction_clauses);
        if (satisfied) return true;
        if (contradiction) {
            restore_assignments(unit_clause_variables);
            last_conflict_level = conflict_level;
#ifdef VARIABLE_STATE_INDEPENDENT_DECAYING_SUM
            for (auto clause : contradiction_clauses)
                for (auto literal : clause->literals)
                    assignment->variable_scores[literal > 0 ? literal : -literal] += CNF::CONTRADICTION_SCORE_BOOST;
            contradiction_clauses.clear();
            std::cout << "Found contradiction at level " << conflict_level << std::endl;
#endif
            return false;
        }
    } while (continue_searching_for_unit_clause_variables);

    std::vector<int> pure_literal_variables;
    bool continue_searching_for_pure_literal_variables = true;

    do {
        if (!assign_pure_literal_variables(pure_literal_variables, continue_searching_for_pure_literal_variables, level)) {
            restore_assignments(unit_clause_variables);
            restore_assignments(pure_literal_variables);
            std::cout << "Found contradiction at level " << level - 1 << std::endl;
            return false;
        }
        cnf->check_satisfiability(assignment, satisfied, contradiction, conflict_level, contradiction_clauses);
        if (satisfied) return true;
        if (contradiction) {
            restore_assignments(unit_clause_variables);
            restore_assignments(pure_literal_variables);
            last_conflict_level = conflict_level;
#ifdef VARIABLE_STATE_INDEPENDENT_DECAYING_SUM
            for (auto clause : contradiction_clauses)
                for (auto literal : clause->literals)
                    assignment->variable_scores[literal > 0 ? literal : -literal] += CNF::CONTRADICTION_SCORE_BOOST;
            contradiction_clauses.clear();
            std::cout << "Found contradiction at level " << conflict_level << std::endl;
#endif
            return false;
        }
    } while (continue_searching_for_pure_literal_variables);

    if (assignment->unassigned_variables.empty()){
        restore_assignments(unit_clause_variables);
        restore_assignments(pure_literal_variables);
        last_conflict_level = conflict_level;
        std::cout << "Found contradiction at level " << level - 1 << std::endl;
        return false;
    }

    int variable;
    std::vector<int> possible_assignments;

#ifdef VARIABLE_STATE_INDEPENDENT_DECAYING_SUM

    std::vector<int> highest_score_variables;
    double high_score = 0;

    for (auto var : assignment->unassigned_variables) {
        if (assignment->variable_scores[var] > high_score) {
            highest_score_variables.clear();
            high_score = assignment->variable_scores[var];
        }
        if (assignment->variable_scores[var] == high_score)
            highest_score_variables.push_back(var);
    }

    variable = highest_score_variables[dis(gen) % highest_score_variables.size()];

    int positive_occurrences = 0, negative_occurrences = 0;

    for (auto clause : cnf->clauses) {
        for (auto literal : clause->literals) {
            if (literal == variable)
                positive_occurrences++;
            if (literal == -variable)
                negative_occurrences++;
        }
    }

    if (positive_occurrences > negative_occurrences) {
        possible_assignments.push_back(1);
        possible_assignments.push_back(0);
    }

    else if (positive_occurrences < negative_occurrences) {
        possible_assignments.push_back(0);
        possible_assignments.push_back(1);
    }

    else {
        possible_assignments.push_back(dis(gen) % 2);
        if (possible_assignments[0] == 0)
            possible_assignments.push_back(1);
        else
            possible_assignments.push_back(0);
    }

#else

    auto it = assignment->unassigned_variables.begin();
    std::advance(it, dis(gen) % assignment->unassigned_variables.size());
    variable = *it;

    possible_assignments.push_back(dis(gen) % 2);
    if (possible_assignments[0] == 0)
        possible_assignments.push_back(1);
    else
        possible_assignments.push_back(0);

#endif

    for (auto value : possible_assignments) {
        std::cout << "On level " << level << ", assigning value " << value << " to x" << variable << std::endl;
        assign_variable(variable, value, level);
        cnf->check_satisfiability(assignment, satisfied, contradiction, conflict_level, contradiction_clauses);
        if (satisfied) return true;
        if (contradiction) {
            restore_assignments(unit_clause_variables);
            restore_assignments(pure_literal_variables);
            last_conflict_level = conflict_level;
#ifdef VARIABLE_STATE_INDEPENDENT_DECAYING_SUM
            for (auto clause : contradiction_clauses)
                for (auto literal : clause->literals)
                    assignment->variable_scores[literal > 0 ? literal : -literal] += CNF::CONTRADICTION_SCORE_BOOST;
            contradiction_clauses.clear();
            std::cout << "Found contradiction at level " << conflict_level << std::endl;
#endif
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
    std::cout << "Found contradiction at level " << level - 1 << std::endl;
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

#ifdef BOUNDED_VARIABLE_ELIMINATION

struct BoundedVariableEliminationMapElement {
    int positive_occurrences = 0;
    int negative_occurrences = 0;
    std::vector<Clause*> clauses_with_positive_occurrences;
    std::vector<Clause*> clauses_with_negative_occurrences;
};

bool DPLL::bounded_variable_elimination() const {

    std::cout << "Starting bounded variable elimination" << std::endl;

    std::map<int, BoundedVariableEliminationMapElement*> bounded_variable_elimination_map;
    std::set<int> variables;

    std::vector<Clause*> clauses;
    std::vector<Clause*> unit_clauses;

    bool variable_eliminated = false;

    for (auto clause : cnf->clauses)
        if (clause->literals.size() == 1)
            unit_clauses.push_back(clause);
        else if (clause->literals.size() > 1) {
            clauses.push_back(clause);
            for (auto literal : clause->literals)
                bounded_variable_elimination_map[literal > 0 ? literal : -literal] = nullptr;
        }

    for (auto it : bounded_variable_elimination_map)
        bounded_variable_elimination_map[it.first] = new BoundedVariableEliminationMapElement();

    for (auto clause : clauses)
        for (auto literal : clause->literals) {
            if (literal > 0) {
                bounded_variable_elimination_map[literal]->positive_occurrences++;
                bounded_variable_elimination_map[literal]->clauses_with_positive_occurrences.push_back(clause);
            }
            else {
                bounded_variable_elimination_map[-literal]->negative_occurrences++;
                bounded_variable_elimination_map[-literal]->clauses_with_negative_occurrences.push_back(clause);
            }
        }

    int max_products = 1;
    int variables_eliminated = 0;
    while (max_products <= 1000 && variables_eliminated < 1) {

        bool found = false;

        for (auto it : bounded_variable_elimination_map) {
            int var = it.first;

            if (!bounded_variable_elimination_map[var])
                continue;

            if (bounded_variable_elimination_map[var]->positive_occurrences * bounded_variable_elimination_map[var]->negative_occurrences > 0 &&
                bounded_variable_elimination_map[var]->positive_occurrences * bounded_variable_elimination_map[var]->negative_occurrences <= max_products) {

                found = true;

                std::vector<Clause*> new_clauses;

                for (auto clause1 : bounded_variable_elimination_map[var]->clauses_with_positive_occurrences) {
                    if (clause1->clause_eliminated)
                        continue;
                    for (auto clause2 : bounded_variable_elimination_map[var]->clauses_with_negative_occurrences) {
                        if (clause2->clause_eliminated)
                            continue;
                        std::set<int> new_clause_literals;
                        for (auto literal : clause1->literals)
                            if (literal != var && -literal != var)
                                new_clause_literals.insert(literal);
                        for (auto literal : clause2->literals)
                            if (literal != var && -literal != var)
                                new_clause_literals.insert(literal);
                        bool tautology = false;
                        for (auto literal : new_clause_literals)
                            if (new_clause_literals.find(-literal) != new_clause_literals.end()) {
                                tautology = true;
                                break;
                            }
                        if (tautology)
                            continue;
                        new_clauses.push_back(new Clause(new_clause_literals));
                    }
                }

                for (auto clause : bounded_variable_elimination_map[var]->clauses_with_positive_occurrences) {
                    if (clause->clause_eliminated)
                        continue;
                    clause->clause_eliminated = true;
                    for (auto literal : clause->literals) {
                        if (literal > 0)
                            bounded_variable_elimination_map[literal]->positive_occurrences--;
                        else
                            bounded_variable_elimination_map[-literal]->negative_occurrences--;
                    }
                }

                for (auto clause : bounded_variable_elimination_map[var]->clauses_with_negative_occurrences) {
                    if (clause->clause_eliminated)
                        continue;
                    clause->clause_eliminated = true;
                    for (auto literal : clause->literals) {
                        if (literal > 0)
                            bounded_variable_elimination_map[literal]->positive_occurrences--;
                        else
                            bounded_variable_elimination_map[-literal]->negative_occurrences--;
                    }
                }

                for (auto clause : new_clauses) {
                    clauses.push_back(clause);
                    for (auto literal : clause->literals) {
                        if (literal > 0) {
                            bounded_variable_elimination_map[literal]->positive_occurrences++;
                            bounded_variable_elimination_map[literal]->clauses_with_positive_occurrences.push_back(clause);
                        }
                        else {
                            bounded_variable_elimination_map[-literal]->negative_occurrences++;
                            bounded_variable_elimination_map[-literal]->clauses_with_negative_occurrences.push_back(clause);
                        }
                    }
                }

                delete bounded_variable_elimination_map[var];
                bounded_variable_elimination_map[var] = nullptr;

                std::cout << "Eliminated variable " << var << std::endl;
                variables_eliminated++;
                variable_eliminated = true;

                break;
            }
        }

        if (!found)
            max_products++;
    }

    std::vector<Clause*> final_clauses;

    final_clauses.reserve(unit_clauses.size());

    for (auto clause : unit_clauses)
        final_clauses.push_back(clause);

    for (auto clause : clauses)
        if (!clause->clause_eliminated)
            final_clauses.push_back(clause);

    cnf->clauses = final_clauses;

    for (auto it : bounded_variable_elimination_map) {
        int var = it.first;
        if (bounded_variable_elimination_map[var])
            delete bounded_variable_elimination_map[var];
    }

    std::cout << "Finished bounded variable elimination, number of clauses is " << cnf->clauses.size() << std::endl;

    return variable_eliminated;
}

#endif