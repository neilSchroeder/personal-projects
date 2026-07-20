# Build Plan: VoidGrit Hybrid MVP Roadmap

This plan is the execution source of truth for the project and maps directly to Beads epics.

## Locked Decisions

* **MVP Direction:** Hybrid MVP (real-time tower defense core + minimal card layer from day one).
* **Orientation:** Landscape ($1920 \times 1080$).
* **Renderer:** Godot 4 Mobile (Forward+).
* **MVP Character Scope:** Scrapper only.
* **Primary Quality Bar:** Stable 5-wave runs with correct cleanup and touch-safe HUD readability.

## Scope Boundaries

### In Scope for MVP (Epic 1)

* 2 tower types: Single-Target Arrow and AoE Splash.
* 2 enemy types: Normal and Fast/Weak.
* Single fixed level with `Path2D` track and 5-wave pacing.
* Towers target furthest progress enemy.
* Win/lose game loop and safe-area HUD.
* Minimal card layer:
* Scrapper starter deck only.
* ENERGY-per-wave budget.
* 2 construct cards (Arrow Tower, Splash Tower).
* 1 maneuver card (Overdrive Target Tower).
* SCRAP drops with one simple mid-wave upgrade action.

### Explicitly Deferred Beyond MVP

* Hacker and Zealot playable decks.
* Relics and procedural node-map meta progression.
* Deep tower-combination trees and advanced path-redirection systems.

## Architecture Baseline

Feature-by-feature structure remains mandatory:

```text
res://
├── assets/
│   ├── audio/
│   └── fonts/
├── common/
│   ├── game_manager.gd
│   └── constants.gd
└── scenes/
	├── gameplay/
	├── towers/
	├── enemies/
	└── ui/
```

Runtime and card layer contract:

* Runtime owns wave state, enemy pathing, combat resolution, and node lifecycle cleanup.
* Card layer orchestrates runtime actions (placement and temporary effects), rather than duplicating runtime combat logic.

## Epic-Driven Execution Plan

### Epic 0: Project Skeleton, Environment, Emulator, and Test Foundation

Beads ID: tower_defense-2zm

Deliverables:

* Project bootstrap with canonical scene/script folder skeleton.
* Local environment setup docs and Godot editor settings.
* Emulator/simulator setup for dev testing and desktop touch emulation.
* Test suite scaffold with smoke checks for boot and key scene/script loading.

Exit Criteria:

* New contributor can set up, run, and execute baseline tests with documented steps.

### Epic 1: Hybrid MVP Vertical Slice

Beads ID: tower_defense-g67

Deliverables:

* Full 5-wave playable loop with 2 towers and 2 enemy types.
* Minimal Scrapper card loop with ENERGY, construct cards, and one maneuver card.
* SCRAP drop and one simple upgrade action.
* Stability/performance checks for repeated runs.

Exit Criteria:

* Playable run can be won/lost; all starter cards function; deferred scope is not partially implemented.

### Epic 2: MVP Polish, QA Hardening, and First Mobile Playtest Packaging

Beads ID: tower_defense-bk7

Deliverables:

* Balance pass, UX clarity pass, touch reliability pass, performance profiling baseline.
* Internal Android playtest package path documented end-to-end.

### Epic 3: Phase-2 Deck Expansion and Tower-Modifier Depth

Beads ID: tower_defense-rfq

Deliverables:

* Expanded Scrapper card pool and richer modifier interactions.
* Improved card targeting/preview/resolution UX and supporting tests.

### Epic 4: Roguelike Meta Progression and Additional Archetypes

Beads ID: tower_defense-oko

Deliverables:

* Procedural run structure, meta systems, and playable Hacker + Zealot decks.

## Dependency Order

1. Epic 0 blocks Epic 1.
2. Epic 1 blocks Epic 2.
3. Epic 2 blocks Epic 3.
4. Epic 3 blocks Epic 4.

## Implementation Notes for Sprint Planning

* Sprint planning should pull from `bd ready` only.
* Any new scope discovered during implementation must be added as child work under the active epic before coding continues.
* If a deferred feature is needed for MVP correctness, create a decision ticket and resolve scope explicitly instead of silently expanding Epic 1.