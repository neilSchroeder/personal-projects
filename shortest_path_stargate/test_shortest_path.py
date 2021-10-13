
from shortest_path_bfs import shortest_path_bfs
import time
import random


def main():
    """ generates random maps and solves them """
    #random.seed(1)

    map_height = 100
    map_width = 200
    number_of_maps = 1
    for map_i in range(number_of_maps):
        rand_map = ""
        for i in range(map_height):
            rand_X = [random.randint(0, 1) for i in range(map_width)]
            row = "".join(
                ["." if rand_X[i] else "X" for i in range(map_width)])
            rand_map = rand_map + row
            if i < map_height-1:
                rand_map = rand_map + "\n"

        #start_loc = (random.randint(0, map_height-1),
        #             random.randint(0, map_width-1))
        #end_loc = (random.randint(0, map_height-1),
        #           random.randint(0, map_width-1))
        start_loc = (0, 0)
        end_loc = (map_height-1, map_width-1)
        rand_map_list = rand_map.split('\n')
        rand_map_list = [list(row) for row in rand_map_list]
        rand_map_list[start_loc[0]][start_loc[1]] = 'S'
        rand_map_list[end_loc[0]][end_loc[1]] = 'G'
        rand_map = '\n'.join([''.join(row) for row in rand_map_list])

        start = time.time()
        soln = shortest_path_bfs(rand_map, watch_evolution=False)
        end = time.time()
        print("the shortest path is:")
        print(soln)
        print("solution was found in", round(end-start, 4), "seconds")
        print()
        time.sleep(5)

if __name__ == '__main__':
    main()
