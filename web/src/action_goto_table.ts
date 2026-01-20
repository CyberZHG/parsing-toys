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
    headerHtml += '<tr class="bg-gray-100 dark:bg-gray-700">'
    headerHtml += '<th class="border border-gray-300 dark:border-gray-600 px-2 py-1 text-gray-900 dark:text-gray-100" rowspan="2">State</th>'
    headerHtml += `<th class="border border-gray-300 dark:border-gray-600 px-2 py-1 text-gray-900 dark:text-gray-100" colspan="${terminals.length}">ACTION</th>`
    headerHtml += `<th class="border border-gray-300 dark:border-gray-600 px-2 py-1 text-gray-900 dark:text-gray-100" colspan="${nonTerminals.length}">GOTO</th>`
    headerHtml += '</tr>'
    headerHtml += '<tr class="bg-gray-100 dark:bg-gray-700">'
    for (const t of terminals) {
        headerHtml += `<th class="border border-gray-300 dark:border-gray-600 px-2 py-1 text-gray-900 dark:text-gray-100">${t}</th>`
    }
    for (const nt of nonTerminals) {
        headerHtml += `<th class="border border-gray-300 dark:border-gray-600 px-2 py-1 text-gray-900 dark:text-gray-100">${nt}</th>`
    }
    headerHtml += '</tr></thead>'

    let bodyHtml = '<tbody>'
    const numStates = table.size()
    for (let i = 0; i < numStates; i++) {
        bodyHtml += `<tr class="bg-white dark:bg-gray-800"><td class="border border-gray-300 dark:border-gray-600 px-2 py-1 text-center text-gray-900 dark:text-gray-100">${i}</td>`
        for (const symbol of [...terminals, ...nonTerminals]) {
            const cellContent = table.getCell(i, symbol, '<br>')
            const hasConflict = table.hasConflictAt(i, symbol)
            const isAccept = cellContent === 'accept'

            let cellClass = 'border border-gray-300 dark:border-gray-600 px-2 py-1 text-center'
            if (hasConflict) {
                cellClass += ' bg-red-100 dark:bg-red-900 text-red-800 dark:text-red-200'
            } else if (isAccept) {
                cellClass += ' bg-green-100 dark:bg-green-900 text-green-800 dark:text-green-200'
            } else {
                cellClass += ' text-gray-900 dark:text-gray-100'
            }

            bodyHtml += `<td class="${cellClass}">${cellContent}</td>`
        }
        bodyHtml += '</tr>'
    }
    bodyHtml += '</tbody>'

    actionGotoTableElement.innerHTML = headerHtml + bodyHtml
    actionGotoTableContainer.hidden = false
}
