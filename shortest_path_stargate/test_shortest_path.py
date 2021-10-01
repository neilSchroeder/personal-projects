
from shortest_path_bfs import shortest_path_bfs
import time
import random

def main():

    maps = [
            ".S.\n...\n.G.",
            ".S...\nXXX..\n.X.XX\n..X..\nG...X",
            "XX.S.XXX..\nXXXX.X..XX\n...X.XX...\nXX...XXX.X\n....XXX...\nXXXX...XXX\nX...XX...X\nX...X...XX\nXXXXXXXX.X\nG........X",
            ".X.X.X....XXXXXX...X\nXX.XX.XXXXXXXXXXX..X\n.X.X.XX..X..X.XXXXXX\nX.X..XXX...XX.X.XXX.\nX.X..X..XXX.X.X.X...\n.XXX..XXXXX.X.X..XX.\nX.XX.SX......XXX..X.\n.XXXXX.XXX...XX..X..\n....X.XX..X.XX.X..XX\n....X..XX..XX..X.XX.\nX...X..XX.X.X.XX...X\n.XXX.........X.XX..G\n..XX.XX.XX.X.XXXXXX.\n.X.X...X.X.XXXX..X.X\n..X..XXX.XX....XXXX.\nXX..XXXXXXX.....XXXX\nXXXX.X.X..XXXXXX...X\nX...X..X..XXXX..X..X\nX.XXXXX..XX..XXX.X.X\nXX.X.XX.XXXX.X..X.XX",
            ".XXX.XXXXXX.XX.X..X..XX.X.XXXX...XXXXX\nX.X...X.X.X.XX..XXXX.XX..XXX....XX...X\nX..X.X..X.XX.XXXX.XX..X...X...X...XXXX\n.XXX.X.X.XX...XXXX...X.XX....XX..X.XXX\nXX..X..X..XXXX...X....XX.XX.X.XX.XX..X\nX.....X.XX.XX......X.X....XXX.X..X..X.\n.XXXXXXX...XXX.XXXX.X.X..X.X.XXX.XXX.X\n.X...XX..X.XGX.X.X.X..XXX..XX.XXXXXX..\n..X.XX.XXXXX....X.XX.XX.X..XX..XX.X.XX\n...X.X.X...XX..X.XXXX..X..X.X..XX.X.X.\nXXX..X..XXX.XX.X.X.X..X...XXXX.XXXXXX.\nX.X..XXXX...X.XXX.X....XX.X.X..XXXX.X.\n...X.XXX.XXX.X.X.X.X..X.X..XX.XXXXX.X.\nX.X..XX.....XXXX.X....XXX..XXX.X.XXXX.\nX.XXX.XXXX.XXXXXXX.X.X.XXXXXXX...XXXX.\n.XXXXXX..X..X.XX..XX.XXX.X...XXX.X.XXX\nXXX.X.XXXX.X......X.X....XXXXXXX..X.XX\nX.X.X....XXX.X....XXX..XX...XXX....XX.\nXX.X.XXXXX..XXXXX...X..X..XXX...X.XXX.\n.X.XXXXXXXXX..X....X..X..X.XX.XX..XXXX\n.X..X.X.XXX..X.X...XX...XX.XXX.X.X.XXX\n.X.XXX..XX.X..X.X.X..X.X.XXXX..XX..X.X\nXX..XXXXXXX.X.XX...XX.X...X.XX.X.X.X.X\n.XX.XXX.X..XX..X.XXX.XX..XX.XXXXXXXX.X\n.X..XX.XXXXX.X...XXX.XXXX.XXX..XXX..X.\n.XX...XXXX.XXXX...X.XX..X...XXX.XX..XX\nXX..X..XX...X..XXX.XXX..XXXXXX..XXX.XX\n.XXXXXX..XXXXX...X.XX.XX.XXX.X...XX...\nX.XXXXX..XXX.XXX.X.XXX.X..XX..XX.X....\nX.X.XX.....XX..XX..X.XXX.X..X.XXXXXXXX\nXXX..XX.XX.XXXX....X.XX.X.X....X..X.XX\nX.XX.XXXX....XXX..XXX..X...XXX.X.X.XXX\n.....XX.X.X.XX.XX.X.SXXX.X....X.XXXXXX\n.XXXX..XX.X.X.XXX.XX...XX..X.XX..XXXXX\nX..X.XXXX.XXX.X..X.X.X.XXX..X.XXX..X.X\n.X.X..XX...XX.XX......X.XXX.XXX.XXXXXX\nX.X...X.X..XX.XXX.X.X..XXXXX..XXX.XXXX\nX..X.X..XXXXX..X...XX.XX.X.XXXXXXX.X.X"
            ]

    """
    for map_i in range(10):
        rand_map = ""
        for i in range(30):
            rand_X = [random.randint(0,3) for i in range(120)]
            row = "".join(["." if rand_X[i] else "X" for i in range(120)])
            rand_map = rand_map + row
            if i < 29:
                rand_map = rand_map + "\n"

        start_loc = (random.randint(0,29), random.randint(0,119))
        end_loc = (random.randint(0,29), random.randint(0,119))
        rand_map_list = rand_map.split('\n')
        rand_map_list = [list(row) for row in rand_map_list]
        rand_map_list[start_loc[0]][start_loc[1]] = 'S'
        rand_map_list[end_loc[0]][end_loc[1]] = 'G'
        rand_map = '\n'.join([''.join(row) for row in rand_map_list])

        start = time.time()
        soln = shortest_path_bfs(rand_map, watch_evolution=True)
        end = time.time()
        print("the shortest path is:")
        print(soln)
        print()
        print("it took", round(end-start,4), "seconds to find the solution")
        print()
        time.sleep(2)

    """
    for map in maps:
        print("the initial state is:")
        print(map)
        print()
        start = time.time()
        soln = shortest_path_bfs(map, watch_evolution=True)
        end = time.time()
        print("the shortest path is:")
        print(soln)
        print()
        print("it took", round(end-start,4), "seconds to find the solution")
        print()
        time.sleep(2)



if __name__ == '__main__':
    main()
