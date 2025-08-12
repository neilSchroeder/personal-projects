"""
Game API routes for the Tango game.
"""

from fastapi import APIRouter, HTTPException, status, Query
from typing import Optional

from ...models.game_models import (
    CreateGameResponse,
    GameState,
    MoveRequest,
    MoveResponse,
    LeaderboardResponse,
    GameValidationResult,
)
from ...services.game_service import GameService
from ...config.puzzle_config import PuzzleConfig

router = APIRouter(prefix="/api/game", tags=["game"])

# Global game service instance
game_service = GameService()


@router.post("/create", response_model=CreateGameResponse)
async def create_game(
    difficulty: Optional[str] = Query(
        None,
        description="Difficulty level: easy, medium, hard, or expert",
        example="medium",
    )
):
    """Create a new Tango game with optional difficulty setting."""
    try:
        # Validate difficulty if provided
        if difficulty and difficulty not in PuzzleConfig.DIFFICULTIES:
            available = list(PuzzleConfig.DIFFICULTIES.keys())
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=f"Invalid difficulty '{difficulty}'. Available: {available}",
            )

        game_state = game_service.create_game(difficulty)
        return CreateGameResponse(game_id=game_state.game_id, game_state=game_state)
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to create game: {str(e)}",
        )


@router.get("/difficulties")
async def get_difficulties():
    """Get all available difficulty levels and their descriptions."""
    return {
        "difficulties": PuzzleConfig.get_available_difficulties(),
        "default": PuzzleConfig.DEFAULT_DIFFICULTY,
    }


@router.get("/leaderboard", response_model=LeaderboardResponse)
async def get_leaderboard():
    """Get the current leaderboard."""
    try:
        entries = game_service.get_leaderboard()
        return LeaderboardResponse(entries=entries)
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to load leaderboard: {str(e)}",
        )


@router.get("/{game_id}", response_model=GameState)
async def get_game_state(game_id: str):
    """Get the current state of a game."""
    game_state = game_service.get_game(game_id)
    if not game_state:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND, detail="Game not found"
        )
    return game_state


@router.post("/{game_id}/move", response_model=MoveResponse)
async def make_move(game_id: str, move: MoveRequest):
    """Make a move in the game."""
    success, message, game_state, validation_errors = game_service.make_move(
        game_id, move.row, move.col, move.piece_type
    )

    if not success:
        return MoveResponse(success=False, message=message, game_state=game_state)

    return MoveResponse(
        success=True,
        message=message,
        game_state=game_state,
        validation_errors=validation_errors if validation_errors else None,
    )


@router.get("/{game_id}/validate", response_model=GameValidationResult)
async def validate_game(game_id: str):
    """Validate the current state of a game."""
    if not game_service.get_game(game_id):
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND, detail="Game not found"
        )

    return game_service.validate_game(game_id)


@router.get("/{game_id}/hint")
async def get_hint(game_id: str):
    """Get a hint for the next logical move in the game."""
    if not game_service.get_game(game_id):
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND, detail="Game not found"
        )

    hint = game_service.get_hint(game_id)
    return hint


@router.delete("/{game_id}")
async def delete_game(game_id: str):
    """Delete a game to free memory."""
    success = game_service.delete_game(game_id)
    if not success:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND, detail="Game not found"
        )

    return {"message": "Game deleted successfully"}
