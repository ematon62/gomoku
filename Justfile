build_dir := "build"

compile:
    cmake -B {{build_dir}} -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++
    cmake --build {{build_dir}} --parallel

start: compile
    ./{{build_dir}}/gomoku

start-back: compile
    ./{{build_dir}}/gomoku_back

fmt:
    find src -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

lint:
    find src -name "*.cpp" -o -name "*.hpp" | xargs clang-tidy -p {{build_dir}}

clean:
    rm -rf {{build_dir}}
