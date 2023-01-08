
"""
Given an array of integers, find the first missing positive integer in linear 
time and constant space. In other words, find the lowest positive integer 
that does not exist in the array. The array can contain duplicates and 
negative numbers as well.

For example, the input [3, 4, -1, 1] should give 2. The input [1, 2, 0] should give 3
"""

def first_missing_pos_int(l):
    # 2n comparisons means O(n) time

    my_dict = {}
    for x in l:
        if x >= 1:
            my_dict[x] = True

    for i in range(1, max(l)):
        if not my_dict[i]:
            return i

    return max(l)+1



def main():

    l = [3, 4, -1, 1]

    ret = first_missing_pos_int(l)
    print(ret)


if __name__ == '__main__':
    main()