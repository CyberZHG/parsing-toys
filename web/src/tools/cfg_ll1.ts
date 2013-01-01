import { setupExamplesMenu } from "../choose_examples.ts";
import { setupCFGEditor, getCFGEditorValue, setCFGEditorValue } from '../cfg_editor.ts';
import { setupCFGResult, updateCFGResult } from '../cfg_result.ts'
import { ContextFreeGrammar, MTable, LLParsingSteps } from '../wasm/index.js'
import { setupLocationHash, getLocationHashValue, setLocationHashValue } from '../location_hash.ts'
import { setupSVGPanZoom, setupSVGDownloadButtons } from '../display_svg.ts'

interface LL1HashData {
    grammar: string
    input?: string
}

function getLL1HashData(): LL1HashData {
    try {
        const value = getLocationHashValue()
        if (!value) return { grammar: '' }
        const data = JSON.parse(value)
        return { grammar: data.grammar || '', input: data.input || '' }
    } catch {
        return { grammar: getLocationHashValue(), input: '' }
    }
}

function setLL1HashData(data: LL1HashData): void {
    setLocationHashValue(JSON.stringify(data))
}

const EXAMPLE_OPTIONS: Record<string, string> = {
    "example1": "Example 1: E -> T E'  E' -> + T E' | ε  T -> F T'  T' -> * F T' | ε  F -> ( E ) | id",
    "example2": "Example 2: S -> i E t S S' | a  S' -> e S | ε  E -> b",
}

const EXAMPLES: Record<string, string> = {
    "example1": `E -> T E'
E' -> + T E' | ε
T -> F T'
T' -> * F T' | ε
F -> ( E ) | id`,
    "example2": `S -> i E t S S' | a
S' -> e S | ε
E -> b`,
}

const EXAMPLE_INPUTS: Record<string, string> = {
    "example1": "id + id * id",
    "example2": "i b t a e a",
}

const EOF_SYMBOL = '¥'

const ll1Button = document.querySelector<HTMLElement>('#button-cfg-ll1')!
const mtableContainer = document.querySelector<HTMLElement>('#mtable-container')!
const mtableElement = document.querySelector<HTMLTableElement>('#mtable')!
const inputString = document.querySelector<HTMLInputElement>('#input-string')!
const llStepsContainer = document.querySelector<HTMLElement>('#ll-steps-container')!
const llStepsTbody = document.querySelector<HTMLElement>('#ll-steps-tbody')!
const parseTreeContainer = document.querySelector<HTMLElement>('#parse-tree-container')!
const parseTreeSVG = document.querySelector<SVGSVGElement>("#parse-tree-svg")!

function renderMTable(table: MTable): void {
    const numNT = table.numNonTerminals()
    const numT = table.numTerminals()

    let headerHtml = '<thead><tr class="bg-gray-100">'
    headerHtml += '<th class="border border-gray-300 px-2 py-1"></th>'
    for (let j = 0; j < numT; j++) {
        headerHtml += `<th class="border border-gray-300 px-2 py-1">${table.getTerminal(j)}</th>`
    }
    headerHtml += '</tr></thead>'

    let bodyHtml = '<tbody>'
    for (let i = 0; i < numNT; i++) {
        const nt = table.getNonTerminal(i)
        bodyHtml += `<tr><td class="border border-gray-300 px-2 py-1 font-semibold bg-gray-50">${nt}</td>`
        for (let j = 0; j < numT; j++) {
            const t = table.getTerminal(j)
            const cellContent = table.getCell(nt, t, '<br>')
            const hasConflict = table.hasConflictAt(nt, t)
            let cellClass = 'border border-gray-300 px-2 py-1 text-center'
            if (hasConflict) {
                cellClass += ' bg-red-100 text-red-800'
            }
            bodyHtml += `<td class="${cellClass}">${cellContent}</td>`
        }
        bodyHtml += '</tr>'
    }
    bodyHtml += '</tbody>'

    mtableElement.innerHTML = headerHtml + bodyHtml
    mtableContainer.hidden = false
}

function renderLLParsingSteps(steps: LLParsingSteps): void {
    llStepsTbody.innerHTML = ''
    for (let i = 0; i < steps.size(); i++) {
        const row = document.createElement('tr')
        row.className = i % 2 === 0 ? 'bg-white' : 'bg-gray-50'

        const stepCell = document.createElement('td')
        stepCell.className = 'border border-gray-300 px-3 py-2 text-center'
        stepCell.textContent = String(i + 1)

        const stackCell = document.createElement('td')
        stackCell.className = 'border border-gray-300 px-3 py-2'
        stackCell.textContent = steps.getStack(i).join(' ')

        const inputCell = document.createElement('td')
        inputCell.className = 'border border-gray-300 px-3 py-2 text-right'
        inputCell.textContent = steps.getRemainingInputs(i).join(' ')

        const actionCell = document.createElement('td')
        actionCell.className = 'border border-gray-300 px-3 py-2'
        const action = steps.getAction(i)
        actionCell.textContent = action
        if (action.startsWith('conflict') || action.startsWith('error')) {
            actionCell.classList.add('text-red-600')
        } else if (action === 'accept') {
            actionCell.classList.add('text-green-600', 'font-semibold')
        }

        row.appendChild(stepCell)
        row.appendChild(stackCell)
        row.appendChild(inputCell)
        row.appendChild(actionCell)
        llStepsTbody.appendChild(row)
    }
}

ll1Button.addEventListener('click', () => {
    const errorMessage = document.querySelector<HTMLTextAreaElement>('#cfg-ll1-error-message')!
    const cfg = new ContextFreeGrammar()
    const code = getCFGEditorValue()
    if (cfg.parse(code)) {
        if (cfg.terminals().includes(EOF_SYMBOL)) {
            errorMessage.textContent = "The " + EOF_SYMBOL + " symbol is used as the end-of-input marker; please do not use it in the grammar."
            errorMessage.parentElement!.hidden = false
            mtableContainer.hidden = true
            llStepsContainer.hidden = true
            parseTreeContainer.hidden = true
        } else {
            cfg.leftRecursionElimination()
            cfg.leftFactoring()
            updateCFGResult(cfg.toString(), cfg.terminals(), cfg.nonTerminals())

            if (cfg.nonTerminals().length > 0) {
                const table = cfg.computeLL1Table()
                renderMTable(table)

                const input = inputString.value.trim()
                if (input) {
                    const steps = table.parse(input)
                    renderLLParsingSteps(steps)
                    llStepsContainer.hidden = false

                    const parseTree = steps.getParseTree()
                    if (parseTree) {
                        const parser = new DOMParser()
                        const treeSvgDoc = parser.parseFromString(parseTree.toSVG(), 'image/svg+xml')
                        parseTree.delete()
                        parseTreeSVG.innerHTML = treeSvgDoc.documentElement.innerHTML
                        parseTreeSVG.setAttribute("viewBox", treeSvgDoc.documentElement.getAttribute("viewBox")!)
                        parseTreeContainer.hidden = false
                    } else {
                        parseTreeContainer.hidden = true
                    }
                } else {
                    llStepsContainer.hidden = true
                    parseTreeContainer.hidden = true
                }
            }
            errorMessage.parentElement!.hidden = true
        }
    } else {
        errorMessage.textContent = cfg.errorMessage()
        errorMessage.parentElement!.hidden = false
        mtableContainer.hidden = true
        llStepsContainer.hidden = true
        parseTreeContainer.hidden = true
    }
    setLL1HashData({ grammar: code, input: inputString.value.trim() })
})

function onExamplesChange(key: string) : void {
    setCFGEditorValue(EXAMPLES[key])
    inputString.value = EXAMPLE_INPUTS[key]
    ll1Button.click()
}

document.querySelector<HTMLElement>('#cfg-editor-intro')!.innerHTML = `
  Context free grammar input:
  <div class="text-gray-500 font-light text-sm">
    <ul>
      <li>The first symbol encountered in the grammar definition is treated as the start symbol</li>
      <li>Left recursion elimination and left factoring are applied automatically</li>
      <li>The <code>${EOF_SYMBOL}</code> symbol is used as the end-of-input marker; please do not use it in the grammar.</li>
    </ul>
  </div>
`

setupExamplesMenu(EXAMPLE_OPTIONS, onExamplesChange)
setupCFGEditor()
setupCFGResult()
setupLocationHash()
setupSVGPanZoom(parseTreeSVG)
setupSVGDownloadButtons(parseTreeSVG, "#parse-tree-download-svg", "#parse-tree-download-png", "parse-tree")

const hashData = getLL1HashData()
setCFGEditorValue(hashData.grammar)
inputString.value = hashData.input || ''
ll1Button.click()
