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

/**
 * Compute FIRST and FOLLOW sets for all non-terminals.
 *
 * FIRST(X) = set of terminals that can begin strings derived from X
 * FOLLOW(X) = set of terminals that can appear immediately after X
 *
 * FIRST rules:
 * 1. FIRST(terminal) = {terminal}
 * 2. If X -> Y₁Y₂...Yₖ, add FIRST(Y₁) to FIRST(X); if Y₁ is nullable, also add FIRST(Y₂), etc.
 * 3. If X -> ε or all Yᵢ are nullable, add ε to FIRST(X)
 *
 * FOLLOW rules:
 * 1. Add $ to FOLLOW(start symbol)
 * 2. If A -> αBβ, add FIRST(β) - {ε} to FOLLOW(B)
 * 3. If A -> αB or A -> αBβ where β is nullable, add FOLLOW(A) to FOLLOW(B)
 */
FirstAndFollowSet ContextFreeGrammar::computeFirstAndFollowSet() const {
    FirstAndFollowSet result;
    if (_ordering.empty()) {
        return result;
    }
    result.ordering = _ordering;

    // FIRST set computation
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
                        // Add FIRST(symbol) - {ε} to FIRST(head)
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
                        break;  // Stop if this symbol is not nullable
                    }
                }
                if (nullable) {
                    addToFirst(head, EMPTY_SYMBOL);
                }
            }
        }
    }

    // FOLLOW set computation
    hasUpdate = true;
    for (const auto& symbol : _ordering) {
        result.follow[symbol];
    }
    result.follow[_ordering[0]].insert(EOF_SYMBOL);  // Rule 1
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
                // Scan right-to-left to track if suffix is nullable
                bool nullable = true;
                for (int i = static_cast<int>(production.size()) - 1; i >= 0; --i) {
                    if (isNonTerminal(production[i])) {
                        // Rule 3: If everything after production[i] is nullable,
                        // add FOLLOW(head) to FOLLOW(production[i])
                        if (nullable) {
                            for (const auto& followSymbol : result.follow[head]) {
                                addToFollow(production[i], followSymbol);
                            }
                        }
                        // Rule 2: Add FIRST(β) - {ε} where β is production[i+1...]
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
