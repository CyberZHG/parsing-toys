import { setupExamplesMenu } from "../choose_examples.ts";
import { setupCFGEditor, getCFGEditorValue, setCFGEditorValue } from '../cfg_editor.ts';
import { setupCFGResult, updateCFGResult } from '../cfg_result.ts'
import { ContextFreeGrammar } from '../wasm/index.js'
import { setupLocationHash, getLocationHashValue, setLocationHashValue } from '../location_hash.ts'

const EXAMPLE_OPTIONS: Record<string, string> = {
    "example1": "Example 1: S -> A B  A -> a | ε  B -> b | ε",
    "example2": "Example 2: S -> a S b | a b",
    "example3": "Example 3: E -> E + T | T  T -> T * F | F  F -> ( E ) | id",
    "example4": "Example 4: S -> A  A -> B  B -> a",
}

const EXAMPLES: Record<string, string> = {
    "example1": "S -> A B\nA -> a | ε\nB -> b | ε",
    "example2": "S -> a S b | a b",
    "example3": "E -> E + T | T\nT -> T * F | F\nF -> ( E ) | id",
    "example4": "S -> A\nA -> B\nB -> a",
}

const cnfButton = document.querySelector<HTMLElement>('#button-cfg-cnf')!

cnfButton.addEventListener('click', () => {
    const errorMessage = document.querySelector<HTMLTextAreaElement>('#cfg-cnf-error-message')!
    const cfg = new ContextFreeGrammar()
    const code = getCFGEditorValue()
    if (cfg.parse(code)) {
        cfg.toChomskyNormalForm()
        updateCFGResult(cfg.toString(), cfg.terminals(), cfg.nonTerminals())
        errorMessage.parentElement!.hidden = true
    } else {
        errorMessage.textContent = cfg.errorMessage()
        errorMessage.parentElement!.hidden = false
    }
    setLocationHashValue(code)
})

function onExamplesChange(key: string): void {
    setCFGEditorValue(EXAMPLES[key])
    cnfButton.click()
}

setupExamplesMenu(EXAMPLE_OPTIONS, onExamplesChange)
setupCFGEditor()
setupCFGResult()
setupLocationHash()

setCFGEditorValue(getLocationHashValue())
cnfButton.click()
