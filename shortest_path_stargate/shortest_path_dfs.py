import operator
import time
_min_length_ = 1000000
_valid_paths_ = []
class shortest_path:

    def __init__(self, map, start = (), goal = (), blocked = {}, loc = (), path = [], start_from_goal=False):
        self.map = [row.copy() for row in map]
        self.start = self.get_start_cell() if start == () else start
        self.goal = self.get_goal_cell() if goal == () else goal
        if start_from_goal:
            self.goal, self.start = self.start, self.goal
        self.blocked = blocked
        if self.blocked == {}:
            self.get_blocked_cells()
        self.loc = loc if loc != () else self.start
        self.path = path + [loc] if loc != () else [self.start]
        self.total_length = 0
        self.branches = []
        self.goal_reached = False
        self.get_path_length()

    def get_start_cell(self):
        i = ['S' in row for row in self.map].index(True)
        j = [x=='S' for x in self.map[i]].index(True)
        return (i,j)

    def get_goal_cell(self):
        i = ['G' in row for row in self.map].index(True)
        j = [x == 'G' for x in self.map[i]].index(True)
        return (i,j)

    def get_blocked_cells(self):
        self.blocked = { (i,j): True if self.map[i][j] == 'X' else False for i in range(len(self.map)) for j in range(len(self.map[i]))}

    def get_path_length(self):
        self.total_length = sum([(abs(self.path[i][0]-self.path[i-1][0])**2 + abs(self.path[i][1]-self.path[i-1][1])**2)**0.5 for i in range(1,len(self.path)-1)])

    def get_distance_from_a_to_b(self, a : tuple, b : tuple):
        return (abs(a[0]-b[0])**2 + abs(a[1]-b[1])**2)**0.5

    def get_available_paths(self):
        paths = []
        count = 0
        for i in range(self.loc[0]-1, self.loc[0]+2):
            for j in range(self.loc[1]-1, self.loc[1]+2):
                if i < 0 or i >= len(self.map):
                    continue
                if j < 0 or j >= len(self.map[i]):
                    continue
                if (i,j) == self.start:
                    continue
                if self.blocked[(i,j)]:
                    continue
                if (i,j) == self.loc:
                    continue
                if (i,j) in self.path:
                    count += 1
                    if count > 1:
                        return []
                    continue
                if (i,j) == self.goal:
                    return [(i,j)]
                paths.append((i,j))
        return paths


    def draw_path(self):
        for coord in self.path:
            if coord == self.start or coord == self.goal:
                continue
            if self.map[coord[0]][coord[1]] == 'S':
                continue
            if self.map[coord[0]][coord[1]] == 'G':
                continue
            self.map[coord[0]][coord[1]] = 'P'
        for i in range(len(self.map)):
            for j in range(len(self.map[i])):
                if self.map[i][j] == 'P' and (i,j) not in self.path:
                    self.map[i][j] == '.'

    def get_a_short_path(self):

        global _min_length_, _valid_paths_
        if self.total_length > _min_length_:
            return False

        available_paths = self.get_available_paths()
        self.draw_path()
        if len(available_paths) == 0:
            return False

        for next_step in available_paths:

            if next_step == self.goal:
                self.path.append(next_step)
                self.get_path_length()
                if len(self.path)==2:
                    _valid_paths_.append(self)
                return True

            self.branches.append(shortest_path(self.map,
                                                start=self.start,
                                                goal=self.goal,
                                                loc=next_step,
                                                blocked=self.blocked.copy(),
                                                path=self.path.copy()))

            if self.branches[-1].get_a_short_path():
                if self.branches[-1].total_length < _min_length_:
                    _valid_paths_.append(self.branches[-1])
                    _min_length_ = self.branches[-1].total_length

        return False

def walk(map_string):
    global _min_length_, _valid_paths_
    _min_length_ = 1000000
    _valid_paths_ = []

    map_list = map_string.split('\n')

    map_list = [list(row) for row in map_list]

    for row in map_list:
        print(row)
    print()

    sp = shortest_path(map_list, start_from_goal=True)
    sp.get_a_short_path()
    if len(_valid_paths_) > 0:
        sp = _valid_paths_[-1]
        sp.draw_path()
    for row in sp.map:
        print(row)
    print()

    return '\n'.join([''.join(row) for row in sp.map]) if len(_valid_paths_)>0 else "Oh for crying out loud..."
