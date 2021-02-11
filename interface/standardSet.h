#include <string>
#include "TH1.h"
#include "TCanvas.h"

using namespace std;

class standardSet{
    public:
        TCanvas * evaluate_EtaPlots(string, string, int, string );
        TCanvas * evaluate_systPlots(std::vector<std::string>, int);
        TCanvas * eval_CoarseEtaPlots(string, string);
        TCanvas * evaluate_EtaDoubleRatioPlots(string, string, string, string, int, string);
        TCanvas * evaluate_CatPlots(string, string);
        TCanvas * evaluate_EtPlots(string, string);
        TCanvas * evaluate_R9Plots(string, string);
        TCanvas * moneyPlots(string, string);
        TCanvas * dataData(string, string);
        TCanvas * eval_EtResolution(string,string);
        TCanvas * etLinearityPlots(std::string _str_data, std::string _str_mc_);
        TCanvas * runPlots(std::string _str_data, std::string _str_mc);
};
