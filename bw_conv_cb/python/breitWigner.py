import numpy as np

class bw:
    """
    produces a relativistic Breit-Wigner distribution
    """

    def getY(self):
        gamma = np.sqrt(self.mean**2*(self.mean**2+self.width**2))
        k = 2*np.sqrt(2)*self.mean*self.width*gamma/(np.pi*np.sqrt(self.mean**2+gamma))
        self.y = k/(np.multiply(np.multiply(self.x,self.x)-self.mean**2,np.multiply(self.x,self.x)-self.mean**2)+self.mean**2 * self.width**2)
        return self.y/np.sum(self.y)

    def __init__(self, x):
        self.x = np.array(x)
        self.mean = 91.188
        self.width = 2.4952
        self.y = self.getY()

    def __delete__(self):
        del self.x
        del self.mean
        del self.width
        del self.y

