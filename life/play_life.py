#!/usr/bin/env python3

import argparse as ap
from python.classes.board_class import board


def main():
    #plays the game of life
    parser = ap.ArgumentParser(description="options for the game of life")
    parser.add_argument('--cycles', dest='cycles', type=int, default=1000,
                        help='Sets the number of cycles of the board.')
    parser.add_argument('--height', dest='height', type=int, default=40,
                        help='Sets the height of the board')
    parser.add_argument('--width', dest='width', type=int, default=175,
                        help='Sets the width of the board.')
    args = parser.parse_args()

    game_of_life = board(cycles=args.cycles, width=args.width, height=args.height)
    
    game_of_life.run()
    while game_of_life._is_running:
        game_of_life.evolve()
        game_of_life.print()


if __name__ == '__main__':
    main()