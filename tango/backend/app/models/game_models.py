"""
Pydantic models for the Tango game API.
"""

from enum import Enum
from typing import List, Optional
from datetime import datetime
from pydantic import BaseModel, Field


class PieceType(str, Enum):
    """Types of pieces that can be placed on the board."""

    EMPTY = "empty"
    SUN = "sun"
    MOON = "moon"


class ConstraintType(str, Enum):
    """Types of constraints between tiles."""

    NONE = "none"
    SAME = "same"  # '=' constraint - tiles must be the same
    DIFFERENT = "different"  # 'X' constraint - tiles must be different


class GameState(BaseModel):
    """Current state of a game."""

    game_id: str
    board: List[List[PieceType]]
    h_constraints: List[List[ConstraintType]]
    v_constraints: List[List[ConstraintType]]
    locked_tiles: List[List[bool]]
    is_complete: bool = False
    start_time: datetime
    completion_time: Optional[datetime] = None
    moves_count: int = 0


class MoveRequest(BaseModel):
    """Request to make a move in the game."""

    row: int = Field(..., ge=0, le=5, description="Row index (0-5)")
    col: int = Field(..., ge=0, le=5, description="Column index (0-5)")
    piece_type: PieceType


class MoveResponse(BaseModel):
    """Response after making a move."""

    success: bool
    message: str
    game_state: Optional[GameState] = None
    validation_errors: Optional[List[str]] = None


class CreateGameResponse(BaseModel):
    """Response when creating a new game."""

    game_id: str
    game_state: GameState


class LeaderboardEntry(BaseModel):
    """Single entry in the leaderboard."""

    time: float
    date: datetime
    formatted_time: str


class LeaderboardResponse(BaseModel):
    """Leaderboard response."""

    entries: List[LeaderboardEntry]


class GameValidationResult(BaseModel):
    """Result of game validation."""

    is_valid: bool
    errors: List[str]
    is_complete: bool


class ErrorResponse(BaseModel):
    """Standard error response."""

    error: str
    detail: Optional[str] = None
