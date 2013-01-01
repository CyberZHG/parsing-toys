import ParsingToysWASMModule from './build/ParsingToysWASM.js'

const ParsingToysWASM = await ParsingToysWASMModule()
const ContextFreeGrammar = ParsingToysWASM.ContextFreeGrammar
ContextFreeGrammar.prototype.terminals = function () {
    const terminals = this._terminals();
    let terminalList = [];
    for (let i = 0; i < terminals.size(); ++i) {
        terminalList.push(terminals.get(i));
    }
    return terminalList;
}
ContextFreeGrammar.prototype.nonTerminals = function () {
    const nonTerminals = this._nonTerminals();
    let nonTerminalList = [];
    for (let i = 0; i < nonTerminals.size(); ++i) {
        nonTerminalList.push(nonTerminals.get(i));
    }
    return nonTerminalList;
}

export {
    ContextFreeGrammar
}
