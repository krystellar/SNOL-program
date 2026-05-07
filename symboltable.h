#pragma once
#include <map>
#include <string>
using namespace std;

enum class VarType { INTEGER, FLOAT };

struct Variable {
    VarType type;
    long long i_val;
    double f_val;
};

using SymTable = map<string, Variable>;
