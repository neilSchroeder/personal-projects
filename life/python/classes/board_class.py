import numpy as np
import random as rand
import itertools
import time

from python.classes.cell_class import cell

class board:

    def __init__(self, cycles=100, width=175, height=40) -> None:
        self.width = width
        self.height = height
        self.board = self.create_board()
        self._is_running = False
        self.cycles = cycles
        pass

    def create_board(self):
        return {loc: cell(loc, rand.randint(0,1) == 0) for loc in itertools.product(range(self.height), range(self.width))}

    def determine_survival(self):
        """ determines if each cell will live or die """
        for key in self.board.keys():
            c = self.board[key]
            if c.is_alive:
                c.will_live = bool(sum([self.board[x].is_alive for x in c.neighbors if x in self.board.keys()]) in [2, 3])
            else:
                c.will_live = bool(sum([self.board[x].is_alive for x in c.neighbors if x in self.board.keys()]) == 3)

    def cycle(self):
        self.cycles -= 1
        for key in self.board.keys():
            c = self.board[key]
            c.is_alive, c.will_live = c.will_live, False

    def get_living_cells(self):
        return sum([self.board[key].is_alive for key in self.board.keys()])

    def run(self):
        """ sets the board state to running """
        self._is_running = True

    def stop(self):
        self._is_running = False
        
    def evolve(self):
        """ loop over all cells and determine if it should live or die"""
        self.determine_survival()
        self.cycle()
        if self.cycles == 0:
            self.stop()
        if self.get_living_cells() == 0:
            self.stop()

    def print(self):
        for i in range(self.height):
            row = []
            for j in range(self.width):
                if self.board[(i,j)].is_alive:
                    row.append('\033[93m'+chr(9608)+'\033[0m')
                else:
                    row.append(chr(9608))
            print(''.join(row))
        print()
        time.sleep(0.25)