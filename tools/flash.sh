#!/bin/bash
# Flash the compiled firmware to an ESP32 device.  This script
# assumes that `idf.py` is available in your PATH and that the
# project has already been built.  Adjust the serial port and
# baud rate as necessary for your setup.

PORT="${1:-/dev/ttyUSB0}"
BAUD="${2:-460800}"

echo "Flashing firmware to $PORT at $BAUD baud..."
idf.py -p "$PORT" -b "$BAUD" flash