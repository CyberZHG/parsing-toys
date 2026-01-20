#include "re.h"
#include "graph_layout.h"
#include <queue>

using namespace std;
using namespace graph_layout;

size_t NFAGraph::size() const {
    return states.size();
}

size_t NFAGraph::numEdges() const {
    return edges.size();
}

size_t NFAGraph::edgeFrom(const size_t index) const {
    return get<0>(edges[index]);
}

size_t NFAGraph::edgeTo(const size_t index) const {
    return get<1>(edges[index]);
}

string NFAGraph::edgeLabel(const size_t index) const {
    return get<2>(edges[index]);
}

string NFAGraph::stateAt(const size_t index) const {
    const auto& state = states[index];
    return to_string(state->id) + " (" + state->type + ")";
}

string NFAGraph::toSVG(const bool darkMode) const {
    DirectedGraphHierarchicalLayout layout;
    layout.setFeedbackArcsMethod(FeedbackArcsMethod::MIN_ID);
    layout.attributes().setVertexDefaultMonospace();
    layout.attributes().setEdgeDefaultMonospace();
    layout.attributes().setVertexDefaultShape(AttributeShape::CIRCLE);
    layout.attributes().setEdgeDefaultSplines(AttributeSplines::LINE);
    layout.attributes().setRankDir(AttributeRankDir::LEFT_TO_RIGHT);
    if (darkMode) {
        layout.attributes().setVertexDefaultColor("white");
        layout.attributes().setVertexDefaultFontColor("white");
        layout.attributes().setEdgeDefaultColor("white");
        layout.attributes().setEdgeDefaultFontColor("white");
    }

    const auto n = states.size();
    const auto& graph = layout.createGraph(static_cast<int>(n));
    vector<string> nodeLabels(n);

    for (size_t i = 0; i < n; ++i) {
        nodeLabels[i] = to_string(states[i]->id);
        if (states[i]->type == "accept") {
            layout.attributes().setVertexShape(static_cast<int>(i), AttributeShape::DOUBLE_CIRCLE);
        }
    }
    layout.setVertexLabels(nodeLabels);

    for (const auto& [u, v, label] : edges) {
        const auto edgeId = graph->addEdge(static_cast<int>(u), static_cast<int>(v));
        layout.setEdgeLabel(edgeId, label);
    }

    layout.layoutGraph();
    return layout.render();
}

size_t DFAGraph::size() const {
    return states.size();
}

size_t DFAGraph::numEdges() const {
    return edges.size();
}

size_t DFAGraph::edgeFrom(const size_t index) const {
    return get<0>(edges[index]);
}

size_t DFAGraph::edgeTo(const size_t index) const {
    return get<1>(edges[index]);
}

string DFAGraph::edgeLabel(const size_t index) const {
    return get<2>(edges[index]);
}

string DFAGraph::stateIdAt(const size_t index) const {
    return states[index]->id;
}

string DFAGraph::stateKeyAt(const size_t index) const {
    return states[index]->key;
}

string DFAGraph::stateTypeAt(const size_t index) const {
    return states[index]->type;
}

string DFAGraph::toSVG(const bool darkMode) const {
    DirectedGraphHierarchicalLayout layout;
    layout.setFeedbackArcsMethod(FeedbackArcsMethod::MIN_ID);
    layout.attributes().setVertexDefaultMonospace();
    layout.attributes().setEdgeDefaultMonospace();
    layout.attributes().setVertexDefaultShape(AttributeShape::CIRCLE);
    layout.attributes().setEdgeDefaultSplines(AttributeSplines::LINE);
    layout.attributes().setRankDir(AttributeRankDir::LEFT_TO_RIGHT);
    if (darkMode) {
        layout.attributes().setVertexDefaultColor("white");
        layout.attributes().setVertexDefaultFontColor("white");
        layout.attributes().setEdgeDefaultColor("white");
        layout.attributes().setEdgeDefaultFontColor("white");
    }

    const auto n = states.size();
    const auto& graph = layout.createGraph(static_cast<int>(n));
    vector<string> nodeLabels(n);

    for (size_t i = 0; i < n; ++i) {
        nodeLabels[i] = states[i]->id;
        if (states[i]->type == "accept") {
            layout.attributes().setVertexShape(static_cast<int>(i), AttributeShape::DOUBLE_CIRCLE);
        }
    }
    layout.setVertexLabels(nodeLabels);

    for (const auto& [u, v, label] : edges) {
        const auto edgeId = graph->addEdge(static_cast<int>(u), static_cast<int>(v));
        layout.setEdgeLabel(edgeId, label);
    }

    layout.layoutGraph();
    return layout.render();
}

NFAGraph RegularExpression::toNFAGraph(const shared_ptr<NFAState>& nfa) {
    NFAGraph graph;
    if (!nfa) {
        return graph;
    }

    unordered_map<size_t, size_t> idToIndex;
    queue<shared_ptr<NFAState>> q;
    q.push(nfa);
    idToIndex[nfa->id] = 0;
    graph.states.push_back(nfa);

    while (!q.empty()) {
        const auto state = q.front();
        q.pop();
        size_t fromIndex = idToIndex[state->id];

        for (const auto& [label, target] : state->edges) {
            if (!idToIndex.contains(target->id)) {
                idToIndex[target->id] = graph.states.size();
                graph.states.push_back(target);
                q.push(target);
            }
            graph.edges.emplace_back(fromIndex, idToIndex[target->id], label);
        }
    }

    return graph;
}

DFAGraph RegularExpression::toDFAGraph(const shared_ptr<DFAState>& dfa) {
    DFAGraph graph;
    if (!dfa) {
        return graph;
    }

    unordered_map<string, size_t> idToIndex;
    queue<shared_ptr<DFAState>> q;
    q.push(dfa);
    idToIndex[dfa->id] = 0;
    graph.states.push_back(dfa);

    while (!q.empty()) {
        const auto state = q.front();
        q.pop();
        size_t fromIndex = idToIndex[state->id];

        for (const auto& [label, target] : state->edges) {
            if (!idToIndex.contains(target->id)) {
                idToIndex[target->id] = graph.states.size();
                graph.states.push_back(target);
                q.push(target);
            }
            graph.edges.emplace_back(fromIndex, idToIndex[target->id], label);
        }
    }

    return graph;
}
