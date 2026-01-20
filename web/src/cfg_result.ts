export function setupCFGResult(elementNamePrefix: string = 'cfg-result') {
    const highlighted = document.querySelector<HTMLElement>(`#${elementNamePrefix}-highlighted`)!
    const highlightedParent = highlighted.parentElement!
    const lineNumbers = document.querySelector<HTMLDivElement>(`#${elementNamePrefix}-line-numbers`)!
    const buttonCopyCode = document.querySelector<HTMLButtonElement>(`#${elementNamePrefix}-button-copy-code`)!

    highlightedParent.addEventListener('scroll', () => {
        lineNumbers.scrollTop = highlightedParent.scrollTop
    })

    buttonCopyCode.addEventListener('click', async () => {
        await navigator.clipboard.writeText(highlighted.textContent)
        const originalText = buttonCopyCode.textContent
        buttonCopyCode.innerHTML = "Copied!"
        buttonCopyCode.disabled = true
        setTimeout(() => {
            buttonCopyCode.textContent = originalText;
            buttonCopyCode.disabled = false
        }, 2000);
    })

    highlightedParent.addEventListener('keydown', (e) => {
        if ((e.ctrlKey || e.metaKey) && e.key === 'a') {
            e.preventDefault()
            const selection = window.getSelection()!
            const range = document.createRange()
            range.selectNodeContents(highlightedParent)
            selection.removeAllRanges()
            selection.addRange(range)
        }
    })
}

function htmlEscape(s: string) : string {
    return s
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;')
        .replace(/'/g, '&#39;')
}

function regexEscape(s: string) : string {
    const characters = ['\\', '.', '*', '+', '?', '^', '$', '{', '}', '(', ')', '|', '[', ']', '/']
    for (const character of characters) {
        s = s.replace(character, '\\' + character)
    }
    return s
}

export function updateCFGResult(code: string, terminals: string[], nonTerminals: string[], elementNamePrefix: string = 'cfg-result') {
    const highlighted = document.querySelector<HTMLElement>(`#${elementNamePrefix}-highlighted`)!
    const lineNumbers = document.querySelector<HTMLDivElement>(`#${elementNamePrefix}-line-numbers`)!

    let highlightedCode = htmlEscape(code)
        .replace(/-&gt;/g, '<span class="text-blue-500">-&gt;</span>')
        .replace(/\|/g, '<span class="text-yellow-600">|</span>')
        .replace(/ε/g, '<span class="text-green-700 dark:text-green-500">ε</span>')
    if (terminals.length > 0) {
        for (let i = 0; i < terminals.length; ++i) {
            terminals[i] = regexEscape(htmlEscape(terminals[i]))
        }
        const terminalsRegex = new RegExp(`(?<=^|\\s)(${terminals.join("|")})(?=$|\\s)`, "g");
        highlightedCode = highlightedCode.replace(terminalsRegex, (match) => {
            return `<span class="text-green-700 dark:text-green-500">${match}</span>`
        })
    }
    if (nonTerminals.length > 0) {
        for (let i = 0; i < nonTerminals.length; ++i) {
            nonTerminals[i] = regexEscape(htmlEscape(nonTerminals[i]))
        }
        const nonTerminalsRegex = new RegExp(`(?<=^|\\s)(${nonTerminals.join("|")})(?=$|\\s)`, "g");
        highlightedCode = highlightedCode.replace(nonTerminalsRegex, (match) => {
            return `<span class="text-blue-800 dark:text-blue-100">${match}</span>`
        })
    }
    highlighted.innerHTML = highlightedCode
    const lines = code.split('\n').length
    let html = ''
    for (let i = 1; i <= lines; i++) {
        html += i + '\n'
    }
    lineNumbers.textContent = html + '\n'
}
