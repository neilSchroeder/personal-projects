#include <string>
#include "TH1.h"
#include "TCanvas.h"

using namespace std;

class standardSet{
    public:
        static TCanvas * evaluate_EtaPlots(string, string, int, string );
        static TCanvas * evaluate_EtaDoubleRatioPlots(string, string, string, string, int, string);
        static TCanvas * evaluate_CatPlots(string, string);
        static TCanvas * evaluate_EtPlots(string, string);
        static TCanvas * evaluate_R9Plots(string, string);
};
