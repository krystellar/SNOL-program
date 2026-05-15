#include "lexer.h"
#include <cctype>
using namespace std;

vector<Token> tokenize(const string& line) {
    vector<Token> tokens;
    size_t i = 0;
    size_t n = line.size();

    while (i < n) {
        unsigned char c = line[i];

        if (isspace(c)) { i++; continue; }

        // Letter → scan word, then classify as keyword or variable
        if (isalpha(c)) {
            int start = i;
            while (i < n && isalnum((unsigned char)line[i])) i++;
            string word = line.substr(start, i - start);
            if (word == "EXIT" && i < n && line[i] == '!') {
                tokens.push_back({TokenType::KW_EXIT, "EXIT!"});
                i++;
            } else if (word == "BEG") {
                tokens.push_back({TokenType::KW_BEG, word});
            } else if (word == "PRINT") {
                tokens.push_back({TokenType::KW_PRINT, word});
            } else {
                tokens.push_back({TokenType::VARIABLE, word});
            }
            continue;
        }

        // Single character operators
        if (c == '=') { tokens.push_back({TokenType::OP_ASSIGN, "="}); i++; continue; }
        if (c == '+') { tokens.push_back({TokenType::OP_ADD, "+"}); i++; continue; }
        if (c == '-') { tokens.push_back({TokenType::OP_SUB, "-"}); i++; continue; }
        if (c == '*') { tokens.push_back({TokenType::OP_MUL, "*"}); i++; continue; }
        if (c == '/') { tokens.push_back({TokenType::OP_DIV, "/"}); i++; continue; }
        if (c == '%') { tokens.push_back({TokenType::OP_MOD, "%"}); i++; continue; }
        if (c == '(') { tokens.push_back({TokenType::LPAREN, "("}); i++; continue; }
        if (c == ')') { tokens.push_back({TokenType::RPAREN, ")"}); i++; continue; }

        // Digit → integer or float literal
        if (isdigit(c)) {
            int start = i;
            while (i < n && isdigit((unsigned char)line[i])) i++;
            bool is_float = false;
            if (i < n && line[i] == '.') {
                is_float = true;
                i++;
                while (i < n && isdigit((unsigned char)line[i])) i++;
            }
            tokens.push_back({is_float ? TokenType::FLOAT_LIT : TokenType::INT_LIT,
                               line.substr(start, i - start)});
            continue;
        }

        // Any other character → unknown token
        int start = i;
        while (i < n && !isspace((unsigned char)line[i])) i++;
        tokens.push_back({TokenType::UNKNOWN, line.substr(start, i - start)});
    }

    return tokens;
}
