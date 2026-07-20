# Epic 0 Task 1 Skeleton Checklist

This checklist tracks baseline repository skeleton requirements for the Godot 4 project.

## Directory Structure

- [x] `assets/audio/`
- [x] `assets/fonts/`
- [x] `common/`
- [x] `scenes/gameplay/`
- [x] `scenes/towers/`
- [x] `scenes/enemies/`
- [x] `scenes/ui/`
- [x] `docs/`

## Placeholder Scenes and Scripts

- [x] `scenes/gameplay/level_1.tscn`
- [x] `scenes/gameplay/level_1.gd`
- [x] `scenes/towers/base_tower.tscn`
- [x] `scenes/towers/base_tower.gd`
- [x] `scenes/enemies/base_enemy.tscn`
- [x] `scenes/enemies/base_enemy.gd`
- [x] `scenes/ui/hud.tscn`
- [x] `scenes/ui/hud.gd`

## Common Autoload-Ready Stubs

- [x] `common/constants.gd` with thematic enums/constants
- [x] `common/game_manager.gd` singleton-ready state shell
- [x] `project.godot` autoload entry for `GameManager`

## Conventions

- [x] GDScript placeholders are statically typed where practical.
- [x] Scene/script paths follow feature-by-feature layout.
- [x] Landscape target (`1920x1080`) and mobile renderer defaults are set.
- [x] Touch emulation from mouse enabled for desktop iteration.

## Emulator Workflow Documentation

- [x] `docs/emulator_workflow.md` exists with Android baseline profile.
- [x] iOS simulator prerequisites and constraints are documented.
- [x] End-to-end contributor runbook is documented.

## Test Harness and Smoke Suite

- [x] `tests/smoke/smoke_runner.gd` exists.
- [x] `scripts/run_smoke_tests.sh` exists for local execution.
- [x] `docs/test_harness.md` documents run commands and pass/fail criteria.
