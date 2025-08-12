f# Tango Game - UV Setup Guide

## Quick Start with UV

This project uses `uv` for fast Python package management and virtual environment handling.

### Prerequisites

1. Install `uv` if you haven't already:
```bash
curl -LsSf https://astral.sh/uv/install.sh | sh
# or
pip install uv
```

### Setup and Run

1. **Initialize the project environment:**
```bash
cd /path/to/tango
uv sync
```

2. **Run the game:**
```bash
# Run the main game
uv run tango

# Or run directly with Python
uv run python tango_game.py
```

3. **Run tests:**
```bash
# Run the test script
uv run tango-test

# Or run directly
uv run python test_tango.py
```

### Development

1. **Install development dependencies:**
```bash
uv sync --extra dev
```

2. **Run code formatting:**
```bash
uv run black .
uv run isort .
```

3. **Run type checking:**
```bash
uv run mypy .
```

4. **Run linting:**
```bash
uv run flake8 .
```

5. **Run tests with coverage:**
```bash
uv run pytest --cov=. --cov-report=html
```

### Project Structure

```
tango/
├── pyproject.toml          # Project configuration
├── README.md              # Main documentation
├── UV_SETUP.md           # This file
├── tango_game.py         # Main game application
├── game_logic.py         # Core game logic
├── puzzle_generator.py   # Puzzle generation
├── test_tango.py        # Test suite
└── leaderboard.json     # Generated leaderboard data
```

### UV Commands Reference

- `uv sync` - Install dependencies and create virtual environment
- `uv add <package>` - Add a new dependency
- `uv remove <package>` - Remove a dependency
- `uv run <command>` - Run command in the project environment
- `uv python install 3.11` - Install a specific Python version
- `uv venv` - Create a virtual environment manually

### Game Features

The Tango game includes:
- 6x6 grid puzzle gameplay
- Rule validation with visual feedback
- Timer and leaderboard system
- Constraint-based puzzle generation
- Persistent save system

Enjoy playing Tango! 🎯
