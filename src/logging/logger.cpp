#include "gomoku.hpp"

spdlog::logger get_logger() {
    // Initialize log file
    const auto tp_utc = std::chrono::system_clock::now();
    std::ostringstream filepath;
    filepath << "logs/" << std::chrono::current_zone()->to_local(tp_utc) << ".log";

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink =
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(filepath.str().c_str(), true);

    spdlog::logger logger("logger", {console_sink, file_sink});
    logger.set_level(spdlog::level::info);
    spdlog::set_pattern("*** [%H:%M:%S %z] %v ***");
    return logger;
}
