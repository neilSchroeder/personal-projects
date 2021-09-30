
class node:

    def __init__(self, map, location, is_goal, is_start):
        self.map = map
        self.loc = location
        self.is_goal = is_goal
        self.is_start = is_start
        self.neighbors = {}
        self.is_active = False
        self.distance_from_start_node = None
        self.min_distance_to_start = 99999

    def activate(self):
        self.is_active = True
        self.neighbors = self.get_neighboring_nodes()

    def set_distance_from_start(self):


    def get_neighboring_nodes(self):
        max_row = len(self.map)
        max_col = len(self.map[0])
        possible_neighbors = [(i,j) for i in range(self.loc[0]-1 if self.loc[0]-1 > 0 else 0 , self.loc[0]+2 if self.loc[0]+2 < max_row else max_row) for j in range(self.loc[1]-1 if self.loc[1]-1 > 0 else 0, self.loc[2]+2 if self.loc[2]+2 < max_col else max_col)]
        for node in possible_neighbors:
            if self.map[node[0]][node[1]] != 'X' and node is not self.loc:
                self.neighbors[node] = self.distance_to_node(node)

    def distance_to_node(self, node):
        return (abs(self.loc[0]-node[0])**2 + abs(self.loc[1]-node[1])**2)**0.5



class graph:

    def __init__(self, map):
        self.map = map
        self.start_node_coords = self.get_start_node_coords(self.map)
        self.goal_node_coords = self.get_goal_node_coords(self.map)
        self.nodes = {}
        self.active_nodes = {}
        self.path = []
        self.populate_nodes()
        self.num_inactive_nodes = self.count_inactive_nodes()

    def get_start_node_coords(self):
        i = ['S' in row for row in self.map].index(True)
        j = [x=='S' for x in self.map[i]].index(True)
        return (i,j)

    def get_goal_node_coords(self):
        i = ['G' in row for row in self.map].index(True)
        j = [x == 'G' for x in self.map[i]].index(True)
        return (i,j)

    def populate_nodes(self):
        for i in range(len(self.map)):
            for j in range(len(self.map[i])):
                if self.map[i][j] != 'X':
                    self.nodes[(i,j)] = node(self.map, (i,j), (i,j)==self.goal_node, (i,j)==self.start_node)

    def count_inactive_nodes(self):
        return sum([not self.nodes[node].is_active for node in self.nodes.keys()])

    def search_for_goal(self):

        start_node = self.nodes[self.start_node]
        start_node.activate()
        queue = start_node.neighbors

        while self.count_inactive_nodes() != 0 and len(queue) > 0:
            #activate the nodes in the queue
            node = queue.pop(0)
            if not node.is_active:
                node.activate()
                self[node.loc] = node
                node.set_distance_from_start(self.active_nodes)
                queue = queue + node.neighbors()



class node:

    __init__(self, location, distance_map):


def wire_DHD_SG1(map):

    map_list = map_string.split('\n')

    map_list = [list(row) for row in map_list]

    my_graph = graph(map_list)
    my_graph.search_for_goal()
