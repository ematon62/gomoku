#include <chrono>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

constexpr std::string_view LOGS_FOLDER("logs/");
constexpr std::string_view LOGS_FILE_EXTENSION(".log");

void setup_default_logger() {
  // Initialize log file
  const auto tp_utc = std::chrono::system_clock::now();
  std::ostringstream filepath;
  filepath << LOGS_FOLDER << std::chrono::current_zone()->to_local(tp_utc) << LOGS_FILE_EXTENSION;

  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file_sink =
      std::make_shared<spdlog::sinks::basic_file_sink_mt>(filepath.str().c_str(), true);

  // Configure default logger: add sinks and pattern
  auto sinks = spdlog::default_logger()->sinks();
  sinks.push_back(console_sink);
  sinks.push_back(file_sink);
  spdlog::default_logger()->set_level(spdlog::level::info);
  spdlog::default_logger()->set_pattern("*** [%H:%M:%S %z] %v ***");
}
