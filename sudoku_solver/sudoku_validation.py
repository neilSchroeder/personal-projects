def not_deterministic(board):
    """ check if board contains minimum entries to be deterministic """
    count = 0
    for row in board:
        for val in row:
            if val != 0:
                count += 1

    return count < 17


def invalid_structure(board):
    """ check if the structure of the board is invalid """
    if len(board) != 9:
        return True
    for row in board:
        if len(row) != 9:
            return True
    return False


def contains_invalid_entries(board):
    """ check if the board contains invalid entries """
    valid_entries = [i for i in range(10)]
    for row in board:
        for val in row:
            if val not in valid_entries:
                return True
    return False


def contains_duplicate_in_row(board):
    """ check if rows contain duplicates """
    for row in board:
        for i in range(1, 10, 1):
            if sum([x == i for x in row]) > 1:
                return True
    return False


def contains_duplicate_in_column(board):
    """ check if columns contain duplicates """
    columns = [[board[i][j] for i in range(9)] for j in range(9)]
    for col in columns:
        for i in range(1, 10, 1):
            if sum([x == i for x in col]) > 1:
                return True
    return False


def contains_duplicate_in_block(board):
    """ check if duplicates exist in the board blocks """

    blocks = [[] for i in range(9)]
    for i in range(9):
        for j in range(9):
            blocks[i//3 * 3 + j//3].append(board[i][j])
    for block in blocks:
        for i in range(1, 10, 1):
            if sum([x == i for x in block]) > 1:
                return True
    return False


def contains_duplicates(board):
    """check if the board contains duplicates"""

    if contains_duplicate_in_row(board):
        return True

    if contains_duplicate_in_column(board):
        return True

    if contains_duplicate_in_block(board):
        return True

    return False


def is_not_valid(board):
    """main validation function, checks if board is valid"""

    if invalid_structure(board):
        return True

    if not_deterministic(board):
        return True

    if contains_invalid_entries(board):
        return True

    if contains_duplicates(board):
        return True

    return False
