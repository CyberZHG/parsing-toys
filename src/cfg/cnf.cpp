#include "cfg.h"
#include <queue>
#include <ranges>
#include <algorithm>

using namespace std;

bool ContextFreeGrammar::isChomskyNormalForm() const {
    if (_ordering.empty()) {
        return true;
    }

    const auto& startSymbol = _ordering[0];
    bool startOnRHS = false;

    for (const auto& productions: _productions | views::values) {
        for (const auto& production : productions) {
            for (const auto& symbol : production) {
                if (symbol == startSymbol) {
                    startOnRHS = true;
                    break;
                }
            }
        }
    }

    for (const auto& [head, productions] : _productions) {
        for (const auto& production : productions) {
            if (production.size() == 1) {
                if (production[0] == EMPTY_SYMBOL) {
                    if (head != startSymbol || startOnRHS) {
                        return false;
                    }
                } else if (!isTerminal(production[0])) {
                    return false;
                }
            } else if (production.size() == 2) {
                if (!isNonTerminal(production[0]) || !isNonTerminal(production[1])) {
                    return false;
                }
            } else {
                return false;
            }
        }
    }
    return true;
}

void ContextFreeGrammar::toChomskyNormalForm() {
    if (_ordering.empty()) {
        return;
    }
    const auto& originalStart = _ordering[0];
    bool startOnRHS = false;
    for (const auto& productions: _productions | views::values) {
        for (const auto& production : productions) {
            for (const auto& symbol : production) {
                if (symbol == originalStart) {
                    startOnRHS = true;
                    break;
                }
            }
        }
    }

    if (startOnRHS) {
        const auto newStart = generatePrimedSymbol(originalStart, false);
        _productions[newStart] = {{originalStart}};
        _ordering.insert(_ordering.begin(), newStart);
    }

    unordered_map<Symbol, Symbol> terminalToNonTerminal;
    vector sortedTerminals(_terminals.begin(), _terminals.end());
    ranges::sort(sortedTerminals);
    for (const auto& terminal : sortedTerminals) {
        if (terminal == EMPTY_SYMBOL) {
            continue;
        }
        string newSymbol = "T_" + terminal;
        int counter = 1;
        while (_productions.contains(newSymbol) || _terminals.contains(newSymbol)) {
            newSymbol = "T_" + terminal + "_" + to_string(counter++);
        }
        terminalToNonTerminal[terminal] = newSymbol;
        _productions[newSymbol] = {{terminal}};
        _ordering.push_back(newSymbol);
    }

    for (auto &productions: _productions | views::values) {
        for (auto& production : productions) {
            if (production.size() >= 2) {
                for (auto& symbol : production) {
                    if (isTerminal(symbol) && symbol != EMPTY_SYMBOL) {
                        symbol = terminalToNonTerminal[symbol];
                    }
                }
            }
        }
    }

    const vector<Symbol> orderingSnapshot = _ordering;
    for (const auto& head : orderingSnapshot) {
        Productions newProductions;
        for (const auto& production : _productions[head]) {
            if (production.size() <= 2) {
                newProductions.push_back(production);
            } else {
                auto current = head;
                for (size_t i = 0; i + 2 < production.size(); ++i) {
                    const auto newSymbol = generatePrimedSymbol(current);
                    newProductions.push_back({production[i], newSymbol});
                    _productions[newSymbol] = {};
                    current = newSymbol;
                }
                _productions[current].push_back({production[production.size() - 2], production[production.size() - 1]});
            }
        }
        _productions[head] = newProductions;
    }

    unordered_set<Symbol> nullable;
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& [head, productions] : _productions) {
            if (nullable.contains(head)) continue;
            for (const auto& production : productions) {
                bool allNullable = true;
                for (const auto& symbol : production) {
                    if (symbol == EMPTY_SYMBOL) continue;
                    if (!nullable.contains(symbol)) {
                        allNullable = false;
                        break;
                    }
                }
                if (allNullable) {
                    nullable.insert(head);
                    changed = true;
                    break;
                }
            }
        }
    }

    const auto& startSymbol = _ordering[0];
    for (auto& [head, productions] : _productions) {
        Productions newProductions;
        for (const auto& production : productions) {
            if (production.size() == 1 && production[0] == EMPTY_SYMBOL) {
                if (head == startSymbol) {
                    newProductions.push_back(production);
                }
                continue;
            }

            vector<size_t> nullableIndices;
            for (size_t i = 0; i < production.size(); ++i) {
                if (nullable.contains(production[i])) {
                    nullableIndices.push_back(i);
                }
            }

            size_t subsets = 1u << nullableIndices.size();
            for (size_t mask = 0; mask < subsets; ++mask) {
                Production newProduction;
                unordered_set<size_t> skipIndices;
                for (size_t i = 0; i < nullableIndices.size(); ++i) {
                    if (mask & (1u << i)) {
                        skipIndices.insert(nullableIndices[i]);
                    }
                }
                for (size_t i = 0; i < production.size(); ++i) {
                    if (!skipIndices.contains(i)) {
                        newProduction.push_back(production[i]);
                    }
                }
                if (!newProduction.empty()) {
                    newProductions.push_back(newProduction);
                } else if (head == startSymbol) {
                    newProductions.push_back({EMPTY_SYMBOL});
                }
            }
        }
        _productions[head] = newProductions;
    }

    unordered_map<Symbol, unordered_set<Symbol>> unitGraph;
    for (const auto& [head, productions] : _productions) {
        for (const auto& production : productions) {
            if (production.size() == 1 && isNonTerminal(production[0])) {
                unitGraph[head].insert(production[0]);
            }
        }
    }

    unordered_map<Symbol, unordered_set<Symbol>> reachable;
    for (const auto& head : _ordering) {
        queue<Symbol> q;
        q.push(head);
        reachable[head].insert(head);
        while (!q.empty()) {
            auto current = q.front();
            q.pop();
            for (const auto& next : unitGraph[current]) {
                if (!reachable[head].contains(next)) {
                    reachable[head].insert(next);
                    q.push(next);
                }
            }
        }
    }

    for (auto& [head, productions] : _productions) {
        Productions newProductions;
        unordered_set<string> seen;
        for (const auto& production : productions) {
            if (production.size() == 1 && isNonTerminal(production[0])) {
                continue;
            }
            if (const auto key = computeProductionKey(production); !seen.contains(key)) {
                seen.insert(key);
                newProductions.push_back(production);
            }
        }
        for (const auto& reachableSymbol : reachable[head]) {
            if (reachableSymbol == head) continue;
            for (const auto& production : _productions[reachableSymbol]) {
                if (production.size() == 1 && isNonTerminal(production[0])) {
                    continue;
                }
                if (const auto key = computeProductionKey(production); !seen.contains(key)) {
                    seen.insert(key);
                    newProductions.push_back(production);
                }
            }
        }
        _productions[head] = newProductions;
    }

    unordered_set<Symbol> reachableFromStart;
    queue<Symbol> reachQueue;
    reachQueue.push(_ordering[0]);
    reachableFromStart.insert(_ordering[0]);
    while (!reachQueue.empty()) {
        auto current = reachQueue.front();
        reachQueue.pop();
        for (const auto& production : _productions[current]) {
            for (const auto& symbol : production) {
                if (isNonTerminal(symbol) && !reachableFromStart.contains(symbol)) {
                    reachableFromStart.insert(symbol);
                    reachQueue.push(symbol);
                }
            }
        }
    }

    vector<Symbol> newOrdering;
    for (const auto& symbol : _ordering) {
        if (reachableFromStart.contains(symbol)) {
            newOrdering.push_back(symbol);
        } else {
            _productions.erase(symbol);
        }
    }
    _ordering = newOrdering;

    deduplicate();
    initTerminals();
}
