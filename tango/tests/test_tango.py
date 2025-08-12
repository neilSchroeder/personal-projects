"""
Test script for Tango game.
"""

import unittest
from src.game_logic import GameLogic, PieceType, ConstraintType
from src.puzzle_generator import PuzzleGenerator


class TestTangoGame(unittest.TestCase):
    """Test cases for Tango game logic."""

    def setUp(self):
        """Set up test fixtures."""
        self.game = GameLogic()

    def test_initial_board(self):
        """Test that the board starts empty."""
        for row in range(6):
            for col in range(6):
                self.assertEqual(self.game.get_piece(row, col), PieceType.EMPTY)

    def test_place_piece(self):
        """Test placing pieces on the board."""
        self.assertTrue(self.game.place_piece(0, 0, PieceType.SUN))
        self.assertEqual(self.game.get_piece(0, 0), PieceType.SUN)

        self.assertTrue(self.game.place_piece(0, 1, PieceType.MOON))
        self.assertEqual(self.game.get_piece(0, 1), PieceType.MOON)

    def test_count_pieces(self):
        """Test counting pieces in rows and columns."""
        # Place some pieces
        self.game.place_piece(0, 0, PieceType.SUN)
        self.game.place_piece(0, 1, PieceType.SUN)
        self.game.place_piece(0, 2, PieceType.MOON)

        suns, moons = self.game.count_pieces_in_row(0)
        self.assertEqual(suns, 2)
        self.assertEqual(moons, 1)

        suns, moons = self.game.count_pieces_in_column(0)
        self.assertEqual(suns, 1)
        self.assertEqual(moons, 0)

    def test_three_consecutive(self):
        """Test detection of three consecutive pieces."""
        # Place three consecutive suns in a row
        self.game.place_piece(0, 0, PieceType.SUN)
        self.game.place_piece(0, 1, PieceType.SUN)
        self.game.place_piece(0, 2, PieceType.SUN)

        self.assertTrue(self.game.has_three_consecutive_in_row(0))

        # Place three consecutive moons in a column
        self.game.place_piece(1, 0, PieceType.MOON)
        self.game.place_piece(2, 0, PieceType.MOON)
        self.game.place_piece(3, 0, PieceType.MOON)

        self.assertTrue(self.game.has_three_consecutive_in_column(0))

    def test_constraints(self):
        """Test constraint setting and checking."""
        # Set a SAME constraint
        self.game.set_horizontal_constraint(0, 0, ConstraintType.SAME)
        self.assertEqual(self.game.get_horizontal_constraint(0, 0), ConstraintType.SAME)

        # Place different pieces - should violate constraint
        self.game.place_piece(0, 0, PieceType.SUN)
        self.game.place_piece(0, 1, PieceType.MOON)

        violations = self.game.check_constraint_violations()
        self.assertTrue(len(violations) > 0)

    def test_puzzle_generator(self):
        """Test puzzle generation."""
        generator = PuzzleGenerator()
        puzzle = generator.generate_simple_puzzle()

        # Check that we have a valid game logic instance
        self.assertIsInstance(puzzle, GameLogic)

        # Check that some pieces are placed
        piece_count = 0
        for row in range(6):
            for col in range(6):
                if puzzle.get_piece(row, col) != PieceType.EMPTY:
                    piece_count += 1

        self.assertGreater(piece_count, 0)


def main():
    """Run the tests."""
    unittest.main()


if __name__ == "__main__":
    main()
