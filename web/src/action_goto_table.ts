import { ContextFreeGrammar } from './wasm/index.js'

const EOF_SYMBOL = '¥'

type ActionGotoTableType = ReturnType<ContextFreeGrammar['computeLR0ActionGotoTable']>

export function renderActionGotoTable(cfg: ContextFreeGrammar, table: ActionGotoTableType): void {
    const actionGotoTableContainer = document.querySelector<HTMLElement>('#action-goto-table-container')!
    const actionGotoTableElement = document.querySelector<HTMLTableElement>('#action-goto-table')!

    const terminals = cfg.terminals().sort()
    terminals.push(EOF_SYMBOL)
    const nonTerminals = cfg.orderedNonTerminals()

    let headerHtml = '<thead>'
    headerHtml += '<tr class="bg-gray-100">'
    headerHtml += '<th class="border border-gray-300 px-2 py-1" rowspan="2">State</th>'
    headerHtml += `<th class="border border-gray-300 px-2 py-1" colspan="${terminals.length}">ACTION</th>`
    headerHtml += `<th class="border border-gray-300 px-2 py-1" colspan="${nonTerminals.length}">GOTO</th>`
    headerHtml += '</tr>'
    headerHtml += '<tr class="bg-gray-100">'
    for (const t of terminals) {
        headerHtml += `<th class="border border-gray-300 px-2 py-1">${t}</th>`
    }
    for (const nt of nonTerminals) {
        headerHtml += `<th class="border border-gray-300 px-2 py-1">${nt}</th>`
    }
    headerHtml += '</tr></thead>'

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

    actionGotoTableElement.innerHTML = headerHtml + bodyHtml
    actionGotoTableContainer.hidden = false
}
