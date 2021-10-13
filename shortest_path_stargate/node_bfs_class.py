from itertools import product
import math

class node:
    """class for nodes in the map"""
    def __init__(self, map, location, is_goal, is_start):
        self.loc = location
        self.map = map
        self.max_row = len(map)
        self.max_col = len(map[0])
        self.is_goal = is_goal
        self.is_start = is_start
        self.neighbors = {}
        self.is_active = False
        self.distance_to_start = 99999
        self.distance_to_goal = 99999
        self.path_length = 99999
        self.parent = ()
        self.cost = 999999

    def activate(self):
        self.is_active = True
        self.get_neighboring_nodes()

    def set_distance_to_start(self, start_loc):
        self.distance_to_start = self.distance_to_node(start_loc)

    def set_distance_to_goal(self, goal_loc):
        self.distance_to_goal = self.distance_to_node(goal_loc)

    def set_parent(self, p):
        self.parent = p

    def set_path_length(self, pl):
        self.path_length = pl
        x = 0.0
        self.cost = (1+x)*self.distance_to_goal + (1-x)*pl

    def get_neighboring_nodes(self):
        """ gets the coordinates of nodes around this node """
        i_min = self.loc[0]-1 if self.loc[0]-1 > 0 else 0
        i_max = self.loc[0]+2 if self.loc[0]+2 < self.max_row else self.max_row
        j_min = self.loc[1]-1 if self.loc[1]-1 > 0 else 0
        j_max = self.loc[1]+2 if self.loc[1]+2 < self.max_col else self.max_col
        self.neighbors = [(i, j) for i,j in product(range(i_min,i_max),range(j_min,j_max))
                            if self.map[i][j] != 'X' and (i,j) is not self.loc]

    def distance_to_node(self, n):
        """returns the distance to a node with coodinates n"""
        return math.sqrt(abs(self.loc[0]-n[0])**2 + abs(self.loc[1]-n[1])**2)

    def print(self):
        """prints out information about this node """
        print('location:', self.loc)
        print('neighbors:', self.neighbors)
        print('distance to start node:', self.distance_from_start_node)
        print('path to start node:', self.path)
