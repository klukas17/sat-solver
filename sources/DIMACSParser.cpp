//
// Created by mihael on 15/05/23.
//

#include "DIMACSParser.h"
#include "fstream"
#include "sstream"
#include "iostream"

CNF *DIMACSParser::parse_file(const std::string& path) {
    std::ifstream in("../" + path);
    std::string line;
    std::vector<Clause*> clauses;
    int number_of_literals;
    while (getline(in, line)) {
        if (line[0] == 'c')
            continue;
        else if (line[0] == 'p') {
            std::istringstream iss(line);
            std::string s;
            std::vector<std::string> elements;
            while (getline(iss, s, ' '))
                if (!s.empty())
                    elements.push_back(s);
            if (elements.size() != 4) {
                std::cout << "File " << path << " is not in the correct format!" << std::endl;
                exit(1);
            }
            number_of_literals = std::stoi(elements[2]);
        }
        else if (line[0] == '%') {
            break;
        }
        else {
            std::istringstream iss(line);
            std::string s;
            std::set<int> literals;
            while (getline(iss, s, ' ')) {
                if (s == "0")
                    break;
                if (s.empty())
                    continue;
                literals.insert(std::stoi(s));
            }
            clauses.push_back(new Clause(literals));
        }
    }
    return new CNF(clauses, number_of_literals);
}