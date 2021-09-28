#ifndef STATISTIC
#define STATISTIC

#include <cctype>
#include <sstream>
#include <iomanip>
#include <vector>
#include "iostream"
#include "iterator"
#include "map"
#include "algorithm"
#include <cctype>
#include <string>
#include "vector"
#include "iostream"
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TF2.h"
#include "TGraphAsymmErrors.h"
#include "TAxis.h"
#include "RtypesCore.h"
#include "../interface/statistic.h"

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(n) << a_value;
    return out.str();
}

typedef Double_t(*H_statistic)(TH1*);
std::map<std::string, H_statistic > function_map = {{"median", statistic::getHistMedian}, {"stats", statistic::getHistStats}, {"mean", statistic::getHistMean}, {"rms", statistic::getHistRMS}, {"stddev", statistic::getHistStDev}};

Double_t statistic::getPercentile(std::vector<Double_t> list, Double_t P) {
    if (list.empty()) {
        std::cout << "Error: Empy vector!" << std::endl;
        return -999;
    }
    if (P > 100.0 || P < 0.0) {
        std::cout << "Error: Invalid percentage!" << std::endl;
        return -999;
    }
    std::sort(list.begin(), list.end());
    UInt_t percentilePosition = ceil((Double_t)(P * list.size() / 100.0)) - 1;
    return list[percentilePosition];
};

Bool_t statistic::checkHist(TH1 * hist) {
    if (hist->Integral() < 0.00001) {
//        std::cout << "Warning: Empty histogram " << hist->GetName() << std::endl;
        return 0;
    }
    return 1;
};

Double_t statistic::getHistStats(TH1 * hist) {
    return hist->Integral();
};

Double_t statistic::getHistMedian(TH1 * hist) {
    if (!checkHist(hist)) return -999;
    const Double_t prob[1] = {0.5};
    Double_t median[1];
    hist->GetQuantiles(1, median, prob);
    return median[0];
};

Double_t statistic::getHistMean(TH1 * hist) {
    if (!checkHist(hist)) return -999;
    return hist->GetMean();
};

Double_t statistic::getHistRMS(TH1 * hist) {
    if (!checkHist(hist)) return -999;
    Double_t mean = hist->GetMean();
    Double_t stdev = hist->GetStdDev();
    return sqrt(mean * mean + stdev * stdev);
};

Double_t statistic::getHistStDev(TH1 * hist) {
    if (!checkHist(hist)) return -999;
    return hist->GetStdDev();
};

TH1D * statistic::getNpercentMinHist(TH1 *hist, Float_t N) {
    if (hist->IsZombie()) {
        cout << "Error: Null histogram!" << endl;
        return NULL;
    }

    UInt_t Nbins = hist->GetNbinsX();
    UInt_t firstBin = 1, lastBin = Nbins;
    Float_t P = N / 100.0;
    if (100 - N < 0.00001) {
        TH1D* same = (TH1D*) hist->Clone();
        return same;
    }
    TH1D* cum = (TH1D*) hist->GetCumulative(1, "cum");
    TH1D* rcum = (TH1D*) hist->GetCumulative(0, "rcum");
    TAxis *xaxis = hist->GetXaxis();
    Double_t integral = cum->GetBinContent(Nbins);
    Double_t tempInterval = (xaxis->GetXmax()) - (xaxis->GetXmin());
    Double_t partialIntegral = 0;
    Double_t leftEdge, rightEdge;

    for (UInt_t i = 1; i < Nbins + 1; i++) {
        if (P * integral > rcum->GetBinContent(i)) break;
        for (UInt_t j = i; j < Nbins + 1; j++) {
            partialIntegral = cum->GetBinContent(j) - cum->GetBinContent(i - 1);
            leftEdge = xaxis->GetBinLowEdge(i);
            rightEdge = xaxis->GetBinUpEdge(j);

            if ((partialIntegral / integral) >= P && (tempInterval > (rightEdge - leftEdge))) {
                lastBin = j;
                firstBin = i;
                tempInterval = rightEdge - leftEdge;
                break;
            }
        }
    }
    cum->Delete();
    rcum->Delete();

    std::vector<Double_t> newBins;
    for (UInt_t i = firstBin; i < lastBin + 1; i++) {
        newBins.push_back(xaxis->GetBinLowEdge(i));
    }
    newBins.push_back(xaxis->GetBinUpEdge(lastBin));
    UInt_t newNbins = newBins.size() - 1;
    std::string title = "min_" + to_string_with_precision(N, 0) + "_percent_" + hist->GetName();
    TH1D *histOut = new TH1D(title.c_str(), "", newNbins, newBins.data());
    for (UInt_t i = 1, j = firstBin; i < newNbins + 1 && j < lastBin + 1; i++, j++) {
        histOut->SetBinContent(i, hist->GetBinContent(j));
    }
    return histOut;
};

double statistic::getHistStatistic(TH1 * hist, std::string statistic, Float_t N) {
    TH1 * tempHist = getNpercentMinHist(hist, N);
    H_statistic thisFunction = function_map.at(statistic);
    hist->ComputeIntegral(0);
    double _statistic = thisFunction(tempHist);
    tempHist->Delete();

    return _statistic;
};

errors* statistic::getHistStatistic(TH1 * hist, string statistic, Float_t N, Int_t Nresamples, Double_t confidenceLevel) {
    UInt_t sampleSize = ceil(hist->Integral());
    Double_t _statistic = getHistStatistic(hist, statistic, N);
    std::vector<Double_t> deviations;
    for (Int_t i = 0; i < Nresamples; i++) {
        TH1D* reSample = (TH1D*) hist->Clone("reSample");
        reSample->Reset("ICES");
        reSample->FillRandom(hist, sampleSize);
        reSample->ComputeIntegral(0);
        Double_t deviation = getHistStatistic(reSample, statistic, N) - _statistic;
        reSample->Delete();
        deviations.push_back(deviation);
    }
    Double_t lowerError = -getPercentile(deviations, 50 - confidenceLevel / 2);
    Double_t upperError = getPercentile(deviations, 50 + confidenceLevel / 2);
    errors* out = new errors(_statistic, lowerError, upperError);
    return out;
};

TH1D* statistic::D2toXstatistic(TH2 *h2, std::string statistic, Float_t N) {
    if (N < 0 || N > 100) {
        cout << "Error: Invalid minimum percentage of y-bin data requested." << endl;
        return NULL;
    }
    std::string name = h2->GetName();
    name.insert(0, statistic + "_");
    if (N < 100)name.insert(0, "min" + to_string_with_precision(N, 0) + "%_");
    UInt_t nXbins = h2->GetXaxis()->GetNbins();
    TH1D *outputHist = new TH1D(name.c_str(), name.c_str(), nXbins, h2->GetXaxis()->GetXbins()->GetArray());
    outputHist->Sumw2();
    for (UInt_t i = 1; i < nXbins + 1; i++) {
        TH1D * binHist = (TH1D *) h2->ProjectionY((name + "_bin_" + to_string(i)).c_str(), i, i, "e");
        if (checkHist(binHist)) {
            Double_t _statistic = getHistStatistic(binHist, statistic, N);
            outputHist->SetBinContent(i, _statistic);
        }
        binHist->Delete();
    }
    outputHist->GetYaxis()->SetTitle(name.c_str());
    outputHist->GetXaxis()->SetTitle(h2->GetXaxis()->GetTitle());
    return outputHist;
};

TGraphAsymmErrors* statistic::D2toXstatistic(TH2 *h2, std::string statistic, Float_t N, Int_t Nresamples, Double_t confidenceLevel) {
    if (N < 0 || N > 100) {
        cout << "Error: Invalid minimum percentage of y-bin data requested." << endl;
        return NULL;
    }
    std::string name = h2->GetName();
    name.insert(0, statistic + "_");
    if (N < 100)name.insert(0, "min" + to_string_with_precision(N, 0) + "%_");
    TAxis* xAxis = h2->GetXaxis();
    UInt_t nXbins = xAxis->GetNbins();
    vector<errors*> xCollection;
    vector<errors*> yCollection;
    for (UInt_t i = 1; i < nXbins + 1; i++) {
        TH1D *binHist = h2->ProjectionY((name + to_string(i)).c_str(), i, i, "e");
        if (checkHist(binHist)) {
            Double_t x_val = xAxis->GetBinCenter(i);
            Double_t errLowx = x_val - xAxis->GetBinLowEdge(i);
            Double_t errUpx = xAxis->GetBinUpEdge(i) - x_val;
            errors *x = new errors(x_val, errLowx, errUpx);
            errors *y = getHistStatistic(binHist, statistic, N, Nresamples, confidenceLevel);
            xCollection.push_back(x);
            yCollection.push_back(y);
        }
        binHist->Delete();
    }
    TGraphAsymmErrors* out = buildGraph(xCollection, yCollection);
    out->SetName(name.c_str());
    out->SetTitle(name.c_str());
    name = h2->GetYaxis()->GetTitle();
    out->GetYaxis()->SetTitle(name.c_str());
    out->GetXaxis()->SetTitle(h2->GetXaxis()->GetTitle());
    clear_errors_vector(xCollection);
    clear_errors_vector(yCollection);
    return out;
};

std::vector<Double_t> statistic::histBins(TH1* hist) {
    UInt_t nbins = hist->GetNbinsX();
    if (nbins == 0) {
        std::cout << "Error : No Bins!" << std::endl;
        return {};
    };
    std::vector<Double_t> bins;
    TAxis* axis = hist->GetXaxis();
    bins.push_back(axis->GetBinLowEdge(1));
    for (UInt_t i = 1; i < nbins + 1; i++) {
        bins.push_back(axis->GetBinUpEdge(i));
    }
    return bins;
};

//TH2D* statistic::D3toD2(TH3* threeD, std::string statistic, Int_t N) {
//    if (!checkHist(threeD)) {
//	return NULL;
//    }
//    UInt_t xN = threeD->GetNbinsX();
//    UInt_t yN = threeD->GetNbinsY();
//    std::string name = threeD->GetZaxis()->GetTitle();
//    name.insert(0, statistic + "_");
//    if (N < 100)name.insert(0, "min" + std::to_string(N) + "%_");
//    TH2D* outHist = (TH2D*) threeD->Project3D("xy"); // get clone of the projection and clear it.
//    outHist->Reset();
//    outHist->SetName(name.c_str());
//    outHist->GetXaxis()->SetTitle(threeD->GetXaxis()->GetTitle());
//    outHist->GetYaxis()->SetTitle(threeD->GetYaxis()->GetTitle());
//    outHist->GetZaxis()->SetTitle(name.c_str());
//    for (UInt_t i = 1; i < xN + 1; i++) {
//	for (UInt_t j = 1; j < yN + 1; j++) {
//	    TH1D* binHist = NULL;
//	    binHist = threeD->ProjectionZ((to_string(i) + "-" + to_string(j)).c_str(), i, i, j, j);
//	    if (binHist->Integral() != 0) {
//		Double_t _statistic = getHistStatistic(binHist, statistic, N);
//		outHist->SetBinContent(i, j, _statistic);
//	    }
//	}
//    }
//    return outHist;
//};

TGraphAsymmErrors* statistic::buildGraph(std::vector<errors*> xCollection, std::vector<errors*>yCollection) {
    if (xCollection.size() != yCollection.size()) {
        cout << "Error: Unequal coordinate lists.";
        return NULL;
    }
    vector<Double_t> x, xErrL, xErrH, y, yErrL, yErrH;
    copyErrorsToDouble(xCollection, x);
    copyErrorsToDouble(xCollection, xErrL, -1);
    copyErrorsToDouble(xCollection, xErrH, 1);
    copyErrorsToDouble(yCollection, y);
    copyErrorsToDouble(yCollection, yErrL, -1);
    copyErrorsToDouble(yCollection, yErrH, 1);
    TGraphAsymmErrors* out = new TGraphAsymmErrors(x.size(), x.data(), y.data(), xErrL.data(), xErrH.data(), yErrL.data(), yErrH.data());
    return out;
};

void statistic::copyErrorsToDouble(std::vector<errors*> errorList, std::vector<Double_t> & vector, Int_t pos) {
    if (pos < -1 || pos > 1) {
        cout << "Error: Invalid position." << endl;
        return;
    }
    vector.clear();
    Double_t errors::*posVal = NULL;
    if (pos == 0) posVal = &errors::x;
    else if (pos == -1) posVal = &errors::errLow;
    else if (pos == 1) posVal = &errors::errHigh;
    for (UInt_t i = 0; i < errorList.size(); i++) {
        errors* error = errorList[i];
        Double_t value = error->*posVal;
        vector.push_back(value);
    }
};

void statistic::clear_errors_vector(std::vector<errors*> & vec) {
    for (std::vector<errors*>::iterator it = vec.begin(); it != vec.end(); ++it) {
        delete (*it);
    }
    vec.clear();
};

std::vector<Double_t> statistic::array2vec(const Double_t *array) {
    std::vector<Double_t> vec(array, array + sizeof (array) / sizeof (array[0]));
    return vec;
};

std::string statistic::removeNonAlpha(std::string word) {
    word.erase(std::remove_if(word.begin(), word.end(),
        [](char ch) {
            return !::iswalnum(ch); }), word.end());
    return word;
};

//void statistic::print_array(const Double_t* array) {
//    cout << "Elements: " << endl;
//    for (UInt_t i = 0; i < sizeof (array) / sizeof (array[0]); i++) {
//	cout << array[i] << ", ";
//    }
//    cout << endl;
//};

//void test() {
//    statistic s;
//    TH2D* hist = new TH2D("hist", "", 100, -5, 5, 100, -5, 5);
//    TF2 *xyg = new TF2("xyg", "xygaus", -5, 5, -5, 5);
//    xyg->SetParameters(1, 0, 1, 0, 1);
//    hist->FillRandom("xyg", 1000000);
//    //    TGraphAsymmErrors* graph = s.D2toXstatistic(hist,"median", 100, 100, 95 );
//    //    graph->Draw();
//    TH1D* test = s.D2toXstatistic(hist, "median", 100);
//
//}
#endif
