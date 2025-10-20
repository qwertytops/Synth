cmake_minimum_required(VERSION 3.15)
project(Synth LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

# set(CMAKE_AUTOMOC ON)

# Add include directories
include_directories(include)

# Collect all source files
file(GLOB_RECURSE SOURCES src/*.cpp)

# QT: Set CMAKE_PREFIX_PATH for brew-installed Qt
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt/lib/cmake")

# QT: Find required modules
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Add executable
add_executable(synth ${SOURCES})

# Link Apple frameworks
find_library(APPLICATIONSERVICES_FRAMEWORK ApplicationServices)
find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)

target_link_libraries(synth
    ${APPLICATIONSERVICES_FRAMEWORK}
    ${AUDIOTOOLBOX_FRAMEWORK}
    Qt6::Widgets
)

add_custom_target( run
    COMMAND synth                       # what to execute
    DEPENDS synth                       # make sure the binary is up-to-date
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}  # where to run
    COMMENT "Building and launching Synth…"              # (optional)
)
add_custom_target(debug
    COMMAND lldb ./synth
    DEPENDS synth
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Launching Synth under LLDB…"
)

# Set platform-specific output paths
if(APPLE)
    set_target_properties(synth PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        OSX_ARCHITECTURES "arm64"
    )
endif()
