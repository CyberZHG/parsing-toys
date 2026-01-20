const DARK_MODE_KEY = 'parsing-toys-dark-mode'

export function isDarkMode(): boolean {
    const stored = localStorage.getItem(DARK_MODE_KEY)
    if (stored !== null) {
        return stored === 'true'
    }
    return window.matchMedia('(prefers-color-scheme: dark)').matches
}

export function setDarkMode(dark: boolean): void {
    localStorage.setItem(DARK_MODE_KEY, String(dark))
    applyDarkMode(dark)
}

export function toggleDarkMode(): boolean {
    const newValue = !isDarkMode()
    setDarkMode(newValue)
    return newValue
}

export function applyDarkMode(dark: boolean): void {
    if (dark) {
        document.documentElement.classList.add('dark')
    } else {
        document.documentElement.classList.remove('dark')
    }
}

export function initDarkMode(): void {
    applyDarkMode(isDarkMode())
}
