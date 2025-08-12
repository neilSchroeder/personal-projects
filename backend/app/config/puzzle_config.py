"""
Puzzle generation configuration settings.
Adjust these values to control puzzle difficulty and generation parameters.
"""

from dataclasses import dataclass
from typing import Dict, Optional
import os


@dataclass
class DifficultyConfig:
    """Configuration for a specific difficulty level."""

    name: str
    starting_pieces_min: int
    starting_pieces_max: int
    constraint_probability: float
    description: str


class PuzzleConfig:
    """Main configuration class for puzzle generation."""

    # Define difficulty levels
    DIFFICULTIES: Dict[str, DifficultyConfig] = {
        "easy": DifficultyConfig(
            name="Easy",
            starting_pieces_min=6,
            starting_pieces_max=10,
            constraint_probability=0.25,
            description="More starting pieces and constraints for easier solving",
        ),
        "medium": DifficultyConfig(
            name="Medium",
            starting_pieces_min=3,
            starting_pieces_max=6,
            constraint_probability=0.15,
            description="Balanced difficulty with moderate hints",
        ),
        "hard": DifficultyConfig(
            name="Hard",
            starting_pieces_min=1,
            starting_pieces_max=3,
            constraint_probability=0.10,
            description="Fewer starting pieces and constraints for challenge",
        ),
        "expert": DifficultyConfig(
            name="Expert",
            starting_pieces_min=0,
            starting_pieces_max=2,
            constraint_probability=0.05,
            description="Minimal hints for maximum challenge",
        ),
    }

    # Default difficulty (can be overridden by environment variable)
    DEFAULT_DIFFICULTY = os.getenv("TANGO_DIFFICULTY", "medium")

    # Board configuration
    BOARD_SIZE = 6
    PIECES_PER_ROW_COL = 3  # 3 suns and 3 moons per row/column

    # Generation settings
    MAX_PLACEMENT_ATTEMPTS = 20  # Max attempts to place a starting piece
    MAX_GENERATION_ATTEMPTS = 5  # Max attempts to generate a valid puzzle

    @classmethod
    def get_difficulty(cls, difficulty_name: Optional[str] = None) -> DifficultyConfig:
        """Get difficulty configuration by name."""
        if difficulty_name is None:
            difficulty_name = cls.DEFAULT_DIFFICULTY

        if difficulty_name not in cls.DIFFICULTIES:
            print(
                f"Unknown difficulty '{difficulty_name}', using default '{cls.DEFAULT_DIFFICULTY}'"
            )
            difficulty_name = cls.DEFAULT_DIFFICULTY

        return cls.DIFFICULTIES[difficulty_name]

    @classmethod
    def get_available_difficulties(cls) -> Dict[str, str]:
        """Get all available difficulties with their descriptions."""
        return {key: config.description for key, config in cls.DIFFICULTIES.items()}

    @classmethod
    def set_custom_difficulty(
        cls,
        name: str,
        starting_pieces_min: int,
        starting_pieces_max: int,
        constraint_probability: float,
        description: str = "Custom difficulty",
    ) -> None:
        """Add or update a custom difficulty level."""
        cls.DIFFICULTIES[name] = DifficultyConfig(
            name=name.title(),
            starting_pieces_min=starting_pieces_min,
            starting_pieces_max=starting_pieces_max,
            constraint_probability=constraint_probability,
            description=description,
        )


# Convenience function for easy access
def get_puzzle_config(difficulty: Optional[str] = None) -> DifficultyConfig:
    """Get puzzle configuration for the specified difficulty."""
    return PuzzleConfig.get_difficulty(difficulty)
