#include <string>
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TCanvas.h"
#include "RooFitResult.h"

using namespace std;

class utilities{
    public:
        RooFitResult * fitBreitGaus(TH1*, bool);
        TCanvas * PlotData(TH1F*,  string, string, string, string, double, double, bool);
        TCanvas * PlotMedianProfilesWithFit(TH2F*, string, TH2F*, string, string, string, string, double, double);
        TCanvas * PlotMedianProfiles(TH2F*, string, TH2F*, string, string, string, string, double, double, double, double);
        TCanvas * PlotRMSProfilesWithFit(TH2F*, TH2F*, string, string, string, double, double);
        TCanvas * PlotDataMC(TH1F*, TH1F*, TH1F*, string, string, string, double, double);
        TCanvas * PlotHistWithFit(TH1F*, string, RooFitResult*, string, string);
        TCanvas * PlotDataMC(TH1F*, string, TH1F*, string, string, string, string, double, double, double, double, bool, bool, string, double, double, string);
        TCanvas * PlotDataMC(TH1F*, string, TH1F*, string, TH1F*, string, string, string, double, double, double, double, bool, bool, string, double, double, string);
        TCanvas * moneyPlot(TH1F*, TH1F*, TH1F*, string, int);
        TCanvas * moneyPlot_wRatio(TH1F*, TH1F*, TH1F*, string, int);    
        TCanvas * dataDataPlot_wRatio( TH1F*, TH1F*, std::string, int, std::string, std::string);
        TH1F * GetMedianProfilesWithError( TH2F*, TH2F*);
        TH1F * GetMedianProfilesWithError( TH2F*);
        TH1F * GetMedianProfilesWithError( TH2F*, string, string, string, string, double, double, double, double, bool);
        double GetHistIntegral(TH1 *);
        double EvaluateChiSquare(TH1F *, TH1F *, double, double);

        TCanvas * plotRooFitResult(TH1F * hist, RooFitResult * res, std::string histTitle, std::string dataTitle);
        TCanvas * plotRuns2018(TH1F * d0, TH1F * d1, TH1F * d2, TH1F * d3, float m0, float m1, float m2, float m3, float yMin, float yMax,string);
//        TCanvas * PlotTwoDdataMCplus(TH2F*, TH2F*, TH2F*, TH2F*, string, string, string, double, double, double, double);
};
