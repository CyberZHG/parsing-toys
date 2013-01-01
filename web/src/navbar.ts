document.querySelector<HTMLButtonElement>("#menu-btn")!.addEventListener('click', () => {
    const menu = document.querySelector<HTMLDivElement>("#mobile-menu")!
    menu.classList.toggle('hidden')
})
