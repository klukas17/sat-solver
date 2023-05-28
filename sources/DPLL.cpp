//
// Created by mihael on 15/05/23.
//

#include "DPLL.h"
#include "climits"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, INT_MAX);

DPLL::DPLL(Assignment *assignment, CNF *cnf) {
    this->assignment = assignment;
    this->cnf = cnf;
}

void DPLL::solve() {

}