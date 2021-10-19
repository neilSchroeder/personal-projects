import random as rand
import numpy as np

def pi_from_random(n):
    """ from a flat random distribution of numbers between 0 and 1, calculate pi """
    area_circle = 0
    for i in range(n):
        area_circle += int( (rand.random()**2 + rand.random()**2) <= 1)
    return 4*float(area_circle)/float(n)
    

def main():
    n = int(input("please enter a number of points to throw for the mc estimate of pi:\n"))
    pi = pi_from_random(n)
    print(pi)
    return


if __name__ == "__main__":
    main()