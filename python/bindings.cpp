#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <memory>
#include "cfg.h"
#include "automaton.h"
#include "re.h"
using namespace std;

namespace py = pybind11;

struct AutomatonWrapper {
    unique_ptr<FiniteAutomaton> automaton;

    explicit AutomatonWrapper(unique_ptr<FiniteAutomaton> a) : automaton(std::move(a)) {}

    size_t size() const { return automaton->size(); }
    string to_svg(const bool darkMode = false) const { return automaton->toSVG(darkMode); }
};

PYBIND11_MODULE(_core, m, py::mod_gil_not_used()) {
    m.doc() = "Python bindings for Parsing Toys - CFG parsing algorithms";

    py::class_<FirstAndFollowSet>(m, "FirstAndFollowSet")
        .def(py::init<>())
        .def("size", &FirstAndFollowSet::size)
        .def("symbol_at", &FirstAndFollowSet::symbolAt, py::arg("index"))
        .def("get_nullable", &FirstAndFollowSet::getNullable, py::arg("symbol"))
        .def("get_first_set", &FirstAndFollowSet::getFirstSet, py::arg("symbol"))
        .def("get_follow_set", &FirstAndFollowSet::getFollowSet, py::arg("symbol"))
    ;

    py::class_<LRParsingSteps>(m, "LRParsingSteps")
        .def(py::init<>())
        .def("size", [](const LRParsingSteps& self) { return self.stack.size(); })
        .def("get_stack", [](const LRParsingSteps& self, size_t i) { return self.stack[i]; }, py::arg("index"))
        .def("get_symbols", [](const LRParsingSteps& self, size_t i) { return self.symbols[i]; }, py::arg("index"))
        .def("get_remaining_inputs", [](const LRParsingSteps& self, size_t i) { return self.remainingInputs[i]; }, py::arg("index"))
        .def("get_action", [](const LRParsingSteps& self, size_t i) { return self.actions[i]; }, py::arg("index"))
        .def("__str__", &LRParsingSteps::toString)
    ;

    py::class_<ParseTreeNode, shared_ptr<ParseTreeNode>>(m, "ParseTreeNode")
        .def(py::init<>())
        .def_readonly("terminal", &ParseTreeNode::terminal)
        .def_readonly("label", &ParseTreeNode::label)
        .def_readonly("children", &ParseTreeNode::children)
        .def("size", &ParseTreeNode::size)
        .def("to_svg", &ParseTreeNode::toSVG, py::arg("dark_mode") = false)
        .def("__str__", [](const ParseTreeNode& self) { return self.toString(); })
    ;

    py::class_<ActionGotoTable>(m, "ActionGotoTable")
        .def(py::init<>())
        .def(py::init<size_t>(), py::arg("n"))
        .def("size", [](const ActionGotoTable& self) { return self.actions.size(); })
        .def("has_conflict", py::overload_cast<>(&ActionGotoTable::hasConflict, py::const_))
        .def("has_conflict_at", py::overload_cast<size_t, const Symbol&>(&ActionGotoTable::hasConflict, py::const_), py::arg("index"), py::arg("symbol"))
        .def("get_cell", [](const ActionGotoTable& self, size_t index, const string& symbol, const string& separator) {
            return self.toString(index, symbol, separator);
        }, py::arg("index"), py::arg("symbol"), py::arg("separator") = " / ")
        .def("parse", &ActionGotoTable::parse, py::arg("s"))
        .def_property_readonly("parse_tree", [](const ActionGotoTable& self) { return self.parseTree; })
    ;

    py::class_<LLParsingSteps>(m, "LLParsingSteps")
        .def(py::init<>())
        .def("size", [](const LLParsingSteps& self) { return self.stack.size(); })
        .def("get_stack", [](const LLParsingSteps& self, size_t i) { return self.stack[i]; }, py::arg("index"))
        .def("get_remaining_inputs", [](const LLParsingSteps& self, size_t i) { return self.remainingInputs[i]; }, py::arg("index"))
        .def("get_action", [](const LLParsingSteps& self, size_t i) { return self.actions[i]; }, py::arg("index"))
        .def("__str__", &LLParsingSteps::toString)
    ;

    py::class_<MTable>(m, "MTable")
        .def(py::init<>())
        .def("num_non_terminals", &MTable::numNonTerminals)
        .def("num_terminals", &MTable::numTerminals)
        .def("get_non_terminal", &MTable::getNonTerminal, py::arg("index"))
        .def("get_terminal", &MTable::getTerminal, py::arg("index"))
        .def("has_conflict", py::overload_cast<>(&MTable::hasConflict, py::const_))
        .def("has_conflict_at", py::overload_cast<const Symbol&, const Symbol&>(&MTable::hasConflict, py::const_), py::arg("non_terminal"), py::arg("terminal"))
        .def("get_cell", &MTable::getCell, py::arg("non_terminal"), py::arg("terminal"), py::arg("separator") = " / ")
        .def("parse", &MTable::parse, py::arg("s"))
        .def_property_readonly("parse_tree", [](const MTable& self) { return self.parseTree; })
        .def("__str__", [](const MTable& self) { return self.toString(); })
    ;

    py::class_<CYKTable>(m, "CYKTable")
        .def(py::init<size_t>(), py::arg("size"))
        .def("size", &CYKTable::size)
        .def("get_cell", &CYKTable::getCell, py::arg("r"), py::arg("c"))
        .def("get_cell_string", &CYKTable::getCellString, py::arg("r"), py::arg("c"), py::arg("separator") = ", ")
        .def_property_readonly("accepted", [](const CYKTable& self) { return self.accepted; })
        .def_property_readonly("parse_tree", [](const CYKTable& self) { return self.parseTree; })
    ;

    py::class_<AutomatonWrapper>(m, "FiniteAutomaton")
        .def("size", &AutomatonWrapper::size)
        .def("to_svg", &AutomatonWrapper::to_svg, py::arg("dark_mode") = false)
    ;

    py::class_<ContextFreeGrammar>(m, "ContextFreeGrammar")
        .def(py::init<>())
        .def_property_readonly_static("EMPTY_SYMBOL", [](py::object) { return ContextFreeGrammar::EMPTY_SYMBOL; })
        .def_property_readonly_static("DOT_SYMBOL", [](py::object) { return ContextFreeGrammar::DOT_SYMBOL; })
        .def_property_readonly_static("EOF_SYMBOL", [](py::object) { return ContextFreeGrammar::EOF_SYMBOL; })
        .def("parse", &ContextFreeGrammar::parse, py::arg("s"))
        .def("error_message", &ContextFreeGrammar::errorMessage)
        .def("terminals", &ContextFreeGrammar::terminals)
        .def("non_terminals", &ContextFreeGrammar::nonTerminals)
        .def("ordered_non_terminals", &ContextFreeGrammar::orderedNonTerminals)
        .def("is_terminal", &ContextFreeGrammar::isTerminal, py::arg("symbol"))
        .def("is_non_terminal", &ContextFreeGrammar::isNonTerminal, py::arg("symbol"))
        .def("left_factoring", &ContextFreeGrammar::leftFactoring, py::arg("expand") = false)
        .def("left_recursion_elimination", &ContextFreeGrammar::leftRecursionElimination)
        .def("compute_first_and_follow_set", &ContextFreeGrammar::computeFirstAndFollowSet)
        .def("compute_lr0_automaton", [](ContextFreeGrammar& self) {
            return AutomatonWrapper(self.computeLR0Automaton());
        })
        .def("compute_lr0_action_goto_table", [](const ContextFreeGrammar& self, AutomatonWrapper& wrapper) {
            return self.computeLR0ActionGotoTable(wrapper.automaton);
        }, py::arg("automaton"))
        .def("compute_slr1_automaton", [](ContextFreeGrammar& self) {
            return AutomatonWrapper(self.computeSLR1Automaton());
        })
        .def("compute_slr1_action_goto_table", [](const ContextFreeGrammar& self, AutomatonWrapper& wrapper) {
            return self.computeSLR1ActionGotoTable(wrapper.automaton);
        }, py::arg("automaton"))
        .def("compute_lr1_automaton", [](ContextFreeGrammar& self) {
            return AutomatonWrapper(self.computeLR1Automaton());
        })
        .def("compute_lr1_action_goto_table", [](const ContextFreeGrammar& self, AutomatonWrapper& wrapper) {
            return self.computeLR1ActionGotoTable(wrapper.automaton);
        }, py::arg("automaton"))
        .def("compute_lalr1_automaton", [](ContextFreeGrammar& self) {
            return AutomatonWrapper(self.computeLALR1Automaton());
        })
        .def("compute_lalr1_action_goto_table", [](const ContextFreeGrammar& self, AutomatonWrapper& wrapper) {
            return self.computeLALR1ActionGotoTable(wrapper.automaton);
        }, py::arg("automaton"))
        .def("compute_ll1_table", &ContextFreeGrammar::computeLL1Table)
        .def("is_chomsky_normal_form", &ContextFreeGrammar::isChomskyNormalForm)
        .def("to_chomsky_normal_form", &ContextFreeGrammar::toChomskyNormalForm)
        .def("cyk_parse", &ContextFreeGrammar::cykParse, py::arg("s"))
        .def("__str__", &ContextFreeGrammar::toString)
    ;

    py::class_<NFAState, shared_ptr<NFAState>>(m, "NFAState")
        .def(py::init<>())
        .def_readonly("id", &NFAState::id)
        .def_readonly("type", &NFAState::type)
    ;

    py::class_<DFAState, shared_ptr<DFAState>>(m, "DFAState")
        .def(py::init<>())
        .def_readonly("id", &DFAState::id)
        .def_readonly("key", &DFAState::key)
        .def_readonly("type", &DFAState::type)
    ;

    py::class_<NFAGraph>(m, "NFAGraph")
        .def(py::init<>())
        .def("size", &NFAGraph::size)
        .def("to_svg", &NFAGraph::toSVG, py::arg("dark_mode") = false)
        .def("state_at", &NFAGraph::stateAt, py::arg("index"))
        .def("num_edges", &NFAGraph::numEdges)
        .def("edge_from", &NFAGraph::edgeFrom, py::arg("index"))
        .def("edge_to", &NFAGraph::edgeTo, py::arg("index"))
        .def("edge_label", &NFAGraph::edgeLabel, py::arg("index"))
    ;

    py::class_<DFAGraph>(m, "DFAGraph")
        .def(py::init<>())
        .def("size", &DFAGraph::size)
        .def("to_svg", &DFAGraph::toSVG, py::arg("dark_mode") = false)
        .def("state_id_at", &DFAGraph::stateIdAt, py::arg("index"))
        .def("state_key_at", &DFAGraph::stateKeyAt, py::arg("index"))
        .def("state_type_at", &DFAGraph::stateTypeAt, py::arg("index"))
        .def("num_edges", &DFAGraph::numEdges)
        .def("edge_from", &DFAGraph::edgeFrom, py::arg("index"))
        .def("edge_to", &DFAGraph::edgeTo, py::arg("index"))
        .def("edge_label", &DFAGraph::edgeLabel, py::arg("index"))
    ;

    py::class_<RegularExpression>(m, "RegularExpression")
        .def(py::init<>())
        .def(py::init<const string&>(), py::arg("pattern"))
        .def_property_readonly_static("EPSILON", [](py::object) { return RegularExpression::EPSILON; })
        .def("parse", &RegularExpression::parse, py::arg("pattern"))
        .def("error_message", &RegularExpression::errorMessage)
        .def("to_nfa", &RegularExpression::toNFA)
        .def_static("to_dfa", &RegularExpression::toDFA, py::arg("nfa"))
        .def_static("to_min_dfa", &RegularExpression::toMinDFA, py::arg("dfa"))
        .def_static("to_nfa_graph", &RegularExpression::toNFAGraph, py::arg("nfa"))
        .def_static("to_dfa_graph", &RegularExpression::toDFAGraph, py::arg("dfa"))
    ;
}
