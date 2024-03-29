import time


class sudoku:
    """ class to contain and solve a sudoku board """

    def __init__(self, board, watch_progression=False):
        self.block_coords = {}
        self.check_for_duplicates = True
        self.get_blocks()
        self.board = board
        self.possibilities = {}
        self.singletons = []
        self.is_valid = True
        self.initialize_possibilities()
        self.branch = []
        self.min_depth = 3
        self.watch = watch_progression

    def get_blocks(self):
        """ makes the blocks in the board """
        for i in range(9):
            self.block_coords[i] = []
        for i in range(9):
            for j in range(9):
                self.block_coords[(i//3)*3 + j//3].append((i, j))

    def get_column(self, coord):
        """ retrieves the column associated with coord """
        return [self.board[i][coord[1]] for i in range(9)]

    def get_block(self, coord):
        """ retrieves the block associated with coord """
        return [self.board[c[0]][c[1]]
                for c in self.block_coords[coord[0]//3 * 3 + coord[1]//3]]

    def get_row(self, coord):
        """ retrieves the row associated with coord """
        return self.board[coord[0]]

    def get_row_coords(self, coord):
        """ retrieves the coordinates in a row associated with coord """
        return [(coord[0], i) for i in range(0, 9) if (coord[0], i) != coord]

    def get_column_coords(self, coord):
        """ retrieves the coordinates in a column associated with coord """
        return [(i, coord[1]) for i in range(0, 9) if (i, coord[1]) != coord]

    def get_block_coords(self, coord):
        """ retrieves the coordinates in a block associated with coord """
        return self.block_coords[coord[0]//3 * 3 + coord[1]//3]

    def initialize_possibilities(self):
        """ initializes the possibilities of each unfilled point """
        self.possibilities = {}
        for i, row in enumerate(self.board):
            for j, val in enumerate(row):
                if val != 0:
                    continue
                used_nums = (row
                             + self.get_block((i, j))
                             + self.get_column((i, j))
                             )
                used_nums = list(set(used_nums))
                self.possibilities[(i, j)] = [x for x in range(1, 10)
                                              if x not in used_nums]
                if len(self.possibilities[(i, j)]) == 1:
                    self.singletons.append((i, j))
                if len(self.possibilities[(i, j)]) == 0:
                    self.is_valid = False
                    break

    def update_possibilities(self, updated_coord):
        """ updated the list of possibilities for empty points """
        if updated_coord in self.possibilities.keys():
            self.possibilities.pop(updated_coord)

        coords_to_update = (self.get_block_coords(updated_coord)
                            + self.get_row_coords(updated_coord)
                            + self.get_column_coords(updated_coord)
                            )
        coords_to_update = list(set(coords_to_update))

        for coord in coords_to_update:
            if self.board[coord[0]][coord[1]] == 0:
                if self.board[updated_coord[0]][updated_coord[1]] in self.possibilities[coord]:
                    self.possibilities[coord].pop(
                        self.possibilities[coord].index(
                            self.board[updated_coord[0]][updated_coord[1]]
                        )
                    )
                if len(self.possibilities[coord]) == 1:
                    self.singletons.append(coord)
                if len(self.possibilities[coord]) == 0:
                    self.is_valid = False
                    break
            else:
                if coord in self.possibilities.keys():
                    self.possibilities.pop(coord)

    def is_solved(self):
        """ checks if the board is solved """
        return not any([0 in row for row in self.board])

    def is_invalid_solution(self):
        """ checks if the current state of the board is an invalid solution """
        for key in self.possibilities:
            if (len(self.possibilities[key]) == 0
                    and self.board[key[0]][key[1]] == 0):
                return True
        return False

    def get_shortest_key(self):
        """ gets the point with the fewest possibilities """
        min = 10
        ret = ()
        for key in self.possibilities:
            if len(self.possibilities[key]) < min:
                min = len(self.possibilities[key])
                ret = key
                if min == 2:
                    return ret, min

        return ret, min

    def get_copy(self):
        return [row.copy() for row in self.board]

    def fill_singleton(self):
        coord = self.singletons[0]
        self.singletons = []
        self.board[coord[0]][coord[1]] = self.possibilities[coord][0]
        return coord

    def draw_board(self):
        """ draws the board and it's current state """
        print(25*"-")
        for r, row in enumerate(self.board):
            row_string = "| "
            for i, val in enumerate(row):
                str_val = ("\033[93m"+str(val)+"\033[0m"
                           if val != 0 else str(val))
                row_string = (row_string
                              + str_val
                              + " " if i not in [2, 5, 8] else
                              row_string + str_val + " | ")
            print(row_string)
            if r in [2, 5]:
                print(25*"-")
        print(25*"-")
        time.sleep(0.02)

    def solve_by_tree(self, depth):
        """ solves the board using a depth first approach """
        if not self.is_valid:
            return False

        while len(self.singletons) > 0:
            filled_singleton = self.fill_singleton()
            if self.watch:
                self.draw_board()
            self.update_possibilities(filled_singleton)
            if not self.is_valid:
                return False

        if self.is_solved():
            return True

        key_to_update, n_possibilities_at_key = self.get_shortest_key()

        for i in range(n_possibilities_at_key):
            b = self.get_copy()
            b[key_to_update[0]][key_to_update[1]] = (
                self.possibilities[key_to_update][i]
            )
            self.branch.append(sudoku(b, watch_progression=self.watch))

        if depth > self.min_depth:
            solutions = [leaf.solve_by_tree(depth+1) for leaf in self.branch]
            if all(solutions) and len(solutions) > 1:
                print("found multiple solutions at a backtrack depth of",
                      depth)
                self.board = self.branch[0].board
                raise Exception
            elif any(solutions):
                self.board = self.branch[solutions.index(True)].board
                return True
            else:
                return False
        else:
            for leaf in self.branch:
                if leaf.solve_by_tree(depth+1):
                    self.board = leaf.board
                    return True
            return False
