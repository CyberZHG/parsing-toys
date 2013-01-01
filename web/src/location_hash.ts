function updateLocationInputValue() {
    const urlInput = document.querySelector<HTMLInputElement>('#footer-url')!
    urlInput.value = window.location.href
}

export function setupLocationHash() {
    updateLocationInputValue()
    const buttonCopyURL = document.querySelector<HTMLButtonElement>('#footer-url-copy-button')!
    buttonCopyURL.addEventListener('click', async () => {
        const urlInput = document.querySelector<HTMLInputElement>('#footer-url')!
        await navigator.clipboard.writeText(urlInput.value)
        const originalText = buttonCopyURL.textContent
        buttonCopyURL.innerHTML = "Copied!"
        buttonCopyURL.disabled = true
        setTimeout(() => {
            buttonCopyURL.textContent = originalText;
            buttonCopyURL.disabled = false
        }, 2000);
    })
}

export function getLocationHash(): string {
    return window.location.hash.replace(/^#/, "")
}

export function setLocationHash(hash: string): void {
    if (hash.length <= 4096) {
        window.location.hash = hash.startsWith("#") ? hash : `#${hash}`
        updateLocationInputValue()
    } else {
        const urlInput = document.querySelector<HTMLInputElement>('#footer-url')!
        urlInput.value = "No URL is generated because the payload is too large."
    }
}

export function getLocationHashValue(): string {
    return decodeURIComponent(
        Array.from(atob(getLocationHash()))
            .map(c => `%${c.charCodeAt(0).toString(16).padStart(2, "0")}`)
            .join("")
    )
}

export function setLocationHashValue(value: string): void {
    const hashValue = btoa(
        encodeURIComponent(value).replace(
            /%([0-9A-F]{2})/g,
            (_, p1) => String.fromCharCode(parseInt(p1, 16))
        )
    )
    setLocationHash(hashValue)
}
