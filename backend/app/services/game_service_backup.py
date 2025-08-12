"""
Game service for handling Tango game logic and state management.
"""

import json
import uuid
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Tuple

# Import original game logic
import sys
import os

# For now, let's create a simplified version that doesn't depend on the original game logic
# This allows the web app to run while we work on the integration

from ..models.game_models import (
    GameState,
    PieceType,
    ConstraintType,
    LeaderboardEntry,
    GameValidationResult,
)


class GameService:
    """Service for managing Tango game instances and logic."""

    def __init__(self):
        self.active_games: Dict[str, "SimpleGameLogic"] = {}
        self.game_states: Dict[str, GameState] = {}
        self.leaderboard_file = Path("leaderboard.json")

    def create_game(self) -> GameState:
        """Create a new game with a simple puzzle."""
        game_id = str(uuid.uuid4())
        
        # Create a simple 6x6 board with some predefined constraints
        board = [[PieceType.EMPTY for _ in range(6)] for _ in range(6)]
        
        # Add some sample constraints
        h_constraints = [[ConstraintType.NONE for _ in range(5)] for _ in range(6)]
        v_constraints = [[ConstraintType.NONE for _ in range(6)] for _ in range(5)]
        
        # Add a few sample constraints for testing
        h_constraints[0][0] = ConstraintType.SAME
        h_constraints[1][1] = ConstraintType.DIFFERENT
        v_constraints[0][0] = ConstraintType.SAME
        v_constraints[1][2] = ConstraintType.DIFFERENT
        
        locked_tiles = [[False for _ in range(6)] for _ in range(6)]
        
        # Pre-place a few pieces for initial state
        board[0][0] = PieceType.SUN
        board[0][1] = PieceType.SUN  # satisfies same constraint
        locked_tiles[0][0] = True
        locked_tiles[0][1] = True
        
        game_state = GameState(
            game_id=game_id,
            board=board,
            h_constraints=h_constraints,
            v_constraints=v_constraints,
            locked_tiles=locked_tiles,
            is_complete=False,
            start_time=datetime.now(),
            moves_count=0
        )
        
        # Store the game
        self.game_states[game_id] = game_state
        self.active_games[game_id] = SimpleGameLogic(game_state)
        
        return game_state

    def get_game(self, game_id: str) -> Optional[GameState]:
        """Get the current state of a game."""
        return self.game_states.get(game_id)

    def make_move(self, game_id: str, row: int, col: int, piece_type: PieceType) -> Tuple[bool, str, Optional[GameState]]:
        """Make a move in the specified game."""
        if game_id not in self.game_states:
            return False, "Game not found", None
            
        game_state = self.game_states[game_id]
        game_logic = self.active_games[game_id]
        
        # Check if tile is locked
        if game_state.locked_tiles[row][col]:
            return False, "Cannot modify locked tile", None
            
        # Check bounds
        if not (0 <= row < 6 and 0 <= col < 6):
            return False, "Invalid position", None
            
        # Update the board
        game_state.board[row][col] = piece_type
        game_state.moves_count += 1
        
        # Simple validation - check if game is complete
        game_state.is_complete = self._check_if_complete(game_state)
        
        if game_state.is_complete:
            game_state.completion_time = datetime.now()
            # Add to leaderboard
            self._add_to_leaderboard(game_state)
        
        self.game_states[game_id] = game_state
        
        return True, "Move successful", game_state

    def _check_if_complete(self, game_state: GameState) -> bool:
        """Simple check if the game is complete (all tiles filled)."""
        for row in game_state.board:
            for cell in row:
                if cell == PieceType.EMPTY:
                    return False
        return True

    def validate_game(self, game_id: str) -> GameValidationResult:
        """Validate the current state of a game."""
        if game_id not in self.game_states:
            return GameValidationResult(
                is_valid=False,
                errors=["Game not found"],
                is_complete=False
            )
            
        game_state = self.game_states[game_id]
        errors = []
        
        # Simple validation for now
        # TODO: Implement full constraint validation
        
        return GameValidationResult(
            is_valid=len(errors) == 0,
            errors=errors,
            is_complete=game_state.is_complete
        )

    def get_leaderboard(self) -> List[LeaderboardEntry]:
        """Get the current leaderboard."""
        try:
            if self.leaderboard_file.exists():
                with open(self.leaderboard_file, 'r') as f:
                    data = json.load(f)
                    return [LeaderboardEntry(**entry) for entry in data.get('entries', [])]
            return []
        except Exception as e:
            print(f"Error loading leaderboard: {e}")
            return []

    def _add_to_leaderboard(self, game_state: GameState):
        """Add a completed game to the leaderboard."""
        if not game_state.completion_time:
            return
            
        duration = (game_state.completion_time - game_state.start_time).total_seconds()
        
        entry = LeaderboardEntry(
            time=duration,
            date=game_state.completion_time,
            formatted_time=f"{duration:.1f}s"
        )
        
        leaderboard = self.get_leaderboard()
        leaderboard.append(entry)
        
        # Sort by completion time and keep top 10
        leaderboard.sort(key=lambda x: x.time)
        leaderboard = leaderboard[:10]
        
        # Save back to file
        try:
            data = {"entries": [entry.dict() for entry in leaderboard]}
            with open(self.leaderboard_file, 'w') as f:
                json.dump(data, f, indent=2, default=str)
        except Exception as e:
            print(f"Error saving leaderboard: {e}")


class SimpleGameLogic:
    """Simplified game logic for initial implementation."""
    
    def __init__(self, game_state: GameState):
        self.game_state = game_state

import json
import uuid
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Tuple

# Import original game logic
import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__), "../../original_src"))

try:
    from game_logic import (
        GameLogic,
        PieceType as OriginalPieceType,
        ConstraintType as OriginalConstraintType,
    )
    from puzzle_generator import PuzzleGenerator
    from config import BOARD_SIZE, MAX_LEADERBOARD_ENTRIES
    IMPORTS_AVAILABLE = True
except ImportError as e:
    print(f"Warning: Could not import original game modules: {e}")
    # Fallback values for development
    BOARD_SIZE = 6
    MAX_LEADERBOARD_ENTRIES = 10
    IMPORTS_AVAILABLE = False
    
    # Create dummy classes for fallback
    class OriginalPieceType:
        EMPTY = 0
        SUN = 1
        MOON = 2
    
    class OriginalConstraintType:
        NONE = 0
        SAME = 1
        DIFFERENT = 2
        
    class GameLogic:
        def __init__(self, board_size):
            self.board_size = board_size
            
    class PuzzleGenerator:
        def __init__(self, board_size):
            self.board_size = board_size

from ..models.game_models import (
    GameState,
    PieceType,
    ConstraintType,
    LeaderboardEntry,
    GameValidationResult,
)


class GameService:
    """Service for managing Tango game instances and logic."""

    def __init__(self):
        self.active_games: Dict[str, GameLogic] = {}
        self.game_states: Dict[str, GameState] = {}
        self.puzzle_generator = PuzzleGenerator(BOARD_SIZE)
        self.leaderboard_file = Path("leaderboard.json")

    def _convert_piece_type_to_api(self, piece: OriginalPieceType) -> PieceType:
        """Convert original PieceType to API PieceType."""
        mapping = {
            OriginalPieceType.EMPTY: PieceType.EMPTY,
            OriginalPieceType.SUN: PieceType.SUN,
            OriginalPieceType.MOON: PieceType.MOON,
        }
        return mapping[piece]

    def _convert_piece_type_from_api(self, piece: PieceType) -> OriginalPieceType:
        """Convert API PieceType to original PieceType."""
        mapping = {
            PieceType.EMPTY: OriginalPieceType.EMPTY,
            PieceType.SUN: OriginalPieceType.SUN,
            PieceType.MOON: OriginalPieceType.MOON,
        }
        return mapping[piece]

    def _convert_constraint_type_to_api(
        self, constraint: OriginalConstraintType
    ) -> ConstraintType:
        """Convert original ConstraintType to API ConstraintType."""
        mapping = {
            OriginalConstraintType.NONE: ConstraintType.NONE,
            OriginalConstraintType.SAME: ConstraintType.SAME,
            OriginalConstraintType.DIFFERENT: ConstraintType.DIFFERENT,
        }
        return mapping[constraint]

    def _game_logic_to_state(
        self, game_id: str, game_logic: GameLogic, start_time: datetime
    ) -> GameState:
        """Convert GameLogic instance to GameState."""
        # Convert board
        board = []
        for row in game_logic.board:
            board.append([self._convert_piece_type_to_api(piece) for piece in row])

        # Convert horizontal constraints
        h_constraints = []
        for row in game_logic.h_constraints:
            h_constraints.append(
                [self._convert_constraint_type_to_api(constraint) for constraint in row]
            )

        # Convert vertical constraints
        v_constraints = []
        for row in game_logic.v_constraints:
            v_constraints.append(
                [self._convert_constraint_type_to_api(constraint) for constraint in row]
            )

        # Check if game is complete
        is_complete = game_logic.is_solution_complete()
        completion_time = datetime.now() if is_complete else None

        return GameState(
            game_id=game_id,
            board=board,
            h_constraints=h_constraints,
            v_constraints=v_constraints,
            locked_tiles=game_logic.locked_tiles,
            is_complete=is_complete,
            start_time=start_time,
            completion_time=completion_time,
            moves_count=0,  # TODO: Track moves count
        )

    def create_game(self) -> GameState:
        """Create a new game instance."""
        game_id = str(uuid.uuid4())

        # Generate new puzzle
        game_logic = self.puzzle_generator.generate_puzzle()
        if not game_logic:
            # Fallback if generation fails
            game_logic = GameLogic(BOARD_SIZE)

        start_time = datetime.now()

        # Store the game
        self.active_games[game_id] = game_logic

        # Create and store game state
        game_state = self._game_logic_to_state(game_id, game_logic, start_time)
        self.game_states[game_id] = game_state

        return game_state

    def get_game_state(self, game_id: str) -> Optional[GameState]:
        """Get current state of a game."""
        return self.game_states.get(game_id)

    def make_move(
        self, game_id: str, row: int, col: int, piece_type: PieceType
    ) -> Tuple[bool, str, Optional[GameState]]:
        """Make a move in the game."""
        game_logic = self.active_games.get(game_id)
        if not game_logic:
            return False, "Game not found", None

        # Check if tile can be placed
        if not game_logic.can_place_piece(row, col):
            return False, "Cannot place piece on locked tile", None

        # Convert piece type and place
        original_piece = self._convert_piece_type_from_api(piece_type)
        success = game_logic.place_piece(row, col, original_piece)

        if not success:
            return False, "Invalid move", None

        # Update game state
        game_state = self.game_states[game_id]
        start_time = game_state.start_time
        updated_state = self._game_logic_to_state(game_id, game_logic, start_time)
        updated_state.moves_count = game_state.moves_count + 1
        self.game_states[game_id] = updated_state

        # Check if game is complete and update leaderboard
        if updated_state.is_complete and updated_state.completion_time:
            completion_time_seconds = (
                updated_state.completion_time - start_time
            ).total_seconds()
            self._add_to_leaderboard(
                completion_time_seconds, updated_state.completion_time
            )

        return True, "Move successful", updated_state

    def validate_game(self, game_id: str) -> GameValidationResult:
        """Validate current game state."""
        game_logic = self.active_games.get(game_id)
        if not game_logic:
            return GameValidationResult(
                is_valid=False, errors=["Game not found"], is_complete=False
            )

        errors = []

        # Check row/column constraints
        for i in range(BOARD_SIZE):
            row_errors = game_logic.validate_row(i)
            col_errors = game_logic.validate_column(i)
            errors.extend(row_errors)
            errors.extend(col_errors)

        # Check constraint violations
        constraint_errors = game_logic.validate_constraints()
        errors.extend(constraint_errors)

        is_valid = len(errors) == 0
        is_complete = game_logic.is_solution_complete() if is_valid else False

        return GameValidationResult(
            is_valid=is_valid, errors=errors, is_complete=is_complete
        )

    def get_leaderboard(self) -> List[LeaderboardEntry]:
        """Get the current leaderboard."""
        try:
            if self.leaderboard_file.exists():
                with open(self.leaderboard_file, "r") as f:
                    data = json.load(f)
                    return [
                        LeaderboardEntry(
                            time=entry["time"],
                            date=datetime.fromisoformat(entry["date"]),
                            formatted_time=entry["formatted_time"],
                        )
                        for entry in data
                    ]
            return []
        except (json.JSONDecodeError, KeyError, ValueError):
            return []

    def _add_to_leaderboard(self, time_seconds: float, completion_date: datetime):
        """Add a new entry to the leaderboard."""
        # Format time as MM:SS
        minutes = int(time_seconds // 60)
        seconds = int(time_seconds % 60)
        formatted_time = f"{minutes:02d}:{seconds:02d}"

        new_entry = {
            "time": time_seconds,
            "date": completion_date.isoformat(),
            "formatted_time": formatted_time,
        }

        # Load existing leaderboard
        leaderboard = []
        if self.leaderboard_file.exists():
            try:
                with open(self.leaderboard_file, "r") as f:
                    leaderboard = json.load(f)
            except (json.JSONDecodeError, FileNotFoundError):
                leaderboard = []

        # Add new entry and sort by time
        leaderboard.append(new_entry)
        leaderboard.sort(key=lambda x: x["time"])

        # Keep only top entries
        leaderboard = leaderboard[:MAX_LEADERBOARD_ENTRIES]

        # Save back to file
        with open(self.leaderboard_file, "w") as f:
            json.dump(leaderboard, f, indent=2)

    def delete_game(self, game_id: str) -> bool:
        """Delete a game instance to free memory."""
        if game_id in self.active_games:
            del self.active_games[game_id]
            del self.game_states[game_id]
            return True
        return False
