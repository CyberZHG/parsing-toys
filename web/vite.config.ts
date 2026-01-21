import { defineConfig } from 'vite'
import { resolve } from 'path'
import handlebars from 'vite-plugin-handlebars'
import tailwindcss from '@tailwindcss/vite'
import Sitemap from 'vite-plugin-sitemap'

export default defineConfig({
    base: "/parsing-toys",
    build: {
        rollupOptions: {
            input: {
                main: resolve(__dirname, 'index.html'),
                cfg_reformat: resolve(__dirname, 'tools', 'cfg_reformat.html'),
                cfg_left_factoring: resolve(__dirname, 'tools', 'cfg_left_factoring.html'),
                cfg_left_recursion: resolve(__dirname, 'tools', 'cfg_left_recursion.html'),
                cnf: resolve(__dirname, 'tools', 'cfg_cnf.html'),
                cyk: resolve(__dirname, 'tools', 'cfg_cyk.html'),
                first_and_follow: resolve(__dirname, 'tools', 'cfg_first_and_follow.html'),
                ll1: resolve(__dirname, 'tools', 'cfg_ll1.html'),
                lr0: resolve(__dirname, 'tools', 'cfg_lr0.html'),
                slr1: resolve(__dirname, 'tools', 'cfg_slr1.html'),
                lr1: resolve(__dirname, 'tools', 'cfg_lr1.html'),
                lalr1: resolve(__dirname, 'tools', 'cfg_lalr1.html'),
                min_dfa: resolve(__dirname, 'tools', 're_min_dfa.html'),
            }
        }
    },
    plugins: [
        handlebars({
            partialDirectory: resolve(__dirname, 'partials'),
        }),
        tailwindcss(),
        Sitemap({ hostname: 'https://cyberzhg.github.io/parsing-toys', changefreq: 'monthly' }),
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
