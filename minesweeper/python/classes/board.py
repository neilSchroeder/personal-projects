import time

from matplotlib.pyplot import close, flag

_kVerbose = False
_kWatch = True

from python.classes.tile import tile

class board:
    # board class

    def __init__(self, map, n_bombs) -> None:
        self.valid_map = True
        self.map = {}
        self.remaining_bombs = n_bombs
        self.rows = None
        self.cols = None
        self.extract_map(map)
        self.open_tiles = [
            key for key in self.map.keys() if self.map[key].is_open]

    def extract_map(self, m):
        # extracts the map into tiles
        m = [x.split(' ') for x in m.strip().split('\n')]
        self.rows = len(m)
        self.cols = len(m[0])
        for i, row in enumerate(m):
            for j, t in enumerate(list(row)):
                is_open = True if t == '0' else False
                self.map[(i, j)] = tile((i, j), is_open, str(t))

    def update_map(self, **options):
        self.map_string = ""
        for i in range(self.rows):
            row = []
            for j in range(self.cols):
                tile_char = "\033[93m?\033[0m" if not self.map[(i, j)].is_open else f"{self.map[(i, j)].nearby_bombs}"
                if 'bound' in options:
                    tile_char = f"\033[94m{tile_char}\033[0m" if (i,j) in options['bound'] else tile_char
                tile_char = "x" if self.map[(i, j)].bomb else tile_char
                row.append(tile_char)
            self.map_string = self.map_string + " ".join(row) + '\n'
    
    def print(self):
        # prints the board to the terminal
        self.update_map(bound=self.get_boundary_tiles('open'))
        print(f'There are {self.remaining_bombs} bombs remaining')
        print(self.map_string)
        time.sleep(0.1)

    def get_boundary_tiles(self, status):
        # returns a list of all non-zero tiles which are open and not flagged as bombs
        ret = []
        _kOpenTiles = True if status == "open" else False
        if _kOpenTiles:
            # get all tiles which are open and have at least one closed neighbor
            for t in self.map:
                # don't include bombs
                if self.map[t].bomb:
                    continue
                if self.map[t].is_open and any(n in self.map and (not self.map[n].is_open) for n in self.map[t].nearby_tiles):
                    ret.append(t)
        else:
            # get all tiles which are closed and have at least one open neighbor
            for t in self.map:
                # don't include bombs
                if self.map[t].bomb:
                    continue
                if not self.map[t].is_open and any(n in self.map and self.map[n].is_open for n in self.map[t].nearby_tiles):
                    ret.append(t)
        return ret

    def open(self, iRow, iCol):
        # proxy for "open" function in codewars.com
        if _kVerbose: print(f"({iRow},{iCol}),", self.map[(iRow,iCol)].init_char)

        if self.map[(iRow,iCol)].init_char == "x" and not self.map[(iRow,iCol)].bomb:
            self.valid_map = False
            print(f"({iRow},{iCol}) BOOOOOOMMMM, that was a bomb")
            return 
            
        nearby = sum([1 for x in self.map[(iRow,iCol)].nearby_tiles if x in self.map and self.map[x].init_char =='x'])
        if _kVerbose: print(f'found {nearby} bombs when opening ({iRow},{iCol})')
        return nearby

    def get_closed_neighbors(self, t):
        return [x for x in self.map[t].nearby_tiles if x in self.map and not self.map[x].is_open and not self.map[x].bomb]

    def get_bomb_probs(self, open_tiles):
        # returns a dictionary of probabilities
        # indicating how likely it is that any closed tile is a bomb
        probs = {}
        if len(open_tiles) == 0:
            # you really worked yourself into a corner
            self.valid_map = False
            return None

        # evaluate probability of tile to be bomb
        for ot in open_tiles:
            eff_bombs = self.get_effective_bomb_count(ot)
            closed_neighbors = self.get_closed_neighbors(ot)
            if eff_bombs == len(closed_neighbors) and len(closed_neighbors) > 0:
                for x in closed_neighbors:
                    probs[x] = [(eff_bombs, closed_neighbors, 1.)]
                break
            if eff_bombs < len(closed_neighbors):
                for x in closed_neighbors:
                    if x not in probs:
                        probs[x] = []
                    probs[x].append((
                        eff_bombs,
                        len(closed_neighbors),
                        float(eff_bombs)/float(len(closed_neighbors))
                    ))
            if eff_bombs > len(closed_neighbors):
                print("too many bombs")
                self.valid_map = False

        return probs

    def assign_bombs(self, probs, _kAllowGuess):
        # assigns tiles with probability 1 as bombs
        self.update_map(bound=self.get_boundary_tiles('open'))
        if probs is None or len(probs) < 1:
            return

        _bombAssigned = False

        for key in probs:
            if any(x[-1] == 1. for x in probs[key]):
                if _kVerbose:
                    print(f'assigning {key} as bomb')
                self.map[key].bomb = True
                self.map[key].is_open = True
                _bombAssigned = True
                self.remaining_bombs -= 1
                self.update_map()
                if _kWatch: self.print()
                return

        # if we didn't find a tile which is guaranteed to be a bomb
        if not _bombAssigned:
            max_prob = max([y[2] for x in probs for y in probs[x]])
            max_prob_tiles = [x for x in probs if any(y[2] == max_prob for y in probs[x])]

            if not _kAllowGuess and max_prob == 1/len(max_prob_tiles):
                print("I'm being forced to guess, fail")
                self.valid_map = False
                return

            avg_probs = {x: sum(y[2] for y in probs[x])/len(probs[x]) for x in probs}
            max_avg_prob = max(avg_probs[x] for x in avg_probs)
            x, *_ = [x for x in avg_probs if avg_probs[x] == max_avg_prob]

            _kAllowAssign = True
            for t in self.map[x].nearby_tiles:
                if t not in self.map or self.map[t].bomb or not self.map[t].is_open:
                    continue
                flagged_bombs = sum([1 for x in self.map[t].nearby_tiles if x in self.map and self.map[x].bomb])
                if self.map[t].nearby_bombs == flagged_bombs:
                    self.update_map()
                    _kAllowAssign = False
                    break
                elif self.map[t].nearby_bombs < flagged_bombs:
                    print("too many bombs")
                    self.valid_map = False
                    return
                else:
                    pass

            if _kAllowAssign:
                #for y in probs:
                #    print(y, probs[y], avg_probs[y])
                self.map[x].bomb = True
                self.map[x].is_open = True

                if _kVerbose: 
                    for y in probs:
                        print(y, probs[y], avg_probs[y])
                    print(f'flagging {x} as a bomb')

                self.remaining_bombs -= 1
                self.update_map()
                if _kWatch: self.print()
                return

    def get_effective_bomb_count(self, t):
        # returns the effective number of bombs this square has 
        if self.map[t].nearby_bombs is None:
            return -1
        return self.map[t].nearby_bombs - len([x for x in self.map[t].nearby_tiles if x in self.map and self.map[x].bomb])

    def check_open_tiles(self, boundary):
        # opens tiles when all bombs around a boundary tile are accounted for
        ret = False

        open_tiles = [x for x in boundary if self.map[x].is_open and not self.map[x].bomb]

        while open_tiles:
            ot = open_tiles.pop(0)
            if len(self.get_closed_neighbors(ot)) == 0:
                continue

            # get effective bomb count
            if self.get_effective_bomb_count(ot) == 0:
                # we can safely open all nearby tiles
                closed_neighbors = self.get_closed_neighbors(ot)
                if len(closed_neighbors) > 0:
                    open_tiles += closed_neighbors
                    for x in closed_neighbors:
                        ret = True
                        self.map[x].update(self.open(x[0],x[1]))
                        self.update_map()
                        if not self.valid_map:
                            print(ot)
                            return False


        return ret
