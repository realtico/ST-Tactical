cmake_minimum_required(VERSION 3.16)

project(ST_Tactical VERSION 1.0.0 LANGUAGES C)

# Set C99 Standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Cross-platform compiler flags (Optimization and Warnings)
if(MSVC)
    add_compile_options(/W4 /O2)
else()
    add_compile_options(-Wall -Wextra -O3)
endif()

# We specify that we only want the raylib library itself, ignoring examples/games.
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

# Download and include Raylib 5.0 at build-time using FetchContent
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.0
)
FetchContent_MakeAvailable(raylib)

# Setup our executable sources
set(SOURCES
    "src/video/terminal.c"
    "src/video/test_video.c"
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Include local headers (Zenit-Video and Miniaudio)
target_include_directories(${PROJECT_NAME} PRIVATE "include")

# Link Raylib to our executable
# The Raylib target provided by FetchContent is usually just 'raylib'
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Copy assets folder to output directory to make it easier to run
# Post-build step to copy the assets folder to the binary directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)
