import { RegularExpression, NFAGraph, DFAGraph } from '../wasm/index.js'
import { setupLocationHash, getLocationHashValue, setLocationHashValue } from '../location_hash.ts'
import { setupSVGPanZoom, setupSVGDownloadButtons } from '../display_svg.ts'

const regexInput = document.querySelector<HTMLInputElement>('#regex-input')!
const convertButton = document.querySelector<HTMLButtonElement>('#button-convert')!
const errorContainer = document.querySelector<HTMLElement>('#error-container')!
const errorMessage = document.querySelector<HTMLElement>('#error-message')!

const nfaContainer = document.querySelector<HTMLElement>('#nfa-container')!
const nfaTbody = document.querySelector<HTMLTableSectionElement>('#nfa-tbody')!
const nfaSvg = document.querySelector<SVGSVGElement>('#nfa-svg')!

const dfaContainer = document.querySelector<HTMLElement>('#dfa-container')!
const dfaTbody = document.querySelector<HTMLTableSectionElement>('#dfa-tbody')!
const dfaSvg = document.querySelector<SVGSVGElement>('#dfa-svg')!

const minDfaContainer = document.querySelector<HTMLElement>('#min-dfa-container')!
const minDfaTbody = document.querySelector<HTMLTableSectionElement>('#min-dfa-tbody')!
const minDfaSvg = document.querySelector<SVGSVGElement>('#min-dfa-svg')!

function renderNFATable(graph: NFAGraph): void {
    nfaTbody.innerHTML = ''
    const symbols = new Set<string>()
    for (let i = 0; i < graph.numEdges(); i++) {
        symbols.add(graph.edgeLabel(i))
    }
    const sortedSymbols = Array.from(symbols).sort()

    const headerRow = nfaTbody.parentElement!.querySelector('thead tr')!
    while (headerRow.children.length > 2) {
        headerRow.removeChild(headerRow.lastChild!)
    }
    for (const symbol of sortedSymbols) {
        const th = document.createElement('th')
        th.className = 'border border-gray-300 px-3 py-2 text-center font-semibold'
        th.textContent = symbol
        headerRow.appendChild(th)
    }

    const transitions: Map<number, Map<string, number[]>> = new Map()
    for (let i = 0; i < graph.numEdges(); i++) {
        const from = graph.edgeFrom(i)
        const to = graph.edgeTo(i)
        const label = graph.edgeLabel(i)
        if (!transitions.has(from)) transitions.set(from, new Map())
        const fromMap = transitions.get(from)!
        if (!fromMap.has(label)) fromMap.set(label, [])
        fromMap.get(label)!.push(to)
    }

    for (let i = 0; i < graph.size(); i++) {
        const stateInfo = graph.stateAt(i)
        const match = stateInfo.match(/^(\d+) \((\w+)\)$/)
        const stateId = match ? match[1] : String(i)
        const stateType = match ? match[2] : ''

        const tr = document.createElement('tr')
        const tdState = document.createElement('td')
        tdState.className = 'border border-gray-300 px-3 py-2 text-center'
        tdState.textContent = stateId
        tr.appendChild(tdState)

        const tdType = document.createElement('td')
        tdType.className = 'border border-gray-300 px-3 py-2 text-center'
        tdType.textContent = stateType
        tr.appendChild(tdType)

        const fromMap = transitions.get(i) || new Map()
        for (const symbol of sortedSymbols) {
            const td = document.createElement('td')
            td.className = 'border border-gray-300 px-3 py-2 text-center'
            const targets = fromMap.get(symbol) || []
            td.textContent = targets.length > 0 ? `{${targets.join(', ')}}` : ''
            tr.appendChild(td)
        }

        nfaTbody.appendChild(tr)
    }
}

function renderDFATable(graph: DFAGraph): void {
    dfaTbody.innerHTML = ''
    const symbols = new Set<string>()
    for (let i = 0; i < graph.numEdges(); i++) {
        symbols.add(graph.edgeLabel(i))
    }
    const sortedSymbols = Array.from(symbols).sort()

    const headerRow = dfaTbody.parentElement!.querySelector('thead tr')!
    while (headerRow.children.length > 3) {
        headerRow.removeChild(headerRow.lastChild!)
    }
    for (const symbol of sortedSymbols) {
        const th = document.createElement('th')
        th.className = 'border border-gray-300 px-3 py-2 text-center font-semibold'
        th.textContent = symbol
        headerRow.appendChild(th)
    }

    const transitions: Map<number, Map<string, number>> = new Map()
    for (let i = 0; i < graph.numEdges(); i++) {
        const from = graph.edgeFrom(i)
        const to = graph.edgeTo(i)
        const label = graph.edgeLabel(i)
        if (!transitions.has(from)) transitions.set(from, new Map())
        transitions.get(from)!.set(label, to)
    }

    for (let i = 0; i < graph.size(); i++) {
        const tr = document.createElement('tr')

        const tdState = document.createElement('td')
        tdState.className = 'border border-gray-300 px-3 py-2 text-center'
        tdState.textContent = graph.stateIdAt(i)
        tr.appendChild(tdState)

        const tdKey = document.createElement('td')
        tdKey.className = 'border border-gray-300 px-3 py-2 text-center'
        tdKey.textContent = `{${graph.stateKeyAt(i)}}`
        tr.appendChild(tdKey)

        const tdType = document.createElement('td')
        tdType.className = 'border border-gray-300 px-3 py-2 text-center'
        tdType.textContent = graph.stateTypeAt(i)
        tr.appendChild(tdType)

        const fromMap = transitions.get(i) || new Map()
        for (const symbol of sortedSymbols) {
            const td = document.createElement('td')
            td.className = 'border border-gray-300 px-3 py-2 text-center'
            const target = fromMap.get(symbol)
            td.textContent = target !== undefined ? String(target) : ''
            tr.appendChild(td)
        }

        dfaTbody.appendChild(tr)
    }
}

function renderSVG(svg: SVGSVGElement, svgContent: string): void {
    const parser = new DOMParser()
    const svgDoc = parser.parseFromString(svgContent, 'image/svg+xml')
    svg.innerHTML = svgDoc.documentElement.innerHTML
    svg.setAttribute('viewBox', svgDoc.documentElement.getAttribute('viewBox')!)
}

convertButton.addEventListener('click', () => {
    const pattern = regexInput.value
    const re = new RegularExpression()

    if (!re.parse(pattern)) {
        errorMessage.textContent = re.errorMessage()
        errorContainer.hidden = false
        nfaContainer.hidden = true
        dfaContainer.hidden = true
        minDfaContainer.hidden = true
        setLocationHashValue(pattern)
        return
    }

    errorContainer.hidden = true

    const nfa = re.toNFA()
    const nfaGraph = RegularExpression.toNFAGraph(nfa)
    renderNFATable(nfaGraph)
    renderSVG(nfaSvg, nfaGraph.toSVG())
    nfaContainer.hidden = false

    const dfa = RegularExpression.toDFA(nfa)
    const dfaGraph = RegularExpression.toDFAGraph(dfa)
    renderDFATable(dfaGraph)
    renderSVG(dfaSvg, dfaGraph.toSVG())
    dfaContainer.hidden = false

    const minDfa = RegularExpression.toMinDFA(dfa)
    const minDfaGraph = RegularExpression.toDFAGraph(minDfa)
    renderMinDFATable(minDfaGraph)
    renderSVG(minDfaSvg, minDfaGraph.toSVG())
    minDfaContainer.hidden = false

    nfa.delete()
    dfa.delete()
    minDfa.delete()

    setLocationHashValue(pattern)
})

function renderMinDFATable(graph: DFAGraph): void {
    minDfaTbody.innerHTML = ''
    const symbols = new Set<string>()
    for (let i = 0; i < graph.numEdges(); i++) {
        symbols.add(graph.edgeLabel(i))
    }
    const sortedSymbols = Array.from(symbols).sort()

    const headerRow = minDfaTbody.parentElement!.querySelector('thead tr')!
    while (headerRow.children.length > 3) {
        headerRow.removeChild(headerRow.lastChild!)
    }
    for (const symbol of sortedSymbols) {
        const th = document.createElement('th')
        th.className = 'border border-gray-300 px-3 py-2 text-center font-semibold'
        th.textContent = symbol
        headerRow.appendChild(th)
    }

    const transitions: Map<number, Map<string, number>> = new Map()
    for (let i = 0; i < graph.numEdges(); i++) {
        const from = graph.edgeFrom(i)
        const to = graph.edgeTo(i)
        const label = graph.edgeLabel(i)
        if (!transitions.has(from)) transitions.set(from, new Map())
        transitions.get(from)!.set(label, to)
    }

    for (let i = 0; i < graph.size(); i++) {
        const tr = document.createElement('tr')

        const tdState = document.createElement('td')
        tdState.className = 'border border-gray-300 px-3 py-2 text-center'
        tdState.textContent = graph.stateIdAt(i)
        tr.appendChild(tdState)

        const tdKey = document.createElement('td')
        tdKey.className = 'border border-gray-300 px-3 py-2 text-center'
        tdKey.textContent = `{${graph.stateKeyAt(i)}}`
        tr.appendChild(tdKey)

        const tdType = document.createElement('td')
        tdType.className = 'border border-gray-300 px-3 py-2 text-center'
        tdType.textContent = graph.stateTypeAt(i)
        tr.appendChild(tdType)

        const fromMap = transitions.get(i) || new Map()
        for (const symbol of sortedSymbols) {
            const td = document.createElement('td')
            td.className = 'border border-gray-300 px-3 py-2 text-center'
            const target = fromMap.get(symbol)
            td.textContent = target !== undefined ? String(target) : ''
            tr.appendChild(td)
        }

        minDfaTbody.appendChild(tr)
    }
}

setupLocationHash()
setupSVGPanZoom(nfaSvg)
setupSVGDownloadButtons(nfaSvg, '#nfa-download-svg', '#nfa-download-png', 'nfa')
setupSVGPanZoom(dfaSvg)
setupSVGDownloadButtons(dfaSvg, '#dfa-download-svg', '#dfa-download-png', 'dfa')
setupSVGPanZoom(minDfaSvg)
setupSVGDownloadButtons(minDfaSvg, '#min-dfa-download-svg', '#min-dfa-download-png', 'min-dfa')

const savedPattern = getLocationHashValue()
if (savedPattern) {
    regexInput.value = savedPattern
}
convertButton.click()
