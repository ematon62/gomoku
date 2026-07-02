#pragma once

#include "spdlog/spdlog.h"

// Configure the default logger
// Level: debug, format: "[date time] message"
// Logger prints both to stdout and to filepath: logs/[date time].log
void setup_default_logger();
