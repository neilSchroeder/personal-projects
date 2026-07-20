# Epic 0 Task 2: Emulator and Device-Testing Workflow

This runbook defines the daily development test loop for mobile behavior before physical device packaging.

## Scope and Validation Targets

- Android emulator baseline profile and startup checklist
- iOS simulator prerequisites and constraints
- Godot touch emulation verification
- End-to-end workflow a new contributor can run

## Prerequisites

### Common

- Godot 4.4+ installed
- Project cloned locally
- `project.godot` present at repository root

### Android (Linux/macOS)

- Android Studio installed
- Android SDK + Emulator installed from SDK Manager
- At least one x86_64 system image
- Hardware virtualization enabled:
  - Linux: KVM
  - macOS (Apple Silicon/Intel): Hypervisor framework

### iOS (macOS only)

- Xcode installed
- Xcode command-line tools installed
- iOS Simulator runtime downloaded in Xcode

## Godot Touch Emulation Verification

This repository is configured to emulate touch input from mouse.

Expected setting in `project.godot`:

- `[input_devices/pointing]`
- `emulate_touch_from_mouse=true`

Quick verification steps:

1. Open the project in Godot.
2. Go to Project Settings -> Input Devices -> Pointing.
3. Confirm Emulate Touch From Mouse is enabled.
4. Run the main scene and left-click in the game viewport.
5. Confirm click events are received by touch handlers (`InputEventScreenTouch`) once gameplay input is implemented.

## Android Emulator Baseline Profile

Use this baseline profile for team consistency:

- Device: Pixel 6 (or equivalent 1080p class profile)
- API level: Android 14 (API 34)
- ABI: x86_64
- RAM: 4096 MB
- Graphics: Hardware - GLES 2.0/3.0 compatible mode
- Storage: Default AVD allocation
- Orientation during tests: Landscape

Create AVD checklist:

1. Open Android Studio -> Device Manager.
2. Create Virtual Device.
3. Select Pixel 6.
4. Select Android 14 x86_64 image.
5. Name device `VoidGrit_API34_Pixel6`.
6. Set graphics to hardware acceleration.
7. Save and boot once to confirm successful startup.

## Android Startup Checklist (Daily)

1. Start `VoidGrit_API34_Pixel6` from Device Manager.
2. Wait for home screen idle state.
3. Launch Godot and open this project.
4. Ensure game window is landscape (`1920x1080` target behavior).
5. Run project and exercise touch/click flows.
6. Record observed behavior in Beads note if regressions appear.

## Epic 1 Visualization Quickstart

Use the helper script from repository root to launch emulator + Godot preview:

```bash
./scripts/launch_mobile_preview.sh VoidGrit_API34_Pixel6
```

Manual equivalent flow:

```bash
emulator -avd VoidGrit_API34_Pixel6
godot4 --path .
```

When the project opens, run `level_1.tscn` and verify:

- enemies progress along path and towers attack automatically
- wave number advances through 5 waves
- base HP decreases when enemies reach end
- end state shows `Victory` or `Game Over`

## iOS Simulator Notes and Constraints

Prerequisites:

1. macOS host required.
2. Xcode and iOS runtime installed.
3. Apple signing/export is not required for simulator-only checks.

Recommended simulator profile:

- Device: iPhone 15 (or similar notch device)
- iOS runtime: latest stable runtime installed in Xcode
- Orientation during tests: Landscape

Constraints:

- iOS simulator is macOS-only and cannot be run natively on Linux.
- Performance characteristics differ from physical devices.
- Input latency and GPU behavior are only approximate.
- Final performance validation must happen on real hardware in later epics.

## End-to-End New Contributor Workflow

1. Install prerequisites for your OS (Godot + Android Studio; Xcode if on macOS).
2. Clone repository and open it in Godot.
3. Verify touch emulation setting in Project Settings.
4. Create and boot `VoidGrit_API34_Pixel6` Android emulator.
5. Run the project from Godot and verify it launches to `level_1.tscn`.
6. Execute manual input smoke check in landscape mode.
7. If on macOS, run equivalent orientation/input checks in iOS Simulator.
8. Log results in Beads using `bd note tower_defense-2zm.2 "<result>"`.

## Manual Smoke Checklist

- [ ] Project launches without configuration errors.
- [ ] Landscape presentation is correct.
- [ ] Touch emulation is enabled and usable.
- [ ] Emulator boots reliably and remains stable during run.
- [ ] iOS notes are understood (or validated directly on macOS).

## Troubleshooting

- Emulator does not boot:
  - Re-check virtualization support and AVD image compatibility.
- Touch input does not trigger:
  - Re-check `emulate_touch_from_mouse=true` and relevant input handlers.
- Unexpected low performance:
  - Confirm emulator is using hardware graphics acceleration.
  - Use lower emulator resolution only for debugging; keep baseline profile for QA consistency.
