#include "cfg.h"
#include "string_utils.h"
#include <ranges>
#include <format>
#include <algorithm>

using namespace std;


bool ActionGotoTable::hasConflict() const {
    for (const auto& subActions : actions) {
        for (const auto& action : subActions | views::values) {
            if (action.size() > 1) {
                return true;
            }
        }
    }
    return false;
}

bool ActionGotoTable::hasConflict(const size_t index, const Symbol& symbol) const {
    if (const auto it = actions[index].find(symbol); it != actions[index].end()) {
        return it->second.size() > 1;
    }
    return false;
}

string ActionGotoTable::toString(const size_t index, const Symbol& symbol, const string& separator) const {
    if (const auto it = actions[index].find(symbol); it != actions[index].end()) {
        return stringJoin(it->second, separator);
    }
    return "";
}

std::string ActionGotoTable::toString(const ContextFreeGrammar& grammar, const string& separator) const {
    auto terminals = grammar.terminals();
    ranges::sort(terminals);
    const auto& nonTerminals = grammar.orderedNonTerminals();
    terminals.emplace_back(ContextFreeGrammar::EOF_SYMBOL);
    string result = "| State |";
    for (const auto& symbol : terminals) {
        result += " " + symbol + " |";
    }
    for (const auto& symbol : nonTerminals) {
        result += " " + symbol + " |";
    }
    result += "\n";
    result += "|:-:|";
    for (size_t i = 0; i < terminals.size() + nonTerminals.size(); i++) {
        result += ":-:|";
    }
    result += "\n";
    for (size_t i = 0; i < actions.size(); ++i) {
        result += format("| {} |", i);
        for (const auto& symbol : terminals) {
            result += " " + toString(i, symbol, separator) + " |";
        }
        for (const auto& symbol : nonTerminals) {
            result += " " + toString(i, symbol, separator) + " |";
        }
        result += "\n";
    }
    return result;
}
