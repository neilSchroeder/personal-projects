#!/usr/bin/env python3

import sudoku
import time

def print_board(message, board):
    print(message)
    for row in board:
        print(row)
    print()

def solve(problem):

    
    start = time.time()
    try:
        soln = sudoku.sudoku_solver(problem)
        end = time.time()
        print_board("the initial state was:",problem)
        print("the solution was found in", round(end-start,4), "seconds")
        print_board("the solution is:", soln)
    except:
        end = time.time()
        print("an exception was encountered after", round(end-start,4), "seconds")

def main():
    problem_multiple = [[0, 0, 0, 1, 0, 0, 0, 0, 8],
                    [0, 8, 0, 0, 9, 0, 0, 3, 0],
                    [2, 0, 0, 0, 0, 5, 4, 0, 0],
                    [4, 0, 0, 0, 0, 1, 8, 0, 0],
                    [0, 3, 0, 0, 7, 0, 0, 4, 0],
                    [0, 0, 7, 9, 0, 0, 0, 0, 3],
                    [0, 0, 8, 4, 0, 0, 0, 0, 6],
                    [0, 2, 0, 0, 5, 0, 0, 8, 0],
                    [1, 0, 0, 0, 0, 2, 0, 0, 0]]

    problem_hard = [[1, 0, 9, 0, 4, 0, 0, 8, 0],
                [0, 5, 0, 0, 0, 0, 0, 1, 0],
                [0, 3, 0, 0, 2, 0, 5, 0, 7],
                [0, 0, 8, 0, 0, 9, 0, 0, 0],
                [9, 0, 0, 7, 1, 0, 4, 0, 0],
                [0, 0, 0, 4, 0, 0, 7, 0, 0],
                [0, 0, 6, 0, 0, 5, 0, 0, 0],
                [0, 0, 7, 0, 8, 6, 0, 0, 3],
                [0, 0, 0, 1, 0, 0, 2, 0, 0]]

    problem_multiple_hard = [[0, 0, 7, 0, 3, 0, 0, 9, 6],
                             [0, 0, 4, 1, 0, 0, 0, 5, 0],
                             [3, 0, 0, 0, 0, 5, 0, 0, 0],
                             [0, 0, 0, 0, 7, 8, 9, 0, 0],
                             [0, 0, 0, 0, 0, 0, 1, 0, 0],
                             [0, 0, 0, 0, 6, 0, 3, 0, 4],
                             [6, 0, 2, 0, 0, 4, 0, 0, 0],
                             [0, 0, 1, 0, 0, 7, 0, 0, 0],
                             [5, 0, 0, 0, 0, 0, 8, 0, 0]]

    solve(problem_hard)
    solve(problem_multiple)
    solve(problem_multiple_hard)

if __name__ == '__main__':
    main()
