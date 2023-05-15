#include <iostream>

#include "CNF.h"
#include "DIMACSParser.h"

int main(int argc, char** args) {
    if (argc < 2) {
        std::cout << "First argument must be a path to the .cnf file" << std::endl;
        exit(1);
    }
    CNF* cnf = DIMACSParser::parse_file(args[1]);
    return 0;
}
