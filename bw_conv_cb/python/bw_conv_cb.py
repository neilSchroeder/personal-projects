import numpy as np
import argparse as ap

import fit

def main():
    data = np.genfromtxt("config/test_data.dat",dtype=float,delimiter='\t')

    x_vals = [x[0] for x in data]
    y_vals = [x[1] for x in data] 

    

    fit.fit(x_vals,y_vals, [1.424, 1.86, np.average(x_vals, weights=y_vals)-91.188, 1.])


main()
