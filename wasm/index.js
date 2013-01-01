import ParsingToysWASMModule from './build/ParsingToysWASM.js'

const ParsingToysWASM = await ParsingToysWASMModule()
const ContextFreeGrammar = ParsingToysWASM.ContextFreeGrammar

export {
    ContextFreeGrammar
}
