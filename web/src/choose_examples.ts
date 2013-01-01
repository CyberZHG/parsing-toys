export function setupExamplesMenu(options: Record<string, string>, onValueChange: (arg0: string) => void) {
    const examplesMenu = document.querySelector<HTMLUListElement>('#examples-menu')!
    const examplesMenuButtonShow = document.querySelector<HTMLUListElement>('#examples-menu-button-show')!

    let optionsHTML = ""
    Object.keys(options).forEach(key => {
        optionsHTML += `<li class="px-3 py-2 hover:bg-gray-100 cursor-pointer" data-value="${key}">${options[key]}</li>`
    })
    examplesMenu.innerHTML = optionsHTML

    examplesMenu.addEventListener('click', (e) => {
        const target = e.target as HTMLElement | null
        if (!target) {
            return
        }
        const li = target.closest('li')
        if (!li) {
            return
        }
        const value = li.dataset.value!
        examplesMenu.classList.toggle('hidden')
        examplesMenuButtonShow.innerHTML = li.textContent.trim() + "<span>▾</span>"
        onValueChange(value)
    })

    examplesMenuButtonShow.addEventListener('click', () => {
        examplesMenu.classList.toggle('hidden')
    })
}