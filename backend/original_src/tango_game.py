"""
Main Tango game implementation using pygame.
"""

import pygame
import json
import time
from datetime import datetime
from pathlib import Path
from typing import Optional, Tuple

from .game_logic import GameLogic, PieceType, ConstraintType
from .puzzle_generator import PuzzleGenerator
from .config import (
    WHITE,
    BLACK,
    GRAY,
    LIGHT_GRAY,
    DARK_GRAY,
    BLUE,
    RED,
    YELLOW,
    GREEN,
    ORANGE,
    BOARD_SIZE,
    CELL_SIZE,
    CONSTRAINT_SIZE,
    MARGIN,
    LEFT_PANEL_WIDTH,
    SIDEBAR_WIDTH,
    MAIN_FONT_SIZE,
    SMALL_FONT_SIZE,
    ENCODING,
    MAX_LEADERBOARD_ENTRIES,
    LEADERBOARD_FILENAME,
)


class TangoGame:
    """Main Tango game class using pygame."""

    # colors
    WHITE = WHITE
    BLACK = BLACK
    GRAY = GRAY
    GREEN = GREEN
    LIGHT_GRAY = LIGHT_GRAY
    DARK_GRAY = DARK_GRAY
    BLUE = BLUE
    RED = RED
    YELLOW = YELLOW
    ORANGE = ORANGE

    # game constants
    BOARD_SIZE = BOARD_SIZE
    CELL_SIZE = CELL_SIZE
    CONSTRAINT_SIZE = CONSTRAINT_SIZE
    MARGIN = MARGIN
    LEFT_PANEL_WIDTH = LEFT_PANEL_WIDTH
    SIDEBAR_WIDTH = SIDEBAR_WIDTH
    MAIN_FONT_SIZE = MAIN_FONT_SIZE
    SMALL_FONT_SIZE = SMALL_FONT_SIZE
    ENCODING = ENCODING
    MAX_LEADERBOARD_ENTRIES = MAX_LEADERBOARD_ENTRIES
    LEADERBOARD_FILENAME = LEADERBOARD_FILENAME

    def __init__(self):
        pygame.init()

        # Calculate window size
        board_width = (
            BOARD_SIZE * CELL_SIZE + (BOARD_SIZE - 1) * CONSTRAINT_SIZE + 2 * MARGIN
        )
        board_height = (
            BOARD_SIZE * CELL_SIZE + (BOARD_SIZE - 1) * CONSTRAINT_SIZE + 2 * MARGIN
        )

        self.window_width = LEFT_PANEL_WIDTH + board_width + SIDEBAR_WIDTH + 4 * MARGIN
        self.window_height = board_height + 2 * MARGIN

        self.screen = pygame.display.set_mode((self.window_width, self.window_height))
        pygame.display.set_caption("Tango Puzzle Game")

        # Game state
        self.game_logic = GameLogic()
        self.generator = PuzzleGenerator()
        self.clock = pygame.time.Clock()
        self.running = True
        self.start_time = time.time()
        self.completion_time = None  # Track completion time
        self.font = pygame.font.Font(None, MAIN_FONT_SIZE)
        self.small_font = pygame.font.Font(None, SMALL_FONT_SIZE)

        # Click tracking for triple click
        self.last_click_time = 0
        self.last_click_pos = None
        self.click_count = 0

        # Leaderboard
        self.leaderboard_file = Path(LEADERBOARD_FILENAME)
        self.leaderboard = self.load_leaderboard()

        # Generate initial puzzle
        self.new_game()

    def load_leaderboard(self) -> list:
        """Load leaderboard from file."""
        if self.leaderboard_file.exists():
            try:
                with open(self.leaderboard_file, "r", encoding=ENCODING) as f:
                    return json.load(f)
            except (json.JSONDecodeError, IOError):
                return []
        return []

    def save_leaderboard(self):
        """Save leaderboard to file."""
        try:
            with open(self.leaderboard_file, "w", encoding=ENCODING) as f:
                json.dump(self.leaderboard, f, indent=2)
        except IOError:
            pass  # Silently fail if can't save

    def add_to_leaderboard(self, time_taken: float):
        """Add a new time to the leaderboard."""
        entry = {
            "time": time_taken,
            "date": datetime.now().isoformat(),
            "formatted_time": self.format_time(time_taken),
        }
        self.leaderboard.append(entry)
        self.leaderboard.sort(key=lambda x: x["time"])
        self.leaderboard = self.leaderboard[
            :MAX_LEADERBOARD_ENTRIES
        ]  # Keep top entries
        self.save_leaderboard()

    def format_time(self, seconds: float) -> str:
        """Format time in MM:SS format."""
        minutes = int(seconds // 60)
        seconds = int(seconds % 60)
        return f"{minutes:02d}:{seconds:02d}"

    def new_game(self):
        """Start a new game."""
        self.game_logic = self.generator.generate_puzzle()
        self.start_time = time.time()
        self.completion_time = None  # Reset completion time

    def get_cell_at_pos(self, pos: Tuple[int, int]) -> Optional[Tuple[int, int]]:
        """Get the cell coordinates at the given screen position."""
        x, y = pos

        # Account for left panel and margin
        board_start_x = LEFT_PANEL_WIDTH + MARGIN
        x -= board_start_x
        y -= MARGIN

        if x < 0 or y < 0:
            return None

        # Calculate which cell was clicked
        cell_x = 0
        cell_y = 0
        current_x = 0
        current_y = 0

        # Find column
        for col in range(BOARD_SIZE):
            if current_x <= x < current_x + CELL_SIZE:
                cell_x = col
                break
            current_x += CELL_SIZE
            if col < BOARD_SIZE - 1:  # Add constraint space except after last column
                current_x += CONSTRAINT_SIZE
        else:
            return None

        # Find row
        for row in range(BOARD_SIZE):
            if current_y <= y < current_y + CELL_SIZE:
                cell_y = row
                break
            current_y += CELL_SIZE
            if row < BOARD_SIZE - 1:  # Add constraint space except after last row
                current_y += CONSTRAINT_SIZE
        else:
            return None

        return (cell_y, cell_x)

    def handle_click(self, pos: Tuple[int, int]):
        """Handle mouse click on the board."""
        cell = self.get_cell_at_pos(pos)
        if not cell:
            return

        row, col = cell

        # Check if the tile is locked (cannot be changed)
        if self.game_logic.is_tile_locked(row, col):
            return  # Don't allow changes to locked tiles

        # Get current piece and cycle to next one
        current_piece = self.game_logic.get_piece(row, col)

        if current_piece == PieceType.EMPTY:
            # Empty -> Sun
            self.game_logic.place_piece(row, col, PieceType.SUN)
        elif current_piece == PieceType.SUN:
            # Sun -> Moon
            self.game_logic.place_piece(row, col, PieceType.MOON)
        else:  # PieceType.MOON
            # Moon -> Empty
            self.game_logic.place_piece(row, col, PieceType.EMPTY)

        # Check if puzzle is complete
        if self.game_logic.is_complete():
            if self.completion_time is None:  # First time completing
                self.completion_time = time.time() - self.start_time
                self.add_to_leaderboard(self.completion_time)

    def draw_piece(self, surface, x: int, y: int, piece: PieceType, size: int):
        """Draw a piece (sun or moon) at the given position."""
        center_x = x + size // 2
        center_y = y + size // 2
        radius = size // 4  # Reduced size for sun

        if piece == PieceType.SUN:
            # Draw sun
            pygame.draw.circle(surface, self.YELLOW, (center_x, center_y), radius)
            pygame.draw.circle(surface, self.ORANGE, (center_x, center_y), radius, 2)

            # Draw sun rays
            for angle in range(0, 360, 45):
                import math

                start_x = center_x + int((radius + 3) * math.cos(math.radians(angle)))
                start_y = center_y + int((radius + 3) * math.sin(math.radians(angle)))
                end_x = center_x + int((radius + 8) * math.cos(math.radians(angle)))
                end_y = center_y + int((radius + 8) * math.sin(math.radians(angle)))
                pygame.draw.line(
                    surface, self.ORANGE, (start_x, start_y), (end_x, end_y), 2
                )

        elif piece == PieceType.MOON:
            # Draw moon (semi-circle) - blue with larger white circle
            pygame.draw.circle(surface, self.BLUE, (center_x, center_y), radius)
            # Larger white circle to create a proper crescent/semi-circle effect
            white_radius = int(radius * 0.8)
            white_offset = radius // 4
            pygame.draw.circle(
                surface, self.WHITE, (center_x + white_offset, center_y), white_radius
            )

    def draw_constraint(
        self,
        surface,
        x: int,
        y: int,
        constraint: ConstraintType,
        horizontal: bool = True,
    ):
        """Draw a constraint symbol."""
        if constraint == ConstraintType.NONE:
            return

        if horizontal:
            center_x = x + self.CONSTRAINT_SIZE // 2
            center_y = y + self.CELL_SIZE // 2
        else:
            center_x = x + self.CELL_SIZE // 2
            center_y = y + self.CONSTRAINT_SIZE // 2

        if constraint == ConstraintType.SAME:
            # Draw '=' symbol
            pygame.draw.line(
                surface,
                self.BLACK,
                (center_x - 8, center_y - 3),
                (center_x + 8, center_y - 3),
                3,
            )
            pygame.draw.line(
                surface,
                self.BLACK,
                (center_x - 8, center_y + 3),
                (center_x + 8, center_y + 3),
                3,
            )
        elif constraint == ConstraintType.DIFFERENT:
            # Draw 'X' symbol
            pygame.draw.line(
                surface,
                self.RED,
                (center_x - 8, center_y - 8),
                (center_x + 8, center_y + 8),
                3,
            )
            pygame.draw.line(
                surface,
                self.RED,
                (center_x - 8, center_y + 8),
                (center_x + 8, center_y - 8),
                3,
            )

    def draw_board(self):
        """Draw the game board."""
        # Draw board background - positioned after left panel
        board_x = self.LEFT_PANEL_WIDTH + self.MARGIN
        board_y = self.MARGIN

        current_y = board_y

        for row in range(6):
            current_x = board_x

            for col in range(6):
                # Draw cell
                cell_rect = pygame.Rect(
                    current_x, current_y, self.CELL_SIZE, self.CELL_SIZE
                )

                # Check if this cell has violations
                violations = self.game_logic.check_constraint_violations()
                has_violation = any(v[0] == row and v[1] == col for v in violations)

                # Check if this cell is locked
                is_locked = self.game_logic.is_tile_locked(row, col)

                if has_violation:
                    pygame.draw.rect(self.screen, self.RED, cell_rect)
                elif not self.game_logic.is_valid_state():
                    pygame.draw.rect(self.screen, (255, 200, 200), cell_rect)
                elif is_locked:
                    # Locked tiles get a light gray background
                    pygame.draw.rect(self.screen, (230, 230, 230), cell_rect)
                else:
                    pygame.draw.rect(self.screen, self.WHITE, cell_rect)

                pygame.draw.rect(self.screen, self.BLACK, cell_rect, 2)

                # Draw piece
                piece = self.game_logic.get_piece(row, col)
                if piece != PieceType.EMPTY:
                    self.draw_piece(
                        self.screen, current_x, current_y, piece, self.CELL_SIZE
                    )

                current_x += self.CELL_SIZE

                # Draw horizontal constraint
                if col < 5:
                    constraint = self.game_logic.get_horizontal_constraint(row, col)
                    self.draw_constraint(
                        self.screen, current_x, current_y, constraint, True
                    )
                    current_x += self.CONSTRAINT_SIZE

            current_y += self.CELL_SIZE

            # Draw vertical constraints
            if row < 5:
                current_x = board_x
                for col in range(6):
                    constraint = self.game_logic.get_vertical_constraint(row, col)
                    self.draw_constraint(
                        self.screen, current_x, current_y, constraint, False
                    )
                    current_x += self.CELL_SIZE
                    if col < 5:
                        current_x += self.CONSTRAINT_SIZE

                current_y += self.CONSTRAINT_SIZE

    def draw_sidebar(self):
        """Draw the sidebar with game info."""
        sidebar_x = (
            self.LEFT_PANEL_WIDTH
            + self.MARGIN * 2
            + 6 * self.CELL_SIZE
            + 5 * self.CONSTRAINT_SIZE
        )

        # Draw timer
        if self.completion_time is not None:
            # Game is complete, show completion time
            time_text = self.font.render(
                f"Time: {self.format_time(self.completion_time)}", True, self.GREEN
            )
        else:
            # Game in progress, show current elapsed time
            elapsed_time = time.time() - self.start_time
            time_text = self.font.render(
                f"Time: {self.format_time(elapsed_time)}", True, self.BLACK
            )
        self.screen.blit(time_text, (sidebar_x, 50))

        # Draw completion status
        if self.game_logic.is_complete():
            status_text = self.font.render("COMPLETE!", True, self.GREEN)
        elif self.game_logic.is_valid_state():
            status_text = self.font.render("Valid", True, self.GREEN)
        else:
            status_text = self.font.render("Invalid", True, self.RED)
        self.screen.blit(status_text, (sidebar_x, 100))

        # Draw leaderboard
        y_offset = 150
        board_title = self.font.render("Best Times:", True, self.BLACK)
        self.screen.blit(board_title, (sidebar_x, y_offset))

        for i, entry in enumerate(self.leaderboard[:5]):
            if i < 5:
                time_text = self.small_font.render(
                    f"{i+1}. {entry['formatted_time']}", True, self.BLACK
                )
                self.screen.blit(time_text, (sidebar_x, y_offset + 30 + i * 20))

    def draw_buttons(self):
        """Draw the control buttons in the left panel."""
        # Position buttons in the left panel
        button_x = self.MARGIN
        button_y_start = self.MARGIN
        button_width = 140  # Increased width
        button_height = 50  # Increased height

        # New Game button
        new_game_rect = pygame.Rect(
            button_x, button_y_start, button_width, button_height
        )
        pygame.draw.rect(self.screen, self.LIGHT_GRAY, new_game_rect)
        pygame.draw.rect(self.screen, self.BLACK, new_game_rect, 2)

        button_text = self.font.render("New Game", True, self.BLACK)
        text_rect = button_text.get_rect(center=new_game_rect.center)
        self.screen.blit(button_text, text_rect)

        # Exit button
        exit_rect = pygame.Rect(
            button_x, button_y_start + button_height + 10, button_width, button_height
        )
        pygame.draw.rect(self.screen, self.LIGHT_GRAY, exit_rect)
        pygame.draw.rect(self.screen, self.BLACK, exit_rect, 2)

        button_text = self.font.render("Exit", True, self.BLACK)
        text_rect = button_text.get_rect(center=exit_rect.center)
        self.screen.blit(button_text, text_rect)

        return new_game_rect, exit_rect

    def handle_button_click(self, pos: Tuple[int, int], new_game_rect, exit_rect):
        """Handle clicks on buttons."""
        if new_game_rect.collidepoint(pos):
            self.new_game()
        elif exit_rect.collidepoint(pos):
            self.running = False

    def run(self):
        """Main game loop."""
        while self.running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    if event.button == 1:  # Left click
                        # Check if clicking on buttons first
                        new_game_rect, exit_rect = (
                            self.draw_buttons()
                        )  # Get button rects
                        if new_game_rect.collidepoint(
                            event.pos
                        ) or exit_rect.collidepoint(event.pos):
                            self.handle_button_click(
                                event.pos, new_game_rect, exit_rect
                            )
                        else:
                            self.handle_click(event.pos)
                elif event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_n:
                        self.new_game()
                    elif event.key == pygame.K_q or event.key == pygame.K_ESCAPE:
                        self.running = False

            # Clear screen
            self.screen.fill(self.WHITE)

            # Draw left panel background
            left_panel_rect = pygame.Rect(
                0, 0, self.LEFT_PANEL_WIDTH, self.window_height
            )
            pygame.draw.rect(self.screen, self.LIGHT_GRAY, left_panel_rect)
            pygame.draw.line(
                self.screen,
                self.BLACK,
                (self.LEFT_PANEL_WIDTH, 0),
                (self.LEFT_PANEL_WIDTH, self.window_height),
                2,
            )

            # Draw everything
            self.draw_board()
            self.draw_sidebar()
            new_game_rect, exit_rect = self.draw_buttons()

            # Draw instructions in the left panel
            instructions = [
                "Click to cycle:",
                "Empty -> Sun -> Moon",
                "",
                "N: New Game",
                "Q/Esc: Quit",
            ]

            instructions_y_start = 150
            for i, instruction in enumerate(instructions):
                text = self.small_font.render(instruction, True, self.BLACK)
                self.screen.blit(text, (self.MARGIN, instructions_y_start + i * 25))

            pygame.display.flip()
            self.clock.tick(60)

        pygame.quit()


def main():
    """Entry point for the game."""
    game = TangoGame()
    game.run()


if __name__ == "__main__":
    main()
