#!/bin/bash

# Shell script to run Tango game with proper environment setup

# Set environment variables for better X11 compatibility
export PYTHONUNBUFFERED=1
export SDL_VIDEODRIVER=x11

# Check if uv is available
if command -v uv &> /dev/null; then
    echo "Running Tango game with uv..."
    uv run tango
else
    echo "uv not found, running with Python directly..."
    python -m src.tango_game
fi
