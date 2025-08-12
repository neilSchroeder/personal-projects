class tile:
    # tile class

    def __init__(self, loc, is_open, init_char) -> None:
        self.loc = loc
        self.init_char = init_char
        self.is_open = is_open
        self.bomb = False
        self.nearby_bombs = None if not self.is_open else 0
        self.nearby_tiles = self.get_nearby_tiles()

    def print(self):
        print("~"*40)
        print(f"{self.loc=}")
        print(f"{self.init_char=}")
        print(f"{self.is_open=}")
        print(f"{self.bomb=}")
        print(f"{self.nearby_bombs=}")
        print(f"{self.nearby_tiles=}")
        print("~"*40)

    def get_nearby_tiles(self):
        return [(i, j) for i in range(self.loc[0]-1, self.loc[0]+2) for j in range(self.loc[1]-1, self.loc[1]+2) if (i,j) != self.loc]

    def update(self, nearby_bombs):
        self.nearby_bombs = nearby_bombs
        self.is_open = True