import { setupExamplesMenu } from "../choose_examples.ts";
import { setupCFGEditor, getCFGEditorValue, setCFGEditorValue } from '../cfg_editor.ts';
import { ContextFreeGrammar } from '../wasm/index.js'
import { setupLocationHash, getLocationHashValue, setLocationHashValue } from '../location_hash.ts'
import { setupSVGPanZoom } from '../finite_automaton.ts'

const EXAMPLE_OPTIONS: Record<string, string> = {
    "example1": "Example 1: S -> S S + | S S * | a",
    "example2": "Example 2: E -> E + T | T  T -> T * F | F  F -> ( E ) | id",
}

const EXAMPLES: Record<string, string> = {
    "example1": "S -> S S + \n   | S S * \n   | a",
    "example2": `E -> E + T | T
T -> T * F | F
F -> ( E ) | id`,
}

const lr0Button = document.querySelector<HTMLElement>('#button-cfg-lr0')!
const automatonSVG = document.querySelector<SVGSVGElement>("#automaton-svg")!
lr0Button.addEventListener('click', () => {
    const errorMessage = document.querySelector<HTMLTextAreaElement>('#cfg-lr0-error-message')!
    const cfg = new ContextFreeGrammar()
    const code = getCFGEditorValue()
    if (cfg.parse(code)) {
        if (cfg.terminals().includes("¥")) {
            errorMessage.textContent = "The ¥ symbol is used as the end-of-input marker; please do not use it in the grammar."
            errorMessage.parentElement!.hidden = false
        } else {
            if (cfg.nonTerminals().length > 0) {
                const automaton = cfg.computeLR0Automaton();
                const parser = new DOMParser()
                const svgDoc = parser.parseFromString(automaton.toSVG(), 'image/svg+xml')
                automatonSVG.innerHTML = svgDoc.documentElement.innerHTML
                automatonSVG.setAttribute("viewBox", svgDoc.documentElement.getAttribute("viewBox")!)
            }
            errorMessage.parentElement!.hidden = true
        }
    } else {
        errorMessage.textContent = cfg.errorMessage()
        errorMessage.parentElement!.hidden = false
    }
    setLocationHashValue(code)
})

function onExamplesChange(key: string) : void {
    setCFGEditorValue(EXAMPLES[key])
    lr0Button.click()
}

document.querySelector<HTMLElement>('#cfg-editor-intro')!.innerHTML = `
  Context free grammar input:
  <div class="text-gray-500 font-light text-sm">
    <ul>
      <li>The first symbol encountered in the grammar definition is treated as the start symbol</li>
      <li>The <code>¥</code> symbol is used as the end-of-input marker; please do not use it in the grammar.</li>
    </ul>
  </div>
`

setupExamplesMenu(EXAMPLE_OPTIONS, onExamplesChange)
setupCFGEditor()
setupLocationHash()
setupSVGPanZoom(automatonSVG)

setCFGEditorValue(getLocationHashValue())
lr0Button.click()
