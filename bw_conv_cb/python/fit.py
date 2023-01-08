import breitWigner 
import crystalBall
import numpy as np
from scipy.optimize import minimize
from scipy.special import xlogy
import matplotlib.pyplot as plt

def get_chi_sqr(a, b):
    c = np.sum(a)*b / np.sum(b)
    return np.sum(np.divide(np.multiply(a-c,a-c),a))/(len(a)-1)

def NLL(a,b):

    nll = xlogy(a,b)
    nll[nll==-np.inf] = 0
    nll = np.sum(nll)/len(nll)

    penalty = xlogy(np.sum(a) - a, 1 - b)
    penalty[penalty==-np.inf] = 0
    penalty = np.sum(penalty)/len(penalty)
    
    return -2*(nll+penalty)


def target(guess, chi_sqr=False):
    global thisBW
    global thisCB
    global _DATA_

    thisBW.update(guess[0])
    thisCB.update(guess[1::])
    y_vals = np.convolve(thisBW.y,thisCB.y, mode='same')
    y_vals = y_vals/np.sum(y_vals)

    if chi_sqr:
        return get_chi_sqr(_DATA_, y_vals)

    ret = NLL(_DATA_,y_vals) * get_chi_sqr(_DATA_, y_vals)
    return ret

def scan(guess):
    global thisBW
    global thisCB
    global _DATA_

    ret = guess

    scan_cv = np.arange(85,93,0.01)
    scan_alpha = np.arange(0.1,100,0.5)
    scan_n = np.arange(0.1,10,0.5)
    scan_mean = np.arange(-2,2,0.025)
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

def errors(result):
    """
    Returns the error on the fit parameters:
    works by finding the edges of the region at which the target function increases by 68.3%
    """

    ret = []
    result = [x for x in result]

    # generate scan values (+- 10% of value)
    val = 0.1
    scan_vals = [np.arange(x*(1-val*np.sign(x)), x*(1+val*np.sign(x)), 0.01) for x in result]

    # loop over scan vals and evaluate NLL on each
    for iii,vals in enumerate(scan_vals):

        # create the parameter sets to evaluate the chi_sqr on  
        par_set = [result[:iii] + [x] + result[iii+1:] for x in vals]

        # evaluate chi for all "scan values"
        chi_vals = np.array([target(x) for x in par_set])
        chi_mask = chi_vals <= np.min(chi_vals)*1.683
        locs = np.where(chi_mask == True)
        while len(locs) == 0 and val < 1:
            val += 0.1
            scan_vals[iii] = np.arange(result[iii]*(1-val), result[iii]*(1+val), 0.01)
            par_set = [result[:iii] + [x] + result[iii+1:] for x in vals]
            chi_vals = np.array([target(x, chi_sqr=True) for x in par_set])
            locs = np.where(chi_mask == True)

        if len(locs) == 0:
            ret.append((result[iii], result[iii]))
        else:
            ret.append((abs(result[iii] - vals[locs[0][0]]), abs(result[iii] - vals[locs[0][-1]])))

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
    guess = [cv, 1.2, 1.46,mean,width]

    bounds = [(87, 92), (0.1,10), (0.1,20), (-2,3), (0.1,10)]

    thisBW = breitWigner.bw(x)
    thisCB = crystalBall.cb(x, guess)
    guess = scan(guess)
    
    result = minimize(target, 
            np.array(guess), 
            method="L-BFGS-B",
            bounds=bounds,
            options={"eps":0.0001}
            )

    #print(result)
    #if not result.success:
        #print(result)
        #raise Exception

    thisBW.update(result.x[0])
    thisCB.update(result.x[1::])
    y_vals = np.convolve(thisBW.y,thisCB.y,mode='same')
    y_vals = np.sum(y)*y_vals/np.sum(y_vals)
    print(y_vals)
    chi_sqr = np.sum(np.divide(np.multiply(y-y_vals,y-y_vals),y))/(len(y)-len(result.x))
    #uncertainties = errors(result.x) 
    
    
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
    
    
    return result, chi_sqr
    
