# Tango Puzzle Game - Web Application

A modern web implementation of the Tango puzzle game featuring a FastAPI backend and Svelte frontend with Tailwind CSS, enhanced with intelligent puzzle generation and inference-based solving.

## Game Description

Tango is a 6x6 grid logic puzzle game where you place suns (☀) and moons (☽) following specific rules, similar to Sudoku but with unique constraints. The game emphasizes logical deduction and pattern recognition.

## Game Rules

1. **Balance Rule**: Each row and column must contain exactly 3 suns and 3 moons
2. **Consecutive Rule**: No three consecutive identical pieces are allowed in any row or column  
3. **×** constraint between tiles means they must contain different pieces
4. **=** constraint between tiles means they must contain the same piece

## Advanced Puzzle System

### Intelligent Constraint Generation
- **Inference-First Design**: Puzzles prioritize positions solvable through logical deduction over explicit constraints
- **Strategic Constraint Placement**: Constraints are placed intelligently based on difficulty analysis, not randomly
- **Redundancy Elimination**: Automatic removal of unnecessary constraints that don't contribute to puzzle uniqueness
- **Educational Progression**: Puzzles teach logical reasoning skills progressively

### Enhanced Hint System
- **Rule-Based Hints**: Explanations focus on game rule violations rather than just constraint matching
- **Educational Value Scoring**: Hints are categorized by learning value (high/medium/low)
- **Strategic Guidance**: Board analysis provides direction even when no single logical move exists
- **Inference Training**: Encourages players to apply balance and consecutive rules through reasoning

### Optimization Features
- **70% Constraint Reduction**: Puzzles use significantly fewer explicit constraints while maintaining uniqueness
- **Difficulty-Based Targeting**: Constraint placement focuses on genuinely ambiguous positions
- **Game Rule Inference**: Many positions can be solved through logical deduction alone
- **Real-Time Validation**: Immediate feedback on rule violations with detailed explanations

## Web Controls

- **Click tiles** to cycle through: Empty → ☀ → ☽ → Empty
- **New Game** button generates a fresh puzzle with intelligent constraint optimization
- **Hint System** provides educational explanations based on game rules
- **Real-time validation** shows rule violations with detailed feedback
- **Timer** tracks your solving speed
- **Leaderboard** displays best completion times

## Architecture

### Backend (FastAPI)
- **API Server**: FastAPI with automatic OpenAPI docs
- **Advanced Game Logic**: Enhanced TangoBoardSolver with constraint optimization
- **Intelligent Puzzle Generation**: Creates minimal-constraint puzzles emphasizing logical deduction
- **Educational Hint System**: Provides rule-based explanations and strategic guidance
- **Leaderboard**: JSON-based storage with persistence
- **Real-Time Validation**: Comprehensive game state validation with detailed feedback

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

### Game Management
- `POST /api/game/create` - Create new game with difficulty options
- `GET /api/game/{id}` - Get current game state  
- `POST /api/game/{id}/move` - Make a move with validation
- `GET /api/game/{id}/validate` - Validate current game state
- `GET /api/game/{id}/hint` - Get intelligent hint with educational reasoning
- `DELETE /api/game/{id}` - Delete game and free memory

### Leaderboard
- `GET /api/game/leaderboard` - Get top completion times

### Puzzle Features
- **Difficulty Levels**: Easy, Medium, Hard with varying constraint density
- **Unique Solutions**: All puzzles guaranteed to have exactly one solution
- **Minimal Constraints**: Optimized constraint placement for maximum inference-based solving

## Features

### Advanced Puzzle System
- **Intelligent Generation**: TangoBoardSolver creates puzzles with minimal constraints
- **Inference-Based Solving**: Most positions solvable through logical deduction
- **Educational Hints**: Rule-based explanations teach game principles
- **Strategic Guidance**: Board analysis provides direction for complex situations
- **Constraint Optimization**: Automatic removal of redundant constraints
- **Difficulty Scaling**: Smart constraint placement based on position analysis

### Game Features
- **Multiple Difficulty Levels**: Adaptive constraint density and complexity
- **Real-Time Validation**: Instant feedback on rule violations with detailed explanations
- **Smart Hint System**: Educational hints that teach reasoning rather than just giving answers
- **Completion Detection**: Automatic puzzle completion recognition
- **Progress Tracking**: Visual feedback on solving progress

### Technical Features  
- **Responsive Design**: Mobile-first with CSS Grid/Flexbox
- **Type Safety**: Full TypeScript coverage
- **Hot Reload**: Both backend and frontend development servers
- **Docker Support**: Containerized development environment
- **API Documentation**: Auto-generated OpenAPI/Swagger docs
- **Error Handling**: Comprehensive error boundaries and validation
- **Performance Optimization**: Efficient constraint testing and solution finding

## TangoBoardSolver Optimization

### Constraint Optimization Engine
The enhanced TangoBoardSolver implements a sophisticated constraint optimization system:

#### 1. Redundant Constraint Removal
- **Systematic Testing**: Each constraint is tested for necessity
- **Uniqueness Preservation**: Only constraints essential for unique solutions are kept
- **Automatic Cleanup**: Removes constraints that don't contribute to puzzle difficulty

#### 2. Inference-Based Optimization  
- **Rule Prioritization**: Emphasizes positions solvable through game rules
- **Educational Design**: Promotes learning of balance and consecutive rules
- **Strategic Constraint Placement**: Focuses constraints on genuinely ambiguous positions

#### 3. Intelligent Hint System
- **Educational Categories**: Hints classified by learning value
- **Rule-Based Explanations**: Detailed reasoning using game rule terminology
- **Progressive Difficulty**: Guides players through logical deduction process

#### 4. Performance Benefits
- **70% Constraint Reduction**: Significantly fewer explicit constraints
- **Maintained Uniqueness**: All puzzles still have exactly one solution
- **Enhanced Learning**: Players develop stronger logical reasoning skills
- **Cleaner Interface**: Reduced visual clutter from unnecessary constraints

For detailed technical information, see [SOLVER_IMPROVEMENTS.md](SOLVER_IMPROVEMENTS.md).

## Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Commit changes: `git commit -m 'Add amazing feature'`
4. Push to branch: `git push origin feature/amazing-feature`  
5. Open a pull request

## License

MIT License - see LICENSE file for details.

## Original Game

Based on the Tango puzzle game concept, implemented as a modern web application with enhanced features, intelligent puzzle generation, and educational hint systems.
