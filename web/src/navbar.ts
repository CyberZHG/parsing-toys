import { initDarkMode, toggleDarkMode } from './dark_mode.ts'

initDarkMode()

document.querySelector<HTMLButtonElement>("#menu-btn")!.addEventListener('click', () => {
    const menu = document.querySelector<HTMLDivElement>("#mobile-menu")!
    menu.classList.toggle('hidden')
})

document.querySelector<HTMLButtonElement>("#dark-mode-toggle")?.addEventListener('click', () => {
    toggleDarkMode()
})

document.querySelector<HTMLButtonElement>("#dark-mode-toggle-mobile")?.addEventListener('click', () => {
    toggleDarkMode()
})
