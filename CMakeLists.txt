# CMakeLists.txt for KineToxin
cmake_minimum_required(VERSION 3.16)

project(KineToxin VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(MSVC)
    # MSVC specific flags
    add_compile_options(/W4 /WX /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    
    # Release optimizations
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2 /Ob2 /DNDEBUG)
    endif()
    
    # Debug information
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od /Zi)
    endif()
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/utils
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/evasion
    ${CMAKE_SOURCE_DIR}/src/persistence
)

# Utility Libraries
set(UTILS_SOURCES
    src/utils/crypto.cpp
    src/utils/crypto.h
)

add_library(KineToxinUtils STATIC ${UTILS_SOURCES})

# Core Surveillance Module
set(SURVEILLANCE_SOURCES
    src/core/surveillance/location_tracker.h
    # Add .cpp files as they are created
)

# Evasion Module
set(EVASION_SOURCES
    src/evasion/vm_detection.h
    # Add .cpp files as they are created
)

# Link libraries for Windows
if(WIN32)
    target_link_libraries(KineToxinUtils
        advapi32
        crypt32
        user32
        kernel32
        ole32
        oleaut32
        uuid
        sensorsapi
        locationapi
    )
endif()

# Main executable (to be created later)
# add_executable(KineToxin src/main.cpp)
# target_link_libraries(KineToxin KineToxinUtils)

# Testing (using Google Test - to be configured later)
option(BUILD_TESTS "Build test programs" ON)

if(BUILD_TESTS)
    enable_testing()
    # find_package(GTest REQUIRED)
    
    # Add test subdirectory when tests are created
    # add_subdirectory(tests)
endif()

# Installation rules
install(TARGETS KineToxinUtils
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Package configuration
set(CPACK_PACKAGE_NAME "KineToxin")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "Educational Malware Research Project")
set(CPACK_PACKAGE_CONTACT "Hassan Odowa <solo.research00.00@proton.me>")

include(CPack)
