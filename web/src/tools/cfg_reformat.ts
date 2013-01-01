import { setupCFGResult, updateCFGResult } from '../cfg_result.ts'
import { ContextFreeGrammar } from '../wasm/index.js'

setupCFGResult()
document.querySelector<HTMLElement>('#button-cfg-reformat')?.addEventListener('click', () => {
    const editor = document.querySelector<HTMLTextAreaElement>('#cfg-editor')!
    const errorMessage = document.querySelector<HTMLTextAreaElement>('#cfg-reformat-error-message')!
    const cfg = new ContextFreeGrammar()
    if (cfg.parse(editor.value)) {
        updateCFGResult(cfg.toString())
        errorMessage.parentElement!.hidden = true
    } else {
        errorMessage.textContent = cfg.errorMessage()
        errorMessage.parentElement!.hidden = false
    }
})
