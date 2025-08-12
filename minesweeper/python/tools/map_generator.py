import random as r
from tkinter import W


def generate_bomb_locs(rows, cols, n):
    # generates the location of n bombs
    locs = []

    for i in range(n):
        a, b = r.randint(0, rows-1), r.randint(0, cols-1)
        while (a,b) in locs:
            a, b = r.randint(0, rows-1), r.randint(0, cols-1)
        locs.append((a,b))

    return locs

def assign_nearby_bombs(m):
    # counts the number of nearby bombs in each tile

    for i_row, row in enumerate(m):
        for i_col, c in enumerate(row):
            if c == 'x':
                continue
            m[i_row][i_col] = [m[a][b] 
                                for a in range(max(0,i_row-1), min(i_row+2, len(m)))
                                for b in range(max(0,i_col-1), min(i_col+2,len(row)))].count("x")

    return m

            
def generate_map(rows, cols, n):
    # generate the map

    map = []
    bomb_locs = generate_bomb_locs(rows, cols, n)

    for i in range(rows):
        row = [0 if (i,j) not in bomb_locs else "x" for j in range(cols)]
        map.append(row.copy())

    map = assign_nearby_bombs(map)
    return ''.join([' '.join([str(x) for x in row])+'\n' for row in map])
