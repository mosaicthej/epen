cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_COMPILER clang++)
project(CvCCSample LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(OpenCV_DIR "/usr/lib64/cmake/opencv4")

find_package(OpenCV REQUIRED)
add_executable(main main.cpp)

target_compile_features(main PRIVATE cxx_std_17)

target_include_directories(main PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(main
    PRIVATE opencv_core
    opencv_imgcodecs ${OpenCV_LIBS})


