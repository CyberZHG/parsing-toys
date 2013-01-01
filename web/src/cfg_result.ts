export function setupCFGResult(elementNamePrefix: string = 'cfg-result') {
    const highlighted = document.querySelector<HTMLElement>(`#${elementNamePrefix}-highlighted`)!
    const lineNumbers = document.querySelector<HTMLDivElement>(`#${elementNamePrefix}-line-numbers`)!
    const buttonCopyCode = document.querySelector<HTMLButtonElement>(`#${elementNamePrefix}-button-copy-code`)!

    highlighted.parentElement!.addEventListener('scroll', () => {
        lineNumbers.scrollTop = highlighted.parentElement!.scrollTop
    })

    buttonCopyCode.addEventListener('click', async () => {
        await navigator.clipboard.writeText(highlighted.textContent)
    })
}

export function updateCFGResult(code: string, elementNamePrefix: string = 'cfg-result') {
    const highlighted = document.querySelector<HTMLElement>(`#${elementNamePrefix}-highlighted`)!
    const lineNumbers = document.querySelector<HTMLDivElement>(`#${elementNamePrefix}-line-numbers`)!

    highlighted.innerHTML = code
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;')
        .replace(/'/g, '&#39;')
        .replace(/-&gt;/g, '<span class="text-blue-500">-&gt;</span>')
        .replace(/\|/g, '<span class="text-yellow-600">|</span>')
    const lines = code.split('\n').length
    let html = ''
    for (let i = 1; i <= lines; i++) {
        html += i + '\n'
    }
    lineNumbers.textContent = html + '\n'
}
