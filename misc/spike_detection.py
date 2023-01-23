import numpy as np
import matplotlib.pyplot as plt
import time


class real_time_peak_detection():
    def __init__(self, array, lag, threshold, influence):
        self.y = list(array)
        self.length = len(self.y)
        self.lag = lag
        self.threshold = threshold
        self.influence = influence
        self.signals = [0] * len(self.y)
        self.filteredY = np.array(self.y).tolist()
        self.avgFilter = [0] * len(self.y)
        self.stdFilter = [0] * len(self.y)
        self.avgFilter[self.lag - 1] = np.mean(self.y[0:self.lag]).tolist()
        self.stdFilter[self.lag - 1] = np.std(self.y[0:self.lag]).tolist()

    def thresholding_algo(self, new_value):
        self.y.append(new_value)
        i = len(self.y) - 1
        self.length = len(self.y)
        if i < self.lag:
            return 0, 0
        elif i == self.lag:
            self.signals = [0] * len(self.y)
            self.filteredY = np.array(self.y).tolist()
            self.avgFilter = [0] * len(self.y)
            self.stdFilter = [0] * len(self.y)
            self.avgFilter[self.lag] = np.mean(self.y[0:self.lag]).tolist()
            self.stdFilter[self.lag] = np.std(self.y[0:self.lag]).tolist()
            return 0, 0

        self.signals += [0]
        self.filteredY += [0]
        self.avgFilter += [0]
        self.stdFilter += [0]

        if abs(self.y[i] - self.avgFilter[i - 1]) > (self.threshold * self.stdFilter[i - 1]):

            if self.y[i] > self.avgFilter[i - 1]:
                self.signals[i] = 1


            self.filteredY[i] = self.influence * self.y[i] + \
                (1 - self.influence) * self.filteredY[i - 1]
            self.avgFilter[i] = np.mean(self.filteredY[(i - self.lag):i])
            self.stdFilter[i] = np.std(self.filteredY[(i - self.lag):i])
        else:
            self.signals[i] = 0
            self.filteredY[i] = self.y[i]
            self.avgFilter[i] = np.mean(self.filteredY[(i - self.lag):i])
            self.stdFilter[i] = np.std(self.filteredY[(i - self.lag):i])

        return self.signals[i], self.filteredY[i]


def main():

    # generate some data
    data = np.random.normal(1000, 500, 1000)
    truth = np.zeros(len(data))
    # add spikes
    spike_size = 4000
    for i in range(6):
        loc = np.random.randint(0,len(data)-1)
        truth[loc] = 1
        data[loc] = spike_size

    lag = 12
    threshold = 3
    influence = 0.1

    rtpd = real_time_peak_detection(data[:lag], lag, threshold, influence)
    signal = [0 for i in range(lag)]
    filtered_data = [0 for i in range(lag)]

    for i,x in enumerate(data[lag:]):
        s,y = rtpd.thresholding_algo(x)
        signal.append(s)
        filtered_data.append(y)

    fig, axs = plt.subplots(ncols=1, nrows=2)
    axs[0].plot(data)
    axs[0].plot(filtered_data)
    axs[1].plot(signal, label='signal')   
    axs[1].plot(truth, label='truth')
    axs[1].legend(loc='best')
    
    accuracy = np.sum(np.logical_and(signal,truth))/np.sum(truth)
    false_pos = np.sum(np.logical_and(signal, np.logical_not(truth))) / np.sum(np.logical_not(truth))

    print(f'accuracy: {100*accuracy:.2f}, false positive: {false_pos*100:.2f}')

    plt.show()
    


if __name__ == "__main__":
    main()