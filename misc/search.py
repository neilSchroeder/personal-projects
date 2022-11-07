import numpy as np







def main():
    # generate a long random list
    # sort it
    # grab a random element
    # run a binary search

    l = np.unique([np.random.randint(100000) for i in range(100000)])
    l.sort()

    index = np.random.randint(len(l))
    val = l[index]

    bs = binary_search(l, val)
    brs = binary_recursive_search(l, val, 0, len(l))

    print(f'the given index was {index}, the returned index was {brs}')

    assert(index == bs)
    assert(index == brs)


# here are the search algorithms

def binary_search(l, val):
    """ returns the index of val in the list l using a binary search """

    z = len(l)
    a = 0
    i = (z-a) // 2

    while l[i] != val:
        if l[i] < val: # second half
            a = i
        else:          # first half
            z = i 
        
        i = a + (z-a)//2

    return i



def binary_recursive_search(l, val, a, z):
    # a recursive binary search
    i = a + (z-a)//2
    if l[i] != val:
        if l[i] < val:
            i = binary_recursive_search(l, val, i, z)
        else:
            i = binary_recursive_search(l, val, a, i)
    return i


if __name__ == '__main__':
    main()