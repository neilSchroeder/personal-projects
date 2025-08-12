# Tango Puzzle Game - Web Application

A modern web implementation of the Tango puzzle game featuring a FastAPI backend and Svelte frontend with Tailwind CSS.

## Game Description

Tango is a 6x6 grid logic puzzle game where you place suns (☀) and moons (☽) following specific rules, similar to Sudoku but with unique constraints.

## Game Rules

1. Each row and column must contain exactly 3 suns and 3 moons
2. No three consecutive identical pieces are allowed in any row or column  
3. **×** constraint between tiles means they must contain different pieces
4. **=** constraint between tiles means they must contain the same piece

## Web Controls

- **Click tiles** to cycle through: Empty → ☀ → ☽ → Empty
- **New Game** button generates a fresh puzzle
- **Real-time validation** shows rule violations
- **Timer** tracks your solving speed
- **Leaderboard** displays best completion times

## Architecture

### Backend (FastAPI)
- **API Server**: FastAPI with automatic OpenAPI docs
- **Game Logic**: Migrated from original Python game logic
- **Puzzle Generation**: Creates valid, solvable puzzles with constraints
- **Leaderboard**: JSON-based storage with persistence
- **Validation**: Real-time game state validation

### Frontend (SvelteKit + Tailwind)
- **Modern UI**: Responsive design with Tailwind CSS
- **State Management**: Svelte 5 runes for reactive game state
- **Real-time Updates**: Immediate feedback on moves and validation
- **Mobile Friendly**: Responsive layout for all devices
- **Type Safety**: Full TypeScript integration

## Quick Start

### Option 1: Docker Development (Recommended)

```bash
# Clone and navigate to project
cd tango

# Start both backend and frontend with hot reload
docker-compose up --build

# Access the game
open http://localhost:5173
```

### Option 2: Local Development

#### Backend Setup
```bash
cd backend
pip install -r requirements.txt
uvicorn app.main:app --reload --host 0.0.0.0 --port 8000
```

#### Frontend Setup  
```bash
cd frontend
npm install
npm run dev
```

## Development Workflow

### Backend Development
- **API Docs**: http://localhost:8000/docs (Swagger UI)
- **Health Check**: http://localhost:8000/health
- **Hot Reload**: Automatic restart on file changes
- **Testing**: `pytest` for API and game logic tests

### Frontend Development  
- **Dev Server**: http://localhost:5173 (Vite with HMR)
- **Build**: `npm run build` for production
- **Type Check**: `npm run check` for TypeScript validation
- **Testing**: `npm test` for component and integration tests

## Project Structure

```
tango/
├── backend/                 # FastAPI backend
│   ├── app/
│   │   ├── main.py         # FastAPI app and CORS
│   │   ├── models/         # Pydantic data models  
│   │   ├── services/       # Game business logic
│   │   └── api/routes/     # REST API endpoints
│   ├── original_src/       # Migrated game logic
│   ├── requirements.txt    # Python dependencies
│   └── Dockerfile         # Backend container
├── frontend/               # SvelteKit frontend
│   ├── src/
│   │   ├── lib/
│   │   │   ├── components/ # Svelte components
│   │   │   ├── stores/     # Game state management
│   │   │   └── api/        # API client & types
│   │   └── routes/         # Page routes
│   ├── package.json       # Node dependencies
│   └── Dockerfile        # Frontend container  
├── docker-compose.yml     # Development environment
└── README.md             # This file
```

## API Endpoints

- `POST /api/game/create` - Create new game
- `GET /api/game/{id}` - Get game state  
- `POST /api/game/{id}/move` - Make a move
- `GET /api/game/{id}/validate` - Validate game state
- `GET /api/game/leaderboard` - Get leaderboard
- `DELETE /api/game/{id}` - Delete game

## Features

### Game Features
- **Puzzle Generation**: Algorithmically generated solvable puzzles
- **Multiple Difficulty**: Constraint density varies puzzle difficulty  
- **Instant Validation**: Real-time feedback on rule violations
- **Smart Hints**: Visual indicators for constraint violations
- **Completion Detection**: Automatic puzzle completion recognition

### Technical Features  
- **Responsive Design**: Mobile-first with CSS Grid/Flexbox
- **Type Safety**: Full TypeScript coverage
- **Hot Reload**: Both backend and frontend development servers
- **Docker Support**: Containerized development environment
- **API Documentation**: Auto-generated OpenAPI/Swagger docs
- **Error Handling**: Comprehensive error boundaries and validation

## Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Commit changes: `git commit -m 'Add amazing feature'`
4. Push to branch: `git push origin feature/amazing-feature`  
5. Open a pull request

## License

MIT License - see LICENSE file for details.

## Original Game

Based on the Tango puzzle game concept, implemented as a modern web application with enhanced features and responsive design.
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
