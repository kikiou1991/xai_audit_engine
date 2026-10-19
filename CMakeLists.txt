cmake_minimum_required(VERSION 3.20)
project(xai_audit_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization flags for benchmarking
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native)
endif ()

# 1. Tells the compiler where to look for .hpp files when you write #include "..."
include_directories(include)

# 2. Compiles main.cpp together with the .cpp files in src/
add_executable(xai_audit_engine
        src/LogisticRegressionModel.cpp
        src/AuditingEngine.cpp
        main.cpp
)
