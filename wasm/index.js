import ParsingToysWASMModule from './build/ParsingToysWASM.js';

const ParsingToysWASM = await ParsingToysWASMModule();
const ContextFreeGrammar = ParsingToysWASM.ContextFreeGrammar;
const FirstAndFollowSet = ParsingToysWASM.FirstAndFollowSet;
const LRParsingSteps = ParsingToysWASM.LRParsingSteps;
const ActionGotoTable = ParsingToysWASM.ActionGotoTable;
const ParseTreeNode = ParsingToysWASM.ParseTreeNode;

function _vectorStringToArray(vec) {
    let arr = [];
    for (let i = 0; i < vec.size(); ++i) {
        arr.push(vec.get(i));
    }
    return arr;
}

function _vectorSizeTToArray(vec) {
    let arr = [];
    for (let i = 0; i < vec.size(); ++i) {
        arr.push(vec.get(i));
    }
    return arr;
}

ContextFreeGrammar.prototype.terminals = function () {
    return _vectorStringToArray(this._terminals());
}
ContextFreeGrammar.prototype.nonTerminals = function () {
    return _vectorStringToArray(this._nonTerminals());
}
ContextFreeGrammar.prototype.orderedNonTerminals = function () {
    return _vectorStringToArray(this._orderedNonTerminals());
}

FirstAndFollowSet.prototype.getFirstSet = function (symbol) {
    return _vectorStringToArray(this._getFirstSet(symbol))
}

FirstAndFollowSet.prototype.getFollowSet = function (symbol) {
    return _vectorStringToArray(this._getFollowSet(symbol))
}

LRParsingSteps.prototype.getStack = function (index) {
    return _vectorSizeTToArray(this._getStack(index))
}

LRParsingSteps.prototype.getSymbols = function (index) {
    return _vectorStringToArray(this._getSymbols(index))
}

LRParsingSteps.prototype.getRemainingInputs = function (index) {
    return _vectorStringToArray(this._getRemainingInputs(index))
}

export {
    ContextFreeGrammar,
    FirstAndFollowSet,
    LRParsingSteps,
    ActionGotoTable,
    ParseTreeNode,
}
