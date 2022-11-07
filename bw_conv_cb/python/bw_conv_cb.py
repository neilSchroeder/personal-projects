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
    m = 1
    n = 1000
    for i in range(m):
        if i+1 % 100 == 0:
            print(f"{i+1}/{m}   ")
        values = np.random.rand(n)
        value_bins = np.searchsorted(cdf, values)
        random_from_cdf = [x_vals[x] for x in value_bins]

        y_vals, bins = np.histogram(random_from_cdf, bins=24, range=(80, 100))
        new_x_vals = [(bins[i]+bins[i+1])/2 for i in range(len(bins)-1)]

        try:
            x, x_err, chi = fit.fit(new_x_vals, y_vals)
            print(f"fit quality: {chi:.2f}")
        except Exception:
            count += 1

    print(f"the fit has a {100*(m - count)/m}% success rate at {n =}" )


if __name__ == '__main__':
    main()
