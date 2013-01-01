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
}
