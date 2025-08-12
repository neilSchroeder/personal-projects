import time
from node_bfs_class import node
import heapq as hq
from PIL import Image
import imageio
import numpy as np
import os

class graph:
    """a container structure for nodes"""

    def __init__(self, map, watch_evolution=True):
        self.map = [x.copy() for x in map]
        self.pixel_array = [[(0,0,0) if val == 'X' else (255,255,255) for val in row] for row in map]
        self.start_node_coords = self.get_start_node_coords()
        self.goal_node_coords = self.get_goal_node_coords()
        self.nodes = {}
        self.active_nodes = {}
        self.watch_evolution = watch_evolution
        self.populate_nodes()

    def get_start_node_coords(self):
        """gets the coordinates of the start node"""
        i = ['S' in row for row in self.map].index(True)
        j = [x == 'S' for x in self.map[i]].index(True)
        return (i, j)

    def get_goal_node_coords(self):
        """gets the coordinates of the goal node"""
        i = ['G' in row for row in self.map].index(True)
        j = [x == 'G' for x in self.map[i]].index(True)
        return (i, j)

    def populate_nodes(self):
        self.nodes = {(i, j): node(self.map,
                      (i, j),
                      (i, j) == self.goal_node_coords,
                      (i, j) == self.start_node_coords)
                      for i in range(len(self.map))
                      for j in range(len(self.map[i]))
                      if self.map[i][j] != 'X'}

    def get_path(self, end):
        ret = []
        n = end
        while self.nodes[n].parent != self.start_node_coords:
            ret.append(n)
            n = self.nodes[n].parent

        ret.append(n)

        return ret[::-1]

    def a_star_search(self):
        """ different approach to A* """
        queue = []
        hq.heappush(queue, (0, self.start_node_coords))

        self.nodes[self.start_node_coords].set_distance_to_goal(self.goal_node_coords)
        self.nodes[self.start_node_coords].set_path_length(0.)

        while queue:
            tup = hq.heappop(queue)
            n = self.nodes[tup[1]]

            if n.loc == self.goal_node_coords:
                self.draw_path(self.get_path(n.loc))
                #self.draw_path_to_image(self.get_path(n.loc), final_path=True)
                return True

            if not n.is_active:
                n.activate()
                n.set_distance_to_start(self.start_node_coords)
                n.set_distance_to_goal(self.goal_node_coords)

            for neighbor in n.neighbors:
                dist = n.path_length + n.distance_to_node(neighbor)
                if dist < self.nodes[neighbor].path_length:
                    self.nodes[neighbor].set_parent(n.loc)
                    self.nodes[neighbor].set_distance_to_goal(self.goal_node_coords)
                    self.nodes[neighbor].set_path_length(dist)
                    if (self.nodes[neighbor].cost, neighbor) not in queue:
                        hq.heappush(queue, (self.nodes[neighbor].cost, neighbor))

            if self.watch_evolution and queue:
                self.draw_path([x[1] for x in queue], reset=True, symb='Q')
                time.sleep(0.005)

            #if queue and len(queue) > 1:
            #    self.draw_path_to_image(queue, final_path=False)

        return False

    def draw_path(self, path, reset=False, symb="P"):
        """draws a path, and resets the board if necessary"""
        changed_coords = []
        snc = self.start_node_coords
        gnc = self.goal_node_coords
        self.map[snc[0]][snc[1]] = "\033[93mS\033[0m"
        self.map[gnc[0]][gnc[1]] = "\033[93mG\033[0m"

        for coord in path:
            if coord != snc and coord != gnc:
                self.map[coord[0]][coord[1]] = "\033[93m"+symb+"\033[0m"
                changed_coords.append(coord)

        if reset:
            print('\n'.join([''.join(row) for row in self.map]))
            print(flush=True)
            for coord in changed_coords:
                if coord != snc and coord != gnc:
                    self.map[coord[0]][coord[1]] = '.'

    def draw_path_to_image(self, path, final_path=False):
        """ creates images pixel by pixel """
        if final_path:
            adjusted_priorities = [ self.nodes[x].path_length * 255 / self.nodes[self.goal_node_coords].path_length for x in path]
            for i,x in enumerate(path):
                self.pixel_array[x[0]][x[1]] = (0 + adjusted_priorities[i], 0, 255 - adjusted_priorities[i])
        else:
            min_priority = min([x[0] for x in path])
            adjusted_priorities = [x[0]-(min_priority+1) for x in path]
            adjusted_priorities = [x * 255 / max(adjusted_priorities) for x in adjusted_priorities]
            for i,x in enumerate(path):
                self.pixel_array[x[1][0]][x[1][1]] = (255-adjusted_priorities[i], 0, 0+adjusted_priorities[i])

        snc = self.start_node_coords
        gnc = self.goal_node_coords
        self.pixel_array[snc[0]][snc[1]] = (0, 0, 255)
        self.pixel_array[gnc[0]][gnc[1]] = (255, 0, 0)

        pixel_array = np.array(self.pixel_array, np.uint8)
        new_image = Image.fromarray(pixel_array)
        new_image.save(f'example/queue_status_{time.time()}.png')

        if not final_path:
            for i,x in enumerate(path):
                self.pixel_array[x[1][0]][x[1][1]] = (255, 255, 255)


    def create_gif(self):
        """ creates a gif of all the most recently created pngs """
        filenames = [x for x in os.listdir('example') if x.endswith('.png')]
        filenames.sort()
        filenames = filenames[::20] + [filenames[-1] for i in range(240)] #drop frames
        images = list(map(lambda f: imageio.imread('example/'+f), filenames))
        imageio.mimsave(os.path.join('example/a_star_search.gif'), images, format='GIF', fps=120)
        #os.
