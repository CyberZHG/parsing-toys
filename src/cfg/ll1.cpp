#include "cfg.h"
#include "string_utils.h"
#include <ranges>
#include <algorithm>

using namespace std;

void LLParsingSteps::addStep(const vector<Symbol>& _stack, const vector<Symbol>& _remainingInputs, const string& action) {
    stack.emplace_back(_stack);
    remainingInputs.emplace_back(_remainingInputs);
    actions.emplace_back(action);
}

string LLParsingSteps::toString() const {
    string result = R"(| Stack | Input | Action |
|:-:|:-:|:-:|
)";
    for (size_t i = 0; i < stack.size(); i++) {
        result += "| ";
        for (const auto& symbol : stack[i]) {
            result += symbol + " ";
        }
        result += "| ";
        for (const auto& symbol : remainingInputs[i]) {
            result += symbol + " ";
        }
        result += "| " + actions[i] + " |\n";
    }
    return result;
}

void MTable::addEntry(const Symbol& nonTerminal, const Symbol& terminal, const Production& production) {
    if (!nonTerminalIndex.contains(nonTerminal)) {
        nonTerminalIndex[nonTerminal] = nonTerminals.size();
        nonTerminals.push_back(nonTerminal);
        entries.emplace_back();
    }
    if (!terminalIndex.contains(terminal)) {
        terminalIndex[terminal] = terminals.size();
        terminals.push_back(terminal);
    }
    const size_t row = nonTerminalIndex[nonTerminal];
    const size_t col = terminalIndex[terminal];
    if (entries[row].size() <= col) {
        entries[row].resize(col + 1);
    }
    entries[row][col].push_back(production);
}

bool MTable::hasConflict() const {
    for (const auto& row : entries) {
        for (const auto& cell : row) {
            if (cell.size() > 1) {
                return true;
            }
        }
    }
    return false;
}

bool MTable::hasConflict(const Symbol& nonTerminal, const Symbol& terminal) const {
    if (!nonTerminalIndex.contains(nonTerminal) || !terminalIndex.contains(terminal)) {
        return false;
    }
    const size_t row = nonTerminalIndex.at(nonTerminal);
    const size_t col = terminalIndex.at(terminal);
    if (row >= entries.size() || col >= entries[row].size()) return false;
    return entries[row][col].size() > 1;
}

string MTable::getCell(const Symbol& nonTerminal, const Symbol& terminal, const string& separator) const {
    if (!nonTerminalIndex.contains(nonTerminal) || !terminalIndex.contains(terminal)) {
        return "";
    }
    const size_t row = nonTerminalIndex.at(nonTerminal);
    const size_t col = terminalIndex.at(terminal);
    if (row >= entries.size() || col >= entries[row].size()) return "";
    vector<string> results;
    for (const auto& prod : entries[row][col]) {
        string s = nonTerminal + " ->";
        for (const auto& sym : prod) {
            s += " " + sym;
        }
        results.push_back(s);
    }
    return stringJoin(results, separator);
}

size_t MTable::numNonTerminals() const {
    return nonTerminals.size();
}

size_t MTable::numTerminals() const {
    return terminals.size();
}

Symbol MTable::getNonTerminal(const size_t index) const {
    return nonTerminals[index];
}

Symbol MTable::getTerminal(const size_t index) const {
    return terminals[index];
}

LLParsingSteps MTable::parse(const string& s) {
    LLParsingSteps steps;
    vector stack = {ContextFreeGrammar::EOF_SYMBOL};
    if (!nonTerminals.empty()) {
        stack.push_back(nonTerminals[0]);
    }
    vector<Symbol> remaining = stringSplit(s, ' ', true);
    remaining.emplace_back(ContextFreeGrammar::EOF_SYMBOL);

    size_t n = 0;
    for (size_t i = 0; i < remaining.size(); ++i) {
        if (remaining[i] != ContextFreeGrammar::EMPTY_SYMBOL) {
            remaining[n++] = remaining[i];
        }
    }
    remaining.resize(n);

    struct StackNode {
        Symbol symbol;
        shared_ptr<ParseTreeNode> treeNode;
    };
    vector<StackNode> parseStack;
    auto rootNode = make_shared<ParseTreeNode>();
    rootNode->terminal = false;
    rootNode->label = nonTerminals.empty() ? "" : nonTerminals[0];
    parseStack.push_back({ContextFreeGrammar::EOF_SYMBOL, nullptr});
    if (!nonTerminals.empty()) {
        parseStack.push_back({nonTerminals[0], rootNode});
    }

    while (!stack.empty() && !remaining.empty()) {
        const Symbol top = stack.back();
        const Symbol input = remaining.front();

        if (top == ContextFreeGrammar::EOF_SYMBOL && input == ContextFreeGrammar::EOF_SYMBOL) {
            steps.addStep(stack, remaining, "accept");
            parseTree = rootNode;
            break;
        }

        if (top == ContextFreeGrammar::EMPTY_SYMBOL) {
            stack.pop_back();
            parseStack.pop_back();
            continue;
        }

        if (!nonTerminalIndex.contains(top)) {
            if (top == input) {
                steps.addStep(stack, remaining, "match " + top);
                stack.pop_back();
                remaining.erase(remaining.begin());
                auto& node = parseStack.back();
                if (node.treeNode) {
                    node.treeNode->terminal = true;
                }
                parseStack.pop_back();
            } else {
                steps.addStep(stack, remaining, "error: expected " + top);
                break;
            }
        } else {
            if (!terminalIndex.contains(input)) {
                steps.addStep(stack, remaining, "error: unexpected symbol " + input);
                break;
            }
            const size_t row = nonTerminalIndex.at(top);
            const size_t col = terminalIndex.at(input);
            if (row >= entries.size() || col >= entries[row].size() || entries[row][col].empty()) {
                steps.addStep(stack, remaining, "error: no rule for M[" + top + ", " + input + "]");
                break;
            }
            if (entries[row][col].size() > 1) {
                steps.addStep(stack, remaining, "conflict: " + getCell(top, input, " / "));
                break;
            }
            const auto& production = entries[row][col][0];
            string actionStr = top + " ->";
            for (const auto& sym : production) {
                actionStr += " " + sym;
            }
            steps.addStep(stack, remaining, actionStr);

            stack.pop_back();
            auto currentNode = parseStack.back().treeNode;
            parseStack.pop_back();

            for (const auto& it : std::ranges::reverse_view(production)) {
                stack.push_back(it);
            }
            for (const auto& sym : production) {
                auto childNode = make_shared<ParseTreeNode>();
                childNode->label = sym;
                childNode->terminal = !nonTerminalIndex.contains(sym);
                if (currentNode) {
                    currentNode->children.push_back(childNode);
                }
            }
            for (auto it = production.rbegin(); it != production.rend(); ++it) {
                if (currentNode && !currentNode->children.empty()) {
                    size_t idx = production.rend() - it - 1;
                    parseStack.push_back({*it, currentNode->children[idx]});
                } else {
                    parseStack.push_back({*it, nullptr});
                }
            }
        }
    }

    if (stack.empty() && remaining.empty()) {
        parseTree = rootNode;
    }

    return steps;
}

string MTable::toString(const string& separator) const {
    string result = "| |";
    for (const auto& t : terminals) {
        result += " " + t + " |";
    }
    result += "\n|:-:|";
    for (size_t i = 0; i < terminals.size(); i++) {
        result += ":-:|";
    }
    result += "\n";
    for (const auto& nonTerminal : nonTerminals) {
        result += "| " + nonTerminal + " |";
        for (const auto & terminal : terminals) {
            result += " " + getCell(nonTerminal, terminal, separator) + " |";
        }
        result += "\n";
    }
    return result;
}

static unordered_set<Symbol> computeFirstOfProduction(const Production& production, const unordered_map<Symbol, unordered_set<Symbol>>& firstSets) {
    unordered_set<Symbol> result;
    bool allNullable = true;
    for (const auto& symbol : production) {
        if (symbol == ContextFreeGrammar::EMPTY_SYMBOL) {
            continue;
        }
        if (firstSets.contains(symbol)) {
            for (const auto& s : firstSets.at(symbol)) {
                if (s != ContextFreeGrammar::EMPTY_SYMBOL) {
                    result.insert(s);
                }
            }
            if (!firstSets.at(symbol).contains(ContextFreeGrammar::EMPTY_SYMBOL)) {
                allNullable = false;
                break;
            }
        } else {
            result.insert(symbol);
            allNullable = false;
            break;
        }
    }
    if (allNullable) {
        result.insert(ContextFreeGrammar::EMPTY_SYMBOL);
    }
    return result;
}

MTable ContextFreeGrammar::computeLL1Table() const {
    const auto firstFollow = computeFirstAndFollowSet();
    MTable table;
    table.terminals = vector(_terminals.begin(), _terminals.end());
    ranges::sort(table.terminals);
    table.terminals.emplace_back(EOF_SYMBOL);
    for (const auto& symbol : table.terminals) {
        table.terminalIndex[symbol] = table.terminalIndex.size();
    }
    table.nonTerminals = vector(_ordering.begin(), _ordering.end());
    for (const auto& symbol : table.nonTerminals) {
        table.nonTerminalIndex[symbol] = table.nonTerminalIndex.size();
        table.entries.emplace_back(table.terminals.size());
    }

    for (const auto& head : _ordering) {
        for (const auto& production : _productions.at(head)) {
            auto firstSet = computeFirstOfProduction(production, firstFollow.first);
            for (const auto& terminal : firstSet) {
                if (terminal != EMPTY_SYMBOL) {
                    table.addEntry(head, terminal, production);
                }
            }
            if (firstSet.contains(EMPTY_SYMBOL)) {
                if (firstFollow.follow.contains(head)) {
                    for (const auto& terminal : firstFollow.follow.at(head)) {
                        table.addEntry(head, terminal, production);
                    }
                }
            }
        }
    }

    return table;
}
