cmake_minimum_required(VERSION 3.20)
project(OfficeOnFire)

set(CMAKE_CXX_STANDARD 17)

# include_directories(./include)

add_executable(OfficeOnFire game.cpp maze.cpp ghost.cpp player.cpp)

# linker flags for emscripten
set_target_properties(OfficeOnFire PROPERTIES
    LINK_FLAGS "-s MODULARIZE=1 -s EXPORT_ES6=1 -s EXPORT_NAME='Module' --bind -s WASM=1 --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/floors@/floors"
)