
"""
A bit of python containing several sorting algorithms
"""

import numpy as np


def main():

    arr = [np.random.randint(0,10000) for i in range(20)]
    print(arr)
    #selection_sort(arr)
    #bubble_sort(arr)
    merge_sort(arr)
    print(arr)



# here is where the sorting algorithms are:


def selection_sort(arr):
    # sorts by repeatedly finding the minimum O(N^2)
    print("[INFO] selection sort")

    for i in range(len(arr)-1):
        index = arr.index(min(arr[i::]))
        arr[i], arr[index] = arr[index], arr[i]

    return arr


def bubble_sort(arr):
    # sorts by repeatedly percolating O(N^2)
    print("[INFO] bubble sort")

    for i in range(len(arr)-1):
        for j in range(len(arr)-1):
            if arr[j] > arr[j+1]:
                arr[j], arr[j+1] = arr[j+1], arr[j]

    return arr


def merge_sort(arr):
    # sorts by splitting and merging O(Nlog(N))

    if len(arr) < 2: return

    m = len(arr)//2
    L = arr[:m]
    R = arr[m:]

    merge_sort(L)
    merge_sort(R)

    i = j = k = 0

    while i < len(L) and j < len(R):
        if L[i] <= R[j]:
            arr[k] = L[i]
            i += 1
        else:
            arr[k] = R[j]
            j += 1
        k += 1

    # the following loops catch any remaining elements if L and R and disparate
    while i < len(L):
        arr[k] = L[i]
        i += 1
        k += 1
    
    while j < len(R):
        arr[k] = R[j]
        j += 1
        k += 1


if __name__ == '__main__':
    main()