class node:
    """class for nodes in the map"""
    def __init__(self, map, location, is_goal, is_start):
        self.map = map
        self.loc = location
        self.is_goal = is_goal
        self.is_start = is_start
        self.neighbors = {}
        self.is_active = False
        self.distance_from_start_node = 99999
        self.path = []
        self.indegree = 99999

    def activate(self):
        self.is_active = True

    def set_distance_from_start(self, active_nodes):
        """ sets the minimum distance to the start node """
        if self.is_start:
            self.path = [self.loc]
            self.distance_from_start_node = 0
            return

        min_key = ()
        available_keys = []
        for key in self.neighbors.keys():
            if key not in active_nodes.keys():
                continue
            if key == self.loc:
                continue
            if active_nodes[key].indegree > self.indegree:
                continue
            if active_nodes[key].is_start:
                self.distance_from_start_node = self.distance_to_node(key)
                self.path.append(key)
                return
            dist = (self.distance_to_node(key)
                    + active_nodes[key].distance_from_start_node
                    )
            if dist <= self.distance_from_start_node:
                self.distance_from_start_node = dist
                min_key = key
                available_keys.append(min_key)

        self.path = active_nodes[min_key].path + [min_key]

    def get_neighboring_nodes(self):
        """ gets the coordinates of nodes around this node """
        max_row = len(self.map)
        max_col = len(self.map[0])
        i_min = self.loc[0]-1 if self.loc[0]-1 > 0 else 0
        i_max = self.loc[0]+2 if self.loc[0]+2 < max_row else max_row
        j_min = self.loc[1]-1 if self.loc[1]-1 > 0 else 0
        j_max = self.loc[1]+2 if self.loc[1]+2 < max_col else max_col
        possible_neighbors = [(i, j)
                              for i in range(i_min, i_max)
                              for j in range(j_min, j_max)]
        for n in possible_neighbors:
            if self.map[n[0]][n[1]] != 'X' and n is not self.loc:
                self.neighbors[n] = self.distance_to_node(n)

    def distance_to_node(self, n):
        """returns the distance to a node with coodinates n"""
        return (abs(self.loc[0]-n[0])**2 + abs(self.loc[1]-n[1])**2)**0.5

    def print(self):
        """prints out information about this node """
        print('location:', self.loc)
        print('neighbors:', self.neighbors)
        print('distance to start node:', self.distance_from_start_node)
        print('path to start node:', self.path)

    def set_indegree(self, parent_indegree):
        """sets the indegree of the node based on it's parent node"""
        if self.is_start:
            self.indegree = 0
        else:
            self.indegree = parent_indegree+1
