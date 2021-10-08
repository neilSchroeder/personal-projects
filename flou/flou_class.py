class flou:
    """ class to handle the game of flou """

    def __init__(self, game_map):
        self.game_map = self.read_map(game_map)
        self.test_map = self.game_map.copy()
        self.game_width = max([key[1] for key in self.game_map.keys()])
        self.game_height = max([key[0] for key in self.game_map.keys()])
        self.start_nodes = self.get_start_nodes()
        self.solution = []

    def read_map(self, game_map):
        """ reads in the game_map to a list of lists """
        colors = list("ABCDEFGHIJKLMNOP")
        border = ['+', '\|', "-"]
        map = game_map.split('\n')
        map = [list(x.strip("+|-")) for x in map]
        map = [x for x in map if len(x) > 0 ]
        ret = {}
        i_start = 0
        ret = {(i, j): val for i, row in enumerate(map) for j, val in enumerate(row) if val not in border}
        return ret

    def draw(self, map):
        """draws the status of the provided map"""
        i_row = 0
        while i_row <= self.game_height:
            print([map[(i_row, j)] for j in range(0,self.game_height+1)])
            i_row += 1
        print()

    def get_neighbors(self, key, m=None):
        """ gets the available neighbors """
        neighbor_keys = {
            (key[0] + 1, key[1]): "Down",
            (key[0] - 1, key[1]): "Up",
            (key[0], key[1] - 1): "Left",
            (key[0], key[1] + 1): "Right"
                        }
        if m is None:
            m = self.test_map
        return [neighbor_keys[nk] for nk in neighbor_keys.keys()
                    if nk in m.keys() and m[nk] == '.']

    def check_neighbors(self, key, m=None):
        """checks whether a given key has open neighbors"""
        neighbor_keys = {
            (key[0] + 1, key[1]): "Down",
            (key[0] - 1, key[1]): "Up",
            (key[0], key[1] - 1): "Left",
            (key[0], key[1] + 1): "Right"
                        }
        if m is None:
            m = self.test_map

        ret  = [1 for nk in neighbor_keys.keys()
                    if nk in m.keys() and m[nk] == '.']

        return len(ret) > 0

    def get_start_nodes(self):
        """ gets the start nodes and their available directions """
        colors = list("ABCDEFGHIJKLMNOP")
        ret = {}
        border = ["+", "|", "-"]
        for key in self.game_map.keys():
            if self.game_map[key] != "." and self.game_map[key] not in border:
                self.game_map[key] = colors.pop(0)
                ret[key] = self.get_neighbors(key)

        return ret

    def get_next_node(self, key, dir):
        """ gets the next cell in direction dir """
        if dir == 'Up':
            return (key[0] - 1, key[1])
        if dir == 'Down':
            return (key[0] + 1, key[1])
        if dir == 'Left':
            return (key[0], key[1] - 1)
        if dir == 'Right':
            return (key[0], key[1] + 1)

    def turn_right(self,dir):
        """ returns the direction to right of the input direction """
        if dir == 'Up':
            return 'Right'
        if dir == 'Down':
            return 'Left'
        if dir == 'Left':
            return 'Up'
        if dir == 'Right':
            return 'Down'

    def move(self, coord, m=None):
        """
        moves the current key in a given direction
        if an obstacle is encountered, you can only turn right.
        if there's still an obstacle after turning right, finish.
        """
        if m is None:
            m = self.test_map
        key = (coord[0], coord[1])
        dir = coord[2]
        next_node = self.get_next_node(key, dir)
        last_node = key
        while (next_node in m.keys() and m[next_node] == '.'):
            m[next_node] = m[last_node].lower()
            last_node = next_node
            next_node = self.get_next_node(next_node, dir)

        if last_node in self.start_nodes.keys():
            return False

        next_node = self.get_next_node(last_node, self.turn_right(dir))
        if (next_node in m.keys() and m[next_node] == '.'):
            self.move([last_node[0], last_node[1], self.turn_right(dir)], m=m)

        return True

    def map_is_solved(self, map):
        counts = {map[k]: 0 for k in self.start_nodes.keys()}
        for key in map.keys():
            if map[key] == '.':
                return False
            else:
                counts[map[key].upper()] += 1
        return all([counts[x] > 1 for x in counts])

    def play(self):
        """ plays flou """
        valid_paths = []
        path_maps = []
        for key in self.start_nodes.keys():
            for dir in self.start_nodes[key]:
                path_map = self.game_map.copy()
                self.move([key[0], key[1], dir], m=path_map)
                if self.map_is_solved(path_map):
                    self.solution = [[key[0], key[1], dir]]
                    return True
                if all([self.check_neighbors(k, m=path_map) for k in self.start_nodes.keys() if k != key]):
                    valid_paths.append([[key[0],key[1],dir]])
                    path_maps.append(path_map.copy())

        if len(self.start_nodes.keys()) == 1:
            return False

        while valid_paths:

            path = valid_paths.pop(0)
            path_map = path_maps.pop(0)
            used_keys = [(x[0], x[1]) for x in path]
            temp_map = path_map.copy()
            for key in self.start_nodes.keys():
                if key in used_keys:
                    continue

                new_dirs = self.get_neighbors(key, m=temp_map)
                for dir in new_dirs:
                    self.move([key[0], key[1], dir], path_map)
                    remaining_moves = [self.check_neighbors(k, path_map) for k in self.start_nodes.keys() if k not in used_keys and k != key]
                    if all(remaining_moves) or len(remaining_moves) == 0:
                        if len(remaining_moves) == 0 and self.map_is_solved(path_map):
                            self.solution = path.copy() + [[key[0], key[1], dir]]
                            self.game_map = path_map.copy()
                            return True
                        else:
                            if len(remaining_moves) != 0:
                                valid_paths.append(path.copy() + [[key[0], key[1], dir]])
                                path_maps.append(path_map.copy())

                    path_map = temp_map.copy()
        return False
