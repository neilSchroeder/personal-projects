
class puzzle:

    def __init__(self, board:
        self.board = [x[:] for x in board]
        self.locked_positions = [[False for x in row] for row in self.puzzle]

    
    def move_val_to_pos(self, val, pos):
        val_loc = ( [val in row for row in self.Board].index(True),  )
        while self.board[pos[0]][pos[1]] != val:
            #get direction to move
            #move surrounding pieces
            #move val toward loc
            pass
            



def slide_puzzle(p):
    """ solves a slide puzzle """

    """ 
    1) solve the top row
    2) solve the left column
    3) repeat for the new smaller board
    """
    
    this_puzzle = puzzle(p)

    #solve the first row
    for i in range(1, len(this_puzzle.board[0])-1):
        this_puzzle.move_val_to_pos(i, (0,i-1))
    


def main():
    puzzle1 = [
	[4,1,3],
	[2,8,0],
	[7,6,5]
    ]
    slide_puzzle(puzzle)

if __name__ == '__main__':
    main()
