#include "parser.h"
#include <iostream>
#include <sstream>
#include <optional>

using namespace std;

// Forward declarations for expression evaluation
struct EvalResult {
    VarType type;
    long long i_val;
    double f_val;
};

optional<EvalResult> evaluateExpression(const vector<Token>& tokens, size_t& pos, const SymTable& symbols);

EvalResult castToFloat(EvalResult r) {
    if (r.type == VarType::FLOAT) return r;
    return {VarType::FLOAT, 0, (double)r.i_val};
}

optional<EvalResult> parseFactor(const vector<Token>& tokens, size_t& pos, const SymTable& symbols) {
    if (pos >= tokens.size()) return std::nullopt;

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
            cout << "SNOL> Undefined variable [" << tok.value << "]\n";
            return std::nullopt;
        }
        pos++;
        return EvalResult{it->second.type, it->second.i_val, it->second.f_val};
    } else if (tok.type == TokenType::LPAREN) {
        pos++;
        auto res = evaluateExpression(tokens, pos, symbols);
        if (!res) return std::nullopt;
        if (pos >= tokens.size() || tokens[pos].type != TokenType::RPAREN) return std::nullopt;
        pos++;
        return res;
    } else if (tok.type == TokenType::OP_SUB) {
        pos++;
        auto res = parseFactor(tokens, pos, symbols);
        if (!res) return std::nullopt;
        if (res->type == VarType::INTEGER) res->i_val = -res->i_val;
        else res->f_val = -res->f_val;
        return res;
    }
    return std::nullopt;
}

optional<EvalResult> parseTerm(const vector<Token>& tokens, size_t& pos, const SymTable& symbols) {
    auto left = parseFactor(tokens, pos, symbols);
    if (!left) return std::nullopt;

    while (pos < tokens.size() && (tokens[pos].type == TokenType::OP_MUL || tokens[pos].type == TokenType::OP_DIV)) {
        TokenType op = tokens[pos].type;
        pos++;
        auto right = parseFactor(tokens, pos, symbols);
        if (!right) return std::nullopt;

        if (left->type == VarType::FLOAT || right->type == VarType::FLOAT) {
            *left = castToFloat(*left);
            *right = castToFloat(*right);
            if (op == TokenType::OP_MUL) left->f_val *= right->f_val;
            else {
                if (right->f_val == 0) { cout << "SNOL> Division by zero error!\n"; return std::nullopt; }
                left->f_val /= right->f_val;
            }
        } else {
            if (op == TokenType::OP_MUL) left->i_val *= right->i_val;
            else {
                if (right->i_val == 0) { cout << "SNOL> Division by zero error!\n"; return std::nullopt; }
                left->i_val /= right->i_val;
            }
        }
    }
    return left;
}

optional<EvalResult> evaluateExpression(const vector<Token>& tokens, size_t& pos, const SymTable& symbols) {
    auto left = parseTerm(tokens, pos, symbols);
    if (!left) return std::nullopt;

    while (pos < tokens.size() && (tokens[pos].type == TokenType::OP_ADD || tokens[pos].type == TokenType::OP_SUB)) {
        TokenType op = tokens[pos].type;
        pos++;
        auto right = parseTerm(tokens, pos, symbols);
        if (!right) return std::nullopt;

        if (left->type == VarType::FLOAT || right->type == VarType::FLOAT) {
            *left = castToFloat(*left);
            *right = castToFloat(*right);
            if (op == TokenType::OP_ADD) left->f_val += right->f_val;
            else left->f_val -= right->f_val;
        } else {
            if (op == TokenType::OP_ADD) left->i_val += right->i_val;
            else left->i_val -= right->i_val;
        }
    }
    return left;
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
        cout << "Input for [" << tokens[1].value << "]: ";
        string input;
        getline(cin, input);
        try {
            if (input.find('.') != string::npos) {
                symbols[tokens[1].value] = {VarType::FLOAT, 0, stod(input)};
            } else {
                symbols[tokens[1].value] = {VarType::INTEGER, stoll(input), 0.0};
            }
        } catch (...) {
            cout << "SNOL> Invalid input value!\n";
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
                cout << "SNOL> Undefined variable [" << tokens[1].value << "]\n";
            } else {
                if (it->second.type == VarType::INTEGER) cout << it->second.i_val << endl;
                else cout << it->second.f_val << endl;
            }
        } else if (tokens[1].type == TokenType::INT_LIT) {
            cout << tokens[1].value << endl;
        } else if (tokens[1].type == TokenType::FLOAT_LIT) {
            cout << tokens[1].value << endl;
        } else {
            cout << "SNOL> Unknown command! Does not match any valid command of the language.\n";
        }
        return;
    }

    // var = expr
    if (tokens.size() >= 3 && tokens[0].type == TokenType::VARIABLE && tokens[1].type == TokenType::OP_ASSIGN) {
        size_t pos = 2;
        auto res = evaluateExpression(tokens, pos, symbols);
        if (res && pos == tokens.size()) {
            symbols[tokens[0].value] = {res->type, res->i_val, res->f_val};
            return;
        }
        if (pos != tokens.size() && res) {
             cout << "SNOL> Unknown command! Does not match any valid command of the language.\n";
             return;
        }
        // If evaluateExpression failed, it might have already printed an error.
        // If not, we report it.
        if (!res) {
             // Error already reported or general syntax error
        }
        return;
    }

    cout << "SNOL> Unknown command! Does not match any valid command of the language.\n";
}
