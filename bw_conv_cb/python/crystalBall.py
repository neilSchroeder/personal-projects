import numpy as np
from scipy.special import erf

class cb:
    """
    produces a crystal ball distribution
    """

    def __init__(self, x, params):
        self.x = np.array(x)-(min(x)+max(x))/2
        self.alpha = params[0]
        self.n = params[1]
        self.mean = params[2]
        self.width = params[3]
        self.y = []
        self.getY()

    def __delete__(self):
        del self.x
        del self.alpha
        del self.n
        del self.mean
        del self.width
        del self.y

    def getY(self):
        #print(self.alpha, self.n, self.mean, self.width)
        A = np.power((self.n/abs(self.alpha)),self.n)*np.exp(-(abs(self.alpha)**2)/2)
        B = (self.n/abs(self.alpha)) - abs(self.alpha)
        C = (self.n/abs(self.alpha))*(1/(self.n-1))*np.exp(-(abs(self.alpha)**2)/2)
        D = np.sqrt(np.pi/2)*(1+erf(abs(self.alpha)/np.sqrt(2)))
        N = 1/(self.width*(C + D))
        geq = N*np.exp(-1*np.multiply(self.x-self.mean,self.x-self.mean)/(2*(self.width**2)))
        leq = N*A*np.power((B - (self.x-self.mean)/self.width),-1*self.n)
        mask_geq = (self.x-self.mean)/self.width > -1*self.alpha
        self.y = np.array([geq[i] if mask_geq[i] else leq[i] for i in range(len(mask_geq))])
        self.y = self.y/np.sum(self.y)
        return self.y

    def update(self, pars):
        self.alpha = pars[0]
        self.n = pars[1]
        self.mean = pars[2]
        self.width = pars[3]

