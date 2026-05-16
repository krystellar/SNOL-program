// AQUINO, MORAN, PEREZ
// the readl-eval-print loop 

#include <iostream>
#include <string>
#include <vector>
#include "lexer.h"
#include "parser.h"
#include "symboltable.h"

using namespace std;

int main() {
    cout << "The SNOL environment is now active, you may proceed with giving your commands.\n";

    bool should_exit = false;
    SymTable symbols;

    while (!should_exit) {
        cout << "Command: " << flush;
        string line;
        if (!getline(cin, line)) break;
        vector<Token> tokens = tokenize(line);
        executeCommand(tokens, should_exit, symbols);
    }

    cout << "Interpreter is now terminated...\n";
    return 0;
}
