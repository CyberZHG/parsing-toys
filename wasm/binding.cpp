#include "cfg.h"
#include "automaton.h"
#include "re.h"
#include <vector>
#include <string>
#include <memory>
#include <emscripten/bind.h>
using namespace emscripten;
using namespace std;

EMSCRIPTEN_BINDINGS(ParsingToysWASM) {
    register_vector<string>("VectorString");
    register_vector<size_t>("VectorSizeT");
    class_<ContextFreeGrammar>("ContextFreeGrammar")
        .constructor<>()
        .function("parse", &ContextFreeGrammar::parse)
        .function("errorMessage", &ContextFreeGrammar::errorMessage)
        .function("toString", &ContextFreeGrammar::toString)
        .function("_terminals", &ContextFreeGrammar::terminals)
        .function("_nonTerminals", &ContextFreeGrammar::nonTerminals)
        .function("_orderedNonTerminals", &ContextFreeGrammar::orderedNonTerminals)
        .function("leftFactoring", select_overload<void(bool)>(&ContextFreeGrammar::leftFactoring))
        .function("leftRecursionElimination", &ContextFreeGrammar::leftRecursionElimination)
        .function("computeFirstAndFollowSet", &ContextFreeGrammar::computeFirstAndFollowSet)
        .function("computeLR0Automaton", &ContextFreeGrammar::computeLR0Automaton)
        .function("computeLR0ActionGotoTable", &ContextFreeGrammar::computeLR0ActionGotoTable)
        .function("computeSLR1Automaton", &ContextFreeGrammar::computeSLR1Automaton)
        .function("computeSLR1ActionGotoTable", &ContextFreeGrammar::computeSLR1ActionGotoTable)
        .function("computeLR1Automaton", &ContextFreeGrammar::computeLR1Automaton)
        .function("computeLR1ActionGotoTable", &ContextFreeGrammar::computeLR1ActionGotoTable)
        .function("computeLALR1Automaton", &ContextFreeGrammar::computeLALR1Automaton)
        .function("computeLALR1ActionGotoTable", &ContextFreeGrammar::computeLALR1ActionGotoTable)
        .function("computeLL1Table", &ContextFreeGrammar::computeLL1Table)
        .function("isChomskyNormalForm", &ContextFreeGrammar::isChomskyNormalForm)
        .function("toChomskyNormalForm", &ContextFreeGrammar::toChomskyNormalForm)
        .function("cykParse", &ContextFreeGrammar::cykParse)
    ;
    class_<FirstAndFollowSet>("FirstAndFollowSet")
        .constructor<>()
        .function("size", &FirstAndFollowSet::size)
        .function("symbolAt", &FirstAndFollowSet::symbolAt)
        .function("getNullable", &FirstAndFollowSet::getNullable)
        .function("_getFirstSet", &FirstAndFollowSet::getFirstSet)
        .function("_getFollowSet", &FirstAndFollowSet::getFollowSet)
    ;
    class_<FiniteAutomaton>("FiniteAutomaton")
        .constructor<>()
        .function("size", &FiniteAutomaton::size)
        .function("toSVG", &FiniteAutomaton::toSVG, allow_raw_pointers())
    ;
    class_<LRParsingSteps>("LRParsingSteps")
        .constructor<>()
        .function("size", optional_override([](const LRParsingSteps& self) { return self.stack.size(); }))
        .function("_getStack", optional_override([](const LRParsingSteps& self, size_t i) { return self.stack[i]; }))
        .function("_getSymbols", optional_override([](const LRParsingSteps& self, size_t i) { return self.symbols[i]; }))
        .function("_getRemainingInputs", optional_override([](const LRParsingSteps& self, size_t i) { return self.remainingInputs[i]; }))
        .function("getAction", optional_override([](const LRParsingSteps& self, size_t i) { return self.actions[i]; }))
    ;
    class_<ActionGotoTable>("ActionGotoTable")
        .constructor<>()
        .function("size", optional_override([](const ActionGotoTable& self) { return self.actions.size(); }))
        .function("hasConflict", optional_override([](const ActionGotoTable& self) { return self.hasConflict(); }))
        .function("hasConflictAt", optional_override([](const ActionGotoTable& self, size_t index, const string& symbol) { return self.hasConflict(index, symbol); }))
        .function("getCell", optional_override([](const ActionGotoTable& self, size_t index, const string& symbol, const string& separator) { return self.toString(index, symbol, separator); }))
        .function("parse", &ActionGotoTable::parse)
        .function("getParseTree", optional_override([](const ActionGotoTable& self) { return self.parseTree; }))
    ;
    class_<ParseTreeNode>("ParseTreeNode")
        .smart_ptr<shared_ptr<ParseTreeNode>>("ParseTreeNode")
        .function("toSVG", &ParseTreeNode::toSVG, allow_raw_pointers())
    ;
    class_<LLParsingSteps>("LLParsingSteps")
        .constructor<>()
        .function("size", optional_override([](const LLParsingSteps& self) { return self.stack.size(); }))
        .function("_getStack", optional_override([](const LLParsingSteps& self, size_t i) { return self.stack[i]; }))
        .function("_getRemainingInputs", optional_override([](const LLParsingSteps& self, size_t i) { return self.remainingInputs[i]; }))
        .function("getAction", optional_override([](const LLParsingSteps& self, size_t i) { return self.actions[i]; }))
    ;
    class_<MTable>("MTable")
        .constructor<>()
        .function("numNonTerminals", &MTable::numNonTerminals)
        .function("numTerminals", &MTable::numTerminals)
        .function("getNonTerminal", &MTable::getNonTerminal)
        .function("getTerminal", &MTable::getTerminal)
        .function("hasConflict", optional_override([](const MTable& self) { return self.hasConflict(); }))
        .function("hasConflictAt", optional_override([](const MTable& self, const string& nt, const string& t) { return self.hasConflict(nt, t); }))
        .function("getCell", optional_override([](const MTable& self, const string& nt, const string& t, const string& sep) { return self.getCell(nt, t, sep); }))
        .function("parse", &MTable::parse)
        .function("getParseTree", optional_override([](const MTable& self) { return self.parseTree; }))
    ;
    class_<CYKTable>("CYKTable")
        .constructor<size_t>()
        .function("size", &CYKTable::size)
        .function("_getCell", &CYKTable::getCell)
        .function("getCellString", &CYKTable::getCellString)
        .function("isAccepted", optional_override([](const CYKTable& self) { return self.accepted; }))
        .function("getParseTree", optional_override([](const CYKTable& self) { return self.parseTree; }))
    ;
    class_<RegularExpression>("RegularExpression")
        .constructor<>()
        .function("parse", &RegularExpression::parse)
        .function("errorMessage", &RegularExpression::errorMessage)
        .function("toNFA", &RegularExpression::toNFA)
        .class_function("toDFA", &RegularExpression::toDFA)
        .class_function("toMinDFA", &RegularExpression::toMinDFA)
        .class_function("toNFAGraph", &RegularExpression::toNFAGraph)
        .class_function("toDFAGraph", &RegularExpression::toDFAGraph)
    ;
    class_<NFAState>("NFAState")
        .smart_ptr<shared_ptr<NFAState>>("NFAState")
    ;
    class_<DFAState>("DFAState")
        .smart_ptr<shared_ptr<DFAState>>("DFAState")
    ;
    class_<NFAGraph>("NFAGraph")
        .function("size", &NFAGraph::size)
        .function("toSVG", &NFAGraph::toSVG, allow_raw_pointers())
        .function("stateAt", &NFAGraph::stateAt)
        .function("numEdges", &NFAGraph::numEdges)
        .function("edgeFrom", &NFAGraph::edgeFrom)
        .function("edgeTo", &NFAGraph::edgeTo)
        .function("edgeLabel", &NFAGraph::edgeLabel)
    ;
    class_<DFAGraph>("DFAGraph")
        .function("size", &DFAGraph::size)
        .function("toSVG", &DFAGraph::toSVG, allow_raw_pointers())
        .function("stateIdAt", &DFAGraph::stateIdAt)
        .function("stateKeyAt", &DFAGraph::stateKeyAt)
        .function("stateTypeAt", &DFAGraph::stateTypeAt)
        .function("numEdges", &DFAGraph::numEdges)
        .function("edgeFrom", &DFAGraph::edgeFrom)
        .function("edgeTo", &DFAGraph::edgeTo)
        .function("edgeLabel", &DFAGraph::edgeLabel)
    ;
}
