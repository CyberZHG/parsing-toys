export function setupCFGEditor(prefix: string = 'cfg-editor') {
    const editor = document.querySelector<HTMLTextAreaElement>(`#${prefix}`)!
    const highlighted = document.querySelector<HTMLElement>(`#${prefix}-highlighted`)!
    const lineNumbers = document.querySelector<HTMLDivElement>(`#${prefix}-line-numbers`)!
    const buttonInsertEps = document.querySelector<HTMLButtonElement>(`#${prefix}-button-insert-eps`)!
    const buttonCopyCode = document.querySelector<HTMLButtonElement>(`#${prefix}-button-copy-code`)!

    function highlightCFG(code: string) : string {
        return code
            .replace(/&/g, '&amp;')
            .replace(/</g, '&lt;')
            .replace(/>/g, '&gt;')
            .replace(/"/g, '&quot;')
            .replace(/'/g, '&#39;')
            .replace(/-&gt;/g, '<span class="text-blue-500">-&gt;</span>')
            .replace(/→/g, '<span class="text-blue-500">→</span>')
            .replace(/\|/g, '<span class="text-yellow-600">|</span>')
            .replace(/ε/g, '<span class="text-green-700">ε</span>')
    }

    function updateLineNumbers() {
        const lines = editor.value.split('\n').length
        let html = ''
        for (let i = 1; i <= lines; i++) {
            html += i + '\n'
        }
        lineNumbers.textContent = html + '\n'
    }

    function syncEditor() {
        highlighted.innerHTML = highlightCFG(editor.value) + '\n\n'
        updateLineNumbers()
    }

    editor.addEventListener('input', () => {
        syncEditor()
    })

    editor.addEventListener('scroll', () => {
        const pre = highlighted.parentElement!
        pre.scrollTop = editor.scrollTop
        pre.scrollLeft = editor.scrollLeft
        lineNumbers.scrollTop = editor.scrollTop
    })

    buttonInsertEps.addEventListener('click', () => {
        const start = editor.selectionStart
        const end = editor.selectionEnd
        const value = editor.value
        editor.value = value.slice(0, start) + 'ε' + value.slice(end)
        editor.selectionStart = editor.selectionEnd = start + 1
        syncEditor()
        editor.focus()
    })

    buttonCopyCode.addEventListener('click', async () => {
        await navigator.clipboard.writeText(editor.value)
        const originalText = buttonCopyCode.textContent
        buttonCopyCode.innerHTML = "Copied!"
        buttonCopyCode.disabled = true
        setTimeout(() => {
            buttonCopyCode.textContent = originalText;
            buttonCopyCode.disabled = false
        }, 2000);
    })
}

export function getCFGEditorValue(prefix: string = 'cfg-editor'): string {
    const editor = document.querySelector<HTMLTextAreaElement>(`#${prefix}`)!
    return editor.value;
}

export function setCFGEditorValue(value: string, prefix: string = 'cfg-editor') {
    const editor = document.querySelector<HTMLTextAreaElement>(`#${prefix}`)!
    editor.value = value
    editor.dispatchEvent(new Event("input", {bubbles: true, cancelable: true}))
}
