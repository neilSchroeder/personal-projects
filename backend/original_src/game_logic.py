"""
Game logic and validation for Tango puzzle game.
"""

from enum import Enum
from typing import List, Optional, Tuple
from .config import BOARD_SIZE, MAX_PIECES_PER_ROW_COL, CONSECUTIVE_LIMIT


class PieceType(Enum):
    """Types of pieces that can be placed on the board."""

    EMPTY = 0
    SUN = 1
    MOON = 2


class ConstraintType(Enum):
    """Types of constraints between tiles."""

    NONE = 0
    SAME = 1  # '=' constraint - tiles must be the same
    DIFFERENT = 2  # 'X' constraint - tiles must be different


class GameLogic:
    """Core game logic for Tango puzzle."""

    def __init__(self, size: int = BOARD_SIZE):
        self.size = size
        self.board: List[List[PieceType]] = [
            [PieceType.EMPTY for _ in range(size)] for _ in range(size)
        ]
        self.h_constraints: List[List[ConstraintType]] = [
            [ConstraintType.NONE for _ in range(size - 1)] for _ in range(size)
        ]
        self.v_constraints: List[List[ConstraintType]] = [
            [ConstraintType.NONE for _ in range(size)] for _ in range(size - 1)
        ]
        self.locked_tiles: List[List[bool]] = [
            [False for _ in range(size)] for _ in range(size)
        ]

    def reset_board(self):
        """Reset the board to empty state."""
        self.board = [
            [PieceType.EMPTY for _ in range(self.size)] for _ in range(self.size)
        ]
        self.locked_tiles = [
            [False for _ in range(self.size)] for _ in range(self.size)
        ]

    def place_piece(self, row: int, col: int, piece: PieceType) -> bool:
        """Place a piece on the board."""
        if 0 <= row < self.size and 0 <= col < self.size:
            self.board[row][col] = piece
            return True
        return False

    def can_place_piece(self, row: int, col: int) -> bool:
        """Check if a piece can be placed at the given position (not locked)."""
        if 0 <= row < self.size and 0 <= col < self.size:
            return not self.locked_tiles[row][col]
        return False

    def lock_tile(self, row: int, col: int):
        """Lock a tile so it cannot be changed by the player."""
        if 0 <= row < self.size and 0 <= col < self.size:
            self.locked_tiles[row][col] = True

    def is_tile_locked(self, row: int, col: int) -> bool:
        """Check if a tile is locked."""
        if 0 <= row < self.size and 0 <= col < self.size:
            return self.locked_tiles[row][col]
        return False

    def get_piece(self, row: int, col: int) -> PieceType:
        """Get the piece at the given position."""
        if 0 <= row < self.size and 0 <= col < self.size:
            return self.board[row][col]
        return PieceType.EMPTY

    def set_horizontal_constraint(self, row: int, col: int, constraint: ConstraintType):
        """Set horizontal constraint between (row, col) and (row, col+1)."""
        if 0 <= row < self.size and 0 <= col < self.size - 1:
            self.h_constraints[row][col] = constraint

    def set_vertical_constraint(self, row: int, col: int, constraint: ConstraintType):
        """Set vertical constraint between (row, col) and (row+1, col)."""
        if 0 <= row < self.size - 1 and 0 <= col < self.size:
            self.v_constraints[row][col] = constraint

    def get_horizontal_constraint(self, row: int, col: int) -> ConstraintType:
        """Get horizontal constraint between (row, col) and (row, col+1)."""
        if 0 <= row < self.size and 0 <= col < self.size - 1:
            return self.h_constraints[row][col]
        return ConstraintType.NONE

    def get_vertical_constraint(self, row: int, col: int) -> ConstraintType:
        """Get vertical constraint between (row, col) and (row+1, col)."""
        if 0 <= row < self.size - 1 and 0 <= col < self.size:
            return self.v_constraints[row][col]
        return ConstraintType.NONE

    def count_pieces_in_row(self, row: int) -> Tuple[int, int]:
        """Count suns and moons in the given row."""
        suns = sum(1 for piece in self.board[row] if piece == PieceType.SUN)
        moons = sum(1 for piece in self.board[row] if piece == PieceType.MOON)
        return suns, moons

    def count_pieces_in_column(self, col: int) -> Tuple[int, int]:
        """Count suns and moons in the given column."""
        suns = sum(
            1 for row in range(self.size) if self.board[row][col] == PieceType.SUN
        )
        moons = sum(
            1 for row in range(self.size) if self.board[row][col] == PieceType.MOON
        )
        return suns, moons

    def has_three_consecutive_in_row(self, row: int) -> bool:
        """Check if there are three consecutive pieces of the same type in a row."""
        for col in range(self.size - CONSECUTIVE_LIMIT + 1):
            if (
                self.board[row][col] != PieceType.EMPTY
                and self.board[row][col]
                == self.board[row][col + 1]
                == self.board[row][col + 2]
            ):
                return True
        return False

    def has_three_consecutive_in_column(self, col: int) -> bool:
        """Check if there are three consecutive pieces of the same type in a column."""
        for row in range(self.size - CONSECUTIVE_LIMIT + 1):
            if (
                self.board[row][col] != PieceType.EMPTY
                and self.board[row][col]
                == self.board[row + 1][col]
                == self.board[row + 2][col]
            ):
                return True
        return False

    def check_constraint_violations(self) -> List[Tuple[int, int, str]]:
        """Check for constraint violations and return list of violations."""
        violations = []

        # Check horizontal constraints
        for row in range(self.size):
            for col in range(self.size - 1):
                constraint = self.h_constraints[row][col]
                if constraint != ConstraintType.NONE:
                    piece1 = self.board[row][col]
                    piece2 = self.board[row][col + 1]

                    if piece1 != PieceType.EMPTY and piece2 != PieceType.EMPTY:
                        if constraint == ConstraintType.SAME and piece1 != piece2:
                            violations.append((row, col, "horizontal_same"))
                        elif (
                            constraint == ConstraintType.DIFFERENT and piece1 == piece2
                        ):
                            violations.append((row, col, "horizontal_different"))

        # Check vertical constraints
        for row in range(self.size - 1):
            for col in range(self.size):
                constraint = self.v_constraints[row][col]
                if constraint != ConstraintType.NONE:
                    piece1 = self.board[row][col]
                    piece2 = self.board[row + 1][col]

                    if piece1 != PieceType.EMPTY and piece2 != PieceType.EMPTY:
                        if constraint == ConstraintType.SAME and piece1 != piece2:
                            violations.append((row, col, "vertical_same"))
                        elif (
                            constraint == ConstraintType.DIFFERENT and piece1 == piece2
                        ):
                            violations.append((row, col, "vertical_different"))

        return violations

    def is_valid_state(self) -> bool:
        """Check if the current board state is valid."""
        # Check row/column piece counts
        for i in range(self.size):
            suns_row, moons_row = self.count_pieces_in_row(i)
            suns_col, moons_col = self.count_pieces_in_column(i)

            # Each row/column should have at most MAX_PIECES_PER_ROW_COL suns and moons
            if (
                suns_row > MAX_PIECES_PER_ROW_COL
                or moons_row > MAX_PIECES_PER_ROW_COL
                or suns_col > MAX_PIECES_PER_ROW_COL
                or moons_col > MAX_PIECES_PER_ROW_COL
            ):
                return False

            # Check for three consecutive pieces
            if self.has_three_consecutive_in_row(
                i
            ) or self.has_three_consecutive_in_column(i):
                return False

        # Check constraint violations
        if self.check_constraint_violations():
            return False

        return True

    def is_complete(self) -> bool:
        """Check if the puzzle is complete and correctly solved."""
        # Check that all cells are filled
        for row in range(self.size):
            for col in range(self.size):
                if self.board[row][col] == PieceType.EMPTY:
                    return False

        # Check that each row and column has exactly MAX_PIECES_PER_ROW_COL suns and moons
        for i in range(self.size):
            suns_row, moons_row = self.count_pieces_in_row(i)
            suns_col, moons_col = self.count_pieces_in_column(i)

            if (
                suns_row != MAX_PIECES_PER_ROW_COL
                or moons_row != MAX_PIECES_PER_ROW_COL
                or suns_col != MAX_PIECES_PER_ROW_COL
                or moons_col != MAX_PIECES_PER_ROW_COL
            ):
                return False

        return self.is_valid_state()

    def get_hint(self) -> Optional[Tuple[int, int, PieceType]]:
        """Get a hint for the next move."""
        # Find cells that can only have one valid piece type
        for row in range(self.size):
            for col in range(self.size):
                if self.board[row][col] == PieceType.EMPTY:
                    valid_pieces = []

                    for piece_type in [PieceType.SUN, PieceType.MOON]:
                        # Temporarily place the piece
                        self.board[row][col] = piece_type
                        if self.is_valid_state():
                            valid_pieces.append(piece_type)
                        # Remove the piece
                        self.board[row][col] = PieceType.EMPTY

                    # If only one piece type is valid, return it as a hint
                    if len(valid_pieces) == 1:
                        return (row, col, valid_pieces[0])

        return None
