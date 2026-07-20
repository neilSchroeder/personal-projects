#!/usr/bin/env bash
set -euo pipefail

if command -v godot4 >/dev/null 2>&1; then
  GODOT_BIN="godot4"
elif command -v godot >/dev/null 2>&1; then
  GODOT_BIN="godot"
else
  echo "Error: could not find godot4 or godot in PATH." >&2
  exit 127
fi

"${GODOT_BIN}" --headless --path . --script res://tests/smoke/smoke_runner.gd
