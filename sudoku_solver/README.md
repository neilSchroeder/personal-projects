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

just clone the repository, make sure you're using python 3.8.10 and that you have the `time` package, then from a command line just run
`./test_sudoku`
and enjoy the magic. Feel free to dink around and change the input sudoku to try to break it. If you manage to do so, let me know and I'll try to fix it.

## Performance

I'm running this on a System76 Gazelle. Running `lscpu` returns the following:
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
CPU(s):                          12
Thread(s) per core:              2
Core(s) per socket:              6
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           165
Model name:                      Intel(R) Core(TM) i7-10750H CPU @ 2.60GHz
Stepping:                        2
CPU MHz:                         2600.000
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
```

When running the tests I get the following output:

```
the initial state was:
[1, 0, 9, 0, 4, 0, 0, 8, 0]
[0, 5, 2, 0, 0, 0, 0, 1, 0]
[0, 3, 4, 0, 2, 0, 5, 0, 7]
[0, 0, 8, 0, 0, 9, 0, 0, 0]
[9, 0, 0, 7, 1, 0, 4, 0, 0]
[0, 0, 0, 4, 0, 0, 7, 0, 0]
[0, 0, 6, 0, 0, 5, 0, 0, 0]
[0, 0, 7, 0, 8, 6, 0, 0, 3]
[0, 0, 0, 1, 0, 0, 2, 0, 0]

the solution was found in 0.0078 seconds
the solution is:
[1, 7, 9, 5, 4, 3, 6, 8, 2]
[8, 5, 2, 9, 6, 7, 3, 1, 4]
[6, 3, 4, 8, 2, 1, 5, 9, 7]
[7, 4, 8, 6, 3, 9, 1, 2, 5]
[9, 6, 5, 7, 1, 2, 4, 3, 8]
[3, 2, 1, 4, 5, 8, 7, 6, 9]
[2, 9, 6, 3, 7, 5, 8, 4, 1]
[4, 1, 7, 2, 8, 6, 9, 5, 3]
[5, 8, 3, 1, 9, 4, 2, 7, 6]


found multiple solutions at a backtrack depth of 14
an exception was encountered after 0.0146 seconds

the initial state was:
[1, 5, 7, 8, 3, 2, 4, 9, 6]
[0, 0, 4, 1, 0, 0, 0, 5, 0]
[3, 0, 0, 0, 0, 5, 0, 0, 0]
[0, 0, 0, 0, 7, 8, 9, 0, 0]
[0, 0, 0, 0, 0, 0, 1, 0, 0]
[0, 0, 0, 0, 6, 0, 3, 0, 4]
[6, 0, 2, 0, 0, 4, 0, 0, 0]
[0, 0, 1, 0, 0, 7, 0, 0, 0]
[5, 0, 0, 0, 0, 0, 8, 0, 0]

the solution was found in 0.0059 seconds
the solution is:
[1, 5, 7, 8, 3, 2, 4, 9, 6]
[2, 8, 4, 1, 9, 6, 7, 5, 3]
[3, 9, 6, 7, 4, 5, 2, 1, 8]
[4, 1, 5, 3, 7, 8, 9, 6, 2]
[7, 6, 3, 4, 2, 9, 1, 8, 5]
[9, 2, 8, 5, 6, 1, 3, 7, 4]
[6, 7, 2, 9, 8, 4, 5, 3, 1]
[8, 3, 1, 2, 5, 7, 6, 4, 9]
[5, 4, 9, 6, 1, 3, 8, 2, 7]
```
