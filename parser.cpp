#include "parser.h"
#include <iostream>

using namespace std;

void executeCommand(const vector<Token>& tokens, SymTable& table, bool& should_exit) {
    if (tokens.empty()) return;

    // Report the first unknown word and stop
    for (const auto& tok : tokens) {
        if (tok.type == TokenType::UNKNOWN) {
            cout << "SNOL> Unknown word [" << tok.value << "]\n";
            return;
        }
    }

    TokenType first = tokens[0].type;

    // EXIT!
    if (first == TokenType::KW_EXIT) {
        if (tokens.size() != 1) {
            cout << "SNOL> Unknown command! Does not match any valid command of the language.\n";
            return;
        }
        should_exit = true;
        return;
    }

    cout << "SNOL> Unknown command! Does not match any valid command of the language.\n";
}
