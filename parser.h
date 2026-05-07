#pragma once
#include "symboltable.h"
#include "lexer.h"
using namespace std;

void executeCommand(const vector<Token>& tokens, SymTable& table, bool& should_exit);
