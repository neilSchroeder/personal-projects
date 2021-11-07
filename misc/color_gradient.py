import matplotlib.pyplot as plt
import numpy as np


def color_gradient(n):
    """ makes a color gradient with n elements """
    colors = []
    for i in range(n):
        blue = (1 - 2*float(i)/float(n-1))
        if blue < 0:
            blue = 0.
        red = float(i)/float(n-1) if blue == 0 else 0.
        green = 0.0
        if i == (n-1)/2:
            green = 1.
        elif i < (n-1)/2:
            green = 2 * float(i)/float(n-1)
        else:
            green = 2 - 2*float(i)/float(n-1)

        colors.append((red, green, blue))

    return colors


def main():
    blah = "this is a string"
    n = 4
    colors = color_gradient(n)
    x = np.ones(n)
    y = np.linspace(0, n, n)
    print(len(x), len(y))

    fig, ax = plt.subplots(1, 1)
    ax.scatter(x, y, color=colors)

    plt.show()

if __name__ == '__main__':
    main()
