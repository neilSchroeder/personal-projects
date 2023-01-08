"""
Given a list of numbers and a number k, return whether any two numbers from the list add up to k.
For example, given [10, 15, 3, 7] and k of 17, return true since 10 + 7 is 17.
"""

import numpy as np

k = 17
l = [10,15,3,7]


def find_two_that_add(k, l):

    diffs = {}
    for index,n in enumerate(l):
        if n not in diffs.keys():
            diffs[k-n] = index
        else:
            return l[diffs[n]], n
    
    return None, None

def main():

    k = np.random.randint(1, 100)
    l = [np.random.randint(-100, 100) for i in range(20)]

    a,b = find_two_that_add(k, l)
    if a and b:
        print(f"Found that {a} + {b} = {k}")
    else:
        print(f"did not find two numbers in\n{l}\nthat add to {k}")


if __name__ == '__main__':
    main()


