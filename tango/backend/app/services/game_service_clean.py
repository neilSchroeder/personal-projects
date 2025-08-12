"""
Game service for handling Tango game logic and state management.
"""

import json
import uuid
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Tuple

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
            moves_count=0,
        )

        # Store the game
        self.game_states[game_id] = game_state
        self.active_games[game_id] = SimpleGameLogic(game_state)

        return game_state

    def get_game(self, game_id: str) -> Optional[GameState]:
        """Get the current state of a game."""
        return self.game_states.get(game_id)

    def make_move(
        self, game_id: str, row: int, col: int, piece_type: PieceType
    ) -> Tuple[bool, str, Optional[GameState]]:
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
                is_valid=False, errors=["Game not found"], is_complete=False
            )

        game_state = self.game_states[game_id]
        errors = []

        # Simple validation for now
        # TODO: Implement full constraint validation

        return GameValidationResult(
            is_valid=len(errors) == 0, errors=errors, is_complete=game_state.is_complete
        )

    def get_leaderboard(self) -> List[LeaderboardEntry]:
        """Get the current leaderboard."""
        try:
            if self.leaderboard_file.exists():
                with open(self.leaderboard_file, "r") as f:
                    data = json.load(f)
                    return [
                        LeaderboardEntry(**entry) for entry in data.get("entries", [])
                    ]
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
            formatted_time=f"{duration:.1f}s",
        )

        leaderboard = self.get_leaderboard()
        leaderboard.append(entry)

        # Sort by completion time and keep top 10
        leaderboard.sort(key=lambda x: x.time)
        leaderboard = leaderboard[:10]

        # Save back to file
        try:
            data = {"entries": [entry.dict() for entry in leaderboard]}
            with open(self.leaderboard_file, "w") as f:
                json.dump(data, f, indent=2, default=str)
        except Exception as e:
            print(f"Error saving leaderboard: {e}")


class SimpleGameLogic:
    """Simplified game logic for initial implementation."""

    def __init__(self, game_state: GameState):
        self.game_state = game_state
