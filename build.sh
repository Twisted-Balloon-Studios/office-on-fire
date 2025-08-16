source ~/emsdk/emsdk_env.sh
rm -rf build
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake ..
cmake --build .
cd ..
