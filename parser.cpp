// AQUINO, MORAN, PEREZ
// implements executeCommand()

#include "parser.h"
#include <iostream>
#include <optional>
#include <cctype>

using namespace std;

struct EvalResult {
    VarType type;
    long long i_val;
    double f_val;
};

// Forward declarations
optional<EvalResult> evaluateExpression(const vector<Token>& tokens, size_t& pos, const SymTable& symbols, bool& errPrinted);
optional<EvalResult> parseTerm(const vector<Token>& tokens, size_t& pos, const SymTable& symbols, bool& errPrinted);
optional<EvalResult> parseFactor(const vector<Token>& tokens, size_t& pos, const SymTable& symbols, bool& errPrinted);

optional<EvalResult> parseFactor(const vector<Token>& tokens, size_t& pos, const SymTable& symbols, bool& errPrinted) {
    if (pos >= tokens.size()) return nullopt;

    const Token& tok = tokens[pos];
    if (tok.type == TokenType::INT_LIT) {
        pos++;
        return EvalResult{VarType::INTEGER, stoll(tok.value), 0.0};
    } else if (tok.type == TokenType::FLOAT_LIT) {
        pos++;
        return EvalResult{VarType::FLOAT, 0, stod(tok.value)};
    } else if (tok.type == TokenType::VARIABLE) {
        auto it = symbols.find(tok.value);
        if (it == symbols.end()) {
            cout << "SNOL> Error! [" << tok.value << "] is not defined!\n";
            errPrinted = true;
            return nullopt;
        }
        pos++;
        return EvalResult{it->second.type, it->second.i_val, it->second.f_val};
    } else if (tok.type == TokenType::LPAREN) {
        pos++;
        auto res = evaluateExpression(tokens, pos, symbols, errPrinted);
        if (!res) return nullopt;
        if (pos >= tokens.size() || tokens[pos].type != TokenType::RPAREN) return nullopt;
        pos++;
        return res;
    } else if (tok.type == TokenType::OP_SUB) {
        pos++;
        auto res = parseFactor(tokens, pos, symbols, errPrinted);
        if (!res) return nullopt;
        if (res->type == VarType::INTEGER) res->i_val = -res->i_val;
        else res->f_val = -res->f_val;
        return res;
    }
    return nullopt;
}

optional<EvalResult> parseTerm(const vector<Token>& tokens, size_t& pos, const SymTable& symbols, bool& errPrinted) {
    auto left = parseFactor(tokens, pos, symbols, errPrinted);
    if (!left) return nullopt;

    while (pos < tokens.size() &&
           (tokens[pos].type == TokenType::OP_MUL ||
            tokens[pos].type == TokenType::OP_DIV ||
            tokens[pos].type == TokenType::OP_MOD)) {
        TokenType op = tokens[pos].type;
        pos++;
        auto right = parseFactor(tokens, pos, symbols, errPrinted);
        if (!right) return nullopt;

        if (left->type != right->type) {
            cout << "SNOL> Error! Operands must be of the same type in an arithmetic operation!\n";
            errPrinted = true;
            return nullopt;
        }
        if (op == TokenType::OP_MOD) {
            if (left->type == VarType::FLOAT) {
                cout << "SNOL> Error! Modulo operation is only allowed for integer operands!\n";
                errPrinted = true;
                return nullopt;
            }
            if (right->i_val == 0) {
                cout << "SNOL> Error! Division by zero!\n";
                errPrinted = true;
                return nullopt;
            }
            left->i_val %= right->i_val;
        } else if (left->type == VarType::FLOAT) {
            if (op == TokenType::OP_MUL) {
                left->f_val *= right->f_val;
            } else {
                if (right->f_val == 0.0) {
                    cout << "SNOL> Error! Division by zero!\n";
                    errPrinted = true;
                    return nullopt;
                }
                left->f_val /= right->f_val;
            }
        } else {
            if (op == TokenType::OP_MUL) {
                left->i_val *= right->i_val;
            } else {
                if (right->i_val == 0) {
                    cout << "SNOL> Error! Division by zero!\n";
                    errPrinted = true;
                    return nullopt;
                }
                left->i_val /= right->i_val;
            }
        }
    }
    return left;
}

optional<EvalResult> evaluateExpression(const vector<Token>& tokens, size_t& pos, const SymTable& symbols, bool& errPrinted) {
    auto left = parseTerm(tokens, pos, symbols, errPrinted);
    if (!left) return nullopt;

    while (pos < tokens.size() &&
           (tokens[pos].type == TokenType::OP_ADD ||
            tokens[pos].type == TokenType::OP_SUB)) {
        TokenType op = tokens[pos].type;
        pos++;
        auto right = parseTerm(tokens, pos, symbols, errPrinted);
        if (!right) return nullopt;

        if (left->type != right->type) {
            cout << "SNOL> Error! Operands must be of the same type in an arithmetic operation!\n";
            errPrinted = true;
            return nullopt;
        }
        if (left->type == VarType::FLOAT) {
            if (op == TokenType::OP_ADD) left->f_val += right->f_val;
            else left->f_val -= right->f_val;
        } else {
            if (op == TokenType::OP_ADD) left->i_val += right->i_val;
            else left->i_val -= right->i_val;
        }
    }
    return left;
}

static bool isValidIntegerInput(const string& s) {
    if (s.empty()) return false;
    size_t i = 0;
    if (s[i] == '-') i++;
    if (i >= s.size() || !isdigit((unsigned char)s[i])) return false;
    while (i < s.size() && isdigit((unsigned char)s[i])) i++;
    return i == s.size();
}

static bool isValidFloatInput(const string& s) {
    if (s.empty()) return false;
    size_t i = 0;
    if (s[i] == '-') i++;
    if (i >= s.size() || !isdigit((unsigned char)s[i])) return false;
    while (i < s.size() && isdigit((unsigned char)s[i])) i++;
    if (i >= s.size() || s[i] != '.') return false;
    i++;
    while (i < s.size() && isdigit((unsigned char)s[i])) i++;
    return i == s.size();
}

void executeCommand(const vector<Token>& tokens, bool& should_exit, SymTable& symbols) {
    if (tokens.empty()) return;

    for (const auto& tok : tokens) {
        if (tok.type == TokenType::UNKNOWN) {
            cout << "SNOL> Unknown word [" << tok.value << "]\n";
            return;
        }
    }

    // EXIT!
    if (tokens[0].type == TokenType::KW_EXIT) {
        if (tokens.size() != 1) {
            cout << "SNOL> Unknown command! Does not match any valid command of the language.\n";
            return;
        }
        should_exit = true;
        return;
    }

    // BEG var
    if (tokens[0].type == TokenType::KW_BEG) {
        if (tokens.size() != 2 || tokens[1].type != TokenType::VARIABLE) {
            cout << "SNOL> Unknown command! Does not match any valid command of the language.\n";
            return;
        }
        cout << "SNOL> Please enter value for [" << tokens[1].value << "]\n";
        cout << "Input: ";
        string input;
        getline(cin, input);
        if (isValidIntegerInput(input)) {
            symbols[tokens[1].value] = {VarType::INTEGER, stoll(input), 0.0};
        } else if (isValidFloatInput(input)) {
            symbols[tokens[1].value] = {VarType::FLOAT, 0, stod(input)};
        } else {
            cout << "SNOL> Invalid input! Input must be a valid integer or floating-point value.\n";
        }
        return;
    }

    // PRINT out
    if (tokens[0].type == TokenType::KW_PRINT) {
        if (tokens.size() != 2) {
            cout << "SNOL> Unknown command! Does not match any valid command of the language.\n";
            return;
        }
        if (tokens[1].type == TokenType::VARIABLE) {
            auto it = symbols.find(tokens[1].value);
            if (it == symbols.end()) {
                cout << "SNOL> Error! [" << tokens[1].value << "] is not defined!\n";
            } else {
                cout << "SNOL> [" << tokens[1].value << "] = ";
                if (it->second.type == VarType::INTEGER) cout << it->second.i_val << "\n";
                else cout << it->second.f_val << "\n";
            }
        } else if (tokens[1].type == TokenType::INT_LIT || tokens[1].type == TokenType::FLOAT_LIT) {
            cout << "SNOL> " << tokens[1].value << "\n";
        } else {
            cout << "SNOL> Unknown command! Does not match any valid command of the language.\n";
        }
        return;
    }

    // var = expr
    if (tokens[0].type == TokenType::VARIABLE &&
        tokens.size() >= 2 && tokens[1].type == TokenType::OP_ASSIGN) {
        if (tokens.size() < 3) {
            cout << "SNOL> Unknown command! Does not match any valid command of the language.\n";
            return;
        }
        size_t pos = 2;
        bool errPrinted = false;
        auto res = evaluateExpression(tokens, pos, symbols, errPrinted);
        if (res && pos == tokens.size()) {
            symbols[tokens[0].value] = {res->type, res->i_val, res->f_val};
            return;
        }
        if (!errPrinted) {
            cout << "SNOL> Unknown command! Does not match any valid command of the language.\n";
        }
        return;
    }

    // Standalone expression: literal, variable, or arithmetic expression (no output on success)
    {
        size_t pos = 0;
        bool errPrinted = false;
        auto res = evaluateExpression(tokens, pos, symbols, errPrinted);
        if (res && pos == tokens.size()) {
            return;
        }
        if (!errPrinted) {
            cout << "SNOL> Unknown command! Does not match any valid command of the language.\n";
        }
    }
}
