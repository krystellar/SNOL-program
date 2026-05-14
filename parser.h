#pragma once
#include "lexer.h"
#include "symboltable.h"
#include <vector>

void executeCommand(const std::vector<Token>& tokens, bool& should_exit, SymTable& symbols);
