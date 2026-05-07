#pragma once
#include <string>
#include <vector>
using namespace std;

enum class TokenType {
    INT_LIT,
    FLOAT_LIT,
    VARIABLE,
    KW_EXIT,
    UNKNOWN
};

struct Token {
    TokenType type;
    string value;
};

vector<Token> tokenize(const string& line);
