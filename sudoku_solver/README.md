# Sudoku Solver

This is a depth first search algorithm in python which solves sudoku puzzles. This repo serves as a refactored solution to this codewars [prompt](https://www.codewars.com/kata/5588bd9f28dbb06f43000085/train/python).

## Installation and Usage

```
git clone git@github.com:neilSchroeder/tools.git
cd sudoku_solver
python3 test_sudoku.py
```

## Methodology

This software uses a backtracking algorithm. The outline of the algorithm is as follows:

1 Fill as many singletons (cells with only one possible occupant) as possible   
2 When no more singletons exist, find the cell with the fewest possible occupants (2 or more)  
3 Create new boards for all possible occupants of the minimum occupation cell  
4 Attempt to solve each board from 1   

Eventually a solution is found, assuming a solution exists.

## Error Handling

The code will raise an `Exception` if:

- the grid is invalid
- there are multiple solutions
- there are no solutions

## Example

An excerpt from the output of `test_sudoku.py`

```
the initial state was:
-------------------------
| 0 0 5 | 3 0 0 | 0 0 0 |
| 8 0 0 | 0 0 0 | 0 2 0 |
| 0 7 0 | 0 1 0 | 5 0 0 |
-------------------------
| 4 0 0 | 0 0 5 | 3 0 0 |
| 0 1 0 | 0 7 0 | 0 0 6 |
| 0 0 3 | 2 0 0 | 0 8 0 |
-------------------------
| 0 6 0 | 5 0 0 | 0 0 9 |
| 0 0 4 | 0 0 0 | 0 3 0 |
| 0 0 0 | 0 0 9 | 7 0 0 |
-------------------------

the solution was found in 0.0211 seconds
the solution is:
-------------------------
| 1 4 5 | 3 2 7 | 6 9 8 |
| 8 3 9 | 6 5 4 | 1 2 7 |
| 6 7 2 | 9 1 8 | 5 4 3 |
-------------------------
| 4 9 6 | 1 8 5 | 3 7 2 |
| 2 1 8 | 4 7 3 | 9 5 6 |
| 7 5 3 | 2 9 6 | 4 8 1 |
-------------------------
| 3 6 7 | 5 4 2 | 8 1 9 |
| 9 8 4 | 7 6 1 | 2 3 5 |
| 5 2 1 | 8 3 9 | 7 6 4 |
-------------------------
```
