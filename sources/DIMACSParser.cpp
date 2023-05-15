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
        }
        else if (line[0] == '%') {
            break;
        }
        else {
            std::istringstream iss(line);
            std::string s;
            std::set<int> literals;
            std::set<int> negated_literals;
            while (getline(iss, s, ' ')) {
                if (s == "0")
                    break;
                if (s.empty())
                    continue;
                int literal = std::stoi(s);
                if (literal > 0)
                    literals.insert(literal);
                else
                    negated_literals.insert(-literal);
            }
            clauses.push_back(new Clause(literals, negated_literals));
        }
    }
    return new CNF(clauses);
}