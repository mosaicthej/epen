cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_COMPILER clang++)
project(CMakeSFMLOpenCVProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_compile_options(-Wall -Wextra -g)

include(FetchContent)

# Fetch SFML
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Find OpenCV
set(OpenCV_DIR "/usr/lib64/cmake/opencv4") # or wherever your OpenCV 4's CMake files are located
find_package(OpenCV REQUIRED)

# Executable
add_executable(main src/main.cpp)

# Compiler flags
target_compile_features(main PRIVATE cxx_std_17)

# Link SFML and OpenCV
# target_link_libraries(main PRIVATE SFML::Graphics ${OpenCV_LIBS})
target_include_directories(main PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(main 
    PRIVATE sfml-graphics 
    sfml-window 
    sfml-system 
    opencv_core 
    opencv_imgcodecs ${OpenCV_LIBS})
# target_include_directories(SFML_OpenCV_Demo PRIVATE ${OpenCV_INCLUDE_DIRS})

