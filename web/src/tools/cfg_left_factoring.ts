import { setupExamplesMenu } from "../choose_examples.ts";
import { setupCFGEditor, getCFGEditorValue, setCFGEditorValue } from '../cfg_editor.ts';
import { setupCFGResult, updateCFGResult } from '../cfg_result.ts'
import { ContextFreeGrammar } from '../wasm/index.js'
import { setupLocationHash, getLocationHashValue, setLocationHashValue } from '../location_hash.ts'

const EXAMPLE_OPTIONS: Record<string, string> = {
    "example1": "Example 1: S -> i E t S | i E t S e S | a  E -> b",
    "example2": "Example 2: S -> S S + | S S * | a",
    "example3": "Example 3: S -> S + S | S S | ( S ) | S * | a",
    "example4": "Example 4: A -> id | B | a  B -> C  C -> D  D -> id b",
}

const EXAMPLES: Record<string, string> = {
    "example1": "S -> i E t S \n   | i E t S e S \n   | a\nE -> b",
    "example2": "S -> S S + \n   | S S * \n   | a",
    "example3": "S -> S + S \n   | S S \n   | ( S ) \n   | S * \n   | a",
    "example4": "A -> id | B | a\nB -> C\nC -> D\nD -> id b",
}

const reformatButton = document.querySelector<HTMLElement>('#button-cfg-left-factoring')!
reformatButton.addEventListener('click', () => {
    const errorMessage = document.querySelector<HTMLTextAreaElement>('#cfg-left-factoring-error-message')!
    const cfg = new ContextFreeGrammar()
    const code = getCFGEditorValue()
    if (cfg.parse(code)) {
        cfg.leftFactoring()
        updateCFGResult(cfg.toString(), cfg.terminals(), cfg.nonTerminals())
        errorMessage.parentElement!.hidden = true
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
