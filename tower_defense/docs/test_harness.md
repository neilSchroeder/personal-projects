# Epic 0 Task 3: Test Harness and Smoke Suite

This document defines the baseline test harness for early project confidence gates.

## Goal

Provide a minimal, repeatable smoke suite that validates:

- project boot settings are wired
- autoload configuration is present and loadable
- critical scripts load
- critical scenes load and instantiate

## Harness Layout

- Runner script: `tests/smoke/smoke_runner.gd`
- Local wrapper: `scripts/run_smoke_tests.sh`

## How to Run Locally

From repository root:

```bash
./scripts/run_smoke_tests.sh
```

Direct Godot invocation alternative:

```bash
godot4 --headless --path . --script res://tests/smoke/smoke_runner.gd
```

If `godot4` is not available, use `godot`.

## What the Smoke Suite Checks

1. `application/run/main_scene` exists and points to a valid scene.
2. `autoload/GameManager` exists and points to a valid script.
3. Required scripts load:
   - `res://common/constants.gd`
   - `res://common/game_manager.gd`
   - `res://scenes/gameplay/level_1.gd`
   - `res://scenes/towers/base_tower.gd`
   - `res://scenes/enemies/base_enemy.gd`
   - `res://scenes/ui/hud.gd`
4. Required scenes load and instantiate:
   - `res://scenes/gameplay/level_1.tscn`
   - `res://scenes/towers/base_tower.tscn`
   - `res://scenes/enemies/base_enemy.tscn`
   - `res://scenes/ui/hud.tscn`

## Pass/Fail Criteria (Epic 0 Exit Gate)

Pass criteria:

- Smoke runner exits with code `0`
- Output includes `[SMOKE] PASS`

Fail criteria:

- Smoke runner exits non-zero
- Output includes `[SMOKE] FAIL` and one or more failure lines

Any fail is blocking for Epic 0 completion.

## Notes for Future CI

When CI is added, run `scripts/run_smoke_tests.sh` as the first validation step to catch project wiring regressions before deeper gameplay tests.
