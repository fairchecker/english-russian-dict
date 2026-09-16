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

enable_testing()

include(FetchContent)
FetchContent_Declare(unittest_cpp
    GIT_REPOSITORY https://github.com/unittest-cpp/unittest-cpp.git
    GIT_TAG v2.0.0
)
FetchContent_MakeAvailable(unittest_cpp)

add_executable(unit_tests
    tests/unit_tests.cpp
    src/dictionary.cpp
    src/dictionary_node.cpp
)
target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(unit_tests PRIVATE UnitTest++)
add_test(NAME unit_tests COMMAND unit_tests)