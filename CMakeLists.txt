cmake_minimum_required(VERSION 3.20)
project(OfficeOnFire)

set(CMAKE_CXX_STANDARD 17)

# include_directories(./include)

add_executable(OfficeOnFire game.cpp)

# linker flags for emscripten
set_target_properties(OfficeOnFire PROPERTIES
    LINK_FLAGS "-s MODULARIZE=1 -s EXPORT_ES6=1 -s EXPORT_NAME='OfficeOnFireModule' --bind -s WASM=1"
)