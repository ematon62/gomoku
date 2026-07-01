#pragma once

#include <chrono>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"


// Configuration

spdlog::logger get_logger();
