#include "re.h"
#include <algorithm>
#include <queue>
#include <ranges>

using namespace std;

static string toAlphaCount(size_t n) {
    string s;
    while (true) {
        constexpr int len = 26;
        constexpr int a = 'A';
        s = string(1, static_cast<char>(n % len + a)) + s;
        if (n < static_cast<size_t>(len)) {
            break;
        }
        n = n / len - 1;
    }
    return s;
}

static shared_ptr<DFAState> getClosure(const vector<shared_ptr<NFAState>>& nodes) {
    auto closure = make_shared<DFAState>();
    vector<shared_ptr<NFAState>> stack;
    unordered_set<size_t> visited;
    vector<string> symbols;

    for (const auto& node : nodes) {
        stack.push_back(node);
        closure->items.push_back(node);
        visited.insert(node->id);
        if (node->type == "accept") {
            closure->type = "accept";
        }
    }

    while (!stack.empty()) {
        const auto top = stack.back();
        stack.pop_back();

        for (const auto& [label, target] : top->edges) {
            if (label == RegularExpression::EPSILON) {
                if (!visited.contains(target->id)) {
                    stack.push_back(target);
                    closure->items.push_back(target);
                    visited.insert(target->id);
                    if (target->type == "accept") {
                        closure->type = "accept";
                    }
                }
            } else {
                if (ranges::find(symbols, label) == symbols.end()) {
                    symbols.push_back(label);
                }
            }
        }
    }

    ranges::sort(closure->items, [](const auto& a, const auto& b) {
        return a->id < b->id;
    });
    ranges::sort(symbols);

    string key;
    for (const auto& item : closure->items) {
        if (!key.empty()) {
            key += ",";
        }
        key += to_string(item->id);
    }
    closure->key = key;
    closure->symbols = symbols;

    return closure;
}

static shared_ptr<DFAState> getClosedMove(const shared_ptr<DFAState>& state, const string& symbol) {
    vector<shared_ptr<NFAState>> nexts;

    for (const auto& item : state->items) {
        for (const auto& [label, target] : item->edges) {
            if (label == symbol) {
                bool found = false;
                for (const auto& n : nexts) {
                    if (n->id == target->id) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    nexts.push_back(target);
                }
            }
        }
    }

    return getClosure(nexts);
}

shared_ptr<DFAState> RegularExpression::toDFA(const shared_ptr<NFAState>& nfa) {
    if (!nfa) {
        return nullptr;
    }

    auto first = getClosure({nfa});
    unordered_map<string, shared_ptr<DFAState>> states;
    queue<shared_ptr<DFAState>> q;
    size_t count = 0;

    first->id = toAlphaCount(count++);
    states[first->key] = first;
    q.push(first);

    while (!q.empty()) {
        auto top = q.front();
        q.pop();

        for (const auto& symbol : top->symbols) {
            auto closure = getClosedMove(top, symbol);
            if (!states.contains(closure->key)) {
                closure->id = toAlphaCount(count++);
                states[closure->key] = closure;
                q.push(closure);
            }
            top->trans[symbol] = states[closure->key];
            top->edges.emplace_back(symbol, states[closure->key]);
        }
    }

    return first;
}
