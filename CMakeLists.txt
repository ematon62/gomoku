cmake_minimum_required(VERSION 3.16)
project(gomoku VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(ftxui)

set(CMAKE_CXX_CLANG_TIDY clang-tidy)
find_package(spdlog REQUIRED)

file(GLOB_RECURSE FRONTEND_SOURCES src/*.cpp src/*.hpp)

add_executable(gomoku ${FRONTEND_SOURCES})
include_directories(includes)
target_compile_options(gomoku PRIVATE
    -Wall
    -Werror
)
target_link_libraries(gomoku
    ftxui::screen
    ftxui::dom
    ftxui::component
    spdlog::spdlog
)
