
import shortest_path
import time

def main():

    map_10x10 = "XX.S.XXX..\nXXXX.X..XX\n...X.XX...\nXX...XXX.X\n....XXX...\nXXXX...XXX\nX...XX...X\nX...X...XX\nXXXXXXXX.X\nG........X"
    map_5x5 = ".S...\nXXX..\n.X.XX\n..X..\nG...X"
    map_3x3 = ".S.\n...\n.G."
    map_big = ".X.X.X....XXXXXX...X\nXX.XX.XXXXXXXXXXX..X\n.X.X.XX..X..X.XXXXXX\nX.X..XXX...XX.X.XXX.\nX.X..X..XXX.X.X.X...\n.XXX..XXXXX.X.X..XX.\nX.XX.SX......XXX..X.\n.XXXXX.XXX...XX..X..\n....X.XX..X.XX.X..XX\n....X..XX..XX..X.XX.\nX...X..XX.X.X.XX...X\n.XXX.........X.XX..G\n..XX.XX.XX.X.XXXXXX.\n.X.X...X.X.XXXX..X.X\n..X..XXX.XX....XXXX.\nXX..XXXXXXX.....XXXX\nXXXX.X.X..XXXXXX...X\nX...X..X..XXXX..X..X\nX.XXXXX..XX..XXX.X.X\nXX.X.XX.XXXX.X..X.XX"

    start = time.time()
    soln = shortest_path.walk(map_big)
    print(soln)
    print(time.time()-start)

if __name__ == '__main__':
    main()
