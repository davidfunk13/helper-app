cmake_minimum_required(VERSION 3.21)

# Modern version management
project(dave
    VERSION 1.0.0
    DESCRIPTION "Security helper tool for cryptographic analysis and penetration testing"
    LANGUAGES CXX
)

# Project configuration
set(PROJECT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(PROJECT_VERSION_TWEAK 0)

# Modern CMake standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_TESTS "Build unit tests" ON)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis tools" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers (Debug builds only)" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Include standard modules
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Enable testing
if(BUILD_TESTS)
    enable_testing()
    include(CTest)
endif()

# Find Qt6 with proper component specification
find_package(Qt6 6.2 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Gui
)

if(BUILD_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Test)
endif()

# Qt configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ============================================================================
# Static Analysis and Sanitizers
# ============================================================================
if(ENABLE_STATIC_ANALYSIS)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()
    
    find_program(CPPCHECK_EXE NAMES "cppcheck")
    if(CPPCHECK_EXE)
        set(CMAKE_CXX_CPPCHECK "${CPPCHECK_EXE};--enable=all;--inconclusive;--std=c++17")
    endif()
endif()

# Sanitizers (Debug builds only)
if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(SANITIZER_FLAGS "-fsanitize=address,undefined -fno-sanitize-recover=all")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SANITIZER_FLAGS}")
        set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} ${SANITIZER_FLAGS}")
    endif()
endif()

# ============================================================================
# Core Library (Business Logic)
# ============================================================================
add_library(dave_core STATIC
    src/core/decoder.cpp
    src/core/unpacker.cpp
    src/core/curl_builder.cpp
    src/core/decoder.h
    src/core/unpacker.h
    src/core/curl_builder.h
)

# Modern target-based configuration
target_link_libraries(dave_core 
    PUBLIC 
        Qt6::Core
)

target_include_directories(dave_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/core>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(dave_core 
    PUBLIC 
        cxx_std_17
)

# Set target properties
set_target_properties(dave_core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME "dave-core"
    EXPORT_NAME "Core"
)

# Compiler-specific options
target_compile_options(dave_core PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# ============================================================================
# Main Application
# ============================================================================
add_executable(dave 
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
)

# Modern target-based linking
target_link_libraries(dave 
    PRIVATE 
        dave_core
        Qt6::Core 
        Qt6::Widgets
        Qt6::Gui
)

target_include_directories(dave 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(dave 
    PRIVATE 
        cxx_std_17
)

# Set application properties
set_target_properties(dave PROPERTIES
    VERSION ${PROJECT_VERSION}
    OUTPUT_NAME "dave"
)

# Platform-specific settings
if(WIN32)
    set_target_properties(dave PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
elseif(APPLE)
    set_target_properties(dave PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()

# Tests
# ============================================================================
# Unit Tests
# ============================================================================
if(BUILD_TESTS)
    # Test executables
    set(TEST_TARGETS test_decoder test_unpacker test_curl_builder)
    
    foreach(test_target ${TEST_TARGETS})
        add_executable(${test_target} src/tests/${test_target}.cpp)
        
        target_link_libraries(${test_target} 
            PRIVATE 
                dave_core 
                Qt6::Core 
                Qt6::Test
        )
        
        target_include_directories(${test_target} 
            PRIVATE 
                ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
        
        target_compile_features(${test_target} 
            PRIVATE 
                cxx_std_17
        )
        
        # Add test to CTest
        string(REPLACE "test_" "" test_name ${test_target})
        string(TOUPPER ${test_name} test_name_upper)
        add_test(NAME ${test_name_upper}Tests COMMAND ${test_target})
        
        # Set test properties
        set_tests_properties(${test_name_upper}Tests PROPERTIES
            TIMEOUT 30
            LABELS "unit"
        )
    endforeach()

    # Add custom target to run all tests
    add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --parallel 4
        DEPENDS ${TEST_TARGETS}
        COMMENT "Running all unit tests"
    )
endif()

# ============================================================================
# Installation Configuration
# ============================================================================

# Install main application for end users - proper macOS bundle installation
install(TARGETS dave
    BUNDLE DESTINATION .
    COMPONENT Runtime
)


# Install development libraries separately
install(TARGETS dave_core
    EXPORT DaveTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT Development
)

# Install headers
install(DIRECTORY src/core/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dave
    COMPONENT Development
    FILES_MATCHING PATTERN "*.h"
)

# Install export targets for find_package support
install(EXPORT DaveTargets
    FILE DaveTargets.cmake
    NAMESPACE Dave::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Dave
    COMPONENT Development
)

# Create package config files
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/DaveConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/DaveConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Dave
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/DaveConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/DaveConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/DaveConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Dave
    COMPONENT Development
)

# Bundle Qt dependencies during build (industry standard approach)
if(APPLE)
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS ${Qt6_DIR}/../../../bin)
    if(MACDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET dave POST_BUILD
            COMMAND ${MACDEPLOYQT_EXECUTABLE} $<TARGET_BUNDLE_DIR:dave> -no-strip
            COMMENT "Bundling Qt frameworks"
        )
    endif()
endif()

# ============================================================================
# CPack Configuration - Modern Packaging
# ============================================================================
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "Dave")
set(CPACK_PACKAGE_CONTACT "dave@example.com")
set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")

# Enhanced package metadata (no license dialog for DMG)
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")

# Platform-specific installer settings - industry standard
if(APPLE)
    # Standard macOS DMG installer with custom layout
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
    set(CPACK_DMG_FORMAT "UDZO")
    set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/dmg_background.png")
    
    # AppleScript for DMG customization (CPack calls with volume name as argument)
    if(EXISTS "${CMAKE_SOURCE_DIR}/DS_Store_Setup.scpt")
        set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_SOURCE_DIR}/DS_Store_Setup.scpt")
    endif()
    
    # Ensure Applications symlink is included
    set(CPACK_DMG_DISABLE_APPLICATIONS_SYMLINK OFF)
    
    # Only include Runtime components for end users
    set(CPACK_COMPONENTS_ALL Runtime)
elseif(WIN32)
    # Standard Windows NSIS installer
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME} - ${PROJECT_DESCRIPTION}")
    set(CPACK_NSIS_PACKAGE_NAME "${PROJECT_NAME}")
else()
    # Standard Linux packages
    set(CPACK_GENERATOR "DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/System")
endif()

# CPack configuration for installers
include(CPack)