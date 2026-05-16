// AQUINO, MORAN, PEREZ
// declares executeCommand() which takes a token list and drives the actual execution of a SNOL statement

#pragma once
#include "lexer.h"
#include "symboltable.h"
#include <vector>

void executeCommand(const std::vector<Token>& tokens, bool& should_exit, SymTable& symbols);
