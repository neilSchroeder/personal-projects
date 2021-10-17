import random as rand
import numpy as np

def pi_from_random(n):
    """ from a flat random distribution of numbers between 0 and 1, calculate pi """
    x_vals = np.random.random(n)
    y_vals = np.random.random(n)
    r_vals = np.sqrt(np.add(np.power(x_vals,2),np.power(y_vals,2)))
    r_vals = r_vals[r_vals <= 1]
    return 4*len(r_vals)/float(n)
    

def main():
    n = int(input("please enter a number of points to throw for the mc estimate of pi:\n"))
    pi = pi_from_random(n)
    print(pi)
    return


if __name__ == "__main__":
    main()