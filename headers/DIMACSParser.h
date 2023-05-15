//
// Created by mihael on 15/05/23.
//

#ifndef SAT_SOLVER_DIMACSPARSER_H
#define SAT_SOLVER_DIMACSPARSER_H

#include "CNF.h"
#include "string"

class DIMACSParser {
public:
    static CNF* parse_file(const std::string& path);
};


#endif //SAT_SOLVER_DIMACSPARSER_H
