cmake_minimum_required(VERSION 3.14)
project(PolynomialProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  unittest-cpp
  GIT_REPOSITORY https://github.com/unittest-cpp/unittest-cpp.git
  GIT_TAG v2.0.0
  GIT_SUBMODULES ""
)
FetchContent_MakeAvailable(unittest-cpp)

add_library(polynomial_lib src/polynomial.cpp)

target_include_directories(polynomial_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(test_polynomial tests/unit_tests.cpp)
target_include_directories(test_polynomial PRIVATE ${unittest-cpp_SOURCE_DIR})
target_link_libraries(test_polynomial PRIVATE polynomial_lib UnitTest++)

enable_testing()
add_test(NAME PolynomialTests COMMAND test_polynomial)