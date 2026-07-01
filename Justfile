build_dir := "build"

compile:
    cmake -B {{build_dir}} -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build {{build_dir}} --parallel
    ln -sf {{build_dir}}/compile_commands.json compile_commands.json

start: compile
    ./gomoku

start-back: compile
    ./gomoku_back

fmt:
    find src -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

lint:
    find src -name "*.cpp" -o -name "*.hpp" | xargs clang-tidy -p {{build_dir}}

clean:
    rm -rf {{build_dir}}
