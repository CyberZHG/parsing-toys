import { setupExamplesMenu } from "../choose_examples.ts";
import { setupCFGEditor, getCFGEditorValue, setCFGEditorValue } from '../cfg_editor.ts';
import { setupCFGResult, updateCFGResult } from '../cfg_result.ts'
import { ContextFreeGrammar } from '../wasm/index.js'
import { setupLocationHash, getLocationHashValue, setLocationHashValue } from '../location_hash.ts'
import { setupSVGPanZoom, setupSVGDownloadButtons } from '../display_svg.ts'

interface CYKHashData {
    grammar: string
    input?: string
}

function getCYKHashData(): CYKHashData {
    try {
        const value = getLocationHashValue()
        if (!value) return { grammar: '' }
        const data = JSON.parse(value)
        return { grammar: data.grammar || '', input: data.input || '' }
    } catch {
        return { grammar: getLocationHashValue(), input: '' }
    }
}

function setCYKHashData(data: CYKHashData): void {
    setLocationHashValue(JSON.stringify(data))
}

const EXAMPLE_OPTIONS: Record<string, string> = {
    "example1": "Example 1: S -> A B  A -> a | epsilon  B -> b | epsilon",
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

const EXAMPLE_INPUTS: Record<string, string> = {
    "example1": "a b",
    "example2": "a a b b",
    "example3": "id + id * id",
    "example4": "a",
}

const cnfButton = document.querySelector<HTMLElement>('#button-cfg-cnf')!
const cykButton = document.querySelector<HTMLElement>('#button-cfg-cyk')!
const inputString = document.querySelector<HTMLInputElement>('#input-string')!
const cykTableContainer = document.querySelector<HTMLElement>('#cyk-table-container')!
const cykTable = document.querySelector<HTMLTableElement>('#cyk-table')!
const cykResultContainer = document.querySelector<HTMLElement>('#cyk-result-container')!
const cykResultText = document.querySelector<HTMLElement>('#cyk-result-text')!
const parseTreeContainer = document.querySelector<HTMLElement>('#parse-tree-container')!
const parseTreeSVG = document.querySelector<SVGSVGElement>("#parse-tree-svg")!

let currentCNFGrammar: ContextFreeGrammar | null = null

cnfButton.addEventListener('click', () => {
    const errorMessage = document.querySelector<HTMLTextAreaElement>('#cfg-cyk-error-message')!
    const cfg = new ContextFreeGrammar()
    const code = getCFGEditorValue()
    if (cfg.parse(code)) {
        cfg.toChomskyNormalForm()
        currentCNFGrammar = cfg
        updateCFGResult(cfg.toString(), cfg.terminals(), cfg.nonTerminals())
        errorMessage.parentElement!.hidden = true
    } else {
        errorMessage.textContent = cfg.errorMessage()
        errorMessage.parentElement!.hidden = false
        currentCNFGrammar = null
    }
    cykTableContainer.hidden = true
    cykResultContainer.hidden = true
    parseTreeContainer.hidden = true
    setCYKHashData({ grammar: code, input: inputString.value.trim() })
})

cykButton.addEventListener('click', () => {
    const errorMessage = document.querySelector<HTMLTextAreaElement>('#cfg-cyk-error-message')!

    if (!currentCNFGrammar) {
        cnfButton.click()
    }

    if (!currentCNFGrammar) {
        cykTableContainer.hidden = true
        cykResultContainer.hidden = true
        parseTreeContainer.hidden = true
        return
    }

    const input = inputString.value.trim()
    const tokens = input.split(/\s+/).filter(t => t.length > 0)
    const n = tokens.length

    if (n === 0 && input.length === 0) {
        const cykResult = currentCNFGrammar.cykParse("")
        const accepted = cykResult.isAccepted()

        cykTableContainer.hidden = true
        cykResultContainer.hidden = false

        if (accepted) {
            cykResultText.textContent = "Accepted (empty string)"
            cykResultText.className = "text-green-600"

            const parseTree = cykResult.getParseTree()
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
            cykResultText.textContent = "Rejected (empty string not accepted)"
            cykResultText.className = "text-red-600 dark:text-red-400"
            parseTreeContainer.hidden = true
        }

        errorMessage.parentElement!.hidden = true
        setCYKHashData({ grammar: getCFGEditorValue(), input })
        return
    }

    const cykResult = currentCNFGrammar.cykParse(input)
    const accepted = cykResult.isAccepted()

    let headerHtml = '<thead><tr class="bg-gray-100">'
    headerHtml += '<th class="border border-gray-300 dark:bg-gray-700 px-2 py-1"></th>'
    for (let j = 0; j < n; ++j) {
        headerHtml += `<th class="border border-gray-300 dark:bg-gray-700 px-2 py-1">${tokens[j]}</th>`
    }
    headerHtml += '</tr></thead>'

    let bodyHtml = '<tbody>'
    for (let len = n; len >= 1; --len) {
        bodyHtml += '<tr>'
        bodyHtml += `<td class="border border-gray-300 dark:bg-gray-700 px-2 py-1 text-center bg-gray-50 font-semibold">${len}</td>`
        for (let j = 0; j < n; ++j) {
            const i = j
            const endJ = i + len - 1
            if (endJ < n) {
                const cellContent = cykResult.getCellString(i, endJ, ", ")
                const isStartCell = i === 0 && endJ === n - 1
                let cellClass = 'border border-gray-300 dark:bg-gray-700 px-2 py-1 text-center'
                if (isStartCell && accepted) {
                    cellClass += ' bg-green-100 text-green-800 dark:text-green-600'
                } else if (isStartCell && !accepted) {
                    cellClass += ' bg-red-100 text-red-800 dark:text-red-600'
                }
                bodyHtml += `<td class="${cellClass}">${cellContent}</td>`
            } else {
                bodyHtml += '<td class="border border-gray-300 px-2 py-1 bg-gray-200 dark:bg-gray-800"></td>'
            }
        }
        bodyHtml += '</tr>'
    }
    bodyHtml += '</tbody>'

    cykTable.innerHTML = headerHtml + bodyHtml
    cykTableContainer.hidden = false

    cykResultContainer.hidden = false
    if (accepted) {
        cykResultText.textContent = "Accepted"
        cykResultText.className = "text-green-600"
    } else {
        cykResultText.textContent = "Rejected"
        cykResultText.className = "text-red-600"
    }

    if (accepted) {
        const parseTree = cykResult.getParseTree()
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
        parseTreeContainer.hidden = true
    }

    errorMessage.parentElement!.hidden = true
    setCYKHashData({ grammar: getCFGEditorValue(), input })
})

function onExamplesChange(key: string): void {
    setCFGEditorValue(EXAMPLES[key])
    inputString.value = EXAMPLE_INPUTS[key]
    cnfButton.click()
    cykButton.click()
}

setupExamplesMenu(EXAMPLE_OPTIONS, onExamplesChange)
setupCFGEditor()
setupCFGResult()
setupLocationHash()
setupSVGPanZoom(parseTreeSVG)
setupSVGDownloadButtons(parseTreeSVG, "#parse-tree-download-svg", "#parse-tree-download-png", "parse-tree")

const hashData = getCYKHashData()
setCFGEditorValue(hashData.grammar)
inputString.value = hashData.input || ''
if (hashData.grammar) {
    cnfButton.click()
    if (hashData.input) {
        cykButton.click()
    }
}
