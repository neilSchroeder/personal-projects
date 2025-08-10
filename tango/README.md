# Tango Game

A Python implementation of the Tango puzzle game from LinkedIn.

## Game Description

Tango is a 6x6 grid puzzle game where you place suns and moons following specific rules.

## Game Rules

1. Each row and column must contain exactly 3 suns and 3 moons
2. No three consecutive suns or moons are allowed in any row or column
3. An 'X' constraint between two tiles means they must contain different pieces
4. An '=' constraint between two tiles means they must contain the same piece

## Controls

- **Single click**: Place a sun (☀)
- **Double click**: Change to moon (☽)
- **Triple click**: Clear the tile
- **New Game**: Generate a new puzzle
- **Exit**: Save leaderboard and quit

## Installation & Running

### Requirements

- Python 3.12+
- tkinter (usually included with Python)
- No external dependencies required!

### Using UV (Recommended)

```bash
# Install dependencies and setup environment
uv sync

# Run the game
uv run tango

# Alternative: Run directly
uv run tango-game
```

### Using Python directly

```bash
# Run the game
python -m src.tango_game

# Or use the shell script (Linux)
./run_tango.sh
```

### Troubleshooting

**Linux X11 Threading Issues:**
If you see errors like "XInitThreads has not been called", try:

1. Use the shell script: `./run_tango.sh`
2. Set environment variable: `export PYTHONUNBUFFERED=1` before running
3. Use the launcher: `uv run tango` (uses the fixed launcher)

## Features

- Random puzzle generation with valid solutions
- Real-time rule validation with visual feedback
- Timer tracking solve time
- Leaderboard with best times
- Persistent leaderboard storage

## Files

- `src/tango_game.py` - Main game implementation
- `src/game_logic.py` - Core game logic and validation
- `src/puzzle_generator.py` - Puzzle generation algorithms
- `src/launcher.py` - X11-safe launcher for Linux
- `run_tango.sh` - Shell script launcher
- `pyproject.toml` - Project configuration for UV
- `leaderboard.json` - Persistent leaderboard storage (auto-created)

## Dependencies

This project uses **only Python standard library modules**:
- `tkinter` - GUI framework
- `json` - Leaderboard persistence
- `time` / `datetime` - Timer functionality
- `random` - Puzzle generation
- `typing` - Type hints
- `enum` - Game state enums

No external packages required!
