import breitWigner 
import crystalBall
import numpy as np
from scipy.optimize import minimize
from scipy.special import xlogy
from scipy.signal import fftconvolve
import matplotlib.pyplot as plt

def NLL(a,b):

    nll = xlogy(a,b)
    nll[nll==-np.inf] = 0
    nll = np.sum(nll)/len(nll)

    penalty = xlogy(np.sum(a) - a, 1 - b)
    penalty[penalty==-np.inf] = 0
    penalty = np.sum(penalty)/len(penalty)

    b = np.sum(a)*b/np.sum(b)
    chi_sqr = np.sum(np.divide(np.multiply(a-b,a-b),a))/(len(a)-1)
    
    return -2*(nll+penalty)*chi_sqr


def target(guess):
    global thisBW
    global thisCB
    global _DATA_

    thisCB.update(guess)
    y_vals = np.convolve(thisBW.y,thisCB.getY(),mode='same')
    y_vals = y_vals/np.sum(y_vals)

    return NLL(_DATA_,y_vals)


def fit(x,y,guess_cb):
    global thisBW
    global thisCB
    global _DATA_
    
    #make bw distribution
    thisBW = breitWigner.bw(x)
    thisCB = crystalBall.cb(x, guess_cb)
    _DATA_ = y
    guess = guess_cb

    bounds = [(0.01,100), (0.01,100), (-5,5), (0.1,10)]
    
    print(guess)
    result = minimize(target, 
            np.array(guess), 
            method="L-BFGS-B",
            bounds=bounds,
            options={"eps":0.0001}
            )

    print(result)
    thisCB.update(result.x)
    y_vals = np.convolve(thisBW.getY(),thisCB.getY(),mode='same')
    y_vals = np.sum(y)*y_vals/np.sum(y_vals)
    chi_sqr = np.sum(np.divide(np.multiply(y-y_vals,y-y_vals),y))/(len(y)-1)
    print("minimization complete:")
    print("mu:", 91.188+result.x[2])
    print("sigma:", result.x[3])
    print("reduced chi squared:", chi_sqr)
    print()
    print("plotting")

    fig,axs = plt.subplots(ncols=1,nrows=1) 
    axs.scatter(x,
            y,
            label='data',
            color='black',
            marker='o')
    axs.plot(x,
            y_vals,
            label='bw conv cb fit',
            color='darkred',
            linestyle='dashed')
    axs.legend(loc='best')
    fig.savefig("fit.png")
    plt.close(fig)
    return
    
