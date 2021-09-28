# Sudoku Solver

This is a sudoku solver. I wrote this mostly because I stubled upon a prompt for a sudoku solver on codewars (https://www.codewars.com/kata/55171d87236c880cea0004c6/train/python) and didn't think I could solve it. Turns out that I just needed to debug it a bit and then it worked. However, right after that I came across a harder version of this prompt (https://www.codewars.com/kata/5588bd9f28dbb06f43000085/train/python), and to my delight I had some functioning code from which to start optimizing. The code you'll find here is the solution to this second, harder prompt, after having been refactored and cleaned up.

## The Prompt

"There are several difficulty of sudoku games, we can estimate the difficulty of a sudoku game based on how many cells are given of the 81 cells of the game.

Easy sudoku generally have over 32 givens
Medium sudoku have around 30–32 givens
Hard sudoku have around 28–30 givens
Very Hard sudoku have less than 28 givens
Note: The minimum of givens required to create a unique (with no multiple solutions) sudoku game is 17.

A hard sudoku game means that at start no cell will have a single candidates and thus require guessing and trial and error. A very hard will have several layers of multiple candidates for any empty cell.

Task:
Write a function that solves sudoku puzzles of any difficulty. The function will take a sudoku grid and it should return a 9x9 array with the proper answer for the puzzle.

Or it should raise an error in cases of: invalid grid (not 9x9, cell with values not in the range 1~9); multiple solutions for the same puzzle or the puzzle is unsolvable

Python users: python 2 has been disabled." 

https://www.codewars.com/kata/5588bd9f28dbb06f43000085/train/python
Written by [f.rodrigues](https://www.codewars.com/users/f.rodrigues)

## Methodology

The method I've chosen to use is a recursive backtracking algorithm. The outline of the algorithm is as follows:

1 Fill as many singletons (cells with only one possible occupant) as possible 
2 When no more singletons exist, find the cell with the fewest possible occupants (2 or more)
3 Create new boards for all possible occupants of the minimum occupation cell
4 Attempt to solve each board from 1 

Eventually a solution is found, assuming a solution exists. 

## Error Handling

The program will handle all of the errors provided in the prompt. Namely, the program will raise an `Exception` if:

- the grid is invalid
- there are multiple solutions
- there are no solutions

## Usage
