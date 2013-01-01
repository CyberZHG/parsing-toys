#!/usr/bin/env bash

emcmake cmake .. -B build -DPARSING_TOYS_BIND_ES=ON
(cd build && emmake make ParsingToysWASM)
