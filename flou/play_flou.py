
class flou:
    """ class to handle the game of flou """

    def __init__(self, game_map):
        self.game_map = self.read_map(game_map)
        self.start_nodes = self.get_start_nodes()
        self.solution = []

    def read_map(self, game_map):
        """ reads in the game_map to a list of lists """
        border = ["+", "|", "-"]
        map = game_map.split('\n')
        map = [list(x) for x in map]
        ret = {}
        for i, row in enumerate(map):
            for j, val in enumerate(row):
                if val in border:
                    continue
                ret[(i, j)] = val

        return ret

    def get_neighbors(self, key):
        """ gets the available neighbors """
        ret = []
        neighbor_keys = {
            (key[0] - 1, key[1]): "Down",
            (key[0] + 1, key[1]): "Up",
            (key[0], key[1] - 1): "Left",
            (key[0], key[1] + 1): "Right"
                        }
        for nk in neighbor_keys:
            if nk in self.game_map.keys() and self.game_map[key] == '.':
                ret.append(neighbor_keys[key])

    def get_start_nodes(self):
        """ gets the start nodes and their available directions """
        ret = {}
        for key in self.game_map.keys():
            if self.game_map[key] == "B"
                ret[key] = self.get_neighbors(key)

        return ret

    def get_next_node(self, key, dir):
        """ gets the next cell in direction dir """
        i = key[0]-int(dir=='Left')+int(dir=='Right')
        j = key[1]-int(dir=='Down')+int(dir=='Up')
        return (i, j)

    def turn_right(self,dir):
        """ returns the direction to right of the input direction """
        turns = {'Up': 'Right', 'Down':'Left', 'Left': 'Up', 'Right': 'Down'}
        return turns[dir]

    def move(self, key, dir):
        """
        moves the current key in a given direction
        if an obstacle is encountered, you can only turn right.
        if there's still an obstacle after turning right, finish.
        """
        next_node = self.get_next_node(key, dir)
        while (next_node in self.game_map.keys()
                and self.game_map[next_node] == '.'):
            self.game_map[next_node] = 'b'
            next_node = self.get_next_node(next_node,dir)

        next_node = self.get_next_node(next_node, self.turn_right(dir))
        if next_node in self.game_map.keys() and self.game_map[next_node] == '.':
            self.move(next_node, self.turn_right(dir))

    def play(self):

        #build all solutions
        solutions = []
        for key in self.start_nodes.keys():
            if len(solutions) == 0:
                for dir in self.start_nodes[key]:
                    solutions.append([[key[0], key[1], self.start_nodes[key]]])
            else:
                    :
                    for dir in self.start_nodes[key]:
                        soln_copy = [x.copy() for x in soln]
                        soln_copy += [[key[0],key[1], dir]]



        #test all solutions



def play_flou(game_map):
    """ uses the game_map to play flou """

    game = flou(game_map)
    if game.play():
        return game.solution

    return False
