
from sudoku_class import sudoku
import sudoku_validation as validator

def sudoku_solver(board):
    print(flush=True)

    if validator.is_not_valid(board):
        raise Exception

    soln = sudoku(board, watch_progression=True)

    if soln.solve_by_tree(0):
        return soln.board
    raise Exception
