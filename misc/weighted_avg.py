import numpy as np

def main():

    print("weighted average")
    vals = input("values:\n")
    weights = input("weights:\n")

    vals = [float(x) for x in vals.split(',')]
    weights = [float(x) for x in weights.split(',')]

    numerator = sum(x/(y**2) for x,y in zip(vals,weights))
    denominator = sum(1/(y**2) for y in weights)

    mean = numerator/denominator

    std_dev = np.sqrt(
        sum( (x - mean)**2 / w**2 for w,x in zip(weights,vals) ) / (
            ((len(weights) - 1)/len(weights)) * sum( 1/w**2 for w in weights )
        )
    )

    unc_mean = std_dev / np.sqrt(len(weights))

    print(f"weighted average is {100*(1 - numerator/denominator)} $\pm$ {100 * unc_mean}")

if __name__ == '__main__':
    main()
