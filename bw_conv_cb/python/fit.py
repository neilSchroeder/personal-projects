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

    thisBW.update(guess[0])
    thisCB.update(guess[1::])
    y_vals = np.convolve(thisBW.y,thisCB.y,mode='same')
    y_vals = y_vals/np.sum(y_vals)

    return NLL(_DATA_,y_vals)

def scan(guess):
    global thisBW
    global thisCB
    global _DATA_

    ret = guess

    scan_cv = np.arange(85,93,0.01)
    scan_alpha = np.arange(0.1,100,0.5)
    scan_n = np.arange(0.1,10,0.5)
    scan_mean = np.arange(-5,5,0.025)
    scan_width = np.arange(0.1,10,0.1)

    guesses_cv = [[x, ret[1], ret[2], ret[3], ret[4]] for x in scan_cv]
    nll_cv = np.array([target(x) for x in guesses_cv])
    ret[0] = scan_cv[np.argmin(nll_cv)]

    guesses_mean = [[ret[0], ret[1],x, ret[3], ret[4]] for x in scan_mean]
    nll_mean = np.array([target(x) for x in guesses_mean])
    ret[3] = scan_mean[np.argmin(nll_mean)]

    guesses_width = [[ret[0], ret[1], ret[2],ret[3],x] for x in scan_width]
    nll_width = np.array([target(x) for x in guesses_width])
    ret[4] = scan_width[np.argmin(nll_width)]

    guesses_alpha = [[ret[0], x,ret[2],ret[3],ret[4]] for x in scan_alpha]
    nll_alpha = np.array([target(x) for x in guesses_alpha])
    ret[1] = scan_alpha[np.argmin(nll_alpha)]

    guesses_n = [[ret[0], ret[1],x,ret[3],ret[4]] for x in scan_n]
    nll_n = np.array([target(x) for x in guesses_n])
    ret[2] = scan_n[np.argmin(nll_n)]
  
    return ret

def errors(result,min_chi):

    ret = []

    scan_cv = np.arange(result[0]-2*result[4], result[0]+2*result[4], 0.0001)
    scan_mean = np.arange(-(2*abs(result[3])),2*abs(result[3]),0.0001)
    scan_width = np.arange(0.0001,2*result[4],0.0001)

    par_set_cv = [[x ,result[1], result[2], result[3], result[4]] for x in scan_cv]
    par_set_mean = [[result[0],result[1],result[2],x,result[4]] for x in scan_mean]
    par_set_width = [[result[0],result[1],result[2],result[3],x] for x in scan_width]

    chi_cv = np.array([target(x) for x in par_set_cv])
    chi_mean = np.array([target(x) for x in par_set_mean])
    chi_width = np.array([target(x) for x in par_set_width])

    sigma_mask_cv = chi_cv >= np.e * min(chi_cv)
    sigma_mask_mean = chi_mean >= np.e * min(chi_mean)
    sigma_mask_width = chi_width >= np.e * min(chi_width)

    val = []
    for i in range(len(sigma_mask_cv)-1):
        if sigma_mask_cv[i] and not sigma_mask_cv[i+1]:
            val.append(result[0]-scan_cv[i])
        if not sigma_mask_cv[i] and sigma_mask_cv[i+1]:
            val.append(scan_cv[i]-result[0])
            break
    ret.append(val)

    val = []
    for i in range(len(sigma_mask_mean)-1):
        if sigma_mask_mean[i] and not sigma_mask_mean[i+1]:
            val.append(result[3]-scan_mean[i])
        if not sigma_mask_mean[i] and sigma_mask_mean[i+1]:
            val.append(scan_mean[i]-result[3])
            break
    ret.append(val)

    val = []
    for i in range(len(sigma_mask_width)-1):
        if sigma_mask_width[i] and not sigma_mask_width[i+1]:
            val.append(result[4]-scan_width[i])
        if not sigma_mask_width[i] and sigma_mask_width[i+1]:
            val.append(scan_width[i]-result[4])
            break
    ret.append(val)

    return ret
            

def fit(x,y):
    global thisBW
    global thisCB
    global _DATA_
    
    #make bw distribution
    _DATA_ = y
    cv = 91.188
    mean = 91.188 - np.average(x,weights=y)
    width = np.sqrt(np.average(np.multiply(x-np.average(x,weights=y),x-np.average(x,weights=y)), weights=y))
    guess = [cv, 1.424,1.86,mean,width]

    bounds = [(86, 92), (0.01,100), (0.01,10), (-5,5), (0.1,10)]

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
    thisBW.update(result.x[0])
    thisCB.update(result.x[1::])
    y_vals = np.convolve(thisBW.y,thisCB.y,mode='same')
    y_vals = np.sum(y)*y_vals/np.sum(y_vals)
    chi_sqr = np.sum(np.divide(np.multiply(y-y_vals,y-y_vals),y))/len(y)
    uncertainties = errors(result.x, result.fun) 

    print("minimization complete:")
    print("mu:", result.x[0]+result.x[3], "+/-", np.sqrt(max(np.abs(uncertainties[0][0]),np.abs(uncertainties[0][1]) )**2 + max(np.abs(uncertainties[1][0]),np.abs(uncertainties[1][1]) ) ))
    print("sigma:", result.x[4], "+/-", max(np.abs(uncertainties[2][0]),np.abs(uncertainties[2][1])))
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
            linewidth=3,
            linestyle='dashed')
    axs.legend(loc='best')
    fig.savefig("fit.png")
    plt.close(fig)
    return
    
