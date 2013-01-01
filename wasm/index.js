import ParsingToysWASMModule from './build/ParsingToysWASM.js';

const ParsingToysWASM = await ParsingToysWASMModule();
const ContextFreeGrammar = ParsingToysWASM.ContextFreeGrammar;
const FirstAndFollowSet = ParsingToysWASM.FirstAndFollowSet;

function _vectorStringToArray(vec) {
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

FirstAndFollowSet.prototype.getFirstSet = function (symbol) {
    return _vectorStringToArray(this._getFirstSet(symbol))
}

FirstAndFollowSet.prototype.getFollowSet = function (symbol) {
    return _vectorStringToArray(this._getFollowSet(symbol))
}

export {
    ContextFreeGrammar,
    FirstAndFollowSet,
}
