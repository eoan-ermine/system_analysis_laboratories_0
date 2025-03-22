cmake_minimum_required(VERSION 3.27)

project(systems_analysis_laboratories_0 VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 26)

find_package(Qt6 REQUIRED COMPONENTS Gui Widgets Qml Quick QuickControls2)
qt_standard_project_setup()

add_library(common)
target_include_directories(common INTERFACE sources)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

add_executable(cli sources/cli/main.cpp)
target_link_libraries(cli PRIVATE common)

add_subdirectory(sources/plotter)
