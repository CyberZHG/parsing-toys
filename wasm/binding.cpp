#include "cfg.h"
#include "automaton.h"
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
        .function("leftFactoring", &ContextFreeGrammar::leftFactoring)
        .function("leftRecursionElimination", &ContextFreeGrammar::leftRecursionElimination)
        .function("computeFirstAndFollowSet", &ContextFreeGrammar::computeFirstAndFollowSet)
        .function("computeLR0Automaton", &ContextFreeGrammar::computeLR0Automaton)
        .function("computeLR0ActionGotoTable", &ContextFreeGrammar::computeLR0ActionGotoTable)
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
        .function("toSVG", &FiniteAutomaton::toSVG)
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
        .function("hasConflict", optional_override([](const ActionGotoTable& self) { return self.hasConflict(); }))
        .function("parse", &ActionGotoTable::parse)
    ;
}
