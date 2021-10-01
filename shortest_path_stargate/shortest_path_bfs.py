import time
from graph_bfs_class import graph

def shortest_path_bfs(map_string, watch_evolution=True):

    map_list = map_string.split('\n')

    map_list = [list(row) for row in map_list]

    my_graph = graph(map_list,watch_evolution)
    if my_graph.search_for_goal():
        my_graph.find_shortest_path()
        return '\n'.join([''.join(row) for row in my_graph.map])

    return "Oh for crying out loud..."
