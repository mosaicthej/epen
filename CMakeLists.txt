cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_COMPILER clang++)
project(InkAnimation LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -g)

include(FetchContent)

option(ENABLE_DEBUG "Enable debug output" OFF)

# Fetch SFML
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Find OpenCV (ensure your OpenCV path is correctly set)
set(OpenCV_DIR "/usr/lib64/cmake/opencv4") # Adjust this path if needed
find_package(OpenCV REQUIRED)

# Define executable sources explicitly
set(SOURCES
    src/main.cpp
    src/ImageProcessor.cpp
    src/InkAnimator.cpp
)

add_executable(InkAnimation ${SOURCES})
if(ENABLE_DEBUG)
    target_compile_definitions(InkAnimation PRIVATE DEBUG)
endif()


# Include directories
target_include_directories(InkAnimation PRIVATE
    src
    ${OpenCV_INCLUDE_DIRS}
)

# Link SFML and OpenCV correctly
target_link_libraries(InkAnimation PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    ${OpenCV_LIBS}
)

