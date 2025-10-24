cmake_minimum_required(VERSION 3.20)
project(Synth LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/elements)

set(CMAKE_BUILD_TYPE Debug)

# Add include directories
include_directories(include)


# Collect all source files
file(GLOB_RECURSE SOURCES src/*.cpp)

set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt/lib/cmake") # REMOVE THIS

# Find required modules
find_package(Qt6 REQUIRED COMPONENTS Widgets) # REMOVE THIS

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Add executable
add_executable(synth ${SOURCES})


# -------------------------------
# Sanitized variant of Synth
add_executable(synth_san EXCLUDE_FROM_ALL ${SOURCES})

# Copy include paths, link libs, and definitions from synth
target_include_directories(synth_san PRIVATE
    $<TARGET_PROPERTY:synth,INCLUDE_DIRECTORIES>)
target_link_libraries(synth_san PRIVATE
    $<TARGET_PROPERTY:synth,LINK_LIBRARIES>)
target_compile_definitions(synth_san PRIVATE
    $<TARGET_PROPERTY:synth,COMPILE_DEFINITIONS>)

# Add sanitizer flags
target_compile_options(synth_san PRIVATE -fsanitize=address,undefined -g -O1 -fno-omit-frame-pointer)
target_link_options(synth_san PRIVATE -fsanitize=address,undefined)
# -------------------------------


# Link Apple frameworks
find_library(APPLICATIONSERVICES_FRAMEWORK ApplicationServices)
find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)

target_link_libraries(synth
    ${APPLICATIONSERVICES_FRAMEWORK}
    ${AUDIOTOOLBOX_FRAMEWORK}
    Qt6::Widgets # REMOVE THIS
    cycfi::elements
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
add_custom_target(sanitize
    COMMAND $<TARGET_FILE:synth_san>
    DEPENDS synth_san
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running Synth with Address/Undefined Sanitizers…"
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
