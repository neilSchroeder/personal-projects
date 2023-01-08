"""
Given an array of integers, return a new array such that each element at index
i of the new array is the product of all the numbers in the original array 
except the one at i.

For example, if our input was [1, 2, 3, 4, 5], the expected output would be 
[120, 60, 40, 30, 24]. If our input was [3, 2, 1], the expected output would 
be [2, 3, 6]
"""

import numpy as np


def product_of_all_except(l, index):
    mask = np.ones(len(l))
    mask[index] = 0
    mask = np.multiply(mask,l)
    mask[index] = 1
    return np.prod(mask, dtype=np.int64)


def main():

    test_list = [1,2,3,4,5]
    ret = []
    for i in range(len(test_list)):
        ret.append(product_of_all_except(test_list, i))

    print(f'the returned list is {ret}')


if __name__ == '__main__':
    main()