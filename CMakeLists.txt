cmake_minimum_required(VERSION 3.10)
project(CyclomaticComplexity)

# Find LLVM and Clang
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Add include directories
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add definitions
add_definitions(${LLVM_DEFINITIONS})

# Create the plugin
add_library(cyclomatic_complexity SHARED
    src/CyclomaticComplexity.cpp
)

# Set C++ standard
target_compile_features(cyclomatic_complexity PRIVATE cxx_std_17)

# Link against LLVM and Clang libraries
target_link_libraries(cyclomatic_complexity
    PRIVATE
    clangTooling
    clangFrontend
    clangDriver
    clangSerialization
    clangParse
    clangSema
    clangAnalysis
    clangAST
    clangBasic
    clangEdit
    clangLex
    LLVM
)