cmake_minimum_required(VERSION 3.16)

project(englorus
    VERSION 1.0.0
    DESCRIPTION "English-Russian dictionary (BST)"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(englorus
    src/main.cpp
    src/dictionary.cpp
    src/dictionary_node.cpp
)

target_include_directories(englorus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(englorus PRIVATE -Wall -Wextra -Wpedantic)
elseif (MSVC)
    target_compile_options(englorus PRIVATE /W4)
endif()