#include <string>
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

using namespace std;

class utilities{
    public:
        static TCanvas * PlotMedianProfilesWithFit(TH2F*, string, TH2F*, string, string, string, string, double, double);
        static TCanvas * PlotMedianProfiles(TH2F*, string, TH2F*, string, string, string, string, double, double, double, double);
        static TCanvas * PlotRMSProfilesWithFit(TH2F*, TH2F*, string, string, string, double, double);
        static TCanvas * PlotDataMC(TH1F*, TH1F*, TH1F*, string, string, string, double, double);
        static TCanvas * PlotDataMC(TH1F*, string, TH1F*, string, string, string, string, double, double, double, double, bool, bool, string, double, double, string);
        static TCanvas * PlotDataMC(TH1F*, string, TH1F*, string, TH1F*, string, string, string, double, double, double, double, bool, bool, string, double, double, string);
        static TH1F * GetMedianProfilesWithError( TH2F*, TH2F*);
        static TH1F * GetMedianProfilesWithError( TH2F*);
        static double GetHistIntegral(TH1 *);
        static double EvaluateChiSquare(TH1F *, TH1F *, double, double);
//        static TCanvas * PlotTwoDdataMCplus(TH2F*, TH2F*, TH2F*, TH2F*, string, string, string, double, double, double, double);
};
