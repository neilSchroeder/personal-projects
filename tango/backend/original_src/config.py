"""
Configuration constants for the Tango puzzle game.
"""

# Game Logic Constants
BOARD_SIZE = 6
MAX_PIECES_PER_ROW_COL = 3  # Maximum suns or moons per row/column
CONSECUTIVE_LIMIT = 3  # Maximum consecutive same pieces allowed

# UI Layout Constants
CELL_SIZE = 70
MARGIN = 10
CONSTRAINT_SIZE = 20
BUTTON_HEIGHT = 40
SIDEBAR_WIDTH = 200
LEFT_PANEL_WIDTH = 220

# UI Spacing and Positioning
BUTTON_SPACING = 10
INSTRUCTIONS_Y_START = 150
SIDEBAR_Y_START = 50
LEADERBOARD_Y_START = 500
TIMER_Y_POSITION = 50
STATUS_Y_POSITION = 100

# Colors (RGB tuples)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GRAY = (128, 128, 128)
LIGHT_GRAY = (200, 200, 200)
DARK_GRAY = (64, 64, 64)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
YELLOW = (255, 255, 0)
ORANGE = (255, 165, 0)
PURPLE = (128, 0, 128)

# Game piece visual settings
SUN_RADIUS_RATIO = 3  # Cell size divided by this = sun radius
MOON_RADIUS_RATIO = 3  # Cell size divided by this = moon radius
MOON_INNER_RATIO = 2  # Moon radius divided by this = inner white circle radius
SUN_RAY_LENGTH = 15  # Length of sun rays
SUN_RAY_OFFSET = 5  # Distance from sun circle to ray start
SUN_RAY_ANGLES = 8  # Number of rays (360 degrees / this = angle between rays)

# Constraint visual settings
CONSTRAINT_SYMBOL_SIZE = 8  # Half-width/height of constraint symbols
CONSTRAINT_LINE_WIDTH = 3  # Thickness of constraint lines

# Font sizes
MAIN_FONT_SIZE = 36
SMALL_FONT_SIZE = 24

# Puzzle Generation Constants
CONSTRAINT_PROBABILITY = 0.25  # Probability of placing a constraint (0.0-1.0)
CONSTRAINT_PROBABILITY_FALLBACK = 0.3  # Fallback probability for main generator

# Piece removal probabilities for puzzle generation
REMOVAL_PROB_WITH_CONSTRAINTS = 0.9  # Probability to remove piece that has constraints
REMOVAL_PROB_WITHOUT_CONSTRAINTS = (
    0.9  # Probability to remove piece without constraints
)

# Puzzle difficulty settings (for main generator)
MIN_PIECES_TO_REMOVE = 18  # Minimum pieces to remove from solved puzzle
MAX_PIECES_TO_REMOVE = 25  # Maximum pieces to remove from solved puzzle

# Game timing
DOUBLE_CLICK_TIME = 0.5  # Maximum time between clicks for double-click detection
FPS = 60  # Frames per second for game loop

# Leaderboard settings
MAX_LEADERBOARD_ENTRIES = 10  # Maximum number of leaderboard entries to keep

# File settings
LEADERBOARD_FILENAME = "leaderboard.json"
ENCODING = "utf-8"  # File encoding for JSON files
