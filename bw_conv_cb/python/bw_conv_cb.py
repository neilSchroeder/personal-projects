import numpy as np
import argparse as ap

import fit


def main():
    data = np.genfromtxt("config/test_data.dat", dtype=float, delimiter='\t')

    x_vals = [x[0] for x in data]
    y_vals = [x[1] for x in data]

    print(sum(y_vals))
    print(len(x_vals))

    cdf = np.cumsum(y_vals)
    cdf = cdf / cdf[-1]

    count = 0
    m = 10
    n = 5000
    for i in range(m):
        if i+1 % 100 == 0:
            print(f"{i+1}/{m}   ")
        values = np.random.rand(n)
        value_bins = np.searchsorted(cdf, values)
        random_from_cdf = [x_vals[x] for x in value_bins]

        y_vals, bins = np.histogram(random_from_cdf, bins=48, range=(80, 100))
        new_x_vals = [(bins[i]+bins[i+1])/2 for i in range(len(bins)-1)]

        result, chi = fit.fit(new_x_vals, y_vals)
        count += result.success
        if not result.success:
            print(result)
        print(f"fit quality: {chi:.2f}")
        #print(result.x[0]+result.x[3])

    print(f"success rate: {100*count/m}%")


if __name__ == '__main__':
    main()
