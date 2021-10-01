import time
import operator
from node_bfs_class import node
class graph:

    def __init__(self, map, watch_evolution=True):
        self.map = map
        self.start_node_coords = self.get_start_node_coords()
        self.goal_node_coords = self.get_goal_node_coords()
        self.nodes = {}
        self.active_nodes = {}
        self.watch_evolution = watch_evolution
        self.populate_nodes()

    def get_start_node_coords(self):
        i = ['S' in row for row in self.map].index(True)
        j = [x=='S' for x in self.map[i]].index(True)
        return (i,j)

    def get_goal_node_coords(self):
        i = ['G' in row for row in self.map].index(True)
        j = [x == 'G' for x in self.map[i]].index(True)
        return (i,j)

    def populate_nodes(self):
        self.nodes = {(i,j):node(self.map,
                          (i,j),
                          (i,j)==self.goal_node_coords,
                          (i,j)==self.start_node_coords) for i in range(len(self.map)) for j in range(len(self.map[i])) if self.map[i][j] != 'X' }

    def distance_to_goal(self,n):
        return (abs(self.goal_node_coords[0]-n[0])**2 + abs(self.goal_node_coords[1]-n[1])**2)**0.5

    def distance_to_start(self,n):
        return (abs(self.start_node_coords[0]-n[0])**2 + abs(self.start_node_coords[1]-n[1])**2)**0.5

    def topo_sort(self,keys):
        return sorted(sorted(keys, key=lambda x: self.active_nodes[x].indegree), key=lambda x: self.distance_to_start(x))

    def search_for_goal(self):

        queue = [self.start_node_coords]

        count = 0
        while queue:
            #activate the nodes in the queue
            queue_indices = {}
            n = self.nodes[queue.pop(0)]
            if n.is_start:
                n.set_indegree(())


            if not n.is_active:
                n.get_neighboring_nodes()
                self.active_nodes[n.loc] = n
                if not n.is_start:
                    min_indegree = min([self.nodes[key].indegree for key in n.neighbors.keys() if self.nodes[key].is_active])
                    n.set_indegree(min_indegree)
                if not self.nodes[self.goal_node_coords].is_active:
                    queue = queue + [key for key in n.neighbors.keys() if not self.nodes[key].is_active]
                    queue_indices = {x: queue.index(x) for x in queue}
                    queue.sort(key=lambda x: 0.35*queue_indices[x] + 3*n.indegree + 0.32*self.distance_to_goal(x))
                n.activate()

            self.draw_path(queue, reset=True, symb='Q')
            time.sleep(0.005)


        if not self.nodes[self.goal_node_coords].is_active:
            return False

        return True

    def get_path_length(self,path):
        return sum([self.nodes[path[i]].distance_to_node(path[i+1]) for i in range(len(path)-1) ])

    def find_shortest_path(self):

        queue = [self.start_node_coords]
        parent_path_length = {}

        while queue:

            key = queue.pop(0)
            self.active_nodes[key].set_distance_from_start(self.active_nodes)

            if not queue.count(self.goal_node_coords):
                neighbors = [ x for x in self.nodes[key].neighbors if self.nodes[x].is_active and self.nodes[x].indegree > self.nodes[key].indegree and queue.count(x) < 2]
                queue = queue + neighbors
                for neighbor in neighbors:
                    if neighbor in parent_path_length.keys():
                        parent_length = self.nodes[key].distance_from_start_node
                        if parent_length < parent_path_length[neighbor]:
                            parent_path_length[neighbor] = parent_length
                    else:
                        parent_path_length[neighbor] = self.nodes[key].distance_from_start_node

            queue = sorted(sorted(queue, key = lambda x : self.active_nodes[x].indegree), key = lambda x : parent_path_length[x])
            self.draw_path(queue, reset=True, symb='Q')
            time.sleep(0.005)

            if key == self.goal_node_coords:
                continue

        self.active_nodes[self.goal_node_coords].set_distance_from_start(self.active_nodes)
        self.draw_path(self.nodes[self.goal_node_coords].path)

    def draw_path(self, path,reset=False, symb="P"):
        self.map[self.start_node_coords[0]][self.start_node_coords[1]] = "\033[93mS\033[0m"
        self.map[self.goal_node_coords[0]][self.goal_node_coords[1]] = "\033[93mG\033[0m"

        for coord in path:
            if coord != self.start_node_coords and coord != self.goal_node_coords:
                self.map[coord[0]][coord[1]] = "\033[93m"+symb+"\033[0m"

        if reset:
            print('\n'.join([''.join(row) for row in self.map]))
            print()
            for coord in path:
                if coord != self.start_node_coords and coord != self.goal_node_coords:
                    self.map[coord[0]][coord[1]] = '.'
