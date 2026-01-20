#include "cfg.h"
#include "string_utils.h"
#include "graph_layout.h"
#include <ranges>
#include <format>
#include <algorithm>
#include <functional>

using namespace std;
using namespace graph_layout;

void LRParsingSteps::addStep(const vector<size_t>& _stack, const vector<Symbol>& _symbols, const vector<Symbol>& _remainingInputs, const string& action) {
    stack.emplace_back(_stack);
    symbols.emplace_back(_symbols);
    remainingInputs.emplace_back(_remainingInputs);
    actions.emplace_back(action);
}

string LRParsingSteps::toString() const {
    string result = R"(| Stack | Symbols | Inputs | Action |
|:-:|:-:|:-:|:-:|
)";
    for (size_t i = 0; i < stack.size(); i++) {
        result += "| ";
        for (const auto& state : stack[i]) {
            result += format("{} ", state);
        }
        result += "| ";
        for (const auto& symbol : symbols[i]) {
            result += format("{} ", symbol);
        }
        result += "| ";
        for (const auto& symbol : remainingInputs[i]) {
            result += format("{} ", symbol);
        }
        result += "| " + actions[i] + " |\n";
    }
    return result;
}

size_t ParseTreeNode::size() const {
    size_t result = 1;
    for (const auto& child : children) {
        result += child->size();
    }
    return result;
}

string ParseTreeNode::toSVG(const bool darkMode) const {
    DirectedGraphHierarchicalLayout layout;
    layout.attributes().setVertexDefaultMonospace();
    layout.attributes().setVertexDefaultShape(AttributeShape::ELLIPSE);
    layout.attributes().setEdgeDefaultArrowTail(AttributeArrowShape::NORMAL);
    layout.attributes().setEdgeDefaultArrowHead(AttributeArrowShape::NONE);
    layout.attributes().setRankDir(AttributeRankDir::TOP_TO_BOTTOM);
    if (darkMode) {
        layout.attributes().setVertexDefaultColor("white");
        layout.attributes().setVertexDefaultFontColor("white");
        layout.attributes().setEdgeDefaultColor("white");
        layout.attributes().setEdgeDefaultFontColor("white");
    }
    const auto n = size();
    const auto& graph = layout.createGraph(n);
    int globalIndex = 0;
    vector<string> vertexLabels(n);
    function<int(const ParseTreeNode*)> buildGraph = [&](const ParseTreeNode* node) {
        const int u = globalIndex++;
        vertexLabels[u] = node->label;
        if (node->terminal) {
            layout.attributes().setVertexShape(u, AttributeShape::NONE);
        }
        for (const auto& child : node->children) {
            const int v = buildGraph(child.get());
            graph->addEdge(u, v);
        }
        return u;
    };
    buildGraph(this);
    layout.setVertexLabels(vertexLabels);
    layout.layoutGraph();
    return layout.render();
}

string ParseTreeNode::toString(const int indent) const {
    string result(indent, ' ');
    result += label + "\n";
    for (const auto& child : children) {
        result += child->toString(indent + 2);
    }
    return result;
}

void ActionGotoTable::addShift(const size_t index, const Symbol& symbol, const size_t nextState) {
    actions[index][symbol].emplace_back(format("shift {}", nextState));
    nextStates[index][symbol] = nextState;
}

void ActionGotoTable::addGoto(const size_t index, const Symbol& symbol, const size_t nextState) {
    actions[index][symbol].emplace_back(format("{}", nextState));
    nextStates[index][symbol] = nextState;
}

void ActionGotoTable::addReduce(const size_t index, const Symbol& symbol, const Symbol& head, Production production) {
    production.pop_back();
    if (production.empty()) {
        production.emplace_back(ContextFreeGrammar::EMPTY_SYMBOL);
    }
    string action = "reduce " + head + " ->";
    for (const auto& s : production) {
        action += " " + s;
    }
    actions[index][symbol].emplace_back(action);
    reduceHeads[index][symbol] = head;
    reduceProductions[index][symbol] = production;
}

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

/**
 * Simulate LR parsing using the ACTION/GOTO table.
 *
 * @param s Input string (space-separated tokens, e.g., "id + id * id")
 * @return Parsing steps.
 */
LRParsingSteps ActionGotoTable::parse(const string& s) {
    LRParsingSteps steps;
    vector<size_t> stack = {0};   // State stack, initialized with state 0
    vector<string> symbols;       // Symbol stack (terminals and non-terminals)
    vector<string> remaining = stringSplit(s, ' ', true);
    remaining.emplace_back(ContextFreeGrammar::EOF_SYMBOL);
    vector<shared_ptr<ParseTreeNode>> treeNodes = {nullptr};

    // Remove empty symbols from input
    size_t n = 0;
    for (size_t i = 0; i < remaining.size(); ++i) {
        if (remaining[i] != ContextFreeGrammar::EMPTY_SYMBOL) {
            remaining[n++] = remaining[i];
        }
    }
    remaining.resize(n);

    while (!remaining.empty()) {
        const auto state = stack.back();
        const auto nextSymbol = remaining.front();

        // Check for conflicts (multiple actions for same state/symbol)
        if (hasConflict(state, nextSymbol)) {
            steps.addStep(stack, symbols, remaining, "conflict: " + toString(state, nextSymbol));
            break;
        }

        // Look up action in ACTION/GOTO table
        const auto it = actions[state].find(nextSymbol);
        if (it == actions[state].end() || it->second.empty()) {
            steps.addStep(stack, symbols, remaining, "invalid symbol");
            break;
        }
        steps.addStep(stack, symbols, remaining, it->second[0]);
        if (it->second[0] == "accept") {
            parseTree = treeNodes.back();
            break;
        }

        // Shift action: nextState exists for terminals
        if (const auto shiftIt = nextStates[state].find(nextSymbol); shiftIt != nextStates[state].end()) {
            // Push new state and symbol, consume input
            stack.push_back(shiftIt->second);
            symbols.push_back(nextSymbol);
            remaining.erase(remaining.begin(), remaining.begin() + 1);
            treeNodes.emplace_back(make_shared<ParseTreeNode>());
            treeNodes.back()->label = nextSymbol;
            treeNodes.back()->terminal = true;
        } else {
            // Reduce action: A -> α (pop |α| items, then GOTO)
            const auto productionIt = reduceProductions[state].find(nextSymbol);
            if (productionIt == reduceProductions[state].end()) {
                steps.addStep(stack, symbols, remaining, "invalid action/goto table");
                break;
            }

            // Pop |α| states and symbols from both stacks
            auto numPopStates = productionIt->second.size();
            if (productionIt->second.size() == 1 && productionIt->second[0] == ContextFreeGrammar::EMPTY_SYMBOL) {
                numPopStates = 0;
            }
            stack.resize(stack.size() - numPopStates);
            symbols.resize(symbols.size() - numPopStates);
            const auto treeNode = make_shared<ParseTreeNode>();
            treeNode->terminal = false;
            for (size_t i = 0; i < numPopStates; i++) {
                 treeNode->children.emplace_back(treeNodes[treeNodes.size() - numPopStates + i]);
            }
            treeNodes.resize(treeNodes.size() - numPopStates);
            treeNodes.emplace_back(treeNode);

            // Find the reduced non-terminal A
            const auto headIt = reduceHeads[state].find(nextSymbol);
            if (headIt == reduceHeads[state].end()) {
                steps.addStep(stack, symbols, remaining, "invalid action/goto table");
                break;
            }
            treeNode->label = headIt->second + " ->";
            for (const auto& symbol : productionIt->second) {
                treeNode->label += " " + symbol;
            }

            // GOTO[stack.top(), A]: push new state and the non-terminal
            const auto gotoIt = nextStates[stack.back()].find(headIt->second);
            if (gotoIt == nextStates[stack.back()].end()) {
                steps.addStep(stack, symbols, remaining, "invalid action/goto table");
                break;
            }
            stack.push_back(gotoIt->second);
            symbols.push_back(headIt->second);
        }
    }
    return steps;
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
