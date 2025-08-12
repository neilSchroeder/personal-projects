from equation_class import equation
import time
import random
from itertools import permutations

"""
https://www.codewars.com/kata/574e890e296e412a0400149c/train/python
test script for equal_to_24
"""


def equal_to_24(a, b, c, d):
    """ determines whether the number 24 can be made out of the 4 numbers
        provided
    """
    target = 24
    nums = [a, b, c, d]
    nums = list(set(list(permutations(nums, len(nums)))))
    for num in nums:
        eqn = equation(num, target)
        if eqn.solve():
            eqn.build_equation_string()
            return eqn.equation_string

    return "It's not possible!"


def main():

    tests = []
    for i in range(1000):
        tests.append([random.randint(0, 100) for i in range(4)])

    tot_time = 0
    for i in range(len(tests)):
        print(tests[i])
        start = time.time()
        print(equal_to_24(*tests[i]))
        tot_time += time.time() - start
        print(tot_time/float(i+1))

    print(tot_time)
    return


if __name__ == '__main__':
    main()
