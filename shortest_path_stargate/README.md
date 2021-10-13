# Shortest Path: Breadth First Search

An implementation of the A* search algorithm for finding the shortest path through a board with constraints.
This problem serves as a refactored solution to this [prompt on codewars.com](https://www.codewars.com/kata/59669eba1b229e32a300001a/train/python).

# Installation and Usage:

To install and run the example:
```
git clone git@github.com:neilSchroeder/tools.git
cd shortest_path_stargate
python3 test_shortest_path.py
```

# Example

<img src="example/a_star_search.gif" width="300" height="200">

Here is a visualization of the search algorithm and it's priority queue. During
the search, red indicates a lower priority (meaning it will be explored first),
while blue indicates a higher priority (meaning it will be explored last). For
the sake of compression this gif includes only 1 in every 20 frames of the priority
queue.

# To Do

Add in function to check if the board is solvable, or quit out early if the goal can't be reached.
