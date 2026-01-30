#include "cfg.h"
#include "string_utils.h"
#include <format>
#include <ranges>
#include <limits>
#include <algorithm>

using namespace std;

const string ContextFreeGrammar::EMPTY_SYMBOL = "ε";
const string ContextFreeGrammar::DOT_SYMBOL = "·";
const string ContextFreeGrammar::EOF_SYMBOL = "¥";
const string ContextFreeGrammar::LOOKAHEAD_SEPARATOR = "﹐";
const string ContextFreeGrammar::LOOKAHEAD_INNER_SEPARATOR = "／";

bool ContextFreeGrammarToken::operator==(const ContextFreeGrammarToken& other) const {
    return type == other.type && symbol == other.symbol && line == other.line && column == other.column;
}

vector<ContextFreeGrammarToken> ContextFreeGrammar::tokenize(const string& s) {
    const auto graphemes = segmentGraphemes(s);
    const auto n = graphemes.size();
    vector<ContextFreeGrammarToken> tokens;
    size_t line = 1, column = 1;

    auto isWhitespace = [](const string& g) {
        return g.size() == 1 && isspace(static_cast<unsigned char>(g[0]));
    };

    for (size_t i = 0; i < n;) {
        const auto& g = graphemes[i];
        if (g == "\r\n" || g == "\n" || g == "\r") {
            ++line;
            column = 1;
            ++i;
        } else if (isWhitespace(g)) {
            ++column;
            ++i;
        } else if (i + 1 < n && graphemes[i] == "-" && graphemes[i + 1] == ">") {
            tokens.emplace_back(ContextFreeGrammarToken{ContextFreeGrammarToken::Type::PRODUCTION, "->", line, column});
            i += 2;
            column += 2;
        } else if (g == "→") {
            tokens.emplace_back(ContextFreeGrammarToken{ContextFreeGrammarToken::Type::PRODUCTION, "→", line, column});
            ++i;
            ++column;
        } else if (g == "|") {
            tokens.emplace_back(ContextFreeGrammarToken{ContextFreeGrammarToken::Type::ALTERNATION, "|", line, column});
            ++i;
            ++column;
        } else if (g == "｜") {
            tokens.emplace_back(ContextFreeGrammarToken{ContextFreeGrammarToken::Type::ALTERNATION, "｜", line, column});
            ++i;
            ++column;
        } else {
            string symbol;
            size_t numChars = 0;
            for (size_t j = i; j < n; ++j) {
                const auto& gj = graphemes[j];
                if (isWhitespace(gj)
                    || gj == "\r\n" || gj == "\n" || gj == "\r"
                    || gj == "|" || gj == "｜" || gj == "→"
                    || (j + 1 < n && gj == "-" && graphemes[j + 1] == ">")) {
                    break;
                }
                symbol += gj;
                ++numChars;
            }
            if (symbol == "ε" || symbol == "ϵ") {
                tokens.emplace_back(ContextFreeGrammarToken{ContextFreeGrammarToken::Type::SYMBOL, EMPTY_SYMBOL, line, column});
            } else {
                tokens.emplace_back(ContextFreeGrammarToken{ContextFreeGrammarToken::Type::SYMBOL, symbol, line, column});
            }
            i += numChars;
            column += numChars;
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
    const auto lastColumn = tokens.back().column + utf8Length(tokens.back().symbol);
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

std::string ContextFreeGrammar::toSortedString() const {
    size_t maxHeadLength = 0;
    for (const auto& head: _productions | views::keys) {
        maxHeadLength = std::max(maxHeadLength, head.length());
    }
    string grammar;
    const string indent(maxHeadLength, ' ');
    auto ordering(_ordering);
    ranges::sort(ordering);
    for (const auto& head: ordering) {
        auto productions(_productions.at(head));
        ranges::sort(productions, [](const Production& a, const Production& b) {
            const auto n = a.size();
            const auto m = b.size();
            for (size_t i = 0; i < min(n, m); ++i) {
                if (a[i] < b[i]) {
                    return true;
                }
                if (a[i] > b[i]) {
                    return false;
                }
            }
            return n <= m;
        });
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
                if (!_productions.contains(symbol) && symbol != EMPTY_SYMBOL) {
                    _terminals.insert(symbol);
                }
            }
        }
    }
}

vector<string> ContextFreeGrammar::terminals() const {
    return {_terminals.begin(), _terminals.end()};
}

vector<string> ContextFreeGrammar::nonTerminals() const {
    const auto heads = _productions | views::keys;
    return {heads.begin(), heads.end()};
}

const vector<Symbol>& ContextFreeGrammar::orderedNonTerminals() const {
    return _ordering;
}

bool ContextFreeGrammar::isTerminal(const Symbol& symbol) const {
    return _terminals.contains(symbol);
}

bool ContextFreeGrammar::isNonTerminal(const Symbol& symbol) const {
    return _productions.contains(symbol);
}

string ContextFreeGrammar::computeProductionKey(const vector<string>& production) {
    return stringJoin(production, " ");
}

string ContextFreeGrammar::computeProductionKey(const Symbol& head, const Production& production) {
    return head + " -> " + computeProductionKey(production);
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

string ContextFreeGrammar::generatePrimedSymbol(const string& symbol, const bool updateOrdering) {
    if (!_productions.contains(symbol)) {
        throw runtime_error(format("The original symbol '{}' does not exist.", symbol));
    }
    if (symbol.size() >= numeric_limits<int>::max()) {
        throw runtime_error("The input symbol size is too large.");
    }
    auto baseSymbol = symbol;
    const auto n = static_cast<int>(symbol.size());
    auto i = n - 1;
    bool hasPrime = false;
    // Scan backwards to find if symbol already has a prime suffix.
    // e.g., "A'_2" has prime at index 1
    for (; i >= 0; --i) {
        if (symbol[i] == '\'') {
            hasPrime = true;
            break;
        }
    }
    // Check if symbol matches pattern: <base>'_<number>
    // e.g., "A'_2" -> base="A", number=2
    // If not matching (e.g., "A'B" or "A'_x"), treat as no prime.
    int number = 0;
    if (hasPrime && i + 1 < n) {
        if (symbol[i + 1] != '_') {
            hasPrime = false;
        } else {
            if (i + 2 >= n || !isdigit(symbol[i + 2])) {
                hasPrime = false;
            } else {
                bool hasNumber = true;
                for (auto j = i + 3; j < n; ++j) {
                    if (!isdigit(symbol[j])) {
                        hasPrime = false;
                        hasNumber = false;
                    }
                }
                if (hasNumber) {
                    number = stoi(symbol.substr(i + 2));
                }
            }
        }
    }
    // Generate new symbol name.
    // Priority: A -> A' -> A'_1 -> A'_2 -> ...
    string newSymbol;
    if (!hasPrime) {
        // Try simple prime first: "A" -> "A'"
        if (const auto candidate = symbol + "'"; !(_productions.contains(candidate) || _terminals.contains(candidate))) {
            newSymbol = candidate;
        } else {
            baseSymbol = candidate;
        }
    }
    if (newSymbol.empty()) {
        // Try numbered primes: "A" -> "A'_1", "A'_1" -> "A'_2", etc.
        while (true) {
            if (number == numeric_limits<int>::max()) {
                throw runtime_error("Can not find a new symbol name");
            }
            ++number;
            newSymbol = symbol.substr(0, i) + "'_" + to_string(number);
            if (_productions.contains(newSymbol) || _terminals.contains(newSymbol)) {
                baseSymbol = newSymbol;
            } else {
                break;
            }
        }
    }
    // Insert new symbol into ordering (after baseSymbol if found).
    if (updateOrdering) {
        if (const auto it = ranges::find(_ordering, baseSymbol); it == _ordering.end()) {
            _ordering.emplace_back(newSymbol);
        } else {
            _ordering.insert(it + 1, newSymbol);
        }
    }
    return newSymbol;
}

void ContextFreeGrammar::addProduction(const Symbol& head, const Production& production) {
    if (const auto it = ranges::find(_ordering, head); it == _ordering.end()) {
        _ordering.emplace_back(head);
    }
    if (const auto it = _terminals.find(head); it != _terminals.end()) {
        _terminals.erase(it);
    }
    _productions[head].emplace_back(production);
    for (const auto& symbol : production) {
        if (!_productions.contains(symbol) && symbol != EMPTY_SYMBOL) {
            _terminals.insert(symbol);
        }
    }
}

void ContextFreeGrammar::addProductions(const Symbol& head, const Productions& productions) {
    if (const auto it = ranges::find(_ordering, head); it == _ordering.end()) {
        _ordering.emplace_back(head);
    }
    if (const auto it = _terminals.find(head); it != _terminals.end()) {
        _terminals.erase(it);
    }
    for (const auto& production: productions) {
        _productions[head].emplace_back(production);
        for (const auto& symbol : production) {
            if (!_productions.contains(symbol)) {
                _terminals.insert(symbol);
            }
        }
    }
}

ContextFreeGrammar ContextFreeGrammar::operator|(const ContextFreeGrammar& other) const {
    ContextFreeGrammar result;
    for (const auto& symbol : _ordering) {
        result.addProductions(symbol, _productions.at(symbol));
    }
    for (const auto& symbol : other._ordering) {
        result.addProductions(symbol, other._productions.at(symbol));
    }
    return result;
}

void PrintTo(const ContextFreeGrammarToken& token, ostream* os) {
    *os << "{" << static_cast<int>(token.type) << "," << token.symbol << "," << token.line << "," << token.column << "}";
}
