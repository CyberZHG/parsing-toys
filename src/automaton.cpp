#include "automaton.h"
#include "string_utils.h"
#include "graph_layout.h"

using namespace std;
using namespace graph_layout;

string FiniteAutomatonNode::toString() const {
    string result = label + "\n===\n";
    result += kernel.toString() + "---\n";
    result += nonKernel.toString();
    return result;
}

const vector<FiniteAutomatonEdge>& FiniteAutomaton::edges() const {
    return _edges;
}

size_t FiniteAutomaton::size() const {
    return _nodes.size();
}

FiniteAutomatonNode& FiniteAutomaton::nodeAt(const size_t i) {
    return _nodes[i];
}

string FiniteAutomaton::newNodeLabel(const string& prefix) const {
    return prefix + toSubscript(_nodes.size());
}

size_t FiniteAutomaton::addNode(const FiniteAutomatonNode& node) {
    const auto key = node.kernel.toSortedString() + "---\n" + node.nonKernel.toSortedString();
    if (const auto it = _keyToNodeIndex.find(key); it != _keyToNodeIndex.end()) {
        return it->second;
    }
    const size_t index = _nodes.size();
    _keyToNodeIndex[key] = index;
    _nodes.emplace_back(node);
    return index;
}

size_t FiniteAutomaton::addEdge(const FiniteAutomatonEdge& edge) {
    const size_t index = _edges.size();
    _edges.emplace_back(edge);
    return index;
}

size_t FiniteAutomaton::addEdge(const size_t u, const size_t v, const string& label) {
    FiniteAutomatonEdge edge;
    edge.u = u;
    edge.v = v;
    edge.label = label;
    return addEdge(edge);
}

std::string FiniteAutomaton::toSVG(const bool darkMode) const {
    DirectedGraphHierarchicalLayout layout;
    layout.setFeedbackArcsMethod(FeedbackArcsMethod::MIN_ID);
    layout.attributes().setVertexDefaultMonospace();
    layout.attributes().setEdgeDefaultMonospace();
    layout.attributes().setVertexDefaultShape(AttributeShape::RECORD);
    layout.attributes().setEdgeDefaultSplines(AttributeSplines::SPLINE);
    layout.attributes().setRankDir(AttributeRankDir::LEFT_TO_RIGHT);
    if (darkMode) {
        layout.attributes().setVertexDefaultColor("white");
        layout.attributes().setVertexDefaultFontColor("white");
        layout.attributes().setEdgeDefaultColor("white");
        layout.attributes().setEdgeDefaultFontColor("white");
    }
    const auto n = _nodes.size();
    const auto& graph = layout.createGraph(n + 1);
    vector<string> nodeLabels(n + 1);
    for (size_t i = 0; i < n; ++i) {
        const auto& [accept, label, kernel, nonKernel] = _nodes[i];
        nodeLabels[i] = "{";
        nodeLabels[i] += label;
        nodeLabels[i] += "|";
        for (const auto& line : stringSplit(kernel.toString(), '\n')) {
            if (line.empty()) {
                continue;
            }
            nodeLabels[i] += stringReplace(stringReplace(line, '|', "\\|"), ' ', "\\ ") + "\\l";
        }
        if (!nonKernel.nonTerminals().empty()) {
            nodeLabels[i] += "|";
            for (const auto& line : stringSplit(nonKernel.toString(), '\n')) {
                if (line.empty()) {
                    continue;
                }
                nodeLabels[i] += stringReplace(stringReplace(line, '|', "\\|"), ' ', "\\ ") + "\\l";
            }
        }
        nodeLabels[i] += "}";
        if (accept) {
            const auto edgeId = graph->addEdge(static_cast<int>(i), static_cast<int>(n));
            layout.attributes().setEdgeTailLabel(edgeId, ContextFreeGrammar::EOF_SYMBOL);
        }
    }
    nodeLabels[n] = "accept";
    layout.setVertexLabels(nodeLabels);
    layout.attributes().setVertexShape(static_cast<int>(n), AttributeShape::NONE);
    for (const auto& [u, v, label] : _edges) {
        const auto edgeId = graph->addEdge(static_cast<int>(u), static_cast<int>(v));
        layout.attributes().setEdgeTailLabel(edgeId, label);
    }
    layout.layoutGraph();
    return layout.render();
}

string FiniteAutomaton::edgesToString() const {
    string result;
    for (const auto&[u, v, label] : _edges) {
        result += to_string(u) + " -- " + label + " --> " + to_string(v) + "\n";
    }
    return result;
}
