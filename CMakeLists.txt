cmake_minimum_required(VERSION 3.16)
project(gomoku VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(CMAKE_CXX_CLANG_TIDY clang-tidy)
find_package(spdlog REQUIRED)

file(GLOB_RECURSE FRONTEND_SOURCES src/*.cpp src/*.hpp)

add_executable(gomoku ${FRONTEND_SOURCES})
target_link_libraries(gomoku Qt5::Widgets spdlog::spdlog)
