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

def scan(guess):
    global thisBW
    global thisCB
    global _DATA_

    ret = guess

    scan_alpha = np.arange(0.1,100,0.5)
    scan_n = np.arange(0.1,100,0.5)
    scan_mean = np.arange(-5,5,0.025)
    scan_width = np.arange(0.1,10,0.1)

    guesses_mean = [[ret[0],ret[1],x,ret[3]] for x in scan_mean]
    nll_mean = np.array([target(x) for x in guesses_mean])
    ret[2] = scan_mean[np.argmin(nll_mean)]

    guesses_width = [[ret[0],ret[1],ret[2],x] for x in scan_width]
    nll_width = np.array([target(x) for x in guesses_width])
    ret[3] = scan_width[np.argmin(nll_width)]

    guesses_alpha = [[x,ret[1],ret[2],ret[3]] for x in scan_alpha]
    nll_alpha = np.array([target(x) for x in guesses_alpha])
    ret[0] = scan_alpha[np.argmin(nll_alpha)]

    guesses_n = [[ret[0],x,ret[2],ret[3]] for x in scan_n]
    nll_n = np.array([target(x) for x in guesses_n])
    ret[1] = scan_n[np.argmin(nll_n)]
  
    return ret

def errors(result,min_chi):

    ret = []

    scan_mean = np.arange(-(2*abs(result[2])),2*abs(result[2]),0.0001)
    scan_width = np.arange(0.0001,2*result[3],0.0001)

    par_set_mean = [[result[0],result[1],x,result[3]] for x in scan_mean]
    par_set_width = [[result[0],result[1],result[2],x] for x in scan_width]

    chi_mean = np.array([target(x) for x in par_set_mean])
    chi_width = np.array([target(x) for x in par_set_width])

    sigma_mask_mean = chi_mean >= np.e * min_chi
    sigma_mask_width = chi_width >= np.e * min_chi

    val = []
    for i in range(len(sigma_mask_mean)-1):
        if sigma_mask_mean[i] and not sigma_mask_mean[i+1]:
            val.append(result[0]-scan_mean[i])
        if not sigma_mask_mean[i] and sigma_mask_mean[i+1]:
            val.append(scan_mean[i]-result[0])
    ret.append(val)

    val = []
    for i in range(len(sigma_mask_width)-1):
        if sigma_mask_width[i] and not sigma_mask_width[i+1]:
            val.append(result[0]-scan_width[i])
        if not sigma_mask_width[i] and sigma_mask_width[i+1]:
            val.append(scan_width[i]-result[0])
    ret.append(val)

    return ret
            

def fit(x,y):
    global thisBW
    global thisCB
    global _DATA_
    
    #make bw distribution
    _DATA_ = y
    mean = np.average(x,weights=y)-91.188
    width = np.sqrt(np.average(np.multiply(x-np.average(x,weights=y),x-np.average(x,weights=y)), weights=y))
    guess = [1.424,1.86,mean,width]

    bounds = [(0.01,100), (0.01,100), (-5,5), (0.1,10)]

    thisBW = breitWigner.bw(x)
    thisCB = crystalBall.cb(x, guess)
    guess = scan(guess)
    

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
    chi_sqr = np.sum(np.divide(np.multiply(y-y_vals,y-y_vals),y))/len(y)
    uncertainties = errors(result.x, result.fun) 

    print("minimization complete:")
    print("mu:", 91.188+result.x[2], "+/-", min(np.abs(uncertainties[0][0]),np.abs(uncertainties[0][1]) ))
    print("sigma:", result.x[3], "+/-", min(np.abs(uncertainties[1][0]),np.abs(uncertainties[1][1])))
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
    
