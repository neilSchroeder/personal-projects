#!/usr/bin/env python3

import argparse as ap

from python.tools.solver import solve_mine
from python.tools.map_generator import generate_map
from python.tools.map_validator import validate_map

def main():
    # main function to run minesweeper

    parser = ap.ArgumentParser(description="options for minesweeper")

    parser.add_argument("-r","--rows", type=int, default = 10,
                        help="sets the number of rows in the board")
    parser.add_argument("-c","--cols", type=int, default=10,
                        help="sets the number of columns in the board")
    parser.add_argument("-b","--bombs", type=int, default=5,
                        help="number of bombs to put in the board")
    parser.add_argument("-n","--num-games", type=int, default=1, dest="num_games",
                        help="runs n games and returns the win percentage")
    
    # some additional, more advanced, options
    parser.add_argument("--allow-guess", dest="_kAllowGuess", action="store_true", default=False,
                        help="allows the program to guess at where bombs are if it gets stuck")

    args = parser.parse_args()

    count = 0
    for i in range(args.num_games):
        gamemap = generate_map(args.rows, args.cols, args.bombs)
        if args.num_games == 1.:
            print(gamemap)
        solved_map = solve_mine(gamemap, args.bombs, args._kAllowGuess)

        if args.num_games == 1.:
            print(solved_map)
        valid = validate_map(gamemap, solved_map)
        count += int(valid)
        if valid and args.num_games == 1.:
            print("Nicely done, the board is solved")
        elif not valid and args.num_games == 1.:
            print("Huh, something went wrong.")
        else:
            pass

    if args.num_games > 1:
        print(f"Your win rate is {100*count/args.num_games}%")


if __name__ == "__main__":
    main()
