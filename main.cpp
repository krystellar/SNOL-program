#include <iostream>
#include <string>
#include <vector>
#include "lexer.h"
#include "parser.h"

using namespace std;

int main() {
    cout << "The SNOL environment is now active, you may proceed with giving your commands.\n";

    bool should_exit = false;

    while (!should_exit) {
        cout << "Command: " << flush;
        string line;
        if (!getline(cin, line)) break;
        vector<Token> tokens = tokenize(line);
        executeCommand(tokens, should_exit);
    }

    cout << "Interpreter is now terminated...\n";
    return 0;
}
