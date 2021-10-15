
from graph_bfs_class import graph
import time


def shortest_path_bfs(map_string, watch_evolution=True):
    """ control function for finding the shortest path """

    map_list = map_string.split('\n')

    map_list = [list(row) for row in map_list]

    my_graph = graph(map_list, watch_evolution)
    if my_graph.a_star_search():
        #my_graph.create_gif()
        return '\n'.join([''.join(row) for row in my_graph.map])

    return "Oh for crying out loud..."
