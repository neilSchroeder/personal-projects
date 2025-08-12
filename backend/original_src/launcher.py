"""
Launcher for Tango game - handles pygame initialization safely.
"""

import os
import sys


def main():
    """Main launcher function."""
    # Set pygame to use a specific video driver if needed
    if sys.platform.startswith("linux"):
        # Ensure X11 threading is properly initialized
        os.environ.setdefault("SDL_VIDEODRIVER", "x11")

    # Import and run the game
    try:
        from .tango_game import main as game_main

        game_main()
    except ImportError:
        # If running as a script directly
        import tango_game

        tango_game.main()


if __name__ == "__main__":
    main()
