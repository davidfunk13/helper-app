cmake_minimum_required(VERSION 3.16)
project(dave)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable automoc for Qt
set(CMAKE_AUTOMOC ON)

# Create executable
add_executable(dave 
    src/main.cpp
    src/decoder.cpp
    src/unpacker.cpp
    src/curl_builder.cpp
)

# Include directories
target_include_directories(dave PRIVATE src)

# Link Qt libraries
target_link_libraries(dave Qt6::Core Qt6::Widgets)

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