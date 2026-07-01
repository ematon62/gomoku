#!/usr/bin/bash

# Print last log
logs_dir="logs/"

last_log=$(find "$logs_dir" -maxdepth 1 -type f -name "*.log" -printf "%f\n" | sort | tail -n 1)

if [[ -n "$last_log" ]]; then
    cat "$logs_dir$last_log"
else
    echo "No log files found in $logs_dir" >&2
    exit 1
fi
