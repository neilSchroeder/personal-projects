import time

from python.classes.board import board

_kVerbose = False


def solve_mine(g, n, _kAllowGuess):
    # solves the map

    # start by loading the map into the board
    b = board(g, n)
    b.update_map()
    if _kVerbose: b.print()

    # open all the tiles next to the initial open tiles
    for ot in b.open_tiles.copy():
        for t in b.map[ot].nearby_tiles:
            if t in b.open_tiles:
                continue
            if t not in b.map.keys():
                continue
            b.open_tiles.append(t)
            b.map[t].update(b.open(t[0], t[1]))
    b.update_map()
    b.print()

    boundary_closed = b.get_boundary_tiles("closed")
    boundary_open = b.get_boundary_tiles("open")
    b.update_map(bound=boundary_open)
    b.print()

    b.check_open_tiles(boundary_open)

    while b.remaining_bombs > 0 and b.valid_map:

        while b.check_open_tiles(b.get_boundary_tiles("open")):
            b.update_map()

        # get the probability to be a bomb for each closed tile
        probs = b.get_bomb_probs(b.get_boundary_tiles('open'))

        # assign closed tiles as bombs
        b.assign_bombs(probs, _kAllowGuess)

        # check if bombs accounted for
        # open all tiles around tiles with all bombs accounted for 
        b.check_open_tiles(boundary_open)
        if _kVerbose and not b.valid_map:
            print("probs:")
            for x in probs:
                print(x, probs[x])
            return "?"


    if b.valid_map:
        # open the remaining tiles
        for t in b.map:
            if b.map[t].is_open:
                continue
            b.map[t].update(b.open(t[0],t[1]))

    return b.map_string
