An `agents.md` file is an excellent addition to a modern game repository. It acts as a system prompt and contextual roadmap for **AI Coding Assistants** (like GitHub Copilot, Cursor, or Claude Engineer) when they help you build the project. It explicitly tells them how the codebase is structured, what technologies are used, and what rules they *must* follow.

Create a file named `agents.md` in the root of your repository and paste the following content:

```markdown
# AI Agent Context: Mobile Tower Defense (Godot 4 + GDScript)

You are an expert game developer specializing in Godot 4, 2D mobile optimizations, and architectural clean code using GDScript. You are assisting in building a lightweight, highly efficient mobile Tower Defense game.

---

## 🎯 Primary Project Goal
Build a functional, modular Minimum Viable Product (MVP) of a mobile tower defense game optimized for low battery consumption, consistent 60 FPS performance, and cross-platform screen resolution scalability.

---

## 🛠️ Tech Stack & Constraints
- **Engine:** Godot 4.x (Targeting Mobile/Compatibility rendering backend).
- **Language:** GDScript (Strictly static typing where possible).
- **Orientation:** [Choose one: Landscape (1920x1080) OR Portrait (1080x1920)].
- **Input:** Mobile touch event optimization (Mouse emulation enabled for testing).

---

## 📂 Project Architecture
We follow a strict **feature-by-feature** folder structure. Keep scenes, scripts, and localized visual resources grouped together inside their specific module directory.

```text
res://
├── common/                # Shared global scripts and configurations
│   ├── game_manager.gd    # Autoload / Global Singleton State
│   └── constants.gd       # Enums for Game States & Types
└── scenes/                # Module-specific directories
    ├── gameplay/          # Main level structures & loops
    ├── towers/            # Base Tower class & inherited variations
    ├── enemies/           # Base Enemy class & inherited variations
    └── ui/                # HUD, canvas overlays, shop menus

```

---

## 📜 GDScript Coding Standards & Rules

### 1. Static Typing Mandatory

Always enforce static typing on variables, function arguments, and return values.

* **Bad:** `var health = 100`
* **Good:** `var health: int = 100`
* **Good (Inferred):** `var health := 100`
* **Functions:** `func take_damage(amount: int) -> void:`

### 2. Signal Naming & Architecture

Use Godot 4 syntax for signals (`signal_name.emit()`). Use the past tense for events that already happened.

* **Declaration:** `signal enemy_died(gold_reward: int)`
* **Emission:** `enemy_died.emit(gold_reward)`

### 3. Node Referencing

Prefer `@onready` using unique names or explicit paths. Do not hardcode deep node structures that break if re-parented.

* **Good:** `@onready var fire_timer: Timer = %FireTimer` (Using Scene Unique Nodes)

### 4. Memory Cleanliness

Ensure any runtime-spawned objects (Projectiles, Enemies, Visual FX) are explicitly deleted when dead or off-screen using `queue_free()`. Never leave orphan nodes in memory.

---

## 🎮 Core Component Blueprints

### Enemies (PathFollow2D)

* Must be a child of a `PathFollow2D` node to automate movement along a `Path2D`.
* **Logic:** Progress is incremented in `_process(delta)` by `speed * delta`.

### Towers (Area2D)

* Uses `Area2D` with a circular collision shape for range detection.
* Must track overlapping bodies via `body_entered` and `body_exited`.
* **Targeting Priority:** Must sort targets dynamically to target the enemy with the highest `progress` (furthest along the track).

---

## 📱 Mobile Optimizations (Keep in Mind)

1. **Garbage Collection Overhead:** Avoid instantiating hundreds of temporary nodes and immediately freeing them. Reuse elements or manage memory carefully.
2. **Draw Calls:** Keep UI elements lightweight. Prefer structural `_draw()` canvas commands or minimal textures over heavily stacked control nodes where performance dips.
3. **Safe Areas:** All critical game UI overlays must be nested inside a `MarginContainer` styled to accommodate camera notches and physical phone corners.

---

## 🤖 Instructions for Your Output

* When writing code snippets, **always provide the entire updated script** or specify exactly where the changes go using clear comments.
* Do not use deprecated Godot 3 syntax (e.g., do not use `set_process(true)`, `yield()`, or old `connect()` syntax). Use Godot 4 standard practices.
* Think through architectural scale before writing code: prefer component composition over heavy inheritance structures.

```

```

<!-- BEGIN BEADS INTEGRATION v:1 profile:minimal hash:ca08a54f -->
## Beads Issue Tracker

This project uses **bd (beads)** for issue tracking. Run `bd prime` to see full workflow context and commands.

### Quick Reference

```bash
bd ready              # Find available work
bd show <id>          # View issue details
bd update <id> --claim  # Claim work
bd close <id>         # Complete work
```

### Rules

- Use `bd` for ALL task tracking — do NOT use TodoWrite, TaskCreate, or markdown TODO lists
- Run `bd prime` for detailed command reference and session close protocol
- Use `bd remember` for persistent knowledge — do NOT use MEMORY.md files

## Session Completion

**When ending a work session**, you MUST complete ALL steps below. Work is NOT complete until `git push` succeeds.

**MANDATORY WORKFLOW:**

1. **File issues for remaining work** - Create issues for anything that needs follow-up
2. **Run quality gates** (if code changed) - Tests, linters, builds
3. **Update issue status** - Close finished work, update in-progress items
4. **PUSH TO REMOTE** - This is MANDATORY:
   ```bash
   git pull --rebase
   bd dolt push
   git push
   git status  # MUST show "up to date with origin"
   ```
5. **Clean up** - Clear stashes, prune remote branches
6. **Verify** - All changes committed AND pushed
7. **Hand off** - Provide context for next session

**CRITICAL RULES:**
- Work is NOT complete until `git push` succeeds
- NEVER stop before pushing - that leaves work stranded locally
- NEVER say "ready to push when you are" - YOU must push
- If push fails, resolve and retry until it succeeds
<!-- END BEADS INTEGRATION -->
