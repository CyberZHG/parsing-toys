import { LRParsingSteps } from './wasm/index.js'

export function renderParsingSteps(steps: LRParsingSteps): void {
    const lrStepsTbody = document.querySelector<HTMLElement>('#lr-steps-tbody')!
    lrStepsTbody.innerHTML = ''
    for (let i = 0; i < steps.size(); i++) {
        const row = document.createElement('tr')
        row.className = i % 2 === 0 ? 'bg-white dark:bg-gray-800' : 'bg-gray-50 dark:bg-gray-900'

        const stepCell = document.createElement('td')
        stepCell.className = 'border border-gray-300 dark:border-gray-600 px-3 py-2 text-center'
        stepCell.textContent = String(i + 1)

        const stackCell = document.createElement('td')
        stackCell.className = 'border border-gray-300 dark:border-gray-600 px-3 py-2'
        stackCell.textContent = steps.getStack(i).join(' ')

        const symbolsCell = document.createElement('td')
        symbolsCell.className = 'border border-gray-300 dark:border-gray-600 px-3 py-2'
        symbolsCell.textContent = steps.getSymbols(i).join(' ') || ''

        const inputCell = document.createElement('td')
        inputCell.className = 'border border-gray-300 dark:border-gray-600 px-3 py-2 text-right'
        inputCell.textContent = steps.getRemainingInputs(i).join(' ')

        const actionCell = document.createElement('td')
        actionCell.className = 'border border-gray-300 dark:border-gray-600 px-3 py-2'
        const action = steps.getAction(i)
        actionCell.textContent = action
        if (action.startsWith('conflict') || action.startsWith('invalid')) {
            actionCell.classList.add('text-red-600', 'dark:text-red-400')
        } else if (action === 'accept') {
            actionCell.classList.add('text-green-600', 'dark:text-green-400', 'font-semibold')
        }

        row.appendChild(stepCell)
        row.appendChild(stackCell)
        row.appendChild(symbolsCell)
        row.appendChild(inputCell)
        row.appendChild(actionCell)
        lrStepsTbody.appendChild(row)
    }
}
