import { setupExamplesMenu } from "../choose_examples.ts";
import { setupCFGEditor, getCFGEditorValue, setCFGEditorValue } from '../cfg_editor.ts';
import { ContextFreeGrammar } from '../wasm/index.js'
import { setupLocationHash, getLocationHashValue, setLocationHashValue } from '../location_hash.ts'
import { setupSVGPanZoom, setupSVGDownloadButtons } from '../display_svg.ts'
import { renderParsingSteps } from '../lr_steps.ts'

interface LR0HashData {
    grammar: string
    input?: string
}

function getLR0HashData(): LR0HashData {
    try {
        const value = getLocationHashValue()
        if (!value) return { grammar: '' }
        const data = JSON.parse(value)
        return { grammar: data.grammar || '', input: data.input || '' }
    } catch {
        // Fallback for old format (plain string)
        return { grammar: getLocationHashValue(), input: '' }
    }
}

function setLR0HashData(data: LR0HashData): void {
    setLocationHashValue(JSON.stringify(data))
}

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
const actionGotoTableContainer = document.querySelector<HTMLElement>('#action-goto-table-container')!
const actionGotoTable = document.querySelector<HTMLTableElement>('#action-goto-table')!
const inputString = document.querySelector<HTMLInputElement>('#input-string')!
const lrStepsContainer = document.querySelector<HTMLElement>('#lr-steps-container')!
const parseTreeContainer = document.querySelector<HTMLElement>('#parse-tree-container')!
const parseTreeSVG = document.querySelector<SVGSVGElement>("#parse-tree-svg")!

const EOF_SYMBOL = '¥'

type ActionGotoTableType = ReturnType<ContextFreeGrammar['computeLR0ActionGotoTable']>

function renderActionGotoTable(cfg: ContextFreeGrammar, table: ActionGotoTableType): void {
    const terminals = cfg.terminals().sort()
    terminals.push(EOF_SYMBOL)
    const nonTerminals = cfg.orderedNonTerminals()

    // Build header rows
    let headerHtml = '<thead>'
    // First row: State (rowspan=2), ACTION (colspan=terminals), GOTO (colspan=non-terminals)
    headerHtml += '<tr class="bg-gray-100">'
    headerHtml += '<th class="border border-gray-300 px-2 py-1" rowspan="2">State</th>'
    headerHtml += `<th class="border border-gray-300 px-2 py-1" colspan="${terminals.length}">ACTION</th>`
    headerHtml += `<th class="border border-gray-300 px-2 py-1" colspan="${nonTerminals.length}">GOTO</th>`
    headerHtml += '</tr>'
    // Second row: individual terminal and non-terminal symbols
    headerHtml += '<tr class="bg-gray-100">'
    for (const t of terminals) {
        headerHtml += `<th class="border border-gray-300 px-2 py-1">${t}</th>`
    }
    for (const nt of nonTerminals) {
        headerHtml += `<th class="border border-gray-300 px-2 py-1">${nt}</th>`
    }
    headerHtml += '</tr></thead>'

    // Build body rows
    let bodyHtml = '<tbody>'
    const numStates = table.size()
    for (let i = 0; i < numStates; i++) {
        bodyHtml += `<tr><td class="border border-gray-300 px-2 py-1 text-center">${i}</td>`
        for (const symbol of [...terminals, ...nonTerminals]) {
            const cellContent = table.getCell(i, symbol, '<br>')
            const hasConflict = table.hasConflictAt(i, symbol)
            const isAccept = cellContent === 'accept'

            let cellClass = 'border border-gray-300 px-2 py-1 text-center'
            if (hasConflict) {
                cellClass += ' bg-red-100 text-red-800'
            } else if (isAccept) {
                cellClass += ' bg-green-100 text-green-800'
            }

            bodyHtml += `<td class="${cellClass}">${cellContent}</td>`
        }
        bodyHtml += '</tr>'
    }
    bodyHtml += '</tbody>'

    actionGotoTable.innerHTML = headerHtml + bodyHtml
    actionGotoTableContainer.hidden = false
}

lr0Button.addEventListener('click', () => {
    const errorMessage = document.querySelector<HTMLTextAreaElement>('#cfg-lr0-error-message')!
    const cfg = new ContextFreeGrammar()
    const code = getCFGEditorValue()
    if (cfg.parse(code)) {
        if (cfg.terminals().includes("¥")) {
            errorMessage.textContent = "The ¥ symbol is used as the end-of-input marker; please do not use it in the grammar."
            errorMessage.parentElement!.hidden = false
            actionGotoTableContainer.hidden = true
            lrStepsContainer.hidden = true
            parseTreeContainer.hidden = true
        } else {
            if (cfg.nonTerminals().length > 0) {
                const automaton = cfg.computeLR0Automaton();
                const parser = new DOMParser()
                const svgDoc = parser.parseFromString(automaton.toSVG(), 'image/svg+xml')
                automatonSVG.innerHTML = svgDoc.documentElement.innerHTML
                automatonSVG.setAttribute("viewBox", svgDoc.documentElement.getAttribute("viewBox")!)

                const table = cfg.computeLR0ActionGotoTable(automaton)
                renderActionGotoTable(cfg, table)

                const input = inputString.value.trim()
                if (input) {
                    const steps = table.parse(input)
                    renderParsingSteps(steps)
                    lrStepsContainer.hidden = false

                    const parseTree = table.getParseTree()
                    if (parseTree) {
                        const treeSvgDoc = parser.parseFromString(parseTree.toSVG(), 'image/svg+xml')
                        parseTree.delete()
                        parseTreeSVG.innerHTML = treeSvgDoc.documentElement.innerHTML
                        parseTreeSVG.setAttribute("viewBox", treeSvgDoc.documentElement.getAttribute("viewBox")!)
                        parseTreeContainer.hidden = false
                    } else {
                        parseTreeContainer.hidden = true
                    }
                } else {
                    lrStepsContainer.hidden = true
                    parseTreeContainer.hidden = true
                }
            }
            errorMessage.parentElement!.hidden = true
        }
    } else {
        errorMessage.textContent = cfg.errorMessage()
        errorMessage.parentElement!.hidden = false
        actionGotoTableContainer.hidden = true
        lrStepsContainer.hidden = true
        parseTreeContainer.hidden = true
    }
    setLR0HashData({ grammar: code, input: inputString.value.trim() })
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
setupSVGDownloadButtons(automatonSVG)
setupSVGPanZoom(parseTreeSVG)
setupSVGDownloadButtons(parseTreeSVG, "#parse-tree-download-svg", "#parse-tree-download-png", "parse-tree")

const hashData = getLR0HashData()
setCFGEditorValue(hashData.grammar)
inputString.value = hashData.input || ''
lr0Button.click()
