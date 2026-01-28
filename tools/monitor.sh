#!/bin/bash
# Open a serial monitor on the given device.  This is a thin
# wrapper around `idf.py monitor`.  Press Ctrl+] to exit.

PORT="${1:-/dev/ttyUSB0}"

echo "Starting serial monitor on $PORT..."
idf.py -p "$PORT" monitor