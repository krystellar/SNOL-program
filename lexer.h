#pragma once
#include <string>
#include <vector>
using namespace std;

enum class TokenType {
    INT_LIT,
    FLOAT_LIT,
    VARIABLE,
    KW_EXIT,
    KW_BEG,
    KW_PRINT,
    OP_ASSIGN,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    LPAREN,
    RPAREN,
    UNKNOWN
};

struct Token {
    TokenType type;
    string value;
};

vector<Token> tokenize(const string& line);
