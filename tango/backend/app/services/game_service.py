"""
Game service for handling Tango game logic and state management.
"""

import json
import uuid
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Tuple
import sys
import os

# Add the original_src directory to the path to import original game logic
sys.path.append(os.path.join(os.path.dirname(__file__), "..", "..", "original_src"))

from ..models.game_models import (
    GameState,
    PieceType,
    ConstraintType,
    LeaderboardEntry,
    GameValidationResult,
)
from ..config.puzzle_config import get_puzzle_config, PuzzleConfig

try:
    from puzzle_generator import PuzzleGenerator
    from game_logic import (
        GameLogic,
        PieceType as OriginalPieceType,
        ConstraintType as OriginalConstraintType,
    )

    ORIGINAL_LOGIC_AVAILABLE = True
except ImportError as e:
    ORIGINAL_LOGIC_AVAILABLE = False
    print(f"Warning: Original game logic not available ({e}), using simplified version")


class TangoBoardSolver:
    """
    Advanced board solver for Tango puzzles using depth-first search.
    Finds all possible solutions and can add constraints to ensure uniqueness.
    """

    def __init__(
        self,
        board: list[list[PieceType]],
        h_constraints: list[list[ConstraintType]],
        v_constraints: list[list[ConstraintType]],
        locked_tiles: list[list[bool]],
    ):
        self.original_board = board
        self.h_constraints = h_constraints
        self.v_constraints = v_constraints
        self.locked_tiles = locked_tiles
        self.size = 6

        # Find empty positions that need to be filled
        self.empty_positions = []
        for row in range(self.size):
            for col in range(self.size):
                if not locked_tiles[row][col]:
                    self.empty_positions.append((row, col))

    def find_all_solutions(
        self, max_solutions: int = 10
    ) -> list[list[list[PieceType]]]:
        """Find all valid solutions using depth-first search."""
        solutions = []

        # Create initial board with only locked pieces
        board = [[PieceType.EMPTY for _ in range(self.size)] for _ in range(self.size)]
        for row in range(self.size):
            for col in range(self.size):
                if self.locked_tiles[row][col]:
                    board[row][col] = self.original_board[row][col]

        def backtrack(pos: int):
            if len(solutions) >= max_solutions:
                return

            if pos == len(self.empty_positions):
                # Check if this is a complete valid solution
                if self._is_complete_and_valid(board):
                    # Make a deep copy of the solution
                    solution = [row[:] for row in board]
                    solutions.append(solution)
                return

            row, col = self.empty_positions[pos]

            # Try both piece types
            for piece in [PieceType.SUN, PieceType.MOON]:
                board[row][col] = piece

                # Check if this placement is valid so far
                if self._is_placement_valid(board, row, col):
                    backtrack(pos + 1)

                # Backtrack
                board[row][col] = PieceType.EMPTY

        backtrack(0)
        return solutions

    def _is_placement_valid(
        self, board: list[list[PieceType]], row: int, col: int
    ) -> bool:
        """Check if a piece placement is valid (doesn't violate rules so far)."""
        piece = board[row][col]

        # Check consecutive rule in horizontal direction
        # Check left side (if we have 2+ pieces to the left)
        if col >= 2:
            if (
                board[row][col - 1] == board[row][col - 2] == piece
                and piece != PieceType.EMPTY
            ):
                return False
        # Check center (if we have 1 piece on each side)
        if col >= 1 and col < self.size - 1:
            if (
                board[row][col - 1] == piece == board[row][col + 1]
                and piece != PieceType.EMPTY
            ):
                return False
        # Check right side (if we have 2+ pieces to the right)
        if col < self.size - 2:
            if (
                piece == board[row][col + 1] == board[row][col + 2]
                and piece != PieceType.EMPTY
            ):
                return False

        # Check consecutive rule in vertical direction
        # Check top side
        if row >= 2:
            if (
                board[row - 1][col] == board[row - 2][col] == piece
                and piece != PieceType.EMPTY
            ):
                return False
        # Check center
        if row >= 1 and row < self.size - 1:
            if (
                board[row - 1][col] == piece == board[row + 1][col]
                and piece != PieceType.EMPTY
            ):
                return False
        # Check bottom side
        if row < self.size - 2:
            if (
                piece == board[row + 1][col] == board[row + 2][col]
                and piece != PieceType.EMPTY
            ):
                return False

        # Check constraints (only if adjacent pieces are filled)
        # Horizontal constraints
        if col > 0 and self.h_constraints[row][col - 1] != ConstraintType.NONE:
            left_piece = board[row][col - 1]
            if left_piece != PieceType.EMPTY:
                if (
                    self.h_constraints[row][col - 1] == ConstraintType.SAME
                    and left_piece != piece
                ):
                    return False
                if (
                    self.h_constraints[row][col - 1] == ConstraintType.DIFFERENT
                    and left_piece == piece
                ):
                    return False

        if col < self.size - 1 and self.h_constraints[row][col] != ConstraintType.NONE:
            right_piece = board[row][col + 1]
            if right_piece != PieceType.EMPTY:
                if (
                    self.h_constraints[row][col] == ConstraintType.SAME
                    and piece != right_piece
                ):
                    return False
                if (
                    self.h_constraints[row][col] == ConstraintType.DIFFERENT
                    and piece == right_piece
                ):
                    return False

        # Vertical constraints
        if row > 0 and self.v_constraints[row - 1][col] != ConstraintType.NONE:
            top_piece = board[row - 1][col]
            if top_piece != PieceType.EMPTY:
                if (
                    self.v_constraints[row - 1][col] == ConstraintType.SAME
                    and top_piece != piece
                ):
                    return False
                if (
                    self.v_constraints[row - 1][col] == ConstraintType.DIFFERENT
                    and top_piece == piece
                ):
                    return False

        if row < self.size - 1 and self.v_constraints[row][col] != ConstraintType.NONE:
            bottom_piece = board[row + 1][col]
            if bottom_piece != PieceType.EMPTY:
                if (
                    self.v_constraints[row][col] == ConstraintType.SAME
                    and piece != bottom_piece
                ):
                    return False
                if (
                    self.v_constraints[row][col] == ConstraintType.DIFFERENT
                    and piece == bottom_piece
                ):
                    return False

        return True

    def _is_complete_and_valid(self, board: list[list[PieceType]]) -> bool:
        """Check if a complete board is valid (all rules satisfied)."""
        # Check all positions are filled
        for row in range(self.size):
            for col in range(self.size):
                if board[row][col] == PieceType.EMPTY:
                    return False

        # Check balance rule (3 suns and 3 moons per row/column)
        for row in range(self.size):
            sun_count = sum(
                1 for col in range(self.size) if board[row][col] == PieceType.SUN
            )
            if sun_count != 3:
                return False

        for col in range(self.size):
            sun_count = sum(
                1 for row in range(self.size) if board[row][col] == PieceType.SUN
            )
            if sun_count != 3:
                return False

        # Check no more than 2 consecutive pieces
        for row in range(self.size):
            for col in range(self.size - 2):
                if board[row][col] == board[row][col + 1] == board[row][col + 2]:
                    return False

        for row in range(self.size - 2):
            for col in range(self.size):
                if board[row][col] == board[row + 1][col] == board[row + 2][col]:
                    return False

        # Check all constraints are satisfied
        for row in range(self.size):
            for col in range(self.size - 1):
                if self.h_constraints[row][col] != ConstraintType.NONE:
                    left = board[row][col]
                    right = board[row][col + 1]
                    if (
                        self.h_constraints[row][col] == ConstraintType.SAME
                        and left != right
                    ):
                        return False
                    if (
                        self.h_constraints[row][col] == ConstraintType.DIFFERENT
                        and left == right
                    ):
                        return False

        for row in range(self.size - 1):
            for col in range(self.size):
                if self.v_constraints[row][col] != ConstraintType.NONE:
                    top = board[row][col]
                    bottom = board[row + 1][col]
                    if (
                        self.v_constraints[row][col] == ConstraintType.SAME
                        and top != bottom
                    ):
                        return False
                    if (
                        self.v_constraints[row][col] == ConstraintType.DIFFERENT
                        and top == bottom
                    ):
                        return False

        return True

    def remove_redundant_constraints(
        self,
        board: list[list[PieceType]],
        h_constraints: list[list[ConstraintType]],
        v_constraints: list[list[ConstraintType]],
        locked_tiles: list[list[bool]],
    ) -> tuple[list[list[ConstraintType]], list[list[ConstraintType]]]:
        """
        Remove constraints that are redundant given the game rules and current state.
        A constraint is redundant if removing it doesn't create additional valid solutions.
        """
        print("🔍 Analyzing constraints for redundancy...")

        # Make copies to modify
        h_constraints_copy = [row[:] for row in h_constraints]
        v_constraints_copy = [row[:] for row in v_constraints]

        # Count original constraints
        original_h_count = sum(
            sum(1 for c in row if c != ConstraintType.NONE) for row in h_constraints
        )
        original_v_count = sum(
            sum(1 for c in row if c != ConstraintType.NONE) for row in v_constraints
        )
        original_total = original_h_count + original_v_count

        # Test each constraint to see if it's necessary
        removed_constraints = 0

        # Test horizontal constraints
        for row in range(self.size):
            for col in range(self.size - 1):
                if h_constraints_copy[row][col] != ConstraintType.NONE:
                    # Temporarily remove this constraint
                    original_constraint = h_constraints_copy[row][col]
                    h_constraints_copy[row][col] = ConstraintType.NONE

                    # Check if puzzle still has unique solution
                    test_solver = TangoBoardSolver(
                        board, h_constraints_copy, v_constraints_copy, locked_tiles
                    )
                    solutions = test_solver.find_all_solutions(max_solutions=3)

                    if len(solutions) == 1:
                        # Constraint is redundant, keep it removed
                        removed_constraints += 1
                        print(
                            f"   ➖ Removed redundant horizontal constraint at ({row}, {col})"
                        )
                    else:
                        # Constraint is necessary, restore it
                        h_constraints_copy[row][col] = original_constraint

        # Test vertical constraints
        for row in range(self.size - 1):
            for col in range(self.size):
                if v_constraints_copy[row][col] != ConstraintType.NONE:
                    # Temporarily remove this constraint
                    original_constraint = v_constraints_copy[row][col]
                    v_constraints_copy[row][col] = ConstraintType.NONE

                    # Check if puzzle still has unique solution
                    test_solver = TangoBoardSolver(
                        board, h_constraints_copy, v_constraints_copy, locked_tiles
                    )
                    solutions = test_solver.find_all_solutions(max_solutions=3)

                    if len(solutions) == 1:
                        # Constraint is redundant, keep it removed
                        removed_constraints += 1
                        print(
                            f"   ➖ Removed redundant vertical constraint at ({row}, {col})"
                        )
                    else:
                        # Constraint is necessary, restore it
                        v_constraints_copy[row][col] = original_constraint

        # Report reduction
        final_h_count = sum(
            sum(1 for c in row if c != ConstraintType.NONE)
            for row in h_constraints_copy
        )
        final_v_count = sum(
            sum(1 for c in row if c != ConstraintType.NONE)
            for row in v_constraints_copy
        )
        final_total = final_h_count + final_v_count

        print(
            f"📉 Reduced constraints from {original_total} to {final_total} ({removed_constraints} removed)"
        )

        return h_constraints_copy, v_constraints_copy

    def optimize_constraints_for_inference(
        self,
        board: list[list[PieceType]],
        h_constraints: list[list[ConstraintType]],
        v_constraints: list[list[ConstraintType]],
        locked_tiles: list[list[bool]],
    ) -> tuple[list[list[ConstraintType]], list[list[ConstraintType]]]:
        """
        Optimize constraints by prioritizing inference-based solving over explicit constraints.
        This method finds positions that can be solved through logical deduction and removes
        nearby constraints that become redundant.
        """
        print("🧠 Optimizing constraints for inference-based solving...")

        # Create a test board with current locked pieces
        test_board = [
            [PieceType.EMPTY for _ in range(self.size)] for _ in range(self.size)
        ]
        for row in range(self.size):
            for col in range(self.size):
                if locked_tiles[row][col]:
                    test_board[row][col] = board[row][col]

        # Find positions that can be solved through inference
        inference_positions = self._find_inference_solvable_positions(test_board)
        print(f"   🎯 Found {len(inference_positions)} positions solvable by inference")

        # Remove constraints near inference-solvable positions
        h_constraints_copy = [row[:] for row in h_constraints]
        v_constraints_copy = [row[:] for row in v_constraints]

        removed_near_inference = 0

        for row, col in inference_positions:
            # Remove nearby horizontal constraints
            for check_col in [col - 1, col]:
                if 0 <= check_col < self.size - 1:
                    if h_constraints_copy[row][check_col] != ConstraintType.NONE:
                        # Test if removing this constraint maintains uniqueness
                        original = h_constraints_copy[row][check_col]
                        h_constraints_copy[row][check_col] = ConstraintType.NONE

                        test_solver = TangoBoardSolver(
                            board, h_constraints_copy, v_constraints_copy, locked_tiles
                        )
                        solutions = test_solver.find_all_solutions(max_solutions=3)

                        if len(solutions) == 1:
                            removed_near_inference += 1
                            print(
                                f"   ➖ Removed constraint near inference position ({row}, {col})"
                            )
                        else:
                            h_constraints_copy[row][check_col] = original

            # Remove nearby vertical constraints
            for check_row in [row - 1, row]:
                if 0 <= check_row < self.size - 1:
                    if v_constraints_copy[check_row][col] != ConstraintType.NONE:
                        # Test if removing this constraint maintains uniqueness
                        original = v_constraints_copy[check_row][col]
                        v_constraints_copy[check_row][col] = ConstraintType.NONE

                        test_solver = TangoBoardSolver(
                            board, h_constraints_copy, v_constraints_copy, locked_tiles
                        )
                        solutions = test_solver.find_all_solutions(max_solutions=3)

                        if len(solutions) == 1:
                            removed_near_inference += 1
                            print(
                                f"   ➖ Removed constraint near inference position ({row}, {col})"
                            )
                        else:
                            v_constraints_copy[check_row][col] = original

        print(
            f"   📉 Removed {removed_near_inference} constraints near inference-solvable positions"
        )

        return h_constraints_copy, v_constraints_copy

    def _find_inference_solvable_positions(
        self, board: list[list[PieceType]]
    ) -> list[tuple[int, int]]:
        """
        Find positions that can be determined through logical inference from game rules alone.
        """
        inference_positions = []

        # Simulate incremental solving to find inference-based moves
        working_board = [row[:] for row in board]

        max_iterations = 36  # Maximum number of empty cells
        for _ in range(max_iterations):
            found_move = False

            for row in range(self.size):
                for col in range(self.size):
                    if working_board[row][col] == PieceType.EMPTY:
                        # Check what pieces are possible based on game rules only
                        possible_pieces = []

                        for piece in [PieceType.SUN, PieceType.MOON]:
                            working_board[row][col] = piece
                            if self._is_valid_by_game_rules_only(
                                working_board, row, col
                            ):
                                possible_pieces.append(piece)
                            working_board[row][col] = PieceType.EMPTY

                        # If only one piece is possible by game rules, it's inference-solvable
                        if len(possible_pieces) == 1:
                            working_board[row][col] = possible_pieces[0]
                            inference_positions.append((row, col))
                            found_move = True
                            break

                if found_move:
                    break

            if not found_move:
                break

        return inference_positions

    def _is_valid_by_game_rules_only(
        self, board: list[list[PieceType]], row: int, col: int
    ) -> bool:
        """
        Check if a piece placement is valid based on game rules only (no explicit constraints).
        """
        piece = board[row][col]

        # Rule 1: No more than 2 consecutive pieces
        # Check horizontal
        if col >= 2:
            if board[row][col - 1] == board[row][col - 2] == piece != PieceType.EMPTY:
                return False
        if col >= 1 and col < self.size - 1:
            if board[row][col - 1] == piece == board[row][col + 1] != PieceType.EMPTY:
                return False
        if col < self.size - 2:
            if piece == board[row][col + 1] == board[row][col + 2] != PieceType.EMPTY:
                return False

        # Check vertical
        if row >= 2:
            if board[row - 1][col] == board[row - 2][col] == piece != PieceType.EMPTY:
                return False
        if row >= 1 and row < self.size - 1:
            if board[row - 1][col] == piece == board[row + 1][col] != PieceType.EMPTY:
                return False
        if row < self.size - 2:
            if piece == board[row + 1][col] == board[row + 2][col] != PieceType.EMPTY:
                return False

        # Rule 2: Check balance constraints (3 of each piece per row/column)
        row_suns = sum(1 for c in range(self.size) if board[row][c] == PieceType.SUN)
        row_moons = sum(1 for c in range(self.size) if board[row][c] == PieceType.MOON)
        col_suns = sum(1 for r in range(self.size) if board[r][col] == PieceType.SUN)
        col_moons = sum(1 for r in range(self.size) if board[r][col] == PieceType.MOON)

        if piece == PieceType.SUN and (row_suns > 3 or col_suns > 3):
            return False
        if piece == PieceType.MOON and (row_moons > 3 or col_moons > 3):
            return False

        return True

    def add_disambiguating_constraints(
        self,
        solutions: list[list[list[PieceType]]],
        h_constraints: list[list[ConstraintType]],
        v_constraints: list[list[ConstraintType]],
        max_constraints: int = 5,
    ) -> tuple[list[list[ConstraintType]], list[list[ConstraintType]]]:
        """Add constraints to eliminate alternative solutions."""
        if len(solutions) <= 1:
            return h_constraints, v_constraints

        # Find differences between solutions
        differences = self._find_solution_differences(solutions)

        # Sort differences by how many solutions they affect
        constraint_candidates = []

        for (row1, col1, row2, col2), affecting_solutions in differences.items():
            if (
                len(affecting_solutions) > 1
            ):  # This difference affects multiple solutions
                # Determine what constraint would eliminate the alternatives
                primary_solution = solutions[0]

                if row1 == row2:  # Horizontal constraint
                    if col2 == col1 + 1:  # Adjacent horizontally
                        piece1 = primary_solution[row1][col1]
                        piece2 = primary_solution[row1][col2]
                        constraint_type = (
                            ConstraintType.SAME
                            if piece1 == piece2
                            else ConstraintType.DIFFERENT
                        )
                        constraint_candidates.append(
                            (row1, col1, "h", constraint_type, len(affecting_solutions))
                        )

                elif col1 == col2:  # Vertical constraint
                    if row2 == row1 + 1:  # Adjacent vertically
                        piece1 = primary_solution[row1][col1]
                        piece2 = primary_solution[row2][col1]
                        constraint_type = (
                            ConstraintType.SAME
                            if piece1 == piece2
                            else ConstraintType.DIFFERENT
                        )
                        constraint_candidates.append(
                            (row1, col1, "v", constraint_type, len(affecting_solutions))
                        )

        # Sort by impact (number of solutions affected) and add constraints
        constraint_candidates.sort(key=lambda x: x[4], reverse=True)

        added_constraints = 0
        for row, col, direction, constraint_type, _ in constraint_candidates:
            if added_constraints >= max_constraints:
                break

            # Check if we can add this constraint
            if direction == "h" and col < self.size - 1:
                if h_constraints[row][col] == ConstraintType.NONE:
                    h_constraints[row][col] = constraint_type
                    added_constraints += 1
                    print(
                        f"   ➕ Added horizontal {constraint_type.value} constraint at ({row}, {col})"
                    )

            elif direction == "v" and row < self.size - 1:
                if v_constraints[row][col] == ConstraintType.NONE:
                    v_constraints[row][col] = constraint_type
                    added_constraints += 1
                    print(
                        f"   ➕ Added vertical {constraint_type.value} constraint at ({row}, {col})"
                    )

        print(f"   📝 Added {added_constraints} disambiguating constraints")
        return h_constraints, v_constraints

    def _find_solution_differences(
        self, solutions: list[list[list[PieceType]]]
    ) -> dict:
        """Find positions where solutions differ."""
        differences = {}

        # Compare each pair of adjacent positions in all solutions
        for row in range(self.size):
            for col in range(self.size - 1):
                # Check horizontal adjacent pairs
                pairs_seen = set()
                affecting_solutions = []

                for i, solution in enumerate(solutions):
                    pair = (solution[row][col], solution[row][col + 1])
                    pairs_seen.add(pair)
                    affecting_solutions.append(i)

                if len(pairs_seen) > 1:  # Different solutions have different pairs
                    differences[(row, col, row, col + 1)] = affecting_solutions

        for row in range(self.size - 1):
            for col in range(self.size):
                # Check vertical adjacent pairs
                pairs_seen = set()
                affecting_solutions = []

                for i, solution in enumerate(solutions):
                    pair = (solution[row][col], solution[row + 1][col])
                    pairs_seen.add(pair)
                    affecting_solutions.append(i)

                if len(pairs_seen) > 1:  # Different solutions have different pairs
                    differences[(row, col, row + 1, col)] = affecting_solutions

        return differences

    def get_hint(self) -> dict:
        """
        Analyze the current puzzle state and provide a hint for the next logical move.
        Returns a hint with position, piece type, and reasoning prioritizing game rule education.
        """
        # Create current board state with only locked pieces
        board = [[PieceType.EMPTY for _ in range(self.size)] for _ in range(self.size)]
        for row in range(self.size):
            for col in range(self.size):
                if self.locked_tiles[row][col]:
                    board[row][col] = self.original_board[row][col]

        # Find positions that can be determined logically
        logical_moves = self._find_logical_moves(board)

        if logical_moves:
            # Prioritize moves based on educational value (game rules > constraints)
            educational_moves = []
            constraint_moves = []

            for move in logical_moves:
                if (
                    "Rule-based deduction" in move["reasoning"]
                    or "Balance rule deduction" in move["reasoning"]
                ):
                    educational_moves.append(move)
                else:
                    constraint_moves.append(move)

            # Return the best educational move first, then constraint-based moves
            best_moves = educational_moves if educational_moves else constraint_moves
            move = max(best_moves, key=lambda x: x["confidence"])

            return {
                "found": True,
                "row": move["row"],
                "col": move["col"],
                "piece_type": move["piece"].value,
                "reasoning": move["reasoning"],
                "confidence": move["confidence"],
                "hint_type": "logical_deduction",
                "educational_value": (
                    "high" if "Rule-based" in move["reasoning"] else "medium"
                ),
            }

        # If no logical moves, provide educational guidance about the board state
        analysis = self._analyze_board_state(board)
        if analysis["has_opportunities"]:
            return {
                "found": True,
                "row": analysis["suggested_row"],
                "col": analysis["suggested_col"],
                "piece_type": analysis["suggested_piece"],
                "reasoning": analysis["reasoning"],
                "confidence": analysis["confidence"],
                "hint_type": "strategic_guidance",
                "educational_value": "medium",
            }

        # Fallback to general strategic advice
        fallback_move = self._find_best_guess(board)
        if fallback_move:
            return {
                "found": True,
                "row": fallback_move["row"],
                "col": fallback_move["col"],
                "piece_type": fallback_move["piece"].value,
                "reasoning": fallback_move["reasoning"],
                "confidence": fallback_move["confidence"],
                "hint_type": "strategic_guess",
                "educational_value": "low",
            }

        return {
            "found": False,
            "reasoning": "No valid moves found or puzzle is complete",
            "hint_type": "none",
        }

    def _analyze_board_state(self, board: list[list[PieceType]]) -> dict:
        """
        Analyze the current board state to provide strategic guidance even when
        no single logical move is available.
        """
        # Count pieces in each row and column
        row_counts = []
        col_counts = []

        for i in range(self.size):
            row_suns = sum(1 for c in range(self.size) if board[i][c] == PieceType.SUN)
            row_moons = sum(
                1 for c in range(self.size) if board[i][c] == PieceType.MOON
            )
            row_empty = sum(
                1 for c in range(self.size) if board[i][c] == PieceType.EMPTY
            )
            row_counts.append(
                {"suns": row_suns, "moons": row_moons, "empty": row_empty}
            )

            col_suns = sum(1 for r in range(self.size) if board[r][i] == PieceType.SUN)
            col_moons = sum(
                1 for r in range(self.size) if board[r][i] == PieceType.MOON
            )
            col_empty = sum(
                1 for r in range(self.size) if board[r][i] == PieceType.EMPTY
            )
            col_counts.append(
                {"suns": col_suns, "moons": col_moons, "empty": col_empty}
            )

        # Look for rows/columns that are close to their limits
        for row in range(self.size):
            if row_counts[row]["empty"] > 0:
                if row_counts[row]["suns"] == 2:  # Almost at sun limit
                    # Find empty cell in this row
                    for col in range(self.size):
                        if board[row][col] == PieceType.EMPTY:
                            return {
                                "has_opportunities": True,
                                "suggested_row": row,
                                "suggested_col": col,
                                "suggested_piece": "moon",
                                "reasoning": f"Strategic insight: Row {row + 1} already has 2 suns. Focus on moon placements in this row to maintain balance.",
                                "confidence": 70,
                            }
                elif row_counts[row]["moons"] == 2:  # Almost at moon limit
                    for col in range(self.size):
                        if board[row][col] == PieceType.EMPTY:
                            return {
                                "has_opportunities": True,
                                "suggested_row": row,
                                "suggested_col": col,
                                "suggested_piece": "sun",
                                "reasoning": f"Strategic insight: Row {row + 1} already has 2 moons. Focus on sun placements in this row to maintain balance.",
                                "confidence": 70,
                            }

        # Look for columns with similar patterns
        for col in range(self.size):
            if col_counts[col]["empty"] > 0:
                if col_counts[col]["suns"] == 2:  # Almost at sun limit
                    for row in range(self.size):
                        if board[row][col] == PieceType.EMPTY:
                            return {
                                "has_opportunities": True,
                                "suggested_row": row,
                                "suggested_col": col,
                                "suggested_piece": "moon",
                                "reasoning": f"Strategic insight: Column {col + 1} already has 2 suns. Focus on moon placements in this column to maintain balance.",
                                "confidence": 70,
                            }
                elif col_counts[col]["moons"] == 2:  # Almost at moon limit
                    for row in range(self.size):
                        if board[row][col] == PieceType.EMPTY:
                            return {
                                "has_opportunities": True,
                                "suggested_row": row,
                                "suggested_col": col,
                                "suggested_piece": "sun",
                                "reasoning": f"Strategic insight: Column {col + 1} already has 2 moons. Focus on sun placements in this column to maintain balance.",
                                "confidence": 70,
                            }

        # Look for potential consecutive violations
        for row in range(self.size):
            for col in range(self.size - 2):
                if (
                    board[row][col] != PieceType.EMPTY
                    and board[row][col] == board[row][col + 1]
                    and board[row][col + 2] == PieceType.EMPTY
                ):
                    other_piece = (
                        PieceType.MOON
                        if board[row][col] == PieceType.SUN
                        else PieceType.SUN
                    )
                    return {
                        "has_opportunities": True,
                        "suggested_row": row,
                        "suggested_col": col + 2,
                        "suggested_piece": other_piece.value,
                        "reasoning": f"Strategic insight: Placing {other_piece.value} at ({row + 1}, {col + 3}) prevents three consecutive {board[row][col].value}s.",
                        "confidence": 85,
                    }

        return {"has_opportunities": False}

    def _find_logical_moves(self, board: list[list[PieceType]]) -> list[dict]:
        """Find moves that can be determined through logical deduction."""
        logical_moves = []

        for row in range(self.size):
            for col in range(self.size):
                if board[row][col] == PieceType.EMPTY:
                    # Check what pieces are possible at this position
                    possible_pieces = []

                    for piece in [PieceType.SUN, PieceType.MOON]:
                        board[row][col] = piece
                        if self._is_placement_valid(board, row, col):
                            possible_pieces.append(piece)
                        board[row][col] = PieceType.EMPTY

                    # If only one piece is possible, this is a logical move
                    if len(possible_pieces) == 1:
                        reasoning = self._analyze_why_only_choice(
                            board, row, col, possible_pieces[0]
                        )
                        logical_moves.append(
                            {
                                "row": row,
                                "col": col,
                                "piece": possible_pieces[0],
                                "reasoning": reasoning,
                                "confidence": 100,  # Certain move
                            }
                        )

                    # Check for constraint-forced moves
                    elif len(possible_pieces) == 2:
                        constraint_move = self._check_constraint_forcing(
                            board, row, col
                        )
                        if constraint_move:
                            logical_moves.append(constraint_move)

        return logical_moves

    def _analyze_why_only_choice(
        self, board: list[list[PieceType]], row: int, col: int, piece: PieceType
    ) -> str:
        """Analyze why a particular piece is the only valid choice with detailed game rule explanations."""
        reasons = []

        # Check if it's forced by consecutive rule
        other_piece = PieceType.MOON if piece == PieceType.SUN else PieceType.SUN

        # Test the other piece to see what rule it violates
        board[row][col] = other_piece

        # Check horizontal consecutive patterns
        if col >= 2:
            if board[row][col - 1] == board[row][col - 2] == other_piece:
                reasons.append(
                    f"placing {other_piece.value} would create three consecutive {other_piece.value}s in row {row + 1} (game rule violation)"
                )
        if col >= 1 and col < self.size - 1:
            if board[row][col - 1] == other_piece == board[row][col + 1]:
                reasons.append(
                    f"placing {other_piece.value} would create three consecutive {other_piece.value}s in row {row + 1} (game rule violation)"
                )
        if col < self.size - 2:
            if other_piece == board[row][col + 1] == board[row][col + 2]:
                reasons.append(
                    f"placing {other_piece.value} would create three consecutive {other_piece.value}s in row {row + 1} (game rule violation)"
                )

        # Check vertical consecutive patterns
        if row >= 2:
            if board[row - 1][col] == board[row - 2][col] == other_piece:
                reasons.append(
                    f"placing {other_piece.value} would create three consecutive {other_piece.value}s in column {col + 1} (game rule violation)"
                )
        if row >= 1 and row < self.size - 1:
            if board[row - 1][col] == other_piece == board[row + 1][col]:
                reasons.append(
                    f"placing {other_piece.value} would create three consecutive {other_piece.value}s in column {col + 1} (game rule violation)"
                )
        if row < self.size - 2:
            if other_piece == board[row + 1][col] == board[row + 2][col]:
                reasons.append(
                    f"placing {other_piece.value} would create three consecutive {other_piece.value}s in column {col + 1} (game rule violation)"
                )

        # Check balance constraints with detailed counting
        row_suns = sum(1 for c in range(self.size) if board[row][c] == PieceType.SUN)
        row_moons = sum(1 for c in range(self.size) if board[row][c] == PieceType.MOON)
        col_suns = sum(1 for r in range(self.size) if board[r][col] == PieceType.SUN)
        col_moons = sum(1 for r in range(self.size) if board[r][col] == PieceType.MOON)

        if other_piece == PieceType.SUN:
            if row_suns >= 3:
                reasons.append(
                    f"row {row + 1} already has {row_suns} suns (max 3 per row)"
                )
            if col_suns >= 3:
                reasons.append(
                    f"column {col + 1} already has {col_suns} suns (max 3 per column)"
                )
        elif other_piece == PieceType.MOON:
            if row_moons >= 3:
                reasons.append(
                    f"row {row + 1} already has {row_moons} moons (max 3 per row)"
                )
            if col_moons >= 3:
                reasons.append(
                    f"column {col + 1} already has {col_moons} moons (max 3 per column)"
                )

        # Check constraint violations (only mention if no game rule violations found)
        if not reasons:
            if col > 0 and self.h_constraints[row][col - 1] != ConstraintType.NONE:
                left_piece = board[row][col - 1]
                if left_piece != PieceType.EMPTY:
                    if (
                        self.h_constraints[row][col - 1] == ConstraintType.SAME
                        and left_piece != other_piece
                    ):
                        reasons.append(
                            f"constraint requires this cell to match {left_piece.value} on the left"
                        )
                    elif (
                        self.h_constraints[row][col - 1] == ConstraintType.DIFFERENT
                        and left_piece == other_piece
                    ):
                        reasons.append(
                            f"constraint requires this cell to differ from {left_piece.value} on the left"
                        )

            if (
                col < self.size - 1
                and self.h_constraints[row][col] != ConstraintType.NONE
            ):
                right_piece = board[row][col + 1]
                if right_piece != PieceType.EMPTY:
                    if (
                        self.h_constraints[row][col] == ConstraintType.SAME
                        and other_piece != right_piece
                    ):
                        reasons.append(
                            f"constraint requires this cell to match {right_piece.value} on the right"
                        )
                    elif (
                        self.h_constraints[row][col] == ConstraintType.DIFFERENT
                        and other_piece == right_piece
                    ):
                        reasons.append(
                            f"constraint requires this cell to differ from {right_piece.value} on the right"
                        )

            if row > 0 and self.v_constraints[row - 1][col] != ConstraintType.NONE:
                top_piece = board[row - 1][col]
                if top_piece != PieceType.EMPTY:
                    if (
                        self.v_constraints[row - 1][col] == ConstraintType.SAME
                        and top_piece != other_piece
                    ):
                        reasons.append(
                            f"constraint requires this cell to match {top_piece.value} above"
                        )
                    elif (
                        self.v_constraints[row - 1][col] == ConstraintType.DIFFERENT
                        and top_piece == other_piece
                    ):
                        reasons.append(
                            f"constraint requires this cell to differ from {top_piece.value} above"
                        )

            if (
                row < self.size - 1
                and self.v_constraints[row][col] != ConstraintType.NONE
            ):
                bottom_piece = board[row + 1][col]
                if bottom_piece != PieceType.EMPTY:
                    if (
                        self.v_constraints[row][col] == ConstraintType.SAME
                        and other_piece != bottom_piece
                    ):
                        reasons.append(
                            f"constraint requires this cell to match {bottom_piece.value} below"
                        )
                    elif (
                        self.v_constraints[row][col] == ConstraintType.DIFFERENT
                        and other_piece == bottom_piece
                    ):
                        reasons.append(
                            f"constraint requires this cell to differ from {bottom_piece.value} below"
                        )

        board[row][col] = PieceType.EMPTY  # Reset

        if reasons:
            rule_reasons = [r for r in reasons if "game rule violation" in r]
            constraint_reasons = [r for r in reasons if "constraint" in r]
            balance_reasons = [r for r in reasons if "max 3" in r or "already has" in r]

            # Prioritize game rule explanations over constraint explanations
            if rule_reasons:
                primary_reason = rule_reasons[0]
                explanation_type = "Rule-based deduction"
            elif balance_reasons:
                primary_reason = balance_reasons[0]
                explanation_type = "Balance rule deduction"
            else:
                primary_reason = (
                    constraint_reasons[0] if constraint_reasons else reasons[0]
                )
                explanation_type = "Constraint-based deduction"

            return f"{explanation_type}: Only {piece.value} works here because {primary_reason}"
        else:
            return f"Logical deduction: Only {piece.value} is valid at position ({row + 1}, {col + 1})"

    def _check_constraint_forcing(
        self, board: list[list[PieceType]], row: int, col: int
    ) -> dict | None:
        """Check if constraints force a specific piece choice."""
        # This could be expanded for more sophisticated constraint analysis
        return None

    def _find_best_guess(self, board: list[list[PieceType]]) -> dict | None:
        """Find the best position to make an educated guess."""
        # For now, just pick the first empty position
        for row in range(self.size):
            for col in range(self.size):
                if board[row][col] == PieceType.EMPTY:
                    # Try both pieces and see which is more likely based on current state
                    board[row][col] = PieceType.SUN
                    sun_valid = self._is_placement_valid(board, row, col)
                    board[row][col] = PieceType.MOON
                    moon_valid = self._is_placement_valid(board, row, col)
                    board[row][col] = PieceType.EMPTY

                    if sun_valid and not moon_valid:
                        return {
                            "row": row,
                            "col": col,
                            "piece": PieceType.SUN,
                            "reasoning": "Best logical guess based on current constraints",
                            "confidence": 75,
                        }
                    elif moon_valid and not sun_valid:
                        return {
                            "row": row,
                            "col": col,
                            "piece": PieceType.MOON,
                            "reasoning": "Best logical guess based on current constraints",
                            "confidence": 75,
                        }
                    elif sun_valid and moon_valid:
                        return {
                            "row": row,
                            "col": col,
                            "piece": PieceType.SUN,
                            "reasoning": "Either piece could work here - try sun first",
                            "confidence": 50,
                        }

        return None


class GameService:
    """Service for managing Tango game instances and logic."""

    def __init__(self):
        self.active_games: Dict[str, "SimpleGameLogic"] = {}
        self.game_states: Dict[str, GameState] = {}
        self.leaderboard_file = Path("leaderboard.json")

    def _convert_piece_from_original(self, piece: "OriginalPieceType") -> PieceType:
        """Convert original PieceType to API PieceType."""
        if not ORIGINAL_LOGIC_AVAILABLE:
            return PieceType.EMPTY

        if piece.value == 0:  # EMPTY
            return PieceType.EMPTY
        elif piece.value == 1:  # SUN
            return PieceType.SUN
        elif piece.value == 2:  # MOON
            return PieceType.MOON
        return PieceType.EMPTY

    def _convert_constraint_from_original(
        self, constraint: "OriginalConstraintType"
    ) -> ConstraintType:
        """Convert original ConstraintType to API ConstraintType."""
        if not ORIGINAL_LOGIC_AVAILABLE:
            return ConstraintType.NONE

        if constraint.value == 0:  # NONE
            return ConstraintType.NONE
        elif constraint.value == 1:  # SAME
            return ConstraintType.SAME
        elif constraint.value == 2:  # DIFFERENT
            return ConstraintType.DIFFERENT
        return ConstraintType.NONE

    def _convert_piece_to_original(self, piece: PieceType) -> "OriginalPieceType":
        """Convert API PieceType to original PieceType."""
        if not ORIGINAL_LOGIC_AVAILABLE:
            return None

        try:
            from game_logic import PieceType as OriginalPieceType

            if piece == PieceType.EMPTY:
                return OriginalPieceType.EMPTY
            elif piece == PieceType.SUN:
                return OriginalPieceType.SUN
            elif piece == PieceType.MOON:
                return OriginalPieceType.MOON
            return OriginalPieceType.EMPTY
        except ImportError:
            return None

    def create_game(self, difficulty: Optional[str] = None) -> GameState:
        """Create a new game with proper puzzle generation."""
        game_id = str(uuid.uuid4())

        print(f"Creating new game {game_id[:8]}...")
        print(f"Original logic available: {ORIGINAL_LOGIC_AVAILABLE}")
        if difficulty:
            print(f"Requested difficulty: {difficulty}")

        if ORIGINAL_LOGIC_AVAILABLE:
            # Use the original sophisticated puzzle generator
            try:
                print("Attempting to use original puzzle generator...")
                generator = PuzzleGenerator()
                original_game = generator.generate_simple_puzzle()

                if original_game is None:
                    print(
                        "Original puzzle generator returned None, falling back to sophisticated generation"
                    )
                    # Fallback to sophisticated generation if puzzle generation fails
                    return self._generate_sophisticated_puzzle(game_id, difficulty)

                print("Successfully generated puzzle with original logic")
                # Convert the original game to our API format
                board = []
                for row in range(6):
                    api_row = []
                    for col in range(6):
                        piece = original_game.get_piece(row, col)
                        api_piece = self._convert_piece_from_original(piece)
                        api_row.append(api_piece)
                    board.append(api_row)

                # Convert constraints
                h_constraints = []
                for row in range(6):
                    h_row = []
                    for col in range(5):  # 5 horizontal constraints per row
                        constraint = original_game.get_horizontal_constraint(row, col)
                        api_constraint = self._convert_constraint_from_original(
                            constraint
                        )
                        h_row.append(api_constraint)
                    h_constraints.append(h_row)

                v_constraints = []
                for row in range(5):  # 5 vertical constraint rows
                    v_row = []
                    for col in range(6):
                        constraint = original_game.get_vertical_constraint(row, col)
                        api_constraint = self._convert_constraint_from_original(
                            constraint
                        )
                        v_row.append(api_constraint)
                    v_constraints.append(v_row)

                # Convert locked tiles
                locked_tiles = []
                for row in range(6):
                    locked_row = []
                    for col in range(6):
                        is_locked = original_game.is_tile_locked(row, col)
                        locked_row.append(is_locked)
                    locked_tiles.append(locked_row)

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

                # Store the game with enhanced logic
                self.game_states[game_id] = game_state
                self.active_games[game_id] = SimpleGameLogic(game_state)

                return game_state

            except Exception as e:
                print(f"Error generating puzzle with original logic: {e}")
                print("Falling back to sophisticated generation")
                # Fall back to sophisticated generation
                return self._generate_sophisticated_puzzle(game_id, difficulty)
        else:
            # Original logic not available, use sophisticated generation
            print("Using sophisticated puzzle generation")
            return self._generate_sophisticated_puzzle(game_id, difficulty)

    def _generate_sophisticated_puzzle(
        self, game_id: str, difficulty: Optional[str] = None
    ) -> GameState:
        """Generate a sophisticated, valid puzzle using advanced algorithms."""
        from backend.app.config.puzzle_config import get_puzzle_config
        import random

        config = get_puzzle_config(difficulty)
        print(f"Generating sophisticated {config.name} difficulty puzzle")

        max_attempts = 10
        for attempt in range(max_attempts):
            try:
                print(f"Generation attempt {attempt + 1}/{max_attempts}")

                # Step 1: Generate a complete valid solution using constraint satisfaction
                board = self._generate_valid_complete_solution()
                if not self._validate_complete_solution(board):
                    print(
                        f"Attempt {attempt + 1}: Generated solution is invalid, retrying..."
                    )
                    continue

                print(f"Attempt {attempt + 1}: Generated valid complete solution")

                # Step 2: Add meaningful constraints based on the solution
                h_constraints, v_constraints = self._add_strategic_constraints(
                    board, config
                )

                # Step 3: Remove pieces strategically while maintaining solvability
                locked_tiles = self._remove_pieces_intelligently(board, config)

                # Step 3.5: Ensure unique solution by adding more constraints if needed
                h_constraints, v_constraints = self._ensure_unique_solution(
                    board, h_constraints, v_constraints, locked_tiles, config
                )

                # Step 4: Comprehensive validation
                if self._validate_puzzle_completely(
                    board, h_constraints, v_constraints, locked_tiles
                ):
                    starting_pieces = sum(sum(row) for row in locked_tiles)
                    print(
                        f"Successfully generated {config.name} difficulty puzzle with {starting_pieces} starting pieces"
                    )

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

                    # Store the game state before returning
                    self.game_states[game_id] = game_state
                    self.active_games[game_id] = SimpleGameLogic(game_state)

                    return game_state
                else:
                    print(
                        f"Attempt {attempt + 1}: Final validation failed, retrying..."
                    )
                    continue

            except Exception as e:
                print(f"Error in generation attempt {attempt + 1}: {e}")
                continue

        # If all sophisticated attempts failed, create a basic but valid puzzle
        print(
            "All sophisticated generation attempts failed, creating basic valid puzzle"
        )
        return self._create_basic_valid_puzzle(game_id, config)

    def _generate_valid_complete_solution(self) -> list[list[PieceType]]:
        """Generate a complete valid solution using backtracking with constraint satisfaction."""
        import random

        board = [[PieceType.EMPTY for _ in range(6)] for _ in range(6)]

        def is_valid_placement(
            board: list[list[PieceType]], row: int, col: int, piece: PieceType
        ) -> bool:
            """Check if placing a piece at position (row, col) is valid."""
            # Temporarily place the piece
            original = board[row][col]
            board[row][col] = piece

            # Check horizontal consecutive constraint (no more than 2 in a row)
            consecutive_count = 1

            # Check left
            c = col - 1
            while c >= 0 and board[row][c] == piece:
                consecutive_count += 1
                c -= 1

            # Check right
            c = col + 1
            while c < 6 and board[row][c] == piece:
                consecutive_count += 1
                c += 1

            if consecutive_count > 2:
                board[row][col] = original
                return False

            # Check vertical consecutive constraint
            consecutive_count = 1

            # Check up
            r = row - 1
            while r >= 0 and board[r][col] == piece:
                consecutive_count += 1
                r -= 1

            # Check down
            r = row + 1
            while r < 6 and board[r][col] == piece:
                consecutive_count += 1
                r += 1

            if consecutive_count > 2:
                board[row][col] = original
                return False

            # Check row balance (no more than 3 of same type in a row)
            if col == 5:  # End of row, check balance
                sun_count = sum(1 for c in range(6) if board[row][c] == PieceType.SUN)
                moon_count = sum(1 for c in range(6) if board[row][c] == PieceType.MOON)
                if abs(sun_count - moon_count) > 0:  # Require exact balance
                    board[row][col] = original
                    return False

            # Check column balance (no more than 3 of same type in a column)
            if row == 5:  # End of column, check balance
                sun_count = sum(1 for r in range(6) if board[r][col] == PieceType.SUN)
                moon_count = sum(1 for r in range(6) if board[r][col] == PieceType.MOON)
                if abs(sun_count - moon_count) > 0:  # Require exact balance
                    board[row][col] = original
                    return False

            board[row][col] = original
            return True

        def backtrack(pos: int) -> bool:
            if pos == 36:  # 6x6 = 36 positions
                return True

            row, col = pos // 6, pos % 6

            # Try both pieces in random order for variety
            pieces = [PieceType.SUN, PieceType.MOON]
            random.shuffle(pieces)

            for piece in pieces:
                if is_valid_placement(board, row, col, piece):
                    board[row][col] = piece
                    if backtrack(pos + 1):
                        return True
                    board[row][col] = PieceType.EMPTY

            return False

        if backtrack(0):
            return board
        else:
            # Fallback: create a simple valid pattern
            return self._create_fallback_valid_solution()

    def _create_fallback_valid_solution(self) -> list[list[PieceType]]:
        """Create a fallback valid solution when backtracking fails."""
        import random

        # Create a pattern that guarantees validity
        patterns = [
            # Pattern 1: Alternating with variation
            [
                [
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                ],
                [
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
                ],
                [
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                ],
                [
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
                ],
                [
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                ],
                [
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
                ],
            ],
            # Pattern 2: Diagonal stripes
            [
                [
                    PieceType.SUN,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.SUN,
                ],
                [
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.SUN,
                    PieceType.MOON,
                ],
                [
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.MOON,
                ],
                [
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.SUN,
                ],
                [
                    PieceType.SUN,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.SUN,
                ],
                [
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.SUN,
                    PieceType.MOON,
                ],
            ],
        ]

        return random.choice(patterns)

    def _validate_complete_solution(self, board: list[list[PieceType]]) -> bool:
        """Validate that a complete solution follows all rules."""
        # Check no more than 2 consecutive pieces
        for row in range(6):
            for col in range(4):  # Check 3-in-a-row horizontally
                if (
                    board[row][col] == board[row][col + 1] == board[row][col + 2]
                    and board[row][col] != PieceType.EMPTY
                ):
                    return False

        for row in range(4):  # Check 3-in-a-row vertically
            for col in range(6):
                if (
                    board[row][col] == board[row + 1][col] == board[row + 2][col]
                    and board[row][col] != PieceType.EMPTY
                ):
                    return False

        # Check balance in each row and column
        for row in range(6):
            sun_count = sum(1 for col in range(6) if board[row][col] == PieceType.SUN)
            moon_count = sum(1 for col in range(6) if board[row][col] == PieceType.MOON)
            if sun_count != 3 or moon_count != 3:
                return False

        for col in range(6):
            sun_count = sum(1 for row in range(6) if board[row][col] == PieceType.SUN)
            moon_count = sum(1 for row in range(6) if board[row][col] == PieceType.MOON)
            if sun_count != 3 or moon_count != 3:
                return False

        return True

    def _add_strategic_constraints(
        self, board: list[list[PieceType]], config
    ) -> tuple[list[list[ConstraintType]], list[list[ConstraintType]]]:
        """Add strategic constraints that provide meaningful hints while minimizing over-constraining."""
        import random

        h_constraints = [[ConstraintType.NONE for _ in range(5)] for _ in range(6)]
        v_constraints = [[ConstraintType.NONE for _ in range(6)] for _ in range(5)]

        # First, find positions that would be difficult to solve through inference alone
        difficult_positions = self._find_difficult_inference_positions(board)

        # Add constraints with strategic value, prioritizing difficult positions
        constraint_positions = []

        # Reduced constraint probability - let game rules do more work
        base_probability = config.constraint_probability * 0.3  # Reduce by 70%

        # Horizontal constraints - focus on difficult areas
        for row in range(6):
            for col in range(5):
                is_difficult_area = any(
                    abs(diff_row - row) <= 1 and abs(diff_col - col) <= 1
                    for diff_row, diff_col in difficult_positions
                )

                # Higher probability for difficult areas, lower for easy areas
                probability = (
                    base_probability * 3
                    if is_difficult_area
                    else base_probability * 0.5
                )

                if random.random() < probability:
                    if board[row][col] == board[row][col + 1]:
                        h_constraints[row][col] = ConstraintType.SAME
                        constraint_positions.append(("h", row, col))
                    else:
                        h_constraints[row][col] = ConstraintType.DIFFERENT
                        constraint_positions.append(("h", row, col))

        # Vertical constraints - focus on difficult areas
        for row in range(5):
            for col in range(6):
                is_difficult_area = any(
                    abs(diff_row - row) <= 1 and abs(diff_col - col) <= 1
                    for diff_row, diff_col in difficult_positions
                )

                # Higher probability for difficult areas, lower for easy areas
                probability = (
                    base_probability * 3
                    if is_difficult_area
                    else base_probability * 0.5
                )

                if random.random() < probability:
                    if board[row][col] == board[row + 1][col]:
                        v_constraints[row][col] = ConstraintType.SAME
                        constraint_positions.append(("v", row, col))
                    else:
                        v_constraints[row][col] = ConstraintType.DIFFERENT
                        constraint_positions.append(("v", row, col))

        print(
            f"Added {len(constraint_positions)} strategic constraints ({len(difficult_positions)} difficult positions identified)"
        )
        return h_constraints, v_constraints

    def _find_difficult_inference_positions(
        self, board: list[list[PieceType]]
    ) -> list[tuple[int, int]]:
        """
        Find positions that would be particularly difficult to solve through game rule inference alone.
        These are positions where multiple valid pieces could be placed based on local context.
        """
        difficult_positions = []

        # Create a test scenario with fewer locked pieces to simulate mid-game state
        test_locked = [[False for _ in range(6)] for _ in range(6)]

        # Lock a minimal set of pieces (corners and some strategic positions)
        strategic_locks = [(0, 0), (0, 5), (5, 0), (5, 5), (2, 2), (3, 3)]
        for row, col in strategic_locks:
            test_locked[row][col] = True

        # Test solver to find positions with multiple valid options
        test_solver = TangoBoardSolver(
            board,
            [[ConstraintType.NONE for _ in range(5)] for _ in range(6)],
            [[ConstraintType.NONE for _ in range(6)] for _ in range(5)],
            test_locked,
        )

        # Create test board with only locked pieces
        test_board = [[PieceType.EMPTY for _ in range(6)] for _ in range(6)]
        for row in range(6):
            for col in range(6):
                if test_locked[row][col]:
                    test_board[row][col] = board[row][col]

        # Check each empty position for ambiguity
        for row in range(6):
            for col in range(6):
                if not test_locked[row][col]:
                    valid_pieces = []

                    for piece in [PieceType.SUN, PieceType.MOON]:
                        test_board[row][col] = piece
                        if test_solver._is_valid_by_game_rules_only(
                            test_board, row, col
                        ):
                            valid_pieces.append(piece)
                        test_board[row][col] = PieceType.EMPTY

                    # If both pieces are valid, this position might be difficult
                    if len(valid_pieces) == 2:
                        difficult_positions.append((row, col))

        return difficult_positions

    def _ensure_unique_solution(
        self,
        board: list[list[PieceType]],
        h_constraints: list[list[ConstraintType]],
        v_constraints: list[list[ConstraintType]],
        locked_tiles: list[list[bool]],
        config,
    ) -> tuple[list[list[ConstraintType]], list[list[ConstraintType]]]:
        """Ensure the puzzle has a unique solution using a comprehensive board solver."""
        print("🔍 Analyzing puzzle solutions...")

        # Use the board solver to find all solutions
        solver = TangoBoardSolver(board, h_constraints, v_constraints, locked_tiles)
        solutions = solver.find_all_solutions(max_solutions=10)

        print(f"📊 Found {len(solutions)} possible solutions")

        if len(solutions) == 0:
            print("❌ No valid solutions found - puzzle is unsolvable")
            return h_constraints, v_constraints

        if len(solutions) == 1:
            print("✅ Puzzle already has unique solution")
            # Even if unique, optimize constraints for better inference-based gameplay
            print("🔧 Optimizing constraints for inference-based solving...")
            h_constraints, v_constraints = solver.remove_redundant_constraints(
                board, h_constraints, v_constraints, locked_tiles
            )
            h_constraints, v_constraints = solver.optimize_constraints_for_inference(
                board, h_constraints, v_constraints, locked_tiles
            )
            return h_constraints, v_constraints

        if len(solutions) > 1:
            print(f"⚠️  Multiple solutions detected, adding strategic constraints...")

            # Add constraints to eliminate alternative solutions
            h_constraints, v_constraints = solver.add_disambiguating_constraints(
                solutions, h_constraints, v_constraints, max_constraints=5
            )

            # Remove redundant constraints after disambiguation
            h_constraints, v_constraints = solver.remove_redundant_constraints(
                board, h_constraints, v_constraints, locked_tiles
            )

            # Optimize for inference-based solving
            h_constraints, v_constraints = solver.optimize_constraints_for_inference(
                board, h_constraints, v_constraints, locked_tiles
            )

            # Verify the constraints worked
            new_solver = TangoBoardSolver(
                board, h_constraints, v_constraints, locked_tiles
            )
            new_solutions = new_solver.find_all_solutions(max_solutions=5)
            print(f"🎯 After optimization: {len(new_solutions)} solutions")

        return h_constraints, v_constraints

    def _remove_pieces_intelligently(
        self, board: list[list[PieceType]], config
    ) -> list[list[bool]]:
        """Remove pieces intelligently to create challenging but solvable puzzles with unique solutions."""
        import random

        locked_tiles = [[False for _ in range(6)] for _ in range(6)]

        # Calculate target number of starting pieces
        target_pieces = random.randint(
            config.starting_pieces_min, config.starting_pieces_max
        )

        # Strategy: Keep pieces that create constraints and reduce ambiguity
        piece_importance = []

        for row in range(6):
            for col in range(6):
                importance = 0

                # Corner pieces are strategically important
                if (row, col) in [(0, 0), (0, 5), (5, 0), (5, 5)]:
                    importance += 5

                # Edge pieces help with balance constraints
                elif row == 0 or row == 5 or col == 0 or col == 5:
                    importance += 3

                # Pieces that would create natural constraint points
                # Check if this piece differs from its neighbors
                neighbors_differ = 0
                for dr, dc in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
                    nr, nc = row + dr, col + dc
                    if 0 <= nr < 6 and 0 <= nc < 6:
                        if board[row][col] != board[nr][nc]:
                            neighbors_differ += 1

                importance += (
                    neighbors_differ * 2
                )  # Pieces that differ from neighbors are more valuable

                # Pieces that help with row/column balance are important
                # Count how many of same type in row/column
                row_same = sum(1 for c in range(6) if board[row][c] == board[row][col])
                col_same = sum(1 for r in range(6) if board[r][col] == board[row][col])

                # If this piece helps balance (not too many of same type), it's important
                if row_same <= 3 and col_same <= 3:
                    importance += 1

                # Pieces in the middle that break patterns are valuable
                if 1 <= row <= 4 and 1 <= col <= 4:
                    # Check if this piece breaks a potential pattern
                    pattern_breaker = 0
                    if col > 0 and col < 5:  # Can check horizontal pattern
                        if (
                            board[row][col - 1] == board[row][col + 1]
                            and board[row][col] != board[row][col - 1]
                        ):
                            pattern_breaker += 2
                    if row > 0 and row < 5:  # Can check vertical pattern
                        if (
                            board[row - 1][col] == board[row + 1][col]
                            and board[row][col] != board[row - 1][col]
                        ):
                            pattern_breaker += 2
                    importance += pattern_breaker

                # Add some randomness to avoid completely predictable patterns
                importance += random.random() * 2

                piece_importance.append((importance, row, col))

        # Sort by importance and keep the most important pieces
        piece_importance.sort(reverse=True)

        pieces_to_lock = min(target_pieces, len(piece_importance))

        # Lock the most strategically important pieces
        for i in range(pieces_to_lock):
            _, row, col = piece_importance[i]
            locked_tiles[row][col] = True

        # Clear non-locked pieces
        for row in range(6):
            for col in range(6):
                if not locked_tiles[row][col]:
                    board[row][col] = PieceType.EMPTY

        print(f"Locked {pieces_to_lock} strategically important pieces")
        return locked_tiles

    def _validate_puzzle_completely(
        self,
        board: list[list[PieceType]],
        h_constraints: list[list[ConstraintType]],
        v_constraints: list[list[ConstraintType]],
        locked_tiles: list[list[bool]],
    ) -> bool:
        """Comprehensive validation of the generated puzzle."""

        # Check no consecutive constraint violations
        for row in range(6):
            consecutive = 0
            for col in range(5):
                if h_constraints[row][col] != ConstraintType.NONE:
                    consecutive += 1
                    if consecutive >= 3:
                        print(
                            f"Too many consecutive horizontal constraints at row {row}"
                        )
                        return False
                else:
                    consecutive = 0

        for col in range(6):
            consecutive = 0
            for row in range(5):
                if v_constraints[row][col] != ConstraintType.NONE:
                    consecutive += 1
                    if consecutive >= 3:
                        print(f"Too many consecutive vertical constraints at col {col}")
                        return False
                else:
                    consecutive = 0

        # Check that locked pieces don't violate basic rules
        for row in range(6):
            for col in range(6):
                if locked_tiles[row][col]:
                    if board[row][col] == PieceType.EMPTY:
                        print(f"Locked tile at ({row}, {col}) is empty")
                        return False

        # Check that puzzle has a reasonable number of starting pieces
        starting_pieces = sum(sum(row) for row in locked_tiles)
        if starting_pieces < 2:
            print(f"Too few starting pieces: {starting_pieces}")
            return False

        # Check for unique solution (optional but preferred)
        temp_solver = TangoBoardSolver(
            board, h_constraints, v_constraints, locked_tiles
        )
        solutions = temp_solver.find_all_solutions(max_solutions=3)
        solution_count = len(solutions)
        if solution_count == 0:
            print("Puzzle has no solutions")
            return False
        elif solution_count == 1:
            print("✓ Puzzle has unique solution")
        else:
            print(
                f"⚠ Puzzle has {solution_count} solutions (not unique but acceptable)"
            )

        print("Puzzle passed comprehensive validation")
        return True

    def _create_basic_valid_puzzle(self, game_id: str, config) -> GameState:
        """Create a basic but valid puzzle as final fallback."""
        # Create a simple valid alternating pattern
        board = [[PieceType.EMPTY for _ in range(6)] for _ in range(6)]

        # Use one of our validated patterns
        valid_pattern = [
            [
                PieceType.SUN,
                PieceType.MOON,
                PieceType.SUN,
                PieceType.MOON,
                PieceType.SUN,
                PieceType.MOON,
            ],
            [
                PieceType.MOON,
                PieceType.SUN,
                PieceType.MOON,
                PieceType.SUN,
                PieceType.MOON,
                PieceType.SUN,
            ],
            [
                PieceType.SUN,
                PieceType.MOON,
                PieceType.SUN,
                PieceType.MOON,
                PieceType.SUN,
                PieceType.MOON,
            ],
            [
                PieceType.MOON,
                PieceType.SUN,
                PieceType.MOON,
                PieceType.SUN,
                PieceType.MOON,
                PieceType.SUN,
            ],
            [
                PieceType.SUN,
                PieceType.MOON,
                PieceType.SUN,
                PieceType.MOON,
                PieceType.SUN,
                PieceType.MOON,
            ],
            [
                PieceType.MOON,
                PieceType.SUN,
                PieceType.MOON,
                PieceType.SUN,
                PieceType.MOON,
                PieceType.SUN,
            ],
        ]

        # No constraints to avoid issues
        h_constraints = [[ConstraintType.NONE for _ in range(5)] for _ in range(6)]
        v_constraints = [[ConstraintType.NONE for _ in range(6)] for _ in range(5)]

        # Lock a few strategic pieces
        locked_tiles = [[False for _ in range(6)] for _ in range(6)]
        import random

        lock_positions = [(0, 0), (0, 5), (5, 0), (5, 5)]  # corners
        num_to_lock = min(len(lock_positions), config.starting_pieces_max // 2)

        for i in range(num_to_lock):
            row, col = lock_positions[i]
            locked_tiles[row][col] = True
            board[row][col] = valid_pattern[row][col]

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

        # Store the game state before returning
        self.game_states[game_id] = game_state
        self.active_games[game_id] = SimpleGameLogic(game_state)

        print(f"Created basic valid puzzle with {num_to_lock} locked pieces")
        return game_state

    def get_game(self, game_id: str) -> Optional[GameState]:
        """Get the current state of a game."""
        return self.game_states.get(game_id)

    def get_hint(self, game_id: str) -> dict:
        """Get a hint for the next logical move in the game."""
        if game_id not in self.game_states:
            return {"found": False, "error": "Game not found"}

        game_state = self.game_states[game_id]

        if game_state.is_complete:
            return {"found": False, "reasoning": "Puzzle is already complete!"}

        # Create current board state including user moves
        current_board = [row[:] for row in game_state.board]

        # Create updated locked tiles that includes user moves
        # We need to track both original locked pieces AND user-placed pieces
        current_locked_tiles = [[False for _ in range(6)] for _ in range(6)]

        for row in range(6):
            for col in range(6):
                # A tile is "locked" if it was originally locked OR if the user has placed a piece there
                if (
                    game_state.locked_tiles[row][col]
                    or game_state.board[row][col] != PieceType.EMPTY
                ):
                    current_locked_tiles[row][col] = True

        # Create solver with current state
        solver = TangoBoardSolver(
            current_board,
            game_state.h_constraints,
            game_state.v_constraints,
            current_locked_tiles,  # Use the updated locked tiles that include user moves
        )

        # Get hint from solver
        hint = solver.get_hint()

        # Add some debugging info
        empty_count = sum(
            1
            for row in range(6)
            for col in range(6)
            if game_state.board[row][col] == PieceType.EMPTY
        )
        print(f"DEBUG: Game {game_id} has {empty_count} empty cells")
        print(f"DEBUG: Hint result: {hint}")

        return hint

    def make_move(
        self, game_id: str, row: int, col: int, piece_type: PieceType
    ) -> Tuple[bool, str, Optional[GameState], Optional[List[str]]]:
        """Make a move in the specified game."""
        if game_id not in self.game_states:
            return False, "Game not found", None, None

        game_state = self.game_states[game_id]
        game_logic = self.active_games[game_id]

        # Check if tile is locked
        if game_state.locked_tiles[row][col]:
            return False, "Cannot modify locked tile", None, None

        # Check bounds
        if not (0 <= row < 6 and 0 <= col < 6):
            return False, "Invalid position", None, None

        # Update the board
        game_state.board[row][col] = piece_type
        game_state.moves_count += 1

        # Validate the current state and get detailed errors
        validation_result = self.validate_game(game_id)

        # Check if game is complete (all tiles filled and valid)
        game_state.is_complete = (
            self._check_if_complete(game_state) and validation_result.is_valid
        )

        if game_state.is_complete:
            game_state.completion_time = datetime.now()
            # Add to leaderboard
            self._add_to_leaderboard(game_state)

        self.game_states[game_id] = game_state

        return True, "Move successful", game_state, validation_result.errors

    def _check_if_complete(self, game_state: GameState) -> bool:
        """Check if the game is complete (all tiles filled and rules satisfied)."""
        # First check if all tiles are filled
        for row in game_state.board:
            for cell in row:
                if cell == PieceType.EMPTY:
                    return False

        # Then check if the solution is valid
        validation_result = self.validate_game(game_state.game_id)
        return validation_result.is_valid

    def validate_game(self, game_id: str) -> GameValidationResult:
        """Validate the current state of a game with comprehensive rule checking."""
        if game_id not in self.game_states:
            return GameValidationResult(
                is_valid=False, errors=["Game not found"], is_complete=False
            )

        game_state = self.game_states[game_id]
        errors = []

        # Rule 1: Check row/column piece counts (each should have exactly 3 suns and 3 moons when complete)
        for i in range(6):
            suns_row = sum(1 for piece in game_state.board[i] if piece == PieceType.SUN)
            moons_row = sum(
                1 for piece in game_state.board[i] if piece == PieceType.MOON
            )
            suns_col = sum(
                1 for row in range(6) if game_state.board[row][i] == PieceType.SUN
            )
            moons_col = sum(
                1 for row in range(6) if game_state.board[row][i] == PieceType.MOON
            )

            # Check for too many pieces (more than 3)
            if suns_row > 3:
                errors.append(f"Row {i+1} has too many suns ({suns_row}/3)")
            if moons_row > 3:
                errors.append(f"Row {i+1} has too many moons ({moons_row}/3)")
            if suns_col > 3:
                errors.append(f"Column {i+1} has too many suns ({suns_col}/3)")
            if moons_col > 3:
                errors.append(f"Column {i+1} has too many moons ({moons_col}/3)")

        # Rule 2: Check for three consecutive pieces of the same type
        for row in range(6):
            for col in range(4):  # Check positions 0-3 for start of 3-consecutive
                if (
                    game_state.board[row][col] != PieceType.EMPTY
                    and game_state.board[row][col]
                    == game_state.board[row][col + 1]
                    == game_state.board[row][col + 2]
                ):
                    piece_name = (
                        "suns"
                        if game_state.board[row][col] == PieceType.SUN
                        else "moons"
                    )
                    errors.append(
                        f"Row {row+1} has three consecutive {piece_name} at columns {col+1}-{col+3}"
                    )

        for col in range(6):
            for row in range(4):  # Check positions 0-3 for start of 3-consecutive
                if (
                    game_state.board[row][col] != PieceType.EMPTY
                    and game_state.board[row][col]
                    == game_state.board[row + 1][col]
                    == game_state.board[row + 2][col]
                ):
                    piece_name = (
                        "suns"
                        if game_state.board[row][col] == PieceType.SUN
                        else "moons"
                    )
                    errors.append(
                        f"Column {col+1} has three consecutive {piece_name} at rows {row+1}-{row+3}"
                    )

        # Rule 3: Check constraint violations
        # Horizontal constraints
        for row in range(6):
            for col in range(5):  # 5 horizontal constraints per row
                constraint = game_state.h_constraints[row][col]
                if constraint != ConstraintType.NONE:
                    piece1 = game_state.board[row][col]
                    piece2 = game_state.board[row][col + 1]

                    if piece1 != PieceType.EMPTY and piece2 != PieceType.EMPTY:
                        if constraint == ConstraintType.SAME and piece1 != piece2:
                            errors.append(
                                f"Constraint violation: Row {row+1}, columns {col+1}-{col+2} must be the same"
                            )
                        elif (
                            constraint == ConstraintType.DIFFERENT and piece1 == piece2
                        ):
                            errors.append(
                                f"Constraint violation: Row {row+1}, columns {col+1}-{col+2} must be different"
                            )

        # Vertical constraints
        for row in range(5):  # 5 vertical constraint rows
            for col in range(6):
                constraint = game_state.v_constraints[row][col]
                if constraint != ConstraintType.NONE:
                    piece1 = game_state.board[row][col]
                    piece2 = game_state.board[row + 1][col]

                    if piece1 != PieceType.EMPTY and piece2 != PieceType.EMPTY:
                        if constraint == ConstraintType.SAME and piece1 != piece2:
                            errors.append(
                                f"Constraint violation: Column {col+1}, rows {row+1}-{row+2} must be the same"
                            )
                        elif (
                            constraint == ConstraintType.DIFFERENT and piece1 == piece2
                        ):
                            errors.append(
                                f"Constraint violation: Column {col+1}, rows {row+1}-{row+2} must be different"
                            )

        # Check if game is complete (all tiles filled)
        is_complete = all(
            game_state.board[row][col] != PieceType.EMPTY
            for row in range(6)
            for col in range(6)
        )

        return GameValidationResult(
            is_valid=len(errors) == 0,
            errors=errors,
            is_complete=is_complete and len(errors) == 0,
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

    def delete_game(self, game_id: str) -> bool:
        """Delete a game to free memory."""
        if game_id in self.game_states:
            del self.game_states[game_id]
            del self.active_games[game_id]
            return True
        return False


class SimpleGameLogic:
    """Simplified game logic for initial implementation."""

    def __init__(self, game_state: GameState):
        self.game_state = game_state
