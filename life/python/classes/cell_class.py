
class cell:

    def __init__(self, loc, is_alive) -> None:
        self.loc = loc
        self.is_alive = is_alive
        self.neighbors = self.get_neighbors()
        self.will_live = False
        pass

    def get_neighbors(self):
        neighbors = [(1,0),(-1,0),(0,1),(0,-1)]
        return [(self.loc[0]+x[0], self.loc[1]+x[1]) for x in neighbors]