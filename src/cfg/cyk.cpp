#include "cfg.h"
#include <sstream>
#include <algorithm>
#include <ranges>
#include <functional>

using namespace std;

CYKTable::CYKTable(const size_t size) : n(size), table(size, vector<vector<Symbol>>(size)) {}

size_t CYKTable::size() const {
    return n;
}

vector<Symbol> CYKTable::getCell(const size_t r, const size_t c) const {
    if (r >= n || c >= n || r > c) {
        return {};
    }
    return table[r][c];
}

string CYKTable::getCellString(const size_t r, const size_t c, const string& separator) const {
    if (r >= n || c >= n || r > c) {
        return "";
    }
    string result;
    for (size_t k = 0; k < table[r][c].size(); ++k) {
        if (k > 0) result += separator;
        result += table[r][c][k];
    }
    return result;
}

CYKTable ContextFreeGrammar::cykParse(const string& s) const {
    vector<Symbol> tokens;
    istringstream iss(s);
    string token;
    while (iss >> token) {
        tokens.push_back(token);
    }

    size_t n = tokens.size();
    CYKTable result(n);

    if (n == 0) {
        if (!_ordering.empty()) {
            const auto& startSymbol = _ordering[0];
            for (const auto& [head, productions] : _productions) {
                for (const auto& production : productions) {
                    if (production.size() == 1 && production[0] == EMPTY_SYMBOL && head == startSymbol) {
                        result.accepted = true;
                        result.parseTree = make_shared<ParseTreeNode>();
                        result.parseTree->terminal = false;
                        result.parseTree->label = startSymbol;
                        auto epsilonNode = make_shared<ParseTreeNode>();
                        epsilonNode->terminal = true;
                        epsilonNode->label = EMPTY_SYMBOL;
                        result.parseTree->children.push_back(epsilonNode);
                        break;
                    }
                }
            }
        }
        return result;
    }

    unordered_map<Symbol, vector<Symbol>> terminalToNonTerminal;
    unordered_map<string, vector<pair<Symbol, Production>>> pairToNonTerminal;

    for (const auto& [head, productions] : _productions) {
        for (const auto& production : productions) {
            if (production.size() == 1 && production[0] != EMPTY_SYMBOL) {
                terminalToNonTerminal[production[0]].push_back(head);
            } else if (production.size() == 2) {
                string key = production[0] + " " + production[1];
                pairToNonTerminal[key].emplace_back(head, production);
            }
        }
    }

    struct BackPointer {
        Symbol symbol;
        size_t k{};
        Symbol leftSymbol;
        Symbol rightSymbol;
        bool isTerminal{};
    };
    vector backPointers(n, vector<unordered_map<Symbol, BackPointer>>(n));

    for (size_t i = 0; i < n; ++i) {
        const auto& terminal = tokens[i];
        if (terminalToNonTerminal.contains(terminal)) {
            for (const auto& nt : terminalToNonTerminal[terminal]) {
                result.table[i][i].push_back(nt);
                backPointers[i][i][nt] = {terminal, 0, "", "", true};
            }
        }
    }

    for (size_t len = 2; len <= n; ++len) {
        for (size_t i = 0; i + len <= n; ++i) {
            size_t j = i + len - 1;
            unordered_set<Symbol> added;
            for (size_t k = i; k < j; ++k) {
                for (const auto& B : result.table[i][k]) {
                    for (const auto& C : result.table[k + 1][j]) {
                        if (const string key = B + " " + C; pairToNonTerminal.contains(key)) {
                            for (const auto &A: pairToNonTerminal[key] | views::keys) {
                                if (!added.contains(A)) {
                                    added.insert(A);
                                    result.table[i][j].push_back(A);
                                    backPointers[i][j][A] = {A, k, B, C, false};
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (!_ordering.empty()) {
        const auto& startSymbol = _ordering[0];
        for (const auto& sym : result.table[0][n - 1]) {
            if (sym == startSymbol) {
                result.accepted = true;
                break;
            }
        }
    }

    if (result.accepted) {
        function<shared_ptr<ParseTreeNode>(size_t, size_t, const Symbol&)> buildTree = [&](const size_t r, const size_t c, const Symbol& symbol) -> shared_ptr<ParseTreeNode> {
            auto node = make_shared<ParseTreeNode>();
            node->terminal = false;
            node->label = symbol;

            if (!backPointers[r][c].contains(symbol)) {
                return node;
            }

            const auto& bp = backPointers[r][c][symbol];
            if (bp.isTerminal) {
                const auto child = make_shared<ParseTreeNode>();
                child->terminal = true;
                child->label = bp.symbol;
                node->children.push_back(child);
            } else {
                node->children.push_back(buildTree(r, bp.k, bp.leftSymbol));
                node->children.push_back(buildTree(bp.k + 1, c, bp.rightSymbol));
            }
            return node;
        };

        result.parseTree = buildTree(0, n - 1, _ordering[0]);
    }

    return result;
}
