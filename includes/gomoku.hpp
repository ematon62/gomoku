#pragma once

#include <chrono>
#include <sstream>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

// Configuration

// Level: debug, format: "[date time] message"
// Logger prints both to stdout and to filepath: logs/[date_time].log
spdlog::logger get_logger();
