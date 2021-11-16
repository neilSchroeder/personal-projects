import math
import random 

class cell:

    def __init__(self, loc, is_alive) -> None:
        self.loc = loc
        self.is_alive = is_alive
        self.neighbors = self.get_neighbors()
        self.will_live = False
        self.cycles_wout_change = 0
        self.avg_age = 5

    def get_neighbors(self):
        """ returns a list of the coordinates of neighboring cells """
        neighbors = [(1,0),(-1,0),(0,1),(0,-1)]
        return [(self.loc[0]+x[0], self.loc[1]+x[1]) for x in neighbors]

    def chance_death(self):
        """ returns true if the cell dies by random chance/old age """
        rand_num = random.random()  
        x = 0.05*self.cycles_wout_change
        return rand_num < 0.5*(1 + math.erf(x - self.avg_age))