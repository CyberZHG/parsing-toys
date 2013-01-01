import { defineConfig } from 'vite'
import { resolve } from 'path'
import handlebars from 'vite-plugin-handlebars'
import tailwindcss from '@tailwindcss/vite'

export default defineConfig({
    base: "/parsing-toys",
    build: {
        rollupOptions: {
            input: {
                main: resolve(__dirname, 'index.html'),
                cfg_reformat: resolve(__dirname, 'tools', 'cfg_reformat.html'),
            }
        }
    },
    plugins: [
        handlebars({
            partialDirectory: resolve(__dirname, 'partials'),
        }),
        tailwindcss(),
    ],
    server: {
        fs: {
            allow: ['..']
        }
    },
    esbuild: {
        supported: {
            'top-level-await': true
        },
    },
    optimizeDeps: {
        esbuildOptions: {
            supported: {
                "top-level-await": true
            },
        },
    },
})
