cmake_minimum_required(VERSION 3.15)
project(MSSQLVectorAdapter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(ENABLE_SIMD "Enable SIMD optimizations" ON)
option(ENABLE_AVX2 "Enable AVX2 instructions" ON)
option(ENABLE_AVX512 "Enable AVX-512 instructions" OFF)
option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_TESTS "Build test suite" ON)

# Find required packages
find_package(ODBC REQUIRED)
find_package(Threads REQUIRED)

# SIMD flags
if(ENABLE_SIMD)
    if(MSVC)
        if(ENABLE_AVX2)
            add_compile_options(/arch:AVX2)
        endif()
        if(ENABLE_AVX512)
            add_compile_options(/arch:AVX512)
        endif()
    else()
        if(ENABLE_AVX2)
            add_compile_options(-mavx2 -mfma)
        endif()
        if(ENABLE_AVX512)
            add_compile_options(-mavx512f -mavx512bw -mavx512vl)
        endif()
    endif()
    add_definitions(-DUSE_SIMD=1)
endif()

# Source files
set(HEADERS
    mssql_vector_adapter.h
)

set(SOURCES
    src/mssql_vector_adapter.cpp
    src/connection_pool.cpp
    src/simd_engine.cpp
    src/bulk_copy.cpp
    src/memory_mapped.cpp
    src/async_executor.cpp
    src/vector_stream.cpp
)

# Create library
add_library(mssql_vector_adapter STATIC ${SOURCES} ${HEADERS})

target_include_directories(mssql_vector_adapter
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(mssql_vector_adapter
    PUBLIC
        ODBC::ODBC
        Threads::Threads
)

# Compiler features
target_compile_features(mssql_vector_adapter PUBLIC cxx_std_17)

# Platform specific
if(WIN32)
    target_compile_definitions(mssql_vector_adapter PRIVATE
        -DWIN32_LEAN_AND_MEAN
        -DNOMINMAX
        -D_CRT_SECURE_NO_WARNINGS
    )
else()
    target_compile_options(mssql_vector_adapter PRIVATE
        -Wall -Wextra -Wpedantic
        -fPIC
    )
endif()

# Install
install(TARGETS mssql_vector_adapter
    EXPORT mssql_vector_adapter-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${HEADERS} DESTINATION include)

install(EXPORT mssql_vector_adapter-targets
    FILE mssql_vector_adapter-config.cmake
    NAMESPACE mssql_vector::
    DESTINATION lib/cmake/mssql_vector_adapter
)

# Examples
if(BUILD_EXAMPLES)
    add_executable(example_usage example_usage.cpp)
    target_link_libraries(example_usage PRIVATE mssql_vector_adapter)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    
    # Add test directory if it exists
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
        add_subdirectory(tests)
    endif()
endif()

# Packaging
set(CPACK_PACKAGE_NAME "mssql-vector-adapter")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "Your Name <your.email@example.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "High-performance C++ vector adapter for SQL Server")
include(CPack)

