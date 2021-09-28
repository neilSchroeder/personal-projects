#include <vector>
#include <string>
#include <iostream>

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TH2F.h"
#include "RtypesCore.h"
#include "TGraphAsymmErrors.h"

using namespace std;
struct errors {

    errors(Double_t y, Double_t e1, Double_t e2) : x(y), errLow(e1), errHigh(e2) {
    }

    errors() {
    }
    Double_t x;
    Double_t errLow;
    Double_t errHigh;
};


class statistic {
public:
    /*Function to get N percent minimum interval*/
    static TH1D *getNpercentMinHist(TH1 *hist, Float_t N);

    /*Umbrella functions for calling TH1 statistic functions*/
    static double getHistStatistic(TH1 *hist, std::string statistic, Float_t N);
    static errors* getHistStatistic(TH1 *hist, std::string statistic, Float_t N, Int_t Nresamples, Double_t confidenceLevel);

    /*Functions to get from TH2* a TH1D* or TGraphAsymErrors* of y-statistic vs  x*/
    static TH1D* D2toXstatistic(TH2 *h2, std::string statistic, Float_t N);
    static TGraphAsymmErrors* D2toXstatistic(TH2 *h2, std::string statistic, Float_t N, Int_t Nresamples, Double_t confidenceLevel);

    /*Function to get from TH3 a TH2D* of */
    //    TH2D* D3toD2(TH3* threeD, std::string statistic, Int_t N = 100);

    /*Function for percentile of a vector of doubles*/
    static Double_t getPercentile(vector<Double_t> list, Double_t P);

    /*Function to make basic checks on the histogram*/
    static Bool_t checkHist(TH1 * hist);

    /*Functions to calculate TH1 statistic*/
    static Double_t getHistStats(TH1 * hist);
    static Double_t getHistMedian(TH1 * hist);
    static Double_t getHistMean(TH1 * hist);
    static Double_t getHistRMS(TH1 * hist);
    static Double_t getHistStDev(TH1 * hist);

    /*Tools*/
    static TGraphAsymmErrors* buildGraph(std::vector<errors*> xCollection, std::vector<errors*>yCollection);
    static void copyErrorsToDouble(std::vector<errors*> errorList, std::vector<Double_t> & vector, Int_t pos = 0);
    //    void print_array(const Double_t* array);
    static void clear_errors_vector(std::vector<errors*> & vec);
    static std::vector<Double_t> array2vec(const Double_t *array);
    static std::vector<Double_t> histBins(TH1* hist);
    static std::string removeNonAlpha(std::string word);

};
