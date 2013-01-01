#include "re.h"
#include <algorithm>
#include <queue>
#include <set>
#include <map>
#include <ranges>

using namespace std;

struct HopcroftData {
    vector<string> symbols;
    unordered_map<string, shared_ptr<DFAState>> idMap;
    map<string, map<string, vector<string>>> revEdges;
};

static HopcroftData getReverseEdges(const shared_ptr<DFAState>& start) {
    HopcroftData data;
    queue<shared_ptr<DFAState>> q;
    unordered_set<string> visited;

    q.push(start);
    visited.insert(start->id);

    while (!q.empty()) {
        const auto top = q.front();
        q.pop();
        data.idMap[top->id] = top;

        for (const auto& symbol : top->symbols) {
            if (ranges::find(data.symbols, symbol) == data.symbols.end()) {
                data.symbols.push_back(symbol);
            }

            auto next = top->trans.at(symbol);
            data.revEdges[next->id][symbol].push_back(top->id);

            if (!visited.contains(next->id)) {
                visited.insert(next->id);
                q.push(next);
            }
        }
    }

    return data;
}

static vector<vector<string>> hopcroft(const HopcroftData& data) {
    vector<string> ids;
    for (const auto& id : data.idMap | views::keys) {
        ids.push_back(id);
    }
    ranges::sort(ids);

    vector<string> group1, group2;
    for (const auto& id : ids) {
        if (data.idMap.at(id)->type == "accept") {
            group1.push_back(id);
        } else {
            group2.push_back(id);
        }
    }

    map<string, vector<string>> partitions;
    deque<string> workQueue;
    map<string, size_t> visited;

    auto makeKey = [](const vector<string>& v) {
        string key;
        for (const auto& s : v) {
            if (!key.empty()) {
                key += ",";
            }
            key += s;
        }
        return key;
    };

    string key1 = makeKey(group1);
    partitions[key1] = group1;
    workQueue.push_back(key1);
    visited[key1] = 0;

    if (!group2.empty()) {
        string key2 = makeKey(group2);
        partitions[key2] = group2;
        workQueue.push_back(key2);
    }

    while (!workQueue.empty()) {
        string topKey = workQueue.front();
        workQueue.pop_front();

        if (topKey.empty() || !partitions.contains(topKey)) {
            continue;
        }

        vector<string> top;
        size_t start = 0;
        for (size_t i = 0; i <= topKey.size(); i++) {
            if (i == topKey.size() || topKey[i] == ',') {
                top.push_back(topKey.substr(start, i - start));
                start = i + 1;
            }
        }

        for (const auto& symbol : data.symbols) {
            set<string> revGroup;
            for (const auto& id : top) {
                if (auto it = data.revEdges.find(id); it != data.revEdges.end()) {
                    if (auto it2 = it->second.find(symbol); it2 != it->second.end()) {
                        for (const auto& from : it2->second) {
                            revGroup.insert(from);
                        }
                    }
                }
            }

            vector<string> keys;
            for (const auto &k: partitions | views::keys) {
                keys.push_back(k);
            }

            for (const auto& key : keys) {
                vector<string> g1, g2;
                for (const auto& id : partitions[key]) {
                    if (revGroup.contains(id)) {
                        g1.push_back(id);
                    } else {
                        g2.push_back(id);
                    }
                }

                if (!g1.empty() && !g2.empty()) {
                    partitions.erase(key);
                    string newKey1 = makeKey(g1);
                    string newKey2 = makeKey(g2);
                    partitions[newKey1] = g1;
                    partitions[newKey2] = g2;

                    if (visited.contains(newKey1)) {
                        workQueue.push_back(newKey1);
                        workQueue.push_back(newKey2);
                    } else if (g1.size() <= g2.size()) {
                        visited[newKey1] = workQueue.size();
                        workQueue.push_back(newKey1);
                    } else {
                        visited[newKey2] = workQueue.size();
                        workQueue.push_back(newKey2);
                    }
                }
            }
        }
    }

    vector<vector<string>> result;
    for (const auto& v : partitions | views::values) {
        result.push_back(v);
    }
    return result;
}

static shared_ptr<DFAState> buildMinDFA(const shared_ptr<DFAState>& start, vector<vector<string>>& partitions, const HopcroftData& data) {
    ranges::sort(partitions, [](const auto& a, const auto& b) {
        string ka, kb;
        for (const auto& s : a) {
            if (!ka.empty()) {
                ka += ",";
                ka += s;
            }
        }
        for (const auto& s : b) {
            if (!kb.empty()) {
                kb += ",";
                kb += s;
            }
        }
        return ka < kb;
    });

    for (size_t i = 0; i < partitions.size(); i++) {
        bool containsStart = false;
        for (const auto& id : partitions[i]) {
            if (id == start->id) {
                containsStart = true;
                break;
            }
        }
        if (containsStart && i > 0) {
            swap(partitions[i], partitions[0]);
            break;
        }
    }

    vector<shared_ptr<DFAState>> nodes;
    unordered_map<string, size_t> group;
    map<size_t, map<size_t, set<string>>> edges;

    for (size_t i = 0; i < partitions.size(); i++) {
        auto node = make_shared<DFAState>();
        node->id = to_string(i + 1);
        string key;
        for (const auto& id : partitions[i]) {
            if (!key.empty()) {
                key += ",";
            }
            key += id;
        }
        node->key = key;
        node->type = data.idMap.at(partitions[i][0])->type;

        for (const auto& id : partitions[i]) {
            node->items.push_back(data.idMap.at(id)->items.empty() ? nullptr : data.idMap.at(id)->items[0]);
            group[id] = i;
        }
        nodes.push_back(node);
    }

    for (const auto& [to, symbolMap] : data.revEdges) {
        for (const auto& [symbol, froms] : symbolMap) {
            for (const auto& from : froms) {
                size_t fromGroup = group[from];
                size_t toGroup = group[to];
                edges[fromGroup][toGroup].insert(symbol);
            }
        }
    }

    for (const auto& [from, toMap] : edges) {
        for (const auto& [to, symbols] : toMap) {
            string symbol;
            for (const auto& s : symbols) {
                if (!symbol.empty()) symbol += ",";
                symbol += s;
            }
            nodes[from]->symbols.push_back(symbol);
            nodes[from]->edges.emplace_back(symbol, nodes[to]);
            nodes[from]->trans[symbol] = nodes[to];
        }
    }

    return nodes.empty() ? nullptr : nodes[0];
}

shared_ptr<DFAState> RegularExpression::toMinDFA(const shared_ptr<DFAState>& dfa) {
    if (!dfa) return nullptr;

    auto data = getReverseEdges(dfa);
    auto partitions = hopcroft(data);
    return buildMinDFA(dfa, partitions, data);
}
