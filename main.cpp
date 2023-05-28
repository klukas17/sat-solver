#include <iostream>
#include "chrono"
#include "CNF.h"
#include "DIMACSParser.h"
#include "DPLL.h"
#include "Assignment.h"

int main(int argc, char** args) {
    if (argc < 2) {
        std::cout << "First argument must be a path to the .cnf file" << std::endl;
        exit(1);
    }
    CNF* cnf = DIMACSParser::parse_file(args[1]);
    DPLL* dpll = new DPLL(new Assignment(cnf->number_of_variables), cnf);
    auto start = std::chrono::high_resolution_clock::now();
    dpll->solve();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Duration: %ld.%06lds\n", duration.count() / 1000000, duration.count() % 1000000);
    return 0;
}
