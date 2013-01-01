cmake_minimum_required(VERSION 4.0)
project(ParsingToys)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(PARSING_TOYS_ENABLE_TESTS "Build tests" OFF)
option(PARSING_TOYS_ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(PARSING_TOYS_BIND_ES "Enable ECMAScript binding" OFF)

if(APPLE)
    set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif()

include(FetchContent)

FetchContent_Declare(
        GraphLayout
        GIT_REPOSITORY https://github.com/CyberZHG/GraphLayout.git
        GIT_TAG v1.0.2
)

FetchContent_MakeAvailable(GraphLayout)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(
            -Wall
            -Wextra
            -Werror
            -Wpedantic
            -Wmissing-include-dirs
            -Wundef
            -Wredundant-decls
    )
endif()

if(PARSING_TOYS_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g -fno-elide-constructors -fno-default-inline)
        add_link_options(--coverage)
    endif()
endif()

add_library(ParsingToys STATIC
        include/cfg.h
        src/cfg/cfg.cpp
        include/string_utils.h
        src/string_utils.cpp
        include/production_trie.h
        src/production_trie.cpp
        src/cfg/left_factor.cpp
        src/cfg/left_recursion.cpp
        src/cfg/first_and_follow.cpp
        src/cfg/closure.cpp
        include/automaton.h
        src/automaton.cpp
        src/cfg/lr0.cpp
)

target_include_directories(ParsingToys
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ParsingToys
        PRIVATE GraphLayout
)

if(PARSING_TOYS_ENABLE_TESTS)
    enable_testing()

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
    )

    FetchContent_MakeAvailable(googletest)

    add_executable(runTests
            tests/test_main.cpp
            tests/cfg/test_tokenize.cpp
            tests/cfg/test_parse.cpp
            tests/cfg/test_production_trie.cpp
            tests/test_string_utils.cpp
            tests/cfg/test_primed_symbol.cpp
            tests/cfg/test_left_factor.cpp
            tests/cfg/test_left_recursion.cpp
            tests/cfg/test_first_and_follow.cpp
            tests/cfg/test_closure.cpp
            tests/cfg/test_lr0.cpp
    )

    target_link_libraries(runTests
            PRIVATE ParsingToys
            PRIVATE gtest gtest_main
    )

    if (PARSING_TOYS_ENABLE_RANDOM_TESTS)
        target_compile_definitions(runTests
                PRIVATE PARSING_TOYS_ENABLE_RANDOM_TESTS=ON
        )
    endif ()

    add_test(NAME ParsingToysTests COMMAND runTests)
endif ()

if (PARSING_TOYS_BIND_ES)

    add_executable(ParsingToysWASM
            wasm/binding.cpp)

    target_link_libraries(ParsingToysWASM
            PRIVATE ParsingToys
    )

    target_compile_options(ParsingToysWASM PRIVATE "-O3")

    set_target_properties(ParsingToysWASM PROPERTIES SUFFIX ".js")

    target_link_options(ParsingToysWASM PRIVATE
            "--bind"
            "-sMODULARIZE=1"
            "-sEXPORT_ES6=1"
            "-sEXPORT_NAME=ParsingToysWASMModule"
    )
endif ()