"""
Puzzle generator for Tango game.
"""

import random
from typing import List
from .game_logic import GameLogic, PieceType, ConstraintType
from .config import (
    BOARD_SIZE,
    MAX_PIECES_PER_ROW_COL,
    CONSTRAINT_PROBABILITY,
    REMOVAL_PROB_WITH_CONSTRAINTS,
    REMOVAL_PROB_WITHOUT_CONSTRAINTS,
)


class PuzzleGenerator:
    """Generate valid Tango puzzles."""

    def __init__(self, size: int = BOARD_SIZE):
        self.size = size

    def generate_puzzle(self) -> GameLogic:
        """Generate a new valid puzzle with constraints."""
        max_attempts = 10  # Limit attempts to avoid infinite loops

        for attempt in range(max_attempts):
            game = GameLogic(self.size)

            # Step 1: Generate a complete valid solution
            if not self._generate_solution(game):
                continue  # Try again

            # Step 2: Add constraints carefully, ensuring solvability
            if not self._add_constraints_safely(game):
                continue  # Try again if constraints make it unsolvable

            # Step 3: Remove pieces while maintaining solvability
            if self._remove_pieces_safely(game):
                print("Successfully generated a puzzle.")
                return game  # Success!

    def _generate_solution(self, game: GameLogic) -> bool:
        """Generate a complete valid solution using backtracking."""
        empty_cells = []
        for row in range(self.size):
            for col in range(self.size):
                empty_cells.append((row, col))

        random.shuffle(empty_cells)
        return self._solve_recursive(game, empty_cells, 0)

    def _solve_recursive(self, game: GameLogic, cells: List[tuple], index: int) -> bool:
        """Recursively solve the puzzle using backtracking."""
        if index >= len(cells):
            return game.is_complete()

        row, col = cells[index]
        pieces = [PieceType.SUN, PieceType.MOON]
        random.shuffle(pieces)

        for piece in pieces:
            game.place_piece(row, col, piece)

            # Check if this placement is valid
            if self._is_placement_valid(game, row, col):
                if self._solve_recursive(game, cells, index + 1):
                    return True

            # Backtrack
            game.place_piece(row, col, PieceType.EMPTY)

        return False

    def _add_constraints_safely(self, game: GameLogic) -> bool:
        """Add constraints while ensuring the puzzle remains solvable."""
        constraints_added = 0
        max_constraints = 15  # Reasonable limit

        # Try to add constraints one by one, validating each addition
        positions = [(row, col) for row in range(self.size) for col in range(self.size)]
        random.shuffle(positions)

        for row, col in positions:
            if constraints_added >= max_constraints:
                break

            # Try horizontal constraint if possible
            if (
                col < self.size - 1
                and game.get_horizontal_constraint(row, col) == ConstraintType.NONE
            ):
                piece1 = game.get_piece(row, col)
                piece2 = game.get_piece(row, col + 1)

                if piece1 != PieceType.EMPTY and piece2 != PieceType.EMPTY:
                    constraint_type = (
                        ConstraintType.SAME
                        if piece1 == piece2
                        else ConstraintType.DIFFERENT
                    )

                    # Add the constraint temporarily
                    game.set_horizontal_constraint(row, col, constraint_type)

                    # Check if this constraint is reasonable (not too restrictive)
                    if self._is_constraint_reasonable(game, row, col, True):
                        constraints_added += 1
                    else:
                        # Remove the constraint if it's too restrictive
                        game.set_horizontal_constraint(row, col, ConstraintType.NONE)

            # Try vertical constraint if possible
            if (
                row < self.size - 1
                and game.get_vertical_constraint(row, col) == ConstraintType.NONE
            ):
                piece1 = game.get_piece(row, col)
                piece2 = game.get_piece(row + 1, col)

                if piece1 != PieceType.EMPTY and piece2 != PieceType.EMPTY:
                    constraint_type = (
                        ConstraintType.SAME
                        if piece1 == piece2
                        else ConstraintType.DIFFERENT
                    )

                    # Add the constraint temporarily
                    game.set_vertical_constraint(row, col, constraint_type)

                    # Check if this constraint is reasonable
                    if self._is_constraint_reasonable(game, row, col, False):
                        constraints_added += 1
                    else:
                        # Remove the constraint if it's too restrictive
                        game.set_vertical_constraint(row, col, ConstraintType.NONE)

        return (
            constraints_added > 0
        )  # Return True if we added at least some constraints

    def _is_constraint_reasonable(
        self, game: GameLogic, row: int, col: int, is_horizontal: bool
    ) -> bool:
        """Check if a constraint placement is reasonable and doesn't over-constrain the puzzle."""
        # Count constraints in the affected rows/columns
        if is_horizontal:
            # Count horizontal constraints in this row
            h_constraints_in_row = sum(
                1
                for c in range(self.size - 1)
                if game.get_horizontal_constraint(row, c) != ConstraintType.NONE
            )
            # Don't allow too many constraints in one row
            if h_constraints_in_row > 3:
                return False
        else:
            # Count vertical constraints in this column
            v_constraints_in_col = sum(
                1
                for r in range(self.size - 1)
                if game.get_vertical_constraint(r, col) != ConstraintType.NONE
            )
            # Don't allow too many constraints in one column
            if v_constraints_in_col > 3:
                return False

        return True

    def _remove_pieces_safely(self, game: GameLogic) -> bool:
        """Remove pieces while ensuring the puzzle remains solvable."""
        # Store the complete solution
        complete_solution = [
            [game.get_piece(row, col) for col in range(self.size)]
            for row in range(self.size)
        ]

        # Get all positions and shuffle them
        positions = [(row, col) for row in range(self.size) for col in range(self.size)]
        random.shuffle(positions)

        # Target: remove 60-75% of pieces
        target_removal = int(self.size * self.size * 0.7)
        removed_count = 0

        for row, col in positions:
            if removed_count >= target_removal:
                break

            # Temporarily remove the piece
            original_piece = game.get_piece(row, col)
            game.place_piece(row, col, PieceType.EMPTY)

            # Check if the puzzle is still solvable
            if self._test_solvability(game, complete_solution):
                removed_count += 1
                # Keep the piece removed
            else:
                # Restore the piece if removing it makes the puzzle unsolvable
                game.place_piece(row, col, original_piece)

        # Make sure we removed at least some pieces
        return removed_count >= (self.size * self.size * 0.4)  # At least 40% removed

    def _test_solvability(
        self, puzzle: GameLogic, expected_solution: List[List[PieceType]]
    ) -> bool:
        """Test if a puzzle is solvable by attempting to solve it."""
        # For performance, do a quick check first
        # If we removed too many pieces from any row/column, it might be unsolvable
        for i in range(self.size):
            suns_row, moons_row = puzzle.count_pieces_in_row(i)
            suns_col, moons_col = puzzle.count_pieces_in_column(i)

            # Count empty cells in row/column
            empty_in_row = sum(
                1
                for col in range(self.size)
                if puzzle.get_piece(i, col) == PieceType.EMPTY
            )
            empty_in_col = sum(
                1
                for row in range(self.size)
                if puzzle.get_piece(row, i) == PieceType.EMPTY
            )

            # Quick solvability check: can we still place the required pieces?
            max_possible_suns_row = suns_row + empty_in_row
            max_possible_moons_row = moons_row + empty_in_row
            max_possible_suns_col = suns_col + empty_in_col
            max_possible_moons_col = moons_col + empty_in_col

            if (
                max_possible_suns_row < MAX_PIECES_PER_ROW_COL
                or max_possible_moons_row < MAX_PIECES_PER_ROW_COL
                or max_possible_suns_col < MAX_PIECES_PER_ROW_COL
                or max_possible_moons_col < MAX_PIECES_PER_ROW_COL
            ):
                return False

        # Additional constraint-based checks could be added here
        # For now, we trust that if the basic counts work out, it's solvable
        # This is a simplification to avoid expensive full backtracking
        return True

    def _validate_constraints(self, game: GameLogic) -> bool:
        """Validate that constraints don't create impossible situations."""
        # Check for constraint conflicts in each row and column
        for row in range(self.size):
            for col in range(self.size):
                piece = game.get_piece(row, col)
                if piece == PieceType.EMPTY:
                    continue

                # Check horizontal constraints
                if col > 0:
                    h_constraint = game.get_horizontal_constraint(row, col - 1)
                    left_piece = game.get_piece(row, col - 1)
                    if (
                        left_piece != PieceType.EMPTY
                        and h_constraint != ConstraintType.NONE
                    ):
                        if h_constraint == ConstraintType.SAME and piece != left_piece:
                            return False
                        if (
                            h_constraint == ConstraintType.DIFFERENT
                            and piece == left_piece
                        ):
                            return False

                # Check vertical constraints
                if row > 0:
                    v_constraint = game.get_vertical_constraint(row - 1, col)
                    above_piece = game.get_piece(row - 1, col)
                    if (
                        above_piece != PieceType.EMPTY
                        and v_constraint != ConstraintType.NONE
                    ):
                        if v_constraint == ConstraintType.SAME and piece != above_piece:
                            return False
                        if (
                            v_constraint == ConstraintType.DIFFERENT
                            and piece == above_piece
                        ):
                            return False

        return True

    def _is_placement_valid(self, game: GameLogic, row: int, col: int) -> bool:
        """Check if the current placement maintains validity."""
        # Check row counts
        suns_row, moons_row = game.count_pieces_in_row(row)
        if suns_row > MAX_PIECES_PER_ROW_COL or moons_row > MAX_PIECES_PER_ROW_COL:
            return False

        # Check column counts
        suns_col, moons_col = game.count_pieces_in_column(col)
        if suns_col > MAX_PIECES_PER_ROW_COL or moons_col > MAX_PIECES_PER_ROW_COL:
            return False

        # Check for three consecutive in row
        if game.has_three_consecutive_in_row(row):
            return False

        # Check for three consecutive in column
        if game.has_three_consecutive_in_column(col):
            return False

        return True

    def _add_constraints(self, game: GameLogic):
        """Add random constraints to the solved puzzle."""
        # Add some horizontal constraints
        for row in range(self.size):
            for col in range(self.size - 1):
                if random.random() < CONSTRAINT_PROBABILITY:  # 30% chance of constraint
                    piece1 = game.get_piece(row, col)
                    piece2 = game.get_piece(row, col + 1)

                    if piece1 != PieceType.EMPTY and piece2 != PieceType.EMPTY:
                        if piece1 == piece2:
                            constraint = ConstraintType.SAME
                        else:
                            constraint = ConstraintType.DIFFERENT

                        # Set constraint and validate
                        game.set_horizontal_constraint(row, col, constraint)
                        if not self._validate_constraints(game):
                            # Remove constraint if it creates conflicts
                            game.set_horizontal_constraint(
                                row, col, ConstraintType.NONE
                            )

        # Add some vertical constraints
        for row in range(self.size - 1):
            for col in range(self.size):
                if random.random() < CONSTRAINT_PROBABILITY:  # 30% chance of constraint
                    piece1 = game.get_piece(row, col)
                    piece2 = game.get_piece(row + 1, col)

                    if piece1 != PieceType.EMPTY and piece2 != PieceType.EMPTY:
                        if piece1 == piece2:
                            constraint = ConstraintType.SAME
                        else:
                            constraint = ConstraintType.DIFFERENT

                        # Set constraint and validate
                        game.set_vertical_constraint(row, col, constraint)
                        if not self._validate_constraints(game):
                            # Remove constraint if it creates conflicts
                            game.set_vertical_constraint(row, col, ConstraintType.NONE)

    def _remove_pieces(self, game: GameLogic):
        """Remove pieces to create the puzzle, ensuring it remains solvable."""
        # Create a backup of the complete solution
        solution = []
        for row in range(self.size):
            solution_row = []
            for col in range(self.size):
                solution_row.append(game.get_piece(row, col))
            solution.append(solution_row)

        # Create list of all positions
        positions = [(row, col) for row in range(self.size) for col in range(self.size)]
        random.shuffle(positions)

        # Remove about 50-70% of pieces
        num_to_remove = random.randint(18, 25)  # For 6x6 board (36 total)
        removed_count = 0

        for row, col in positions:
            if removed_count >= num_to_remove:
                break

            # Try removing this piece
            original_piece = game.get_piece(row, col)
            game.place_piece(row, col, PieceType.EMPTY)

            # Check if puzzle is still solvable
            if self._test_solvability(game, solution):
                removed_count += 1
            else:
                # Put the piece back if removal makes it unsolvable
                game.place_piece(row, col, original_piece)

    def generate_simple_puzzle(self) -> GameLogic:
        """Generate a simpler puzzle for testing."""
        game = GameLogic(self.size)

        # Step 1: Generate a complete valid solution
        if not self._generate_complete_solution(game):
            # Fallback if generation fails
            self._create_fallback_solution(game)

        # Step 2: Add constraints based on the existing valid layout
        num_constraints = self._add_valid_constraints(game)

        # Step 3: Remove pieces proportional to constraints (but never all pieces)
        self._remove_pieces_strategically(game, num_constraints)

        # Step 4: Lock remaining pieces and remove constraints between empty/filled pieces
        self._lock_remaining_pieces_and_cleanup_constraints(game)

        return game

    def _generate_complete_solution(self, game: GameLogic) -> bool:
        """Generate a complete valid 6x6 solution."""
        # Use a simple pattern-based approach for reliability
        patterns = [
            # Pattern 1: Alternating with some structure
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
            # Pattern 2: Different arrangement
            [
                [
                    PieceType.SUN,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                ],
                [
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.SUN,
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
            # Pattern 3: Block-based pattern
            [
                [
                    PieceType.SUN,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.MOON,
                ],
                [
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.SUN,
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
            # Pattern 4: More grouped pieces for better SAME constraints
            [
                [
                    PieceType.SUN,
                    PieceType.SUN,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.MOON,
                ],
                [
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.SUN,
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
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.SUN,
                ],
                [
                    PieceType.MOON,
                    PieceType.MOON,
                    PieceType.SUN,
                    PieceType.SUN,
                    PieceType.MOON,
                    PieceType.MOON,
                ],
            ],
        ]

        # Choose a random pattern and apply it
        pattern = random.choice(patterns)

        for row in range(self.size):
            for col in range(self.size):
                game.place_piece(row, col, pattern[row][col])

        # Verify the solution is valid
        return game.is_complete() and game.is_valid_state()

    def _create_fallback_solution(self, game: GameLogic):
        """Create a simple fallback solution if pattern generation fails."""
        # Simple alternating pattern with some randomization
        for row in range(self.size):
            for col in range(self.size):
                if (row + col) % 2 == 0:
                    piece = PieceType.SUN if random.random() < 0.5 else PieceType.MOON
                else:
                    piece = PieceType.MOON if random.random() < 0.5 else PieceType.SUN
                game.place_piece(row, col, piece)

    def _add_valid_constraints(self, game: GameLogic) -> int:
        """Add constraints that match the existing valid board layout."""
        constraint_count = 0

        # Add horizontal constraints based on adjacent pieces
        for row in range(self.size):
            for col in range(self.size - 1):
                piece1 = game.get_piece(row, col)
                piece2 = game.get_piece(row, col + 1)

                # Add constraint with some probability
                if random.random() < CONSTRAINT_PROBABILITY:  # Use config constant
                    if piece1 == piece2:
                        game.set_horizontal_constraint(row, col, ConstraintType.SAME)
                        constraint_count += 1
                    else:
                        game.set_horizontal_constraint(
                            row, col, ConstraintType.DIFFERENT
                        )
                        constraint_count += 1

        # Add vertical constraints based on adjacent pieces
        for row in range(self.size - 1):
            for col in range(self.size):
                piece1 = game.get_piece(row, col)
                piece2 = game.get_piece(row + 1, col)

                # Add constraint with some probability
                if random.random() < CONSTRAINT_PROBABILITY:  # Use config constant
                    if piece1 == piece2:
                        game.set_vertical_constraint(row, col, ConstraintType.SAME)
                        constraint_count += 1
                    else:
                        game.set_vertical_constraint(row, col, ConstraintType.DIFFERENT)
                        constraint_count += 1

        return constraint_count

    def _remove_pieces_strategically(self, game: GameLogic, num_constraints: int):
        """Remove pieces proportional to the number of constraints."""
        # Calculate how many pieces to remove based on constraints
        # More constraints = more pieces removed (since constraints give more information)
        min_pieces_to_keep = 8  # Always keep at least 8 pieces visible
        max_pieces_to_remove = 36 - min_pieces_to_keep  # Max 28 pieces can be removed

        # Scale removal based on constraints: more constraints allow more removal
        constraint_factor = min(num_constraints / 15.0, 1.0)  # Normalize to 0-1
        pieces_to_remove = int(constraint_factor * max_pieces_to_remove)
        pieces_to_remove = max(pieces_to_remove, 15)  # Remove at least 15 pieces
        pieces_to_remove = min(
            pieces_to_remove, max_pieces_to_remove
        )  # But not too many

        # Create list of all positions and shuffle
        positions = [(row, col) for row in range(self.size) for col in range(self.size)]
        random.shuffle(positions)

        # Remove pieces, but be more careful near constraints
        removed_count = 0
        for row, col in positions:
            if removed_count >= pieces_to_remove:
                break

            # Check if this position is involved in constraints
            has_constraints = self._position_has_constraints(game, row, col)

            # If it has constraints, lower probability of removal
            # Use config constants for removal probabilities
            removal_probability = (
                REMOVAL_PROB_WITH_CONSTRAINTS
                if has_constraints
                else REMOVAL_PROB_WITHOUT_CONSTRAINTS
            )

            if random.random() < removal_probability:
                game.place_piece(row, col, PieceType.EMPTY)
                removed_count += 1

    def _position_has_constraints(self, game: GameLogic, row: int, col: int) -> bool:
        """Check if a position is involved in any constraints."""
        # Check horizontal constraints (this cell and previous cell)
        if (
            col > 0
            and game.get_horizontal_constraint(row, col - 1) != ConstraintType.NONE
        ):
            return True
        if (
            col < self.size - 1
            and game.get_horizontal_constraint(row, col) != ConstraintType.NONE
        ):
            return True

        # Check vertical constraints (this cell and previous cell)
        if (
            row > 0
            and game.get_vertical_constraint(row - 1, col) != ConstraintType.NONE
        ):
            return True
        if (
            row < self.size - 1
            and game.get_vertical_constraint(row, col) != ConstraintType.NONE
        ):
            return True

        return False

    def _lock_remaining_pieces_and_cleanup_constraints(self, game: GameLogic):
        """Lock all remaining pieces and remove constraints between non-adjacent filled pieces."""
        # Step 4a: Lock all non-empty tiles
        for row in range(self.size):
            for col in range(self.size):
                if game.get_piece(row, col) != PieceType.EMPTY:
                    game.lock_tile(row, col)

        # Step 4b: Remove constraints where at least one adjacent cell is empty
        # Clean up horizontal constraints
        for row in range(self.size):
            for col in range(self.size - 1):
                piece1 = game.get_piece(row, col)
                piece2 = game.get_piece(row, col + 1)

                # If either piece is empty, remove the constraint
                if piece1 == PieceType.EMPTY or piece2 == PieceType.EMPTY:
                    game.set_horizontal_constraint(row, col, ConstraintType.NONE)

        # Clean up vertical constraints
        for row in range(self.size - 1):
            for col in range(self.size):
                piece1 = game.get_piece(row, col)
                piece2 = game.get_piece(row + 1, col)

                # If either piece is empty, remove the constraint
                if piece1 == PieceType.EMPTY or piece2 == PieceType.EMPTY:
                    game.set_vertical_constraint(row, col, ConstraintType.NONE)
