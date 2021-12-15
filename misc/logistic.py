#!/usr/bin/env python3

import numpy as np
import itertools
import argparse as ap
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button

def main():
    """ main for logistic function """

    def logistic(n: int, r, x0):
        x_vals = [x0]
        for i in range(1,n+1):
            x_vals.append(r * x_vals[-1]*(1 - x_vals[-1]))
        return x_vals

    init_n = 100
    init_r = 3.
    init_x0 = 0.6

    fig, ax = plt.subplots()
    line, = plt.plot(np.linspace(0, init_n+1, init_n+1),
                     logistic(init_n, init_r, init_x0),
                     lw=2)
    ax.set_xlabel("N")

    plt.subplots_adjust(left=0.1, bottom=0.25, right=0.9, top=0.99)

    ax_r = fig.add_axes([0.1, 0.1, 0.8, 0.03])
    r_slider = Slider(
        ax= ax_r,
        label = 'r',
        valmin = 1.,
        valmax = 5.,
        valstep=0.001,
        valinit= init_r,
    )

    ax_x0 = fig.add_axes([0.1, 0.05, 0.8, 0.03])
    x0_slider = Slider(
        ax = ax_x0,
        label= 'x0',
        valmin = 0.,
        valmax = 1.,
        valstep=0.001,
        valinit=init_x0,
    )

    def update(val):
        line.set_ydata(logistic(init_n, r_slider.val, x0_slider.val))
        fig.canvas.draw()
        ax.relim()
        ax.autoscale()

    r_slider.on_changed(update)
    x0_slider.on_changed(update)

    plt.show()


if __name__ == "__main__":
    main()
