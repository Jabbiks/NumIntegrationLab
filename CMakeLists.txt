cmake_minimum_required(VERSION 3.21)
project(NumIntegrationLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Статична бібліотека чисельного інтегрування
add_library(NumIntegrationLib STATIC
    src/NumIntegration.cpp
    src/IntegrationTask.cpp
)

target_include_directories(NumIntegrationLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 2. Демонстраційна програма
add_executable(IntegrationDemo
    demo/main.cpp
)

target_link_libraries(IntegrationDemo
    PRIVATE
        NumIntegrationLib
)

# 3. Демонстрація антипатернів обробки винятків
add_executable(AntipatternDemo
    antipattern/main.cpp
)

# 4. Модульні тести Google Test
find_package(GTest CONFIG REQUIRED)
enable_testing()

add_executable(IntegrationTests
    tests/IntegrationTests.cpp
)

target_link_libraries(IntegrationTests
    PRIVATE
        NumIntegrationLib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(IntegrationTests DISCOVERY_MODE PRE_TEST)