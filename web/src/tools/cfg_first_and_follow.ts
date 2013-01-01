import { setupExamplesMenu } from "../choose_examples.ts";
import { setupCFGEditor, getCFGEditorValue, setCFGEditorValue } from '../cfg_editor.ts';
import { ContextFreeGrammar } from '../wasm/index.js'
import { setupLocationHash, getLocationHashValue, setLocationHashValue } from '../location_hash.ts'

const EXAMPLE_OPTIONS: Record<string, string> = {
    "example1": "Example 1: E -> T E'  T -> F T'  E' -> + T E' | ε  T' -> * F T' | ε  F -> ( E ) | id",
    "example2": "Example 2: S -> A B C  A -> a | ε  B -> b | ε  C -> c",
}

const EXAMPLES: Record<string, string> = {
    "example1": ` E -> T E'
 T -> F T'
E' -> + T E'
    | ε
T' -> * F T'
    | ε
 F -> ( E )
    | id`,
    "example2": `S -> A B C
A -> a
   | ε
B -> b
   | ε
C -> c`,
}

const firstAndFollowButton = document.querySelector<HTMLElement>('#button-cfg-first-and-follow')!
firstAndFollowButton.addEventListener('click', () => {
    const tableBody = document.querySelector<HTMLElement>('#cfg-first-and-follow-result')!
    const errorMessage = document.querySelector<HTMLTextAreaElement>('#cfg-first-and-follow-error-message')!
    const cfg = new ContextFreeGrammar()
    const code = getCFGEditorValue()
    if (cfg.parse(code)) {
        const result = cfg.computeFirstAndFollowSet()
        let resultHTML = ""
        for (let i = 0; i < result.size(); ++i) {
            const symbol = result.symbolAt(i)
            const firstSet = result.getFirstSet(symbol).join(" ")
                .replace(/(^|\s)ε(?=\s|$)/g, '$1<span class="text-green-700">ε</span>')
            const followSet = result.getFollowSet(symbol).join(" ")
                .replace(/(^|\s)\$(?=\s|$)/g, '$1<span class="text-green-700">$</span>')
            resultHTML += '<tr class="odd:bg-white even:bg-gray-50 border-none">'
            resultHTML += `  <td class="px-4 py-2 text-center font-mono">${symbol}</td>`
            if (result.getNullable(symbol)) {
                resultHTML += `  <td class="px-4 py-2 text-center">✓</td>`
            } else {
                resultHTML += `  <td class="px-4 py-2"></td>`
            }
            resultHTML += `  <td class="px-4 py-2 text-center font-mono">${firstSet}</td>`
            resultHTML += `  <td class="px-4 py-2 text-center font-mono">${followSet}</td>`
            resultHTML += '</tr>'
        }
        tableBody.innerHTML = resultHTML
        errorMessage.parentElement!.hidden = true
    } else {
        errorMessage.textContent = cfg.errorMessage()
        errorMessage.parentElement!.hidden = false
    }
    setLocationHashValue(code)
})

function onExamplesChange(key: string) : void {
    setCFGEditorValue(EXAMPLES[key])
    firstAndFollowButton.click()
}

document.querySelector<HTMLElement>('#cfg-editor-intro')!.innerHTML = `
  Context free grammar input:
  <div class="text-gray-500 font-light text-sm">(The first symbol encountered in the grammar definition is treated as the start symbol)</div>
`


setupExamplesMenu(EXAMPLE_OPTIONS, onExamplesChange)
setupCFGEditor()
setupLocationHash()

setCFGEditorValue(getLocationHashValue())
firstAndFollowButton.click()
