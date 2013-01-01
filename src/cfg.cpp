#include "cfg.h"
#include "string_utils.h"
#include <format>
#include <ranges>

const string ContextFreeGrammar::EMPTY_SYMBOL = "ε";

bool ContextFreeGrammarToken::operator==(const ContextFreeGrammarToken& other) const {
    return type == other.type && symbol == other.symbol && line == other.line && column == other.column;
}

vector<ContextFreeGrammarToken> ContextFreeGrammar::tokenize(const string& s) {
    const auto n = s.size();
    vector<ContextFreeGrammarToken> tokens;
    size_t line = 1, column = 1;
    for (size_t i = 0; i < n;) {
        if (i + 1 < n && s[i] == '\r' && s[i + 1] == '\n') {
            ++line;
            column = 1;
            i += 2;
        } else if (s[i] == '\n' || s[i] == '\r') {
            ++line;
            column = 1;
            ++i;
        } else if (isspace(s[i])) {
            ++column;
            ++i;
        } else if (i + 1 < n && s[i] == '-' && s[i + 1] == '>') {
            tokens.emplace_back(ContextFreeGrammarToken{ContextFreeGrammarToken::Type::PRODUCTION, "->", line, column});
            i += 2;
            column += 2;
        } else if (s[i] == '|') {
            tokens.emplace_back(ContextFreeGrammarToken{ContextFreeGrammarToken::Type::ALTERNATION, "|", line, column});
            ++i;
            ++column;
        } else {
            for (size_t j = i + 1; j <= n; ++j) {
                if (j == n || isspace(s[j]) || s[j] == '|' || (j + 1 < n && s[j] == '-' && s[j + 1] == '>')) {
                    if (const auto symbol = s.substr(i, j - i); symbol == "ε" || symbol == "ϵ") {
                        tokens.emplace_back(ContextFreeGrammarToken{ContextFreeGrammarToken::Type::SYMBOL, EMPTY_SYMBOL, line, column});
                    } else {
                        tokens.emplace_back(ContextFreeGrammarToken{ContextFreeGrammarToken::Type::SYMBOL, symbol, line, column});
                    }
                    column += j - i;
                    i = j;
                    break;
                }
            }
        }
    }
    return tokens;
}

bool ContextFreeGrammar::parse(const string& s) {
    const auto tokens = tokenize(s);
    const auto n = tokens.size();
    if (n == 0) {
        return true;
    }
    string head;
    auto hasEmptyProduction = [&](const size_t line, const size_t column) -> bool {
        if (head.empty()) {
            return false;
        }
        const bool hasEmpty = _productions[head].back().empty();
        if (hasEmpty) {
           _errorMessage = "Line " + to_string(line) + " Column " + to_string(column) + ": Found empty production for '" + head + "'.";
        }
        return hasEmpty;
    };
    auto noHeadFound = [&](const size_t line, const size_t column) -> bool {
        if (head.empty()) {
            _errorMessage = "Line " + to_string(line) + " Column " + to_string(column) + ": Can not find the head of the production.";
            return true;
        }
        return false;
    };
    for (size_t i = 0; i < n; ++i) {
        if (tokens[i].type == ContextFreeGrammarToken::Type::PRODUCTION) {
            _errorMessage = format("Line {} Column {}: Can not find the head of the production.", tokens[i].line, tokens[i].column);
            return false;
        }
        if (i + 1 < n && tokens[i].type == ContextFreeGrammarToken::Type::SYMBOL && tokens[i + 1].type == ContextFreeGrammarToken::Type::PRODUCTION) {
            if (hasEmptyProduction(tokens[i].line, tokens[i].column)) {
                return false;
            }
            head = tokens[i].symbol;
            if (!_productions.contains(head)) {
                _ordering.emplace_back(head);
            }
            _productions[head].emplace_back();
            ++i;
        } else if (tokens[i].type == ContextFreeGrammarToken::Type::ALTERNATION) {
            if (noHeadFound(tokens[i].line, tokens[i].column)) {
                return false;
            }
            if (hasEmptyProduction(tokens[i].line, tokens[i].column)) {
                return false;
            }
            _productions[head].emplace_back();
        } else {
            if (noHeadFound(tokens[i].line, tokens[i].column)) {
                return false;
            }
            _productions[head].back().emplace_back(tokens[i].symbol);
        }
    }
    const auto lastLine = tokens.back().line;
    const auto lastColumn = tokens.back().column + tokens.back().symbol.size();
    if (hasEmptyProduction(lastLine, lastColumn)) {
        return false;
    }
    initTerminals();
    deduplicate();
    return true;
}

const string& ContextFreeGrammar::errorMessage() const {
    return _errorMessage;
}

string ContextFreeGrammar::toString() const {
    size_t maxHeadLength = 0;
    for (const auto& head: _productions | views::keys) {
        maxHeadLength = std::max(maxHeadLength, head.length());
    }
    string grammar;
    const string indent(maxHeadLength, ' ');
    for (const auto& head: _ordering) {
        const auto& productions = _productions.find(head)->second;
        for (size_t i = 0; i < productions.size(); ++i) {
            if (i == 0) {
                grammar += string(maxHeadLength - head.length(), ' ');
                grammar += head + " ->";
            } else {
                grammar += indent + "  |";
            }
            for (const auto& symbol: productions[i]) {
                grammar += " " + symbol;
            }
            grammar += "\n";
        }
    }
    return grammar;
}

void ContextFreeGrammar::initTerminals() {
    _terminals.clear();
    for (const auto& productions : _productions | views::values) {
        for (const auto& production: productions) {
            for (const auto& symbol: production) {
                if (!_productions.contains(symbol)) {
                    _terminals.insert(symbol);
                }
            }
        }
    }
}

string ContextFreeGrammar::computeProductionKey(const vector<string>& production) {
    return stringJoin(production, " ");
}

void ContextFreeGrammar::deduplicate() {
    _productionKeys.clear();
    for (auto& [head, productions] : _productions) {
        size_t m = 0;
        for (size_t i = 0; i < productions.size(); ++i) {
            if (const auto key = computeProductionKey(productions[i]); !_productionKeys[head].contains(key)) {
                _productionKeys[head].insert(key);
                if (i != m) {
                    productions[m] = productions[i];
                }
                ++m;
            }
        }
        productions.resize(m);
    }
}

void PrintTo(const ContextFreeGrammarToken& token, ostream* os) {
    *os << "{" << static_cast<int>(token.type) << "," << token.symbol << "," << token.line << "," << token.column << "}";
}
