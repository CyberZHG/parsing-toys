import { setupExamplesMenu } from "../choose_examples.ts";
import { setupCFGEditor, getCFGEditorValue, setCFGEditorValue } from '../cfg_editor.ts';
import { setupCFGResult, updateCFGResult } from '../cfg_result.ts'
import { ContextFreeGrammar } from '../wasm/index.js'
import { setupLocationHash, getLocationHashValue, setLocationHashValue } from '../location_hash.ts'

const EXAMPLE_OPTIONS: Record<string, string> = {
    "example1": "Example 1: S -> S S + | S S * | a",
    "example2": "Example 2: S -> ( L ) | a  L -> L , S | S",
    "example3": "Example 3: A -> C | c d  B -> C | c d  C -> A | B | c d",
    "example4": "Example 4: bexpr -> bexpr or bterm | bterm  bterm -> bterm and bfactor | bfactor  bfactor -> not bfactor | ( bexpr ) | true | false",
}

const EXAMPLES: Record<string, string> = {
    "example1": "S -> S S + \n   | S S * \n   | a",
    "example2": "S -> ( L ) \n   | a L -> L , S \n   | S",
    "example3": "A -> C | c d\nB -> C | c d\nC -> A | B | c d",
    "example4": "  bexpr -> bexpr or bterm\n         | bterm\n  bterm -> bterm and bfactor\n         | bfactor\nbfactor -> not bfactor\n         | ( bexpr )\n         | true\n         | false",
}

const reformatButton = document.querySelector<HTMLElement>('#button-cfg-left-recursion')!
reformatButton.addEventListener('click', () => {
    const errorMessage = document.querySelector<HTMLTextAreaElement>('#cfg-left-recursion-error-message')!
    const cfg = new ContextFreeGrammar()
    const code = getCFGEditorValue()
    if (cfg.parse(code)) {
        if (cfg.leftRecursionElimination()) {
            errorMessage.parentElement!.hidden = true
        } else {
            errorMessage.textContent = cfg.errorMessage()
            errorMessage.parentElement!.hidden = false
        }
        updateCFGResult(cfg.toString(), cfg.terminals(), cfg.nonTerminals())
    } else {
        errorMessage.textContent = cfg.errorMessage()
        errorMessage.parentElement!.hidden = false
    }
    setLocationHashValue(code)
})

function onExamplesChange(key: string) : void {
    setCFGEditorValue(EXAMPLES[key])
    reformatButton.click()
}

setupExamplesMenu(EXAMPLE_OPTIONS, onExamplesChange)
setupCFGEditor()
setupCFGResult()
setupLocationHash()

setCFGEditorValue(getLocationHashValue())
reformatButton.click()
