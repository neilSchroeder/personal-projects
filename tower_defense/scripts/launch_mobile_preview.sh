#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 <avd_name>" >&2
  echo "Tip: list devices with: emulator -list-avds" >&2
  exit 2
fi

AVD_NAME="$1"

if ! command -v emulator >/dev/null 2>&1; then
  echo "Error: Android emulator binary not found in PATH." >&2
  exit 127
fi

if ! command -v godot4 >/dev/null 2>&1 && ! command -v godot >/dev/null 2>&1; then
  echo "Error: neither godot4 nor godot was found in PATH." >&2
  exit 127
fi

if command -v godot4 >/dev/null 2>&1; then
  GODOT_BIN="godot4"
else
  GODOT_BIN="godot"
fi

if ! emulator -list-avds | grep -qx "$AVD_NAME"; then
  echo "Error: AVD '$AVD_NAME' not found." >&2
  echo "Available AVDs:" >&2
  emulator -list-avds >&2 || true
  exit 1
fi

emulator -avd "$AVD_NAME" -no-snapshot-load >/tmp/voidgrit_emulator.log 2>&1 &
EMULATOR_PID=$!

cleanup() {
  if ps -p "$EMULATOR_PID" >/dev/null 2>&1; then
    kill "$EMULATOR_PID" >/dev/null 2>&1 || true
  fi
}
trap cleanup EXIT

echo "Launched emulator PID $EMULATOR_PID. Waiting for adb connectivity..."
adb start-server >/dev/null

wait_for_online_device() {
  local timeout_secs=120
  local elapsed=0
  while [[ $elapsed -lt $timeout_secs ]]; do
    if adb devices | awk 'NR>1 && $2=="device" { found=1 } END { exit(found?0:1) }'; then
      return 0
    fi
    sleep 1
    elapsed=$((elapsed + 1))
  done
  return 1
}

adb wait-for-device
if ! wait_for_online_device; then
  echo "Error: emulator did not reach adb online state within timeout." >&2
  echo "Current adb devices:" >&2
  adb devices -l >&2 || true
  echo "Recent emulator log:" >&2
  tail -n 40 /tmp/voidgrit_emulator.log >&2 || true
  exit 1
fi

echo "Device online. Launching Godot with verbose logs..."
"${GODOT_BIN}" --verbose --path .
