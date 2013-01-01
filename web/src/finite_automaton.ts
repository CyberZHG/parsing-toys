export function setupSVGPanZoom(svg: SVGSVGElement): void {
    let isDragging = false
    let startX = 0
    let startY = 0
    let viewBox = { x: 0, y: 0, width: 0, height: 0 }

    function parseViewBox(): void {
        const vb = svg.getAttribute("viewBox")
        if (vb) {
            const parts = vb.split(/\s+/).map(Number)
            viewBox = { x: parts[0], y: parts[1], width: parts[2], height: parts[3] }
        }
    }

    function updateViewBox(): void {
        svg.setAttribute("viewBox", `${viewBox.x} ${viewBox.y} ${viewBox.width} ${viewBox.height}`)
    }

    svg.addEventListener("mousedown", (e: MouseEvent) => {
        if (e.button !== 0) return
        isDragging = true
        startX = e.clientX
        startY = e.clientY
        parseViewBox()
        e.preventDefault()
    })

    svg.addEventListener("mousemove", (e: MouseEvent) => {
        if (!isDragging) return
        const dx = e.clientX - startX
        const dy = e.clientY - startY
        const scale = viewBox.width / svg.clientWidth
        viewBox.x -= dx * scale
        viewBox.y -= dy * scale
        updateViewBox()
        startX = e.clientX
        startY = e.clientY
    })

    svg.addEventListener("mouseup", () => {
        isDragging = false
    })

    svg.addEventListener("mouseleave", () => {
        isDragging = false
    })

    svg.addEventListener("wheel", (e: WheelEvent) => {
        e.preventDefault()
        parseViewBox()

        const rect = svg.getBoundingClientRect()
        const mouseX = e.clientX - rect.left
        const mouseY = e.clientY - rect.top

        const svgX = viewBox.x + (mouseX / svg.clientWidth) * viewBox.width
        const svgY = viewBox.y + (mouseY / svg.clientHeight) * viewBox.height

        const zoomFactor = e.deltaY > 0 ? 1.1 : 0.9
        const newWidth = viewBox.width * zoomFactor
        const newHeight = viewBox.height * zoomFactor

        viewBox.x = svgX - (mouseX / svg.clientWidth) * newWidth
        viewBox.y = svgY - (mouseY / svg.clientHeight) * newHeight
        viewBox.width = newWidth
        viewBox.height = newHeight

        updateViewBox()
    }, { passive: false })
}
