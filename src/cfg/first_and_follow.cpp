#include "cfg.h"
#include <ranges>
#include <algorithm>

using namespace std;

size_t FirstAndFollowSet::size() const {
    return ordering.size();
}

string FirstAndFollowSet::symbolAt(const size_t index) const {
    return ordering[index];
}

bool FirstAndFollowSet::getNullable(const string& symbol) const {
    return first.find(symbol)->second.contains(ContextFreeGrammar::EMPTY_SYMBOL);
}

vector<string> FirstAndFollowSet::getFirstSet(const string& symbol) const {
    const auto it = first.find(symbol);
    auto result = vector<string>{it->second.begin(), it->second.end()};
    ranges::sort(result);
    return result;
}

vector<string> FirstAndFollowSet::getFollowSet(const string& symbol) const {
    const auto it = follow.find(symbol);
    auto result = vector<string>{it->second.begin(), it->second.end()};
    ranges::sort(result);
    return result;
}

FirstAndFollowSet ContextFreeGrammar::computeFirstAndFollowSet() const {
    FirstAndFollowSet result;
    if (_ordering.empty()) {
        return result;
    }
    result.ordering = _ordering;

    // First
    bool hasUpdate = true;
    result.first[EMPTY_SYMBOL].insert(EMPTY_SYMBOL);
    for (const auto& symbol : _terminals) {
        result.first[symbol].insert(symbol);
    }
    for (const auto& symbol : _ordering) {
        result.first[symbol];
    }
    while (hasUpdate) {
        hasUpdate = false;
        auto addToFirst = [&](const Symbol& head, const Symbol& symbol) {
            if (auto& firstSet = result.first[head]; !firstSet.contains(symbol)) {
                hasUpdate = true;
                firstSet.insert(symbol);
            }
        };
        for (const auto& head : _ordering) {
            for (const auto& production : _productions.at(head)) {
                bool nullable = true;
                for (const auto& symbol : production) {
                    if (isNonTerminal(symbol)) {
                        for (const auto& firstSymbol : result.first[symbol]) {
                            if (firstSymbol != EMPTY_SYMBOL) {
                                addToFirst(head, firstSymbol);
                            }
                        }
                    } else if (symbol != EMPTY_SYMBOL) {
                        addToFirst(head, symbol);
                    }
                    if (!result.getNullable(symbol)) {
                        nullable = false;
                        break;
                    }
                }
                if (nullable) {
                    addToFirst(head, EMPTY_SYMBOL);
                }
            }
        }
    }

    // Follow
    hasUpdate = true;
    for (const auto& symbol : _ordering) {
        result.follow[symbol];
    }
    result.follow[_ordering[0]].insert("$");
    while (hasUpdate) {
        hasUpdate = false;
        auto addToFollow = [&](const Symbol& head, const Symbol& symbol) {
            if (auto& followSet = result.follow[head]; !followSet.contains(symbol)) {
                hasUpdate = true;
                followSet.insert(symbol);
            }
        };
        for (const auto& head : _ordering) {
            for (const auto& production : _productions.at(head)) {
                bool nullable = true;
                for (int i = static_cast<int>(production.size()) - 1; i >= 0; --i) {
                    if (isNonTerminal(production[i])) {
                        if (nullable) {
                            for (const auto& followSymbol : result.follow[head]) {
                                addToFollow(production[i], followSymbol);
                            }
                        }
                        for (int j = i + 1; j < static_cast<int>(production.size()); ++j) {
                            for (const auto& firstSymbol : result.first.at(production[j])) {
                                if (firstSymbol != EMPTY_SYMBOL) {
                                    addToFollow(production[i], firstSymbol);
                                }
                            }
                            if (!result.getNullable(production[j])) {
                                break;
                            }
                        }
                    }
                    if (!result.getNullable(production[i])) {
                        nullable = false;
                    }
                }
            }
        }
    }

    return result;
}
