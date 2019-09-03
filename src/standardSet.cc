#ifndef STANDARD_SET
#define STANDARD_SET

#include "../interface/utilities.h"
#include "../interface/statistic.h"
#include "../interface/standardSet.h"
#include <TH1.h>
#include <TH2.h>
#include <TLegend.h>
#include <TProfile.h>
#include <TLine.h>
#include <TStyle.h>
#include <TString.h>
#include <TF1.h>
#include <TLatex.h>
#include <TAxis.h>
#include <TAttAxis.h>
#include <THistPainter.h>
#include <TColor.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TFile.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <algorithm>
#include <iterator>

using namespace std;

extern string dataTitle;
extern string mcTitle;
extern string outputFile;
extern bool _flag_invMassDist;

const double N_PERCENT_HIST = 60.;

TCanvas * standardSet::evaluate_EtaPlots(string data, string mc1, int kWhichR9, string stat){
    //files
    vector <TFile*> files;
    files.push_back(new TFile(data.c_str(), "READ"));
    files.push_back(new TFile(mc1.c_str(), "READ"));

    TH1F * EBin_low = (TH1F*)files[0]->Get("invMass_ele_EB_in_Low");
    TH1F * EBin_high = (TH1F*)files[0]->Get("invMass_ele_EB_in_High");
    TH1F * EBout_low = (TH1F*)files[0]->Get("invMass_ele_EB_out_Low");
    TH1F * EBout_high = (TH1F*)files[0]->Get("invMass_ele_EB_out_High");
    TH1F * EEin_low = (TH1F*)files[0]->Get("invMass_ele_EE_in_Low");
    TH1F * EEin_high = (TH1F*)files[0]->Get("invMass_ele_EE_in_High");
    TH1F * EEout_low = (TH1F*)files[0]->Get("invMass_ele_EE_out_Low");
    TH1F * EEout_high = (TH1F*)files[0]->Get("invMass_ele_EE_out_High");

    TH1F * down_EBin_low = (TH1F*)files[0]->Get("invMassDown_ele_EB_in_Low");
    TH1F * down_EBin_high = (TH1F*)files[0]->Get("invMassDown_ele_EB_in_High");
    TH1F * down_EBout_low = (TH1F*)files[0]->Get("invMassDown_ele_EB_out_Low");
    TH1F * down_EBout_high = (TH1F*)files[0]->Get("invMassDown_ele_EB_out_High");
    TH1F * down_EEin_low = (TH1F*)files[0]->Get("invMassDown_ele_EE_in_Low");
    TH1F * down_EEin_high = (TH1F*)files[0]->Get("invMassDown_ele_EE_in_High");
    TH1F * down_EEout_low = (TH1F*)files[0]->Get("invMassDown_ele_EE_out_Low");
    TH1F * down_EEout_high = (TH1F*)files[0]->Get("invMassDown_ele_EE_out_High");

    TH1F * up_EBin_low = (TH1F*)files[0]->Get("invMassUp_ele_EB_in_Low");
    TH1F * up_EBin_high = (TH1F*)files[0]->Get("invMassUp_ele_EB_in_High");
    TH1F * up_EBout_low = (TH1F*)files[0]->Get("invMassUp_ele_EB_out_Low");
    TH1F * up_EBout_high = (TH1F*)files[0]->Get("invMassUp_ele_EB_out_High");
    TH1F * up_EEin_low = (TH1F*)files[0]->Get("invMassUp_ele_EE_in_Low");
    TH1F * up_EEin_high = (TH1F*)files[0]->Get("invMassUp_ele_EE_in_High");
    TH1F * up_EEout_low = (TH1F*)files[0]->Get("invMassUp_ele_EE_out_Low");
    TH1F * up_EEout_high = (TH1F*)files[0]->Get("invMassUp_ele_EE_out_High");

    TH1F * mc_EBin_low = (TH1F*)files[1]->Get("invMass_ele_EB_in_Low");
    TH1F * mc_EBin_high = (TH1F*)files[1]->Get("invMass_ele_EB_in_High");
    TH1F * mc_EBout_low = (TH1F*)files[1]->Get("invMass_ele_EB_out_Low");
    TH1F * mc_EBout_high = (TH1F*)files[1]->Get("invMass_ele_EB_out_High");
    TH1F * mc_EEin_low = (TH1F*)files[1]->Get("invMass_ele_EE_in_Low");
    TH1F * mc_EEin_high = (TH1F*)files[1]->Get("invMass_ele_EE_in_High");
    TH1F * mc_EEout_low = (TH1F*)files[1]->Get("invMass_ele_EE_out_Low");
    TH1F * mc_EEout_high = (TH1F*)files[1]->Get("invMass_ele_EE_out_High");

    TH1F * down_mc_EBin_low = (TH1F*)files[1]->Get("invMassDown_ele_EB_in_Low");
    TH1F * down_mc_EBin_high = (TH1F*)files[1]->Get("invMassDown_ele_EB_in_High");
    TH1F * down_mc_EBout_low = (TH1F*)files[1]->Get("invMassDown_ele_EB_out_Low");
    TH1F * down_mc_EBout_high = (TH1F*)files[1]->Get("invMassDown_ele_EB_out_High");
    TH1F * down_mc_EEin_low = (TH1F*)files[1]->Get("invMassDown_ele_EE_in_Low");
    TH1F * down_mc_EEin_high = (TH1F*)files[1]->Get("invMassDown_ele_EE_in_High");
    TH1F * down_mc_EEout_low = (TH1F*)files[1]->Get("invMassDown_ele_EE_out_Low");
    TH1F * down_mc_EEout_high = (TH1F*)files[1]->Get("invMassDown_ele_EE_out_High");

    TH1F * up_mc_EBin_low = (TH1F*)files[1]->Get("invMassUp_ele_EB_in_Low");
    TH1F * up_mc_EBin_high = (TH1F*)files[1]->Get("invMassUp_ele_EB_in_High");
    TH1F * up_mc_EBout_low = (TH1F*)files[1]->Get("invMassUp_ele_EB_out_Low");
    TH1F * up_mc_EBout_high = (TH1F*)files[1]->Get("invMassUp_ele_EB_out_High");
    TH1F * up_mc_EEin_low = (TH1F*)files[1]->Get("invMassUp_ele_EE_in_Low");
    TH1F * up_mc_EEin_high = (TH1F*)files[1]->Get("invMassUp_ele_EE_in_High");
    TH1F * up_mc_EEout_low = (TH1F*)files[1]->Get("invMassUp_ele_EE_out_Low");
    TH1F * up_mc_EEout_high = (TH1F*)files[1]->Get("invMassUp_ele_EE_out_High");
    // want to print out data table:
    //  eta across top, R9 down

    vector< errors* > data_highR9;
    vector< errors* > data_lowR9;
    vector< errors* > mc_highR9;
    vector< errors* > mc_lowR9;

    data_highR9.push_back( statistic::getHistStatistic(EBin_high, "median", N_PERCENT_HIST, 500, 0.95));
    data_highR9.push_back( statistic::getHistStatistic(EBout_high, "median", N_PERCENT_HIST, 500, 0.95));
    data_highR9.push_back( statistic::getHistStatistic(EEin_high, "median", N_PERCENT_HIST, 500, 0.95));
    data_highR9.push_back( statistic::getHistStatistic(EEout_high, "median", N_PERCENT_HIST, 500, 0.95));

    data_lowR9.push_back( statistic::getHistStatistic(EBin_low, "median", N_PERCENT_HIST, 500, 0.95));
    data_lowR9.push_back( statistic::getHistStatistic(EBout_low, "median", N_PERCENT_HIST, 500, 0.95));
    data_lowR9.push_back( statistic::getHistStatistic(EEin_low, "median", N_PERCENT_HIST, 500, 0.95));
    data_lowR9.push_back( statistic::getHistStatistic(EEout_low, "median", N_PERCENT_HIST, 500, 0.95));

    mc_highR9.push_back( statistic::getHistStatistic(mc_EBin_high, "median", N_PERCENT_HIST, 500, 0.95));
    mc_highR9.push_back( statistic::getHistStatistic(mc_EBout_high, "median", N_PERCENT_HIST, 500, 0.95));
    mc_highR9.push_back( statistic::getHistStatistic(mc_EEin_high, "median", N_PERCENT_HIST, 500, 0.95));
    mc_highR9.push_back( statistic::getHistStatistic(mc_EEout_high, "median", N_PERCENT_HIST, 500, 0.95));

    mc_lowR9.push_back( statistic::getHistStatistic(mc_EBin_low, "median", N_PERCENT_HIST, 500, 0.95));
    mc_lowR9.push_back( statistic::getHistStatistic(mc_EBout_low, "median", N_PERCENT_HIST, 500, 0.95));
    mc_lowR9.push_back( statistic::getHistStatistic(mc_EEin_low, "median", N_PERCENT_HIST, 500, 0.95));
    mc_lowR9.push_back( statistic::getHistStatistic(mc_EEout_low, "median", N_PERCENT_HIST, 500, 0.95));

    TH1F* EBin;
    TH1F* EBout;
    TH1F* EEin; 
    TH1F* EEout;

    TH1F* up_EBin; 
    TH1F* up_EBout;
    TH1F* up_EEin; 
    TH1F* up_EEout;

    TH1F* down_EBin;
    TH1F* down_EBout;
    TH1F* down_EEin;
    TH1F* down_EEout;

    TH1F* mc_EBin;
    TH1F* mc_EBout;
    TH1F* mc_EEin;
    TH1F* mc_EEout;

    TH1F* up_mc_EBin;
    TH1F* up_mc_EBout; 
    TH1F* up_mc_EEin;
    TH1F* up_mc_EEout;

    TH1F*  down_mc_EBin;
    TH1F*  down_mc_EBout; 
    TH1F*  down_mc_EEin; 
    TH1F*  down_mc_EEout; 


    //want bins of Eta, combine R9 plots
    if(kWhichR9 == -1 || kWhichR9 == 0){

        EBin = (TH1F*)EBin_low->Clone();
        EBout = (TH1F*)EBout_low->Clone();
        EEin = (TH1F*)EEin_low->Clone();
        EEout = (TH1F*)EEout_low->Clone();

        up_EBin = (TH1F*)up_EBin_low->Clone();
        up_EBout = (TH1F*)up_EBout_low->Clone();
        up_EEin = (TH1F*)up_EEin_low->Clone();
        up_EEout = (TH1F*)up_EEout_low->Clone();

        down_EBin = (TH1F*)down_EBin_low->Clone();
        down_EBout = (TH1F*)down_EBout_low->Clone();
        down_EEin = (TH1F*)down_EEin_low->Clone();
        down_EEout = (TH1F*)down_EEout_low->Clone();

        mc_EBin = (TH1F*)mc_EBin_low->Clone();
        mc_EBout = (TH1F*)mc_EBout_low->Clone();
        mc_EEin = (TH1F*)mc_EEin_low->Clone();
        mc_EEout = (TH1F*)mc_EEout_low->Clone();

        up_mc_EBin = (TH1F*)up_mc_EBin_low->Clone();
        up_mc_EBout = (TH1F*)up_mc_EBout_low->Clone();
        up_mc_EEin = (TH1F*)up_mc_EEin_low->Clone();
        up_mc_EEout = (TH1F*)up_mc_EEout_low->Clone();

        down_mc_EBin = (TH1F*)down_mc_EBin_low->Clone();
        down_mc_EBout = (TH1F*)down_mc_EBout_low->Clone();
        down_mc_EEin = (TH1F*)down_mc_EEin_low->Clone();
        down_mc_EEout = (TH1F*)down_mc_EEout_low->Clone();
    }
    if(kWhichR9 == 0){

        EBin->Add(EBin_high);
        EBout->Add(EBout_high);
        EEin->Add(EEin_high);
        EEout->Add(EEout_high);

        up_EBin->Add(up_EBin_high);
        up_EBout->Add(up_EBout_high);
        up_EEin->Add(up_EEin_high);
        up_EEout->Add(up_EEout_high);

        down_EBin->Add(down_EBin_high);
        down_EBout->Add(down_EBout_high);
        down_EEin->Add(down_EEin_high);
        down_EEout->Add(down_EEout_high);

        mc_EBin->Add(mc_EBin_high);
        mc_EBout->Add(mc_EBout_high);
        mc_EEin->Add(mc_EEin_high);
        mc_EEout->Add(mc_EEout_high);

        up_mc_EBin->Add(up_mc_EBin_high);
        up_mc_EBout->Add(up_mc_EBout_high);
        up_mc_EEin->Add(up_mc_EEin_high);
        up_mc_EEout->Add(up_mc_EEout_high);

        down_mc_EBin->Add(down_mc_EBin_high);
        down_mc_EBout->Add(down_mc_EBout_high);
        down_mc_EEin->Add(down_mc_EEin_high);
        down_mc_EEout->Add(down_mc_EEout_high);
    }
    if(kWhichR9 == 1){

        EBin = (TH1F*)EBin_high->Clone();
        EBout = (TH1F*)EBout_high->Clone();
        EEin = (TH1F*)EEin_high->Clone();
        EEout = (TH1F*)EEout_high->Clone();

        up_EBin = (TH1F*)up_EBin_high->Clone();
        up_EBout = (TH1F*)up_EBout_high->Clone();
        up_EEin = (TH1F*)up_EEin_high->Clone();
        up_EEout = (TH1F*)up_EEout_high->Clone();

        down_EBin = (TH1F*)down_EBin_high->Clone();
        down_EBout = (TH1F*)down_EBout_high->Clone();
        down_EEin = (TH1F*)down_EEin_high->Clone();
        down_EEout = (TH1F*)down_EEout_high->Clone();

        mc_EBin = (TH1F*)mc_EBin_high->Clone();
        mc_EBout = (TH1F*)mc_EBout_high->Clone();
        mc_EEin = (TH1F*)mc_EEin_high->Clone();
        mc_EEout = (TH1F*)mc_EEout_high->Clone();

        up_mc_EBin = (TH1F*)up_mc_EBin_high->Clone();
        up_mc_EBout = (TH1F*)up_mc_EBout_high->Clone();
        up_mc_EEin = (TH1F*)up_mc_EEin_high->Clone();
        up_mc_EEout = (TH1F*)up_mc_EEout_high->Clone();

        down_mc_EBin = (TH1F*)down_mc_EBin_high->Clone();
        down_mc_EBout = (TH1F*)down_mc_EBout_high->Clone();
        down_mc_EEin = (TH1F*)down_mc_EEin_high->Clone();
        down_mc_EEout = (TH1F*)down_mc_EEout_high->Clone();
    }

    EBin->Scale(1/EBin->Integral());
    EBout->Scale(1/EBout->Integral());
    EEin->Scale(1/EEin->Integral());
    EEout->Scale(1/EEout->Integral());
    mc_EBin->Scale(1/mc_EBin->Integral());
    mc_EBout->Scale(1/mc_EBout->Integral());
    mc_EEin->Scale(1/mc_EEin->Integral());
    mc_EEout->Scale(1/mc_EEout->Integral());
    EBin->Rebin(5, "", 0);
    EBout->Rebin(5, "", 0);
    EEin->Rebin(5, "", 0);
    EEout->Rebin(5, "", 0);
    mc_EBin->Rebin(5, "", 0);
    mc_EBout->Rebin(5, "", 0);
    mc_EEin->Rebin(5, "", 0);
    mc_EEout->Rebin(5, "", 0);

    EEin->Rebin(2,"",0);
    mc_EEin->Rebin(2,"",0);
    EBout->Rebin(2,"",0);
    mc_EBout->Rebin(2,"",0);
    EEin->Rebin(2,"",0);
    mc_EEin->Rebin(2,"",0);
    EEout->Rebin(2,"",0);
    mc_EEout->Rebin(2,"",0);

    std::vector<TH1F*> data_hists;
    std::vector<TH1F*> data_hists_up;
    std::vector<TH1F*> data_hists_down;

    std::vector<TH1F*> mc_hists;
    std::vector<TH1F*> mc_hists_up;
    std::vector<TH1F*> mc_hists_down;

    data_hists.push_back(EBin);
    data_hists.push_back(EBout);
    data_hists.push_back((TH1F*)0);
    data_hists.push_back(EEin);
    data_hists.push_back(EEout);

    data_hists_down.push_back(down_EBin);
    data_hists_down.push_back(down_EBout);
    data_hists_down.push_back((TH1F*)0);
    data_hists_down.push_back(down_EEin);
    data_hists_down.push_back(down_EEout);

    data_hists_up.push_back(up_EBin);
    data_hists_up.push_back(up_EBout);
    data_hists_up.push_back((TH1F*)0);
    data_hists_up.push_back(up_EEin);
    data_hists_up.push_back(up_EEout);

    mc_hists.push_back(mc_EBin);
    mc_hists.push_back(mc_EBout);
    mc_hists.push_back((TH1F*)0);
    mc_hists.push_back(mc_EEin);
    mc_hists.push_back(mc_EEout);

    mc_hists_down.push_back(down_mc_EBin);
    mc_hists_down.push_back(down_mc_EBout);
    mc_hists_down.push_back((TH1F*)0);
    mc_hists_down.push_back(down_mc_EEin);
    mc_hists_down.push_back(down_mc_EEout);

    mc_hists_up.push_back(up_mc_EBin);
    mc_hists_up.push_back(up_mc_EBout);
    mc_hists_up.push_back((TH1F*)0);
    mc_hists_up.push_back(up_mc_EEin);
    mc_hists_up.push_back(up_mc_EEout);

    string thisR9String = (kWhichR9 == 1) ? "_highR9" : (kWhichR9 == 0) ? "" : "_lowR9";
    if(_flag_invMassDist){
        utilities::PlotDataMC(EBin, dataTitle, mc_EBin, mcTitle, "standardSet_"+outputFile+"_meeDist_EBin"+thisR9String, "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", 0.5, 1.5, "");
        utilities::PlotDataMC(EBout, dataTitle, mc_EBout, mcTitle, "standardSet_"+outputFile+"_meeDist_EBout"+thisR9String, "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", 0.995, 1.005, "");
        utilities::PlotDataMC(EEin, dataTitle, mc_EEin, mcTitle, "standardSet_"+outputFile+"_meeDist_EEin"+thisR9String, "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", 0.5, 1.5, "");
        utilities::PlotDataMC(EEout, dataTitle, mc_EEout, mcTitle, "standardSet_"+outputFile+"_meeDist_EEout"+thisR9String, "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", 0.5, 1.5, "");
    }

    double etaBins [6] = {0., 1., 1.4442, 1.566, 2., 2.5};

    TH1F * data_loose = new TH1F("data_loose", "", 5, etaBins);
    TH1F * dataDown_loose = new TH1F("dataDown_loose", "", 5, etaBins);
    TH1F * dataUp_loose = new TH1F("dataUp_loose", "", 5, etaBins);

    TH1F * mc_loose = new TH1F("mc_loose", "", 5, etaBins);
    TH1F * mcDown_loose = new TH1F("mcDown_loose", "", 5, etaBins);
    TH1F * mcUp_loose = new TH1F("mcUp_loose", "", 5, etaBins);

    errors* median;
    errors* stddev;

    if(stat.find("resolution") == std::string::npos){
        for(int i = 1; i < 6; i++){
            if( i != 3 ){
                median = statistic::getHistStatistic(data_hists[i-1], stat, N_PERCENT_HIST, 500, 0.95);
                data_loose->SetBinContent(i, median->x);
                data_loose->SetBinError(i, max(median->errLow, median->errHigh));

                median = statistic::getHistStatistic(data_hists_down[i-1], stat, N_PERCENT_HIST, 500, 0.95);
                dataDown_loose->SetBinContent(i, median->x);
                dataDown_loose->SetBinError(i, max(median->errLow, median->errHigh));

                median = statistic::getHistStatistic(data_hists_up[i-1], stat, N_PERCENT_HIST, 500, 0.95);
                dataUp_loose->SetBinContent(i, median->x);
                dataUp_loose->SetBinError(i, max(median->errLow, median->errHigh));

                median = statistic::getHistStatistic(mc_hists[i-1], stat, N_PERCENT_HIST, 500, 0.95);
                mc_loose->SetBinContent(i, median->x);
                mc_loose->SetBinError(i, max(median->errLow, median->errHigh));

                median = statistic::getHistStatistic(mc_hists_down[i-1], stat, N_PERCENT_HIST, 500, 0.95);
                mcDown_loose->SetBinContent(i, median->x);
                mcDown_loose->SetBinError(i, max(median->errLow, median->errHigh));

                median = statistic::getHistStatistic(mc_hists_up[i-1], stat, N_PERCENT_HIST, 500, 0.95);
                mcUp_loose->SetBinContent(i, median->x);
                mcUp_loose->SetBinError(i, max(median->errLow, median->errHigh));
            }
        }
    }
    if(stat.find("resolution") != std::string::npos){
        for(int i = 1; i < 6; i++){
            if(i != 3){
                median = statistic::getHistStatistic(data_hists[i-1], "median", N_PERCENT_HIST, 500, 0.95);
                stddev = statistic::getHistStatistic(data_hists[i-1], "stddev", N_PERCENT_HIST, 500, 0.95);
                data_loose->SetBinContent(i, 100*stddev->x/median->x);
                data_loose->SetBinError(i, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

                median = statistic::getHistStatistic(data_hists_down[i-1], "median", N_PERCENT_HIST, 500, 0.95);
                stddev = statistic::getHistStatistic(data_hists_down[i-1], "stddev", N_PERCENT_HIST, 500, 0.95);
                dataDown_loose->SetBinContent(i, 100*stddev->x/median->x);
                dataDown_loose->SetBinError(i, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

                median = statistic::getHistStatistic(data_hists_up[i-1], "median", N_PERCENT_HIST, 500, 0.95);
                stddev = statistic::getHistStatistic(data_hists_up[i-1], "stddev", N_PERCENT_HIST, 500, 0.95);
                dataUp_loose->SetBinContent(i, 100*stddev->x/median->x);
                dataUp_loose->SetBinError(i, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

                median = statistic::getHistStatistic(mc_hists[i-1], "median", N_PERCENT_HIST, 500, 0.95);
                stddev = statistic::getHistStatistic(mc_hists[i-1], "stddev", N_PERCENT_HIST, 500, 0.95);
                mc_loose->SetBinContent(i, 100*stddev->x/median->x);
                mc_loose->SetBinError(i, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

                median = statistic::getHistStatistic(mc_hists_down[i-1], "median", N_PERCENT_HIST, 500, 0.95);
                stddev = statistic::getHistStatistic(mc_hists_down[i-1], "stddev", N_PERCENT_HIST, 500, 0.95);
                mcDown_loose->SetBinContent(i, 100*stddev->x/median->x);
                mcDown_loose->SetBinError(i, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

                median = statistic::getHistStatistic(mc_hists_up[i-1], "median", N_PERCENT_HIST, 500, 0.95);
                stddev = statistic::getHistStatistic(mc_hists_up[i-1], "stddev", N_PERCENT_HIST, 500, 0.95);
                mcUp_loose->SetBinContent(i, 100*stddev->x/median->x);
                mcUp_loose->SetBinError(i, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));
            }
        }
    }

    TH1F * scalesUncertainties = new TH1F("scalesUnc", "", 5,etaBins);
    TH1F * smearsUncertainties = new TH1F("smearsUnc", "", 5,etaBins);
    TH1F * systUnc = new TH1F("systUnc", "", 5,etaBins);

    int numBins = 0;
    numBins = 6;
    for(int i = 1; i < numBins; i++){
        scalesUncertainties->SetBinContent(i, 1.);
        scalesUncertainties->SetBinError(i, max( fabs(data_loose->GetBinContent(i)-dataUp_loose->GetBinContent(i)), fabs(data_loose->GetBinContent(i)-dataDown_loose->GetBinContent(i))));
        smearsUncertainties->SetBinContent(i, 1.);
        smearsUncertainties->SetBinError(i, max( fabs(mc_loose->GetBinContent(i)-mcUp_loose->GetBinContent(i)), fabs(mc_loose->GetBinContent(i)-mcDown_loose->GetBinContent(i))));
        systUnc->SetBinContent(i, 1.);
        systUnc->SetBinError(i, (data_loose->GetBinContent(i)/mc_loose->GetBinContent(i))* sqrt( pow( scalesUncertainties->GetBinError(i)/data_loose->GetBinContent(i) , 2) + pow( smearsUncertainties->GetBinError(i)/mc_loose->GetBinContent(i), 2)));
    }
    systUnc->SetBinError(3, 0.);

    std::string yTitle = "Median M_{ee} [GeV]";
    if(stat.compare("stddev") == 0) yTitle = "#sigma_{60, M_{ee}} [GeV]";
    if(stat.compare("mean") == 0) yTitle = "Mean M_{ee} [GeV]";
    if(stat.compare("resolution") == 0) yTitle = "#sigma_{60}/M_{ee} [A.U.]";

    utilities::PlotDataMC(data_loose, dataTitle, mc_loose, mcTitle, systUnc, "standardSet_"+outputFile+"_eta"+thisR9String, "|#eta|", yTitle, 0, 2.4999, 89, 92, true, true, "#bf{Data / MC}", -1, -1, "eta");
    utilities::PlotDataMC(data_loose, dataTitle, mc_loose, mcTitle, systUnc, "standardSet_"+outputFile+"_eta"+thisR9String+"_noStatErr", "|#eta|", yTitle, 0, 2.4999, 89, 92, false, true, "#bf{Data / MC}", -1, -1, "eta");
    utilities::PlotDataMC(data_loose, dataTitle, mc_loose, mcTitle,systUnc, "standardSet_"+outputFile+"_eta"+thisR9String+"_noRatio", "|#eta|", yTitle, 0, 2.4999, 88, 94, true, false, "#bf{ Legacy / ReReco }", -1, -1, "eta");
    return utilities::PlotDataMC(data_loose, dataTitle, mc_loose, mcTitle,systUnc, "standardSet_"+outputFile+"_eta"+thisR9String+"_noRatio", "|#eta|", yTitle, 0, 2.4999, 88, 94, true, false, "#bf{ Legacy / ReReco }", -1, -1, "eta");
};

TCanvas * standardSet::evaluate_EtaDoubleRatioPlots(string data1, string mc1, string data2, string mc2, int kWhichR9, string stat){
    //files
    vector <TFile*> files;
    files.push_back(new TFile(data1.c_str(), "READ"));
    files.push_back(new TFile(mc1.c_str(), "READ"));
    files.push_back(new TFile(data2.c_str(), "READ"));
    files.push_back(new TFile(mc2.c_str(), "READ"));

    TH1F * data1_EBin_low = (TH1F*)files[0]->Get("invMass_ele_EB_in_Low");
    TH1F * data1_EBin_high = (TH1F*)files[0]->Get("invMass_ele_EB_in_High");
    TH1F * data1_EBout_low = (TH1F*)files[0]->Get("invMass_ele_EB_out_Low");
    TH1F * data1_EBout_high = (TH1F*)files[0]->Get("invMass_ele_EB_out_High");
    TH1F * data1_EEin_low = (TH1F*)files[0]->Get("invMass_ele_EE_in_Low");
    TH1F * data1_EEin_high = (TH1F*)files[0]->Get("invMass_ele_EE_in_High");
    TH1F * data1_EEout_low = (TH1F*)files[0]->Get("invMass_ele_EE_out_Low");
    TH1F * data1_EEout_high = (TH1F*)files[0]->Get("invMass_ele_EE_out_High");

    TH1F * data1_down_EBin_low = (TH1F*)files[0]->Get("invMassDown_ele_EB_in_Low");
    TH1F * data1_down_EBin_high = (TH1F*)files[0]->Get("invMassDown_ele_EB_in_High");
    TH1F * data1_down_EBout_low = (TH1F*)files[0]->Get("invMassDown_ele_EB_out_Low");
    TH1F * data1_down_EBout_high = (TH1F*)files[0]->Get("invMassDown_ele_EB_out_High");
    TH1F * data1_down_EEin_low = (TH1F*)files[0]->Get("invMassDown_ele_EE_in_Low");
    TH1F * data1_down_EEin_high = (TH1F*)files[0]->Get("invMassDown_ele_EE_in_High");
    TH1F * data1_down_EEout_low = (TH1F*)files[0]->Get("invMassDown_ele_EE_out_Low");
    TH1F * data1_down_EEout_high = (TH1F*)files[0]->Get("invMassDown_ele_EE_out_High");

    TH1F * data1_up_EBin_low = (TH1F*)files[0]->Get("invMassUp_ele_EB_in_Low");
    TH1F * data1_up_EBin_high = (TH1F*)files[0]->Get("invMassUp_ele_EB_in_High");
    TH1F * data1_up_EBout_low = (TH1F*)files[0]->Get("invMassUp_ele_EB_out_Low");
    TH1F * data1_up_EBout_high = (TH1F*)files[0]->Get("invMassUp_ele_EB_out_High");
    TH1F * data1_up_EEin_low = (TH1F*)files[0]->Get("invMassUp_ele_EE_in_Low");
    TH1F * data1_up_EEin_high = (TH1F*)files[0]->Get("invMassUp_ele_EE_in_High");
    TH1F * data1_up_EEout_low = (TH1F*)files[0]->Get("invMassUp_ele_EE_out_Low");
    TH1F * data1_up_EEout_high = (TH1F*)files[0]->Get("invMassUp_ele_EE_out_High");

    TH1F * mc1_EBin_low = (TH1F*)files[1]->Get("invMass_ele_EB_in_Low");
    TH1F * mc1_EBin_high = (TH1F*)files[1]->Get("invMass_ele_EB_in_High");
    TH1F * mc1_EBout_low = (TH1F*)files[1]->Get("invMass_ele_EB_out_Low");
    TH1F * mc1_EBout_high = (TH1F*)files[1]->Get("invMass_ele_EB_out_High");
    TH1F * mc1_EEin_low = (TH1F*)files[1]->Get("invMass_ele_EE_in_Low");
    TH1F * mc1_EEin_high = (TH1F*)files[1]->Get("invMass_ele_EE_in_High");
    TH1F * mc1_EEout_low = (TH1F*)files[1]->Get("invMass_ele_EE_out_Low");
    TH1F * mc1_EEout_high = (TH1F*)files[1]->Get("invMass_ele_EE_out_High");

    TH1F * down_mc1_EBin_low = (TH1F*)files[1]->Get("invMassDown_ele_EB_in_Low");
    TH1F * down_mc1_EBin_high = (TH1F*)files[1]->Get("invMassDown_ele_EB_in_High");
    TH1F * down_mc1_EBout_low = (TH1F*)files[1]->Get("invMassDown_ele_EB_out_Low");
    TH1F * down_mc1_EBout_high = (TH1F*)files[1]->Get("invMassDown_ele_EB_out_High");
    TH1F * down_mc1_EEin_low = (TH1F*)files[1]->Get("invMassDown_ele_EE_in_Low");
    TH1F * down_mc1_EEin_high = (TH1F*)files[1]->Get("invMassDown_ele_EE_in_High");
    TH1F * down_mc1_EEout_low = (TH1F*)files[1]->Get("invMassDown_ele_EE_out_Low");
    TH1F * down_mc1_EEout_high = (TH1F*)files[1]->Get("invMassDown_ele_EE_out_High");

    TH1F * up_mc1_EBin_low = (TH1F*)files[1]->Get("invMassUp_ele_EB_in_Low");
    TH1F * up_mc1_EBin_high = (TH1F*)files[1]->Get("invMassUp_ele_EB_in_High");
    TH1F * up_mc1_EBout_low = (TH1F*)files[1]->Get("invMassUp_ele_EB_out_Low");
    TH1F * up_mc1_EBout_high = (TH1F*)files[1]->Get("invMassUp_ele_EB_out_High");
    TH1F * up_mc1_EEin_low = (TH1F*)files[1]->Get("invMassUp_ele_EE_in_Low");
    TH1F * up_mc1_EEin_high = (TH1F*)files[1]->Get("invMassUp_ele_EE_in_High");
    TH1F * up_mc1_EEout_low = (TH1F*)files[1]->Get("invMassUp_ele_EE_out_Low");
    TH1F * up_mc1_EEout_high = (TH1F*)files[1]->Get("invMassUp_ele_EE_out_High");

    TH1F * data2_EBin_low = (TH1F*)files[2]->Get("invMass_ele_EB_in_Low");
    TH1F * data2_EBin_high = (TH1F*)files[2]->Get("invMass_ele_EB_in_High");
    TH1F * data2_EBout_low = (TH1F*)files[2]->Get("invMass_ele_EB_out_Low");
    TH1F * data2_EBout_high = (TH1F*)files[2]->Get("invMass_ele_EB_out_High");
    TH1F * data2_EEin_low = (TH1F*)files[2]->Get("invMass_ele_EE_in_Low");
    TH1F * data2_EEin_high = (TH1F*)files[2]->Get("invMass_ele_EE_in_High");
    TH1F * data2_EEout_low = (TH1F*)files[2]->Get("invMass_ele_EE_out_Low");
    TH1F * data2_EEout_high = (TH1F*)files[2]->Get("invMass_ele_EE_out_High");

    TH1F * data2_down_EBin_low = (TH1F*)files[2]->Get("invMassDown_ele_EB_in_Low");
    TH1F * data2_down_EBin_high = (TH1F*)files[2]->Get("invMassDown_ele_EB_in_High");
    TH1F * data2_down_EBout_low = (TH1F*)files[2]->Get("invMassDown_ele_EB_out_Low");
    TH1F * data2_down_EBout_high = (TH1F*)files[2]->Get("invMassDown_ele_EB_out_High");
    TH1F * data2_down_EEin_low = (TH1F*)files[2]->Get("invMassDown_ele_EE_in_Low");
    TH1F * data2_down_EEin_high = (TH1F*)files[2]->Get("invMassDown_ele_EE_in_High");
    TH1F * data2_down_EEout_low = (TH1F*)files[2]->Get("invMassDown_ele_EE_out_Low");
    TH1F * data2_down_EEout_high = (TH1F*)files[2]->Get("invMassDown_ele_EE_out_High");

    TH1F * data2_up_EBin_low = (TH1F*)files[2]->Get("invMassUp_ele_EB_in_Low");
    TH1F * data2_up_EBin_high = (TH1F*)files[2]->Get("invMassUp_ele_EB_in_High");
    TH1F * data2_up_EBout_low = (TH1F*)files[2]->Get("invMassUp_ele_EB_out_Low");
    TH1F * data2_up_EBout_high = (TH1F*)files[2]->Get("invMassUp_ele_EB_out_High");
    TH1F * data2_up_EEin_low = (TH1F*)files[2]->Get("invMassUp_ele_EE_in_Low");
    TH1F * data2_up_EEin_high = (TH1F*)files[2]->Get("invMassUp_ele_EE_in_High");
    TH1F * data2_up_EEout_low = (TH1F*)files[2]->Get("invMassUp_ele_EE_out_Low");
    TH1F * data2_up_EEout_high = (TH1F*)files[2]->Get("invMassUp_ele_EE_out_High");

    TH1F * mc2_EBin_low = (TH1F*)files[3]->Get("invMass_ele_EB_in_Low");
    TH1F * mc2_EBin_high = (TH1F*)files[3]->Get("invMass_ele_EB_in_High");
    TH1F * mc2_EBout_low = (TH1F*)files[3]->Get("invMass_ele_EB_out_Low");
    TH1F * mc2_EBout_high = (TH1F*)files[3]->Get("invMass_ele_EB_out_High");
    TH1F * mc2_EEin_low = (TH1F*)files[3]->Get("invMass_ele_EE_in_Low");
    TH1F * mc2_EEin_high = (TH1F*)files[3]->Get("invMass_ele_EE_in_High");
    TH1F * mc2_EEout_low = (TH1F*)files[3]->Get("invMass_ele_EE_out_Low");
    TH1F * mc2_EEout_high = (TH1F*)files[3]->Get("invMass_ele_EE_out_High");

    TH1F * down_mc2_EBin_low = (TH1F*)files[3]->Get("invMassDown_ele_EB_in_Low");
    TH1F * down_mc2_EBin_high = (TH1F*)files[3]->Get("invMassDown_ele_EB_in_High");
    TH1F * down_mc2_EBout_low = (TH1F*)files[3]->Get("invMassDown_ele_EB_out_Low");
    TH1F * down_mc2_EBout_high = (TH1F*)files[3]->Get("invMassDown_ele_EB_out_High");
    TH1F * down_mc2_EEin_low = (TH1F*)files[3]->Get("invMassDown_ele_EE_in_Low");
    TH1F * down_mc2_EEin_high = (TH1F*)files[3]->Get("invMassDown_ele_EE_in_High");
    TH1F * down_mc2_EEout_low = (TH1F*)files[3]->Get("invMassDown_ele_EE_out_Low");
    TH1F * down_mc2_EEout_high = (TH1F*)files[3]->Get("invMassDown_ele_EE_out_High");

    TH1F * up_mc2_EBin_low = (TH1F*)files[3]->Get("invMassUp_ele_EB_in_Low");
    TH1F * up_mc2_EBin_high = (TH1F*)files[3]->Get("invMassUp_ele_EB_in_High");
    TH1F * up_mc2_EBout_low = (TH1F*)files[3]->Get("invMassUp_ele_EB_out_Low");
    TH1F * up_mc2_EBout_high = (TH1F*)files[3]->Get("invMassUp_ele_EB_out_High");
    TH1F * up_mc2_EEin_low = (TH1F*)files[3]->Get("invMassUp_ele_EE_in_Low");
    TH1F * up_mc2_EEin_high = (TH1F*)files[3]->Get("invMassUp_ele_EE_in_High");
    TH1F * up_mc2_EEout_low = (TH1F*)files[3]->Get("invMassUp_ele_EE_out_Low");
    TH1F * up_mc2_EEout_high = (TH1F*)files[3]->Get("invMassUp_ele_EE_out_High");
    // want to print out data table:
    //  eta across top, R9 down

    TH1F* data1_EBin;
    TH1F* data1_EBout;
    TH1F* data1_EEin; 
    TH1F* data1_EEout;

    TH1F* data1_up_EBin; 
    TH1F* data1_up_EBout;
    TH1F* data1_up_EEin; 
    TH1F* data1_up_EEout;

    TH1F* data1_down_EBin;
    TH1F* data1_down_EBout;
    TH1F* data1_down_EEin;
    TH1F* data1_down_EEout;

    TH1F* data2_EBin;
    TH1F* data2_EBout;
    TH1F* data2_EEin; 
    TH1F* data2_EEout;

    TH1F* data2_up_EBin; 
    TH1F* data2_up_EBout;
    TH1F* data2_up_EEin; 
    TH1F* data2_up_EEout;

    TH1F* data2_down_EBin;
    TH1F* data2_down_EBout;
    TH1F* data2_down_EEin;
    TH1F* data2_down_EEout;

     TH1F* mc1_EBin;
     TH1F* mc1_EBout;
     TH1F* mc1_EEin;
     TH1F* mc1_EEout;

    TH1F* up_mc1_EBin;
    TH1F* up_mc1_EBout; 
    TH1F* up_mc1_EEin;
    TH1F* up_mc1_EEout;

    TH1F*  down_mc1_EBin;
    TH1F*  down_mc1_EBout; 
    TH1F*  down_mc1_EEin; 
    TH1F*  down_mc1_EEout; 

     TH1F* mc2_EBin;
     TH1F* mc2_EBout;
     TH1F* mc2_EEin;
     TH1F* mc2_EEout;

    TH1F* up_mc2_EBin;
    TH1F* up_mc2_EBout; 
    TH1F* up_mc2_EEin;
    TH1F* up_mc2_EEout;

    TH1F*  down_mc2_EBin;
    TH1F*  down_mc2_EBout; 
    TH1F*  down_mc2_EEin; 
    TH1F*  down_mc2_EEout; 


    //want bins of Eta, combine R9 plots
    if(kWhichR9 == -1 || kWhichR9 == 0){

       data1_EBin = (TH1F*)data1_EBin_low->Clone();
       data1_EBout = (TH1F*)data1_EBout_low->Clone();
       data1_EEin = (TH1F*)data1_EEin_low->Clone();
       data1_EEout = (TH1F*)data1_EEout_low->Clone();

       data1_up_EBin = (TH1F*)data1_up_EBin_low->Clone();
       data1_up_EBout = (TH1F*)data1_up_EBout_low->Clone();
       data1_up_EEin = (TH1F*)data1_up_EEin_low->Clone();
       data1_up_EEout = (TH1F*)data1_up_EEout_low->Clone();

       data1_down_EBin = (TH1F*)data1_down_EBin_low->Clone();
       data1_down_EBout = (TH1F*)data1_down_EBout_low->Clone();
       data1_down_EEin = (TH1F*)data1_down_EEin_low->Clone();
       data1_down_EEout = (TH1F*)data1_down_EEout_low->Clone();

       data2_EBin = (TH1F*)data2_EBin_low->Clone();
       data2_EBout = (TH1F*)data2_EBout_low->Clone();
       data2_EEin = (TH1F*)data2_EEin_low->Clone();
       data2_EEout = (TH1F*)data2_EEout_low->Clone();

       data2_up_EBin = (TH1F*)data2_up_EBin_low->Clone();
       data2_up_EBout = (TH1F*)data2_up_EBout_low->Clone();
       data2_up_EEin = (TH1F*)data2_up_EEin_low->Clone();
       data2_up_EEout = (TH1F*)data2_up_EEout_low->Clone();

       data2_down_EBin = (TH1F*)data2_down_EBin_low->Clone();
       data2_down_EBout = (TH1F*)data2_down_EBout_low->Clone();
       data2_down_EEin = (TH1F*)data2_down_EEin_low->Clone();
       data2_down_EEout = (TH1F*)data2_down_EEout_low->Clone();

        mc1_EBin = (TH1F*)mc1_EBin_low->Clone();
        mc1_EBout = (TH1F*)mc1_EBout_low->Clone();
        mc1_EEin = (TH1F*)mc1_EEin_low->Clone();
        mc1_EEout = (TH1F*)mc1_EEout_low->Clone();

        up_mc1_EBin = (TH1F*)up_mc1_EBin_low->Clone();
        up_mc1_EBout = (TH1F*)up_mc1_EBout_low->Clone();
        up_mc1_EEin = (TH1F*)up_mc1_EEin_low->Clone();
        up_mc1_EEout = (TH1F*)up_mc1_EEout_low->Clone();

        down_mc1_EBin = (TH1F*)down_mc1_EBin_low->Clone();
        down_mc1_EBout = (TH1F*)down_mc1_EBout_low->Clone();
        down_mc1_EEin = (TH1F*)down_mc1_EEin_low->Clone();
        down_mc1_EEout = (TH1F*)down_mc1_EEout_low->Clone();

        mc2_EBin = (TH1F*)mc2_EBin_low->Clone();
        mc2_EBout = (TH1F*)mc2_EBout_low->Clone();
        mc2_EEin = (TH1F*)mc2_EEin_low->Clone();
        mc2_EEout = (TH1F*)mc2_EEout_low->Clone();

        up_mc2_EBin = (TH1F*)up_mc2_EBin_low->Clone();
        up_mc2_EBout = (TH1F*)up_mc2_EBout_low->Clone();
        up_mc2_EEin = (TH1F*)up_mc2_EEin_low->Clone();
        up_mc2_EEout = (TH1F*)up_mc2_EEout_low->Clone();

        down_mc2_EBin = (TH1F*)down_mc2_EBin_low->Clone();
        down_mc2_EBout = (TH1F*)down_mc2_EBout_low->Clone();
        down_mc2_EEin = (TH1F*)down_mc2_EEin_low->Clone();
        down_mc2_EEout = (TH1F*)down_mc2_EEout_low->Clone();
    }
    if(kWhichR9 == 0){

       data1_EBin->Add(data1_EBin_high);
       data1_EBout->Add(data1_EBout_high);
       data1_EEin->Add(data1_EEin_high);
       data1_EEout->Add(data1_EEout_high);

       data1_up_EBin->Add(data1_up_EBin_high);
       data1_up_EBout->Add(data1_up_EBout_high);
       data1_up_EEin->Add(data1_up_EEin_high);
       data1_up_EEout->Add(data1_up_EEout_high);

       data1_down_EBin->Add(data1_down_EBin_high);
       data1_down_EBout->Add(data1_down_EBout_high);
       data1_down_EEin->Add(data1_down_EEin_high);
       data1_down_EEout->Add(data1_down_EEout_high);

       data2_EBin->Add(data2_EBin_high);
       data2_EBout->Add(data2_EBout_high);
       data2_EEin->Add(data2_EEin_high);
       data2_EEout->Add(data2_EEout_high);

       data2_up_EBin->Add(data2_up_EBin_high);
       data2_up_EBout->Add(data2_up_EBout_high);
       data2_up_EEin->Add(data2_up_EEin_high);
       data2_up_EEout->Add(data2_up_EEout_high);

       data2_down_EBin->Add(data2_down_EBin_high);
       data2_down_EBout->Add(data2_down_EBout_high);
       data2_down_EEin->Add(data2_down_EEin_high);
       data2_down_EEout->Add(data2_down_EEout_high);

        mc1_EBin->Add(mc1_EBin_high);
        mc1_EBout->Add(mc1_EBout_high);
        mc1_EEin->Add(mc1_EEin_high);
        mc1_EEout->Add(mc1_EEout_high);

        up_mc1_EBin->Add(up_mc1_EBin_high);
        up_mc1_EBout->Add(up_mc1_EBout_high);
        up_mc1_EEin->Add(up_mc1_EEin_high);
        up_mc1_EEout->Add(up_mc1_EEout_high);

        down_mc1_EBin->Add(down_mc1_EBin_high);
        down_mc1_EBout->Add(down_mc1_EBout_high);
        down_mc1_EEin->Add(down_mc1_EEin_high);
        down_mc1_EEout->Add(down_mc1_EEout_high);

        mc2_EBin->Add(mc2_EBin_high);
        mc2_EBout->Add(mc2_EBout_high);
        mc2_EEin->Add(mc2_EEin_high);
        mc2_EEout->Add(mc2_EEout_high);

        up_mc2_EBin->Add(up_mc2_EBin_high);
        up_mc2_EBout->Add(up_mc2_EBout_high);
        up_mc2_EEin->Add(up_mc2_EEin_high);
        up_mc2_EEout->Add(up_mc2_EEout_high);

        down_mc2_EBin->Add(down_mc2_EBin_high);
        down_mc2_EBout->Add(down_mc2_EBout_high);
        down_mc2_EEin->Add(down_mc2_EEin_high);
        down_mc2_EEout->Add(down_mc2_EEout_high);
    }
    if(kWhichR9 == 1){

       data1_EBin = (TH1F*)data1_EBin_high->Clone();
       data1_EBout = (TH1F*)data1_EBout_high->Clone();
       data1_EEin = (TH1F*)data1_EEin_high->Clone();
       data1_EEout = (TH1F*)data1_EEout_high->Clone();

       data1_up_EBin = (TH1F*)data1_up_EBin_high->Clone();
       data1_up_EBout = (TH1F*)data1_up_EBout_high->Clone();
       data1_up_EEin = (TH1F*)data1_up_EEin_high->Clone();
       data1_up_EEout = (TH1F*)data1_up_EEout_high->Clone();

       data1_down_EBin = (TH1F*)data1_down_EBin_high->Clone();
       data1_down_EBout = (TH1F*)data1_down_EBout_high->Clone();
       data1_down_EEin = (TH1F*)data1_down_EEin_high->Clone();
       data1_down_EEout = (TH1F*)data1_down_EEout_high->Clone();

       data2_EBin = (TH1F*)data2_EBin_high->Clone();
       data2_EBout = (TH1F*)data2_EBout_high->Clone();
       data2_EEin = (TH1F*)data2_EEin_high->Clone();
       data2_EEout = (TH1F*)data2_EEout_high->Clone();

       data2_up_EBin = (TH1F*)data2_up_EBin_high->Clone();
       data2_up_EBout = (TH1F*)data2_up_EBout_high->Clone();
       data2_up_EEin = (TH1F*)data2_up_EEin_high->Clone();
       data2_up_EEout = (TH1F*)data2_up_EEout_high->Clone();

       data2_down_EBin = (TH1F*)data2_down_EBin_high->Clone();
       data2_down_EBout = (TH1F*)data2_down_EBout_high->Clone();
       data2_down_EEin = (TH1F*)data2_down_EEin_high->Clone();
       data2_down_EEout = (TH1F*)data2_down_EEout_high->Clone();

        mc1_EBin = (TH1F*)mc1_EBin_high->Clone();
        mc1_EBout = (TH1F*)mc1_EBout_high->Clone();
        mc1_EEin = (TH1F*)mc1_EEin_high->Clone();
        mc1_EEout = (TH1F*)mc1_EEout_high->Clone();

        up_mc1_EBin = (TH1F*)up_mc1_EBin_high->Clone();
        up_mc1_EBout = (TH1F*)up_mc1_EBout_high->Clone();
        up_mc1_EEin = (TH1F*)up_mc1_EEin_high->Clone();
        up_mc1_EEout = (TH1F*)up_mc1_EEout_high->Clone();

        down_mc1_EBin = (TH1F*)down_mc1_EBin_high->Clone();
        down_mc1_EBout = (TH1F*)down_mc1_EBout_high->Clone();
        down_mc1_EEin = (TH1F*)down_mc1_EEin_high->Clone();
        down_mc1_EEout = (TH1F*)down_mc1_EEout_high->Clone();

        mc2_EBin = (TH1F*)mc2_EBin_high->Clone();
        mc2_EBout = (TH1F*)mc2_EBout_high->Clone();
        mc2_EEin = (TH1F*)mc2_EEin_high->Clone();
        mc2_EEout = (TH1F*)mc2_EEout_high->Clone();

        up_mc2_EBin = (TH1F*)up_mc2_EBin_high->Clone();
        up_mc2_EBout = (TH1F*)up_mc2_EBout_high->Clone();
        up_mc2_EEin = (TH1F*)up_mc2_EEin_high->Clone();
        up_mc2_EEout = (TH1F*)up_mc2_EEout_high->Clone();

        down_mc2_EBin = (TH1F*)down_mc2_EBin_high->Clone();
        down_mc2_EBout = (TH1F*)down_mc2_EBout_high->Clone();
        down_mc2_EEin = (TH1F*)down_mc2_EEin_high->Clone();
        down_mc2_EEout = (TH1F*)down_mc2_EEout_high->Clone();
    }

    string thisR9String = (kWhichR9 == 1) ? "_highR9" : (kWhichR9 == 0) ? "" : "_lowR9";
    double etaBins [6] = {0., 1., 1.4442, 1.566, 2., 2.5};

    TH1F * data1_loose = new TH1F("data1_loose", "", 4, etaBins);
    TH1F * data1Down_loose = new TH1F("data1Down_loose", "", 4, etaBins);
    TH1F * data1Up_loose = new TH1F("data1Up_loose", "", 4, etaBins);

    TH1F * data2_loose = new TH1F("data2_loose", "", 4, etaBins);
    TH1F * data2Down_loose = new TH1F("data2Down_loose", "", 4, etaBins);
    TH1F * data2Up_loose = new TH1F("data2Up_loose", "", 4, etaBins);

    TH1F * mc1_loose = new TH1F("mc1_loose", "", 4, etaBins);
    TH1F * mc1Down_loose = new TH1F("mc1Down_loose", "", 4, etaBins);
    TH1F * mc1Up_loose = new TH1F("mc1Up_loose", "", 4, etaBins);

    TH1F * mc2_loose = new TH1F("mc2_loose", "", 4, etaBins);
    TH1F * mc2Down_loose = new TH1F("mc2Down_loose", "", 4, etaBins);
    TH1F * mc2Up_loose = new TH1F("mc2Up_loose", "", 4, etaBins);
    
    errors* median;
    errors* stddev;

    if(kWhichR9 == 1){
        data1_EBout->Rebin(2,"",0);
        data2_EBout->Rebin(2,"",0);
    }

    if(stat.find("resolution") == std::string::npos){

    median = statistic::getHistStatistic(data1_EBin, stat, N_PERCENT_HIST, 500, 0.95);
    data1_loose->SetBinContent(1, median->x);
    data1_loose->SetBinError(1, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data1_EBout, stat, N_PERCENT_HIST, 500, 0.95);
    data1_loose->SetBinContent(2, median->x);
    data1_loose->SetBinError(2, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data1_EEin, stat, N_PERCENT_HIST, 500, 0.95);
    data1_loose->SetBinContent(4, median->x);
    data1_loose->SetBinError(4, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data1_EEout, stat, N_PERCENT_HIST, 500, 0.95);
    data1_loose->SetBinContent(5, median->x);
    data1_loose->SetBinError(5, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data1_down_EBin, stat, N_PERCENT_HIST, 500, 0.95);
    data1Down_loose->SetBinContent(1, median->x);
    data1Down_loose->SetBinError(1, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data1_down_EBout, stat, N_PERCENT_HIST, 500, 0.95);
    data1Down_loose->SetBinContent(2, median->x);
    data1Down_loose->SetBinError(2, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data1_down_EEin, stat, N_PERCENT_HIST, 500, 0.95);
    data1Down_loose->SetBinContent(4, median->x);
    data1Down_loose->SetBinError(4, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data1_down_EEout, stat, N_PERCENT_HIST, 500, 0.95);
    data1Down_loose->SetBinContent(5, median->x);
    data1Down_loose->SetBinError(5, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data1_up_EBin, stat, N_PERCENT_HIST, 500, 0.95);
    data1Up_loose->SetBinContent(1, median->x);
    data1Up_loose->SetBinError(1, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data1_up_EBout, stat, N_PERCENT_HIST, 500, 0.95);
    data1Up_loose->SetBinContent(2, median->x);
    data1Up_loose->SetBinError(2, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data1_up_EEin, stat, N_PERCENT_HIST, 500, 0.95);
    data1Up_loose->SetBinContent(4, median->x);
    data1Up_loose->SetBinError(4, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data1_up_EEout, stat, N_PERCENT_HIST, 500, 0.95);
    data1Up_loose->SetBinContent(5, median->x);
    data1Up_loose->SetBinError(5, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data2_EBin, stat, N_PERCENT_HIST, 500, 0.95);
    data2_loose->SetBinContent(1, median->x);
    data2_loose->SetBinError(1, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data2_EBout, stat, N_PERCENT_HIST, 500, 0.95);
    data2_loose->SetBinContent(2, median->x);
    data2_loose->SetBinError(2, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data2_EEin, stat, N_PERCENT_HIST, 500, 0.95);
    data2_loose->SetBinContent(4, median->x);
    data2_loose->SetBinError(4, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data2_EEout, stat, N_PERCENT_HIST, 500, 0.95);
    data2_loose->SetBinContent(5, median->x);
    data2_loose->SetBinError(5, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data2_down_EBin, stat, N_PERCENT_HIST, 500, 0.95);
    data2Down_loose->SetBinContent(1, median->x);
    data2Down_loose->SetBinError(1, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data2_down_EBout, stat, N_PERCENT_HIST, 500, 0.95);
    data2Down_loose->SetBinContent(2, median->x);
    data2Down_loose->SetBinError(2, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data2_down_EEin, stat, N_PERCENT_HIST, 500, 0.95);
    data2Down_loose->SetBinContent(4, median->x);
    data2Down_loose->SetBinError(4, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data2_down_EEout, stat, N_PERCENT_HIST, 500, 0.95);
    data2Down_loose->SetBinContent(5, median->x);
    data2Down_loose->SetBinError(5, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data2_up_EBin, stat, N_PERCENT_HIST, 500, 0.95);
    data2Up_loose->SetBinContent(1, median->x);
    data2Up_loose->SetBinError(1, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data2_up_EBout, stat, N_PERCENT_HIST, 500, 0.95);
    data2Up_loose->SetBinContent(2, median->x);
    data2Up_loose->SetBinError(2, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data2_up_EEin, stat, N_PERCENT_HIST, 500, 0.95);
    data2Up_loose->SetBinContent(4, median->x);
    data2Up_loose->SetBinError(4, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(data2_up_EEout, stat, N_PERCENT_HIST, 500, 0.95);
    data2Up_loose->SetBinContent(5, median->x);
    data2Up_loose->SetBinError(5, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(mc1_EBin, stat, N_PERCENT_HIST, 500, 0.95);
    mc1_loose->SetBinContent(1, median->x);
    mc1_loose->SetBinError(1, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(mc1_EBout, stat, N_PERCENT_HIST, 500, 0.95);
    mc1_loose->SetBinContent(2, median->x);
    mc1_loose->SetBinError(2, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(mc1_EEin, stat, N_PERCENT_HIST, 500, 0.95);
    mc1_loose->SetBinContent(4, median->x);
    mc1_loose->SetBinError(4, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(mc1_EEout, stat, N_PERCENT_HIST, 500, 0.95);
    mc1_loose->SetBinContent(5, median->x);
    mc1_loose->SetBinError(5, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(down_mc1_EBin, stat, N_PERCENT_HIST, 500, 0.95);
    mc1Down_loose->SetBinContent(1, median->x);
    mc1Down_loose->SetBinError(1, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(down_mc1_EBout, stat, N_PERCENT_HIST, 500, 0.95);
    mc1Down_loose->SetBinContent(2, median->x);
    mc1Down_loose->SetBinError(2, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(down_mc1_EEin, stat, N_PERCENT_HIST, 500, 0.95);
    mc1Down_loose->SetBinContent(4, median->x);
    mc1Down_loose->SetBinError(4, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(down_mc1_EEout, stat, N_PERCENT_HIST, 500, 0.95);
    mc1Down_loose->SetBinContent(5, median->x);
    mc1Down_loose->SetBinError(5, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(up_mc1_EBin, stat, N_PERCENT_HIST, 500, 0.95);
    mc1Up_loose->SetBinContent(1, median->x);
    mc1Up_loose->SetBinError(1, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(up_mc1_EBout, stat, N_PERCENT_HIST, 500, 0.95);
    mc1Up_loose->SetBinContent(2, median->x);
    mc1Up_loose->SetBinError(2, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(up_mc1_EEin, stat, N_PERCENT_HIST, 500, 0.95);
    mc1Up_loose->SetBinContent(4, median->x);
    mc1Up_loose->SetBinError(4, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(up_mc1_EEout, stat, N_PERCENT_HIST, 500, 0.95);
    mc1Up_loose->SetBinContent(5, median->x);
    mc1Up_loose->SetBinError(5, max(median->errLow, median->errHigh));


    median = statistic::getHistStatistic(mc2_EBin, stat, N_PERCENT_HIST, 500, 0.95);
    mc2_loose->SetBinContent(1, median->x);
    mc2_loose->SetBinError(1, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(mc2_EBout, stat, N_PERCENT_HIST, 500, 0.95);
    mc2_loose->SetBinContent(2, median->x);
    mc2_loose->SetBinError(2, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(mc2_EEin, stat, N_PERCENT_HIST, 500, 0.95);
    mc2_loose->SetBinContent(4, median->x);
    mc2_loose->SetBinError(4, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(mc2_EEout, stat, N_PERCENT_HIST, 500, 0.95);
    mc2_loose->SetBinContent(5, median->x);
    mc2_loose->SetBinError(5, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(down_mc2_EBin, stat, N_PERCENT_HIST, 500, 0.95);
    mc2Down_loose->SetBinContent(1, median->x);
    mc2Down_loose->SetBinError(1, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(down_mc2_EBout, stat, N_PERCENT_HIST, 500, 0.95);
    mc2Down_loose->SetBinContent(2, median->x);
    mc2Down_loose->SetBinError(2, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(down_mc2_EEin, stat, N_PERCENT_HIST, 500, 0.95);
    mc2Down_loose->SetBinContent(4, median->x);
    mc2Down_loose->SetBinError(4, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(down_mc2_EEout, stat, N_PERCENT_HIST, 500, 0.95);
    mc2Down_loose->SetBinContent(5, median->x);
    mc2Down_loose->SetBinError(5, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(up_mc2_EBin, stat, N_PERCENT_HIST, 500, 0.95);
    mc2Up_loose->SetBinContent(1, median->x);
    mc2Up_loose->SetBinError(1, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(up_mc2_EBout, stat, N_PERCENT_HIST, 500, 0.95);
    mc2Up_loose->SetBinContent(2, median->x);
    mc2Up_loose->SetBinError(2, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(up_mc2_EEin, stat, N_PERCENT_HIST, 500, 0.95);
    mc2Up_loose->SetBinContent(4, median->x);
    mc2Up_loose->SetBinError(4, max(median->errLow, median->errHigh));

    median = statistic::getHistStatistic(up_mc2_EEout, stat, N_PERCENT_HIST, 500, 0.95);
    mc2Up_loose->SetBinContent(5, median->x);
    mc2Up_loose->SetBinError(5, max(median->errLow, median->errHigh));
    }
    if(stat.find("resolution") != std::string::npos){

    median = statistic::getHistStatistic(data1_EBin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(data1_EBin, "stddev", N_PERCENT_HIST, 500, 0.95);
    data1_loose->SetBinContent(1, 100*stddev->x/median->x);
    data1_loose->SetBinError(1, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(data1_EBout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(data1_EBout, "stddev", N_PERCENT_HIST, 500, 0.95);
    data1_loose->SetBinContent(2, 100*stddev->x/median->x);
    data1_loose->SetBinError(2, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(data1_EEin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(data1_EEin, "stddev", N_PERCENT_HIST, 500, 0.95);
    data1_loose->SetBinContent(4, 100*stddev->x/median->x);
    data1_loose->SetBinError(4, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(data1_EEout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(data1_EEout, "stddev", N_PERCENT_HIST, 500, 0.95);
    data1_loose->SetBinContent(5, 100*stddev->x/median->x);
    data1_loose->SetBinError(5, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(data1_down_EBin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(data1_down_EBin, "stddev", N_PERCENT_HIST, 500, 0.95);
    data1Down_loose->SetBinContent(1, 100*stddev->x/median->x);
    data1Down_loose->SetBinError(1, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(data1_down_EBout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(data1_down_EBout, "stddev", N_PERCENT_HIST, 500, 0.95);
    data1Down_loose->SetBinContent(2, 100*stddev->x/median->x);
    data1Down_loose->SetBinError(2, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(data1_down_EEin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(data1_down_EEin, "stddev", N_PERCENT_HIST, 500, 0.95);
    data1Down_loose->SetBinContent(4, 100*stddev->x/median->x);
    data1Down_loose->SetBinError(4, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(data1_down_EEout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(data1_down_EEout, "stddev", N_PERCENT_HIST, 500, 0.95);
    data1Down_loose->SetBinContent(5, 100*stddev->x/median->x);
    data1Down_loose->SetBinError(5, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(data1_up_EBin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(data1_up_EBin, "stddev", N_PERCENT_HIST, 500, 0.95);
    data1Up_loose->SetBinContent(1, 100*stddev->x/median->x);
    data1Up_loose->SetBinError(1, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(data1_up_EBout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(data1_up_EBout, "stddev", N_PERCENT_HIST, 500, 0.95);
    data1Up_loose->SetBinContent(2, 100*stddev->x/median->x);
    data1Up_loose->SetBinError(2, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(data1_up_EEin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(data1_up_EEin, "stddev", N_PERCENT_HIST, 500, 0.95);
    data1Up_loose->SetBinContent(4, 100*stddev->x/median->x);
    data1Up_loose->SetBinError(4, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(data1_up_EEout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(data1_up_EEout, "stddev", N_PERCENT_HIST, 500, 0.95);
    data1Up_loose->SetBinContent(5, 100*stddev->x/median->x);
    data1Up_loose->SetBinError(5, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(mc1_EBin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(mc1_EBin, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc1_loose->SetBinContent(1, 100*stddev->x/median->x);
    mc1_loose->SetBinError(1, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(mc1_EBout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(mc1_EBout, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc1_loose->SetBinContent(2, 100*stddev->x/median->x);
    mc1_loose->SetBinError(2, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(mc1_EEin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(mc1_EEin, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc1_loose->SetBinContent(4, 100*stddev->x/median->x);
    mc1_loose->SetBinError(4, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(mc1_EEout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(mc1_EEout, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc1_loose->SetBinContent(5, 100*stddev->x/median->x);
    mc1_loose->SetBinError(5, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(down_mc1_EBin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(down_mc1_EBin, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc1Down_loose->SetBinContent(1, 100*stddev->x/median->x);
    mc1Down_loose->SetBinError(1, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(down_mc1_EBout,"median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(down_mc1_EBout,"stddev", N_PERCENT_HIST, 500, 0.95);
    mc1Down_loose->SetBinContent(2, 100*stddev->x/median->x);
    mc1Down_loose->SetBinError(2, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(down_mc1_EEin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(down_mc1_EEin, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc1Down_loose->SetBinContent(4, 100*stddev->x/median->x);
    mc1Down_loose->SetBinError(4, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(down_mc1_EEout,"median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(down_mc1_EEout,"stddev", N_PERCENT_HIST, 500, 0.95);
    mc1Down_loose->SetBinContent(5, 100*stddev->x/median->x);
    mc1Down_loose->SetBinError(5, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(up_mc1_EBin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(up_mc1_EBin, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc1Up_loose->SetBinContent(1, 100*stddev->x/median->x);
    mc1Up_loose->SetBinError(1, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(up_mc1_EBout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(up_mc1_EBout, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc1Up_loose->SetBinContent(2, 100*stddev->x/median->x);
    mc1Up_loose->SetBinError(2, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(up_mc1_EEin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(up_mc1_EEin, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc1Up_loose->SetBinContent(4, 100*stddev->x/median->x);
    mc1Up_loose->SetBinError(4, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(up_mc1_EEout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(up_mc1_EEout, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc1Up_loose->SetBinContent(5, 100*stddev->x/median->x);
    mc1Up_loose->SetBinError(5, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(mc2_EBin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(mc2_EBin, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc2_loose->SetBinContent(1, 100*stddev->x/median->x);
    mc2_loose->SetBinError(1, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(mc2_EBout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(mc2_EBout, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc2_loose->SetBinContent(2, 100*stddev->x/median->x);
    mc2_loose->SetBinError(2, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(mc2_EEin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(mc2_EEin, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc2_loose->SetBinContent(4, 100*stddev->x/median->x);
    mc2_loose->SetBinError(4, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(mc2_EEout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(mc2_EEout, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc2_loose->SetBinContent(5, 100*stddev->x/median->x);
    mc2_loose->SetBinError(5, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(down_mc2_EBin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(down_mc2_EBin, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc2Down_loose->SetBinContent(1, 100*stddev->x/median->x);
    mc2Down_loose->SetBinError(1, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(down_mc2_EBout,"median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(down_mc2_EBout,"stddev", N_PERCENT_HIST, 500, 0.95);
    mc2Down_loose->SetBinContent(2, 100*stddev->x/median->x);
    mc2Down_loose->SetBinError(2, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(down_mc2_EEin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(down_mc2_EEin, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc2Down_loose->SetBinContent(4, 100*stddev->x/median->x);
    mc2Down_loose->SetBinError(4, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(down_mc2_EEout,"median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(down_mc2_EEout,"stddev", N_PERCENT_HIST, 500, 0.95);
    mc2Down_loose->SetBinContent(5, 100*stddev->x/median->x);
    mc2Down_loose->SetBinError(5, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(up_mc2_EBin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(up_mc2_EBin, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc2Up_loose->SetBinContent(1, 100*stddev->x/median->x);
    mc2Up_loose->SetBinError(1, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(up_mc2_EBout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(up_mc2_EBout, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc2Up_loose->SetBinContent(2, 100*stddev->x/median->x);
    mc2Up_loose->SetBinError(2, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(up_mc2_EEin, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(up_mc2_EEin, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc2Up_loose->SetBinContent(4, 100*stddev->x/median->x);
    mc2Up_loose->SetBinError(4, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    median = statistic::getHistStatistic(up_mc2_EEout, "median", N_PERCENT_HIST, 500, 0.95);
    stddev = statistic::getHistStatistic(up_mc2_EEout, "stddev", N_PERCENT_HIST, 500, 0.95);
    mc2Up_loose->SetBinContent(5, 100*stddev->x/median->x);
    mc2Up_loose->SetBinError(5, (100*stddev->x/median->x)*sqrt(pow(max(stddev->errLow, stddev->errHigh)/stddev->x, 2) + pow(max(median->errLow, median->errHigh)/median->x, 2)));

    }

    TH1F * scalesUncertainties1 = new TH1F("scalesUnc1", "", 5,etaBins);
    TH1F * smearsUncertainties1 = new TH1F("smearsUnc1", "", 5,etaBins);
    TH1F * scalesUncertainties2 = new TH1F("scalesUnc2", "", 5,etaBins);
    TH1F * smearsUncertainties2 = new TH1F("smearsUnc2", "", 5,etaBins);
    TH1F * systUnc1 = new TH1F("systUnc1", "", 5,etaBins);
    TH1F * systUnc2 = new TH1F("systUnc2", "", 5,etaBins);

    int numBins = 0;
    numBins = 6;
    for(int i = 1; i < numBins; i++){
        scalesUncertainties1->SetBinContent(i, 1.);
        scalesUncertainties1->SetBinError(i, max( fabs(data1_loose->GetBinContent(i)-data1Up_loose->GetBinContent(i)), fabs(data1_loose->GetBinContent(i)-data1Down_loose->GetBinContent(i))));
        smearsUncertainties1->SetBinContent(i, 1.);
        smearsUncertainties1->SetBinError(i, max( fabs(mc1_loose->GetBinContent(i)-mc1Up_loose->GetBinContent(i)), fabs(mc1_loose->GetBinContent(i)-mc1Down_loose->GetBinContent(i))));
        systUnc1->SetBinContent(i, 1.);
        systUnc1->SetBinError(i, (data1_loose->GetBinContent(i)/mc1_loose->GetBinContent(i))* sqrt( pow( scalesUncertainties1->GetBinError(i)/data1_loose->GetBinContent(i) , 2) + pow( smearsUncertainties1->GetBinError(i)/mc1_loose->GetBinContent(i), 2)));

        scalesUncertainties2->SetBinContent(i, 1.);
        scalesUncertainties2->SetBinError(i, max( fabs(data2_loose->GetBinContent(i)-data2Up_loose->GetBinContent(i)), fabs(data2_loose->GetBinContent(i)-data2Down_loose->GetBinContent(i))));
        smearsUncertainties2->SetBinContent(i, 1.);
        smearsUncertainties2->SetBinError(i, max( fabs(mc2_loose->GetBinContent(i)-mc2Up_loose->GetBinContent(i)), fabs(mc2_loose->GetBinContent(i)-mc2Down_loose->GetBinContent(i))));
        systUnc2->SetBinContent(i, 1.);
        systUnc2->SetBinError(i, (data2_loose->GetBinContent(i)/mc2_loose->GetBinContent(i))* sqrt( pow( scalesUncertainties2->GetBinError(i)/data2_loose->GetBinContent(i) , 2) + pow( smearsUncertainties2->GetBinError(i)/mc2_loose->GetBinContent(i), 2)));

    }
    systUnc1->SetBinError(3, 0.);
    systUnc2->SetBinError(3, 0.);

    TH1F * ratio1 = new TH1F("ratio1", "", 5, etaBins);
    TH1F * ratio2 = new TH1F("ratio2", "", 5, etaBins);

    for(int i = 1; i < numBins; i++){
        ratio1->SetBinContent(i, data1_loose->GetBinContent(i)/mc1_loose->GetBinContent(i));
        ratio2->SetBinContent(i, data2_loose->GetBinContent(i)/mc2_loose->GetBinContent(i));
    }
    ratio1->SetBinContent(3, 0);
    ratio2->SetBinContent(3, 0);

    string dataTitle = "Data/MC 2018: 2018 CoarseEtaR9Gain";
    string mcTitle = "Data/MC 2018: 2018 CoarseEtaR9Gain + High Et Threshold";
    string plotTitle = "invMass_2018_doubleRatio_highEtThreshold_g12"+thisR9String;

    return utilities::PlotDataMC(ratio1, dataTitle, ratio2, mcTitle, plotTitle, "|#eta|", "Median M_{ee} Data/MC", 0, 2.4999, 0.99, 1.01, false, true, "#bf{Nominal / High Et Threshold}", 0.998, 1.002, "eta");
};

TCanvas * evaluate_R9Plots( string data, string mc){
    //files
    vector <TFile*> files;
    files.push_back(new TFile(data.c_str(), "READ"));
    files.push_back(new TFile(mc.c_str(), "READ"));

    TH1F * r90_corrected1 = (TH1F*)files[0]->Get("invMass2_R90");
    TH1F * r91_corrected1 = (TH1F*)files[0]->Get("invMass2_R91");
    TH1F * r92_corrected1 = (TH1F*)files[0]->Get("invMass2_R92");
    TH1F * r93_corrected1 = (TH1F*)files[0]->Get("invMass2_R93");
    TH1F * r94_corrected1 = (TH1F*)files[0]->Get("invMass2_R94");
    TH1F * r90_correctedUp1 = (TH1F*)files[0]->Get("invMass2Up_R90");
    TH1F * r91_correctedUp1 = (TH1F*)files[0]->Get("invMass2Up_R91");
    TH1F * r92_correctedUp1 = (TH1F*)files[0]->Get("invMass2Up_R92");
    TH1F * r93_correctedUp1 = (TH1F*)files[0]->Get("invMass2Up_R93");
    TH1F * r94_correctedUp1 = (TH1F*)files[0]->Get("invMass2Up_R94");
    TH1F * r90_correctedDown1 = (TH1F*)files[0]->Get("invMass2Down_R90");
    TH1F * r91_correctedDown1 = (TH1F*)files[0]->Get("invMass2Down_R91");
    TH1F * r92_correctedDown1 = (TH1F*)files[0]->Get("invMass2Down_R92");
    TH1F * r93_correctedDown1 = (TH1F*)files[0]->Get("invMass2Down_R93");
    TH1F * r94_correctedDown1 = (TH1F*)files[0]->Get("invMass2Down_R94");

    TH1F * r90_corrected2 = (TH1F*)files[1]->Get("invMass2_R90");
    TH1F * r91_corrected2 = (TH1F*)files[1]->Get("invMass2_R91");
    TH1F * r92_corrected2 = (TH1F*)files[1]->Get("invMass2_R92");
    TH1F * r93_corrected2 = (TH1F*)files[1]->Get("invMass2_R93");
    TH1F * r94_corrected2 = (TH1F*)files[1]->Get("invMass2_R94");
    TH1F * r90_correctedUp2 = (TH1F*)files[1]->Get("invMass2Up_R90");
    TH1F * r91_correctedUp2 = (TH1F*)files[1]->Get("invMass2Up_R91");
    TH1F * r92_correctedUp2 = (TH1F*)files[1]->Get("invMass2Up_R92");
    TH1F * r93_correctedUp2 = (TH1F*)files[1]->Get("invMass2Up_R93");
    TH1F * r94_correctedUp2 = (TH1F*)files[1]->Get("invMass2Up_R94");
    TH1F * r90_correctedDown2 = (TH1F*)files[1]->Get("invMass2Down_R90");
    TH1F * r91_correctedDown2 = (TH1F*)files[1]->Get("invMass2Down_R91");
    TH1F * r92_correctedDown2 = (TH1F*)files[1]->Get("invMass2Down_R92");
    TH1F * r93_correctedDown2 = (TH1F*)files[1]->Get("invMass2Down_R93");
    TH1F * r94_correctedDown2 = (TH1F*)files[1]->Get("invMass2Down_R94");



    double r9Bins [6] = {0.5, 0.8, 0.9, 0.92, 0.96, 1.};

    vector<TH1F*> data_corrected;
    vector<TH1F*> dataUp_corrected;
    vector<TH1F*> dataDown_corrected;
    vector<TH1F*> mc_corrected;
    vector<TH1F*> mcUp_corrected;
    vector<TH1F*> mcDown_corrected;

    data_corrected.push_back(r90_corrected1);
    data_corrected.push_back(r91_corrected1);
    data_corrected.push_back(r92_corrected1);
    data_corrected.push_back(r93_corrected1);
    data_corrected.push_back(r94_corrected1);
    dataUp_corrected.push_back(r90_correctedUp1);
    dataUp_corrected.push_back(r91_correctedUp1);
    dataUp_corrected.push_back(r92_correctedUp1);
    dataUp_corrected.push_back(r93_correctedUp1);
    dataUp_corrected.push_back(r94_correctedUp1);
    dataDown_corrected.push_back(r90_correctedDown1);
    dataDown_corrected.push_back(r91_correctedDown1);
    dataDown_corrected.push_back(r92_correctedDown1);
    dataDown_corrected.push_back(r93_correctedDown1);
    dataDown_corrected.push_back(r94_correctedDown1);

    mc_corrected.push_back(r90_corrected2);
    mc_corrected.push_back(r91_corrected2);
    mc_corrected.push_back(r92_corrected2);
    mc_corrected.push_back(r93_corrected2);
    mc_corrected.push_back(r94_corrected2);
    mcUp_corrected.push_back(r90_correctedUp2);
    mcUp_corrected.push_back(r91_correctedUp2);
    mcUp_corrected.push_back(r92_correctedUp2);
    mcUp_corrected.push_back(r93_correctedUp2);
    mcUp_corrected.push_back(r94_correctedUp2);
    mcDown_corrected.push_back(r90_correctedDown2);
    mcDown_corrected.push_back(r91_correctedDown2);
    mcDown_corrected.push_back(r92_correctedDown2);
    mcDown_corrected.push_back(r93_correctedDown2);
    mcDown_corrected.push_back(r94_correctedDown2);


    TH1F * data_correctedDist =        new TH1F("data_correctedDist", "", 5, r9Bins);
    TH1F * data_correctedDist_up =     new TH1F("data_correctedDist_up", "", 5, r9Bins);
    TH1F * data_correctedDist_down =   new TH1F("data_correctedDist_down", "", 5, r9Bins);
    TH1F * mc_correctedDist =        new TH1F("mc_correctedDist", "", 5, r9Bins);
    TH1F * mc_correctedDist_up =     new TH1F("mc_correctedDist_up", "", 5, r9Bins);
    TH1F * mc_correctedDist_down =   new TH1F("mc_correctedDist_down", "", 5, r9Bins);

    errors * median;
    for(int i = 0; i < 5; i++){
        median = statistic::getHistStatistic(mc_corrected[i], "median", 100, 400, 0.95);
        mc_correctedDist->SetBinContent(i+1, median->x);
        mc_correctedDist->SetBinError(i+1, max(median->errLow, median->errHigh));

        median = statistic::getHistStatistic(mcUp_corrected[i], "median", 100, 400, 0.95);
        mc_correctedDist_up->SetBinContent(i+1, median->x);
        mc_correctedDist_up->SetBinError(i+1, max(median->errLow, median->errHigh));

        median = statistic::getHistStatistic(mcDown_corrected[i], "median", 100, 400, 0.95);
        mc_correctedDist_down->SetBinContent(i+1, median->x);
        mc_correctedDist_down->SetBinError(i+1, max(median->errLow, median->errHigh));

        median = statistic::getHistStatistic(data_corrected[i], "median", 100, 400, 0.95);
        data_correctedDist->SetBinContent(i+1, median->x);
        data_correctedDist->SetBinError(i+1, max(median->errLow, median->errHigh));

        median = statistic::getHistStatistic(dataUp_corrected[i], "median", 100, 400, 0.95);
        data_correctedDist_up->SetBinContent(i+1, median->x);
        data_correctedDist_up->SetBinError(i+1, max(median->errLow, median->errHigh));

        median = statistic::getHistStatistic(dataDown_corrected[i], "median", 100, 400, 0.95);
        data_correctedDist_down->SetBinContent(i+1, median->x);
        data_correctedDist_down->SetBinError(i+1, max(median->errLow, median->errHigh));
    }

    TH1F * scalesUnc = new TH1F("scalesUnc", "", 5, r9Bins);
    TH1F * smearsUnc = new TH1F("smearsUnc", "", 5, r9Bins);
    TH1F * totalUnc = new TH1F("totalUnc", "", 5, r9Bins);
    for(int i = 1; i < 6; i++){
        scalesUnc->SetBinContent(i, 1.);
        smearsUnc->SetBinContent(i, 1.);
        scalesUnc->SetBinError(i, max(fabs(data_correctedDist_up->GetBinContent(i)-data_correctedDist->GetBinContent(i)), fabs(data_correctedDist_down->GetBinContent(i)-data_correctedDist->GetBinContent(i))));
        smearsUnc->SetBinError(i, max(fabs(mc_correctedDist->GetBinContent(i)-mc_correctedDist_up->GetBinContent(i)), fabs(mc_correctedDist->GetBinContent(i)-mc_correctedDist_down->GetBinContent(i))));
        totalUnc->SetBinContent(i, 1);
        totalUnc->SetBinError(i, data_correctedDist->GetBinContent(i)/mc_correctedDist->GetBinContent(i) * sqrt( pow( scalesUnc->GetBinError(i)/data_correctedDist->GetBinContent(i), 2) + pow( smearsUnc->GetBinError(i)/mc_correctedDist->GetBinContent(i), 2)));
    }

    string dataTitle;
    string mcTitle = "MC: Run-fineEtaR9-Et";
    string plotTitle;
    if( data.find("scaledEt") != std::string::npos){
        dataTitle = "Data: Run-FineEtaR9-Et";
        plotTitle = "invMass_mee_R9_Et";
    }
    if( data.find("scaledNewEt") != std::string::npos){
        dataTitle = "Data: Run-FineEtaR9-Et UncV2";
        plotTitle = "invMass_mee_R9_newEt";
    }
    if( data.find("scaledEtGain") != std::string::npos){
        dataTitle = "Data: Run-FineEtaR9-Et FixGain12";
        plotTitle = "invMass_mee_R9_fixGain";
    }
    if( data.find("mvaID") != std::string::npos){
        dataTitle = "Data: Run-FineEtaR9-Et";
        plotTitle = "invMass_mee_R9_mvaID";
    }


    dataTitle = "Incusive MC: Et Smeared, Uncorrected";
    mcTitle = "Inclusive MC: Et Smeared, R9 Corrected";

    plotTitle = "invMass_mc2016_r9scale_R9";
    return utilities::PlotDataMC(data_correctedDist, dataTitle, mc_correctedDist, mcTitle, plotTitle, "R_{9}", "Median Invariant Mass [GeV]", 0.5, 0.99999, 89, 92, true, true, "#bf{ Uncorrected / Corrected }", 0.995, 1.005, "r90");
    //plotTitle = "invMass_mee2018_R9_noRatio";
    //utilities::PlotDataMC(data_correctedDist, dataTitle, mc_correctedDist, mcTitle, totalUnc, plotTitle, "R_{9} [GeV]", "Median Invariant Mass [GeV]", 0.5, 0.99999, 88, 94, true, false, "#bf{ Data / MC }", 0.99, 1.01, "r90");
    //utilities::PlotDataMC(data_loose, dataTitle, mc_loose, mcTitle, systUnc, plotTitle, "", "Median Invariant Mass [GeV]", 0, 0.999, 89, 92, false, true, "#bf{ Uncorrected / Corrected }", 0.995, 1.005, "categories");


};

TCanvas * standardSet::evaluate_CatPlots( string data, string mc1){
    //files
    vector <TFile*> files;
    files.push_back(new TFile(data.c_str(), "READ"));
    files.push_back(new TFile(mc1.c_str(), "READ"));

    TH1F * EBin_low = (TH1F*)files[0]->Get("invMass_ele_EB_in_Low");
    TH1F * EBin_high = (TH1F*)files[0]->Get("invMass_ele_EB_in_High");
    TH1F * EBout_low = (TH1F*)files[0]->Get("invMass_ele_EB_out_Low");
    TH1F * EBout_high = (TH1F*)files[0]->Get("invMass_ele_EB_out_High");
    TH1F * EEin_low = (TH1F*)files[0]->Get("invMass_ele_EE_in_Low");
    TH1F * EEin_high = (TH1F*)files[0]->Get("invMass_ele_EE_in_High");
    TH1F * EEout_low = (TH1F*)files[0]->Get("invMass_ele_EE_out_Low");
    TH1F * EEout_high = (TH1F*)files[0]->Get("invMass_ele_EE_out_High");

    TH1F * down_EBin_low = (TH1F*)files[0]->Get("invMassDown_ele_EB_in_Low");
    TH1F * down_EBin_high = (TH1F*)files[0]->Get("invMassDown_ele_EB_in_High");
    TH1F * down_EBout_low = (TH1F*)files[0]->Get("invMassDown_ele_EB_out_Low");
    TH1F * down_EBout_high = (TH1F*)files[0]->Get("invMassDown_ele_EB_out_High");
    TH1F * down_EEin_low = (TH1F*)files[0]->Get("invMassDown_ele_EE_in_Low");
    TH1F * down_EEin_high = (TH1F*)files[0]->Get("invMassDown_ele_EE_in_High");
    TH1F * down_EEout_low = (TH1F*)files[0]->Get("invMassDown_ele_EE_out_Low");
    TH1F * down_EEout_high = (TH1F*)files[0]->Get("invMassDown_ele_EE_out_High");

    TH1F * up_EBin_low = (TH1F*)files[0]->Get("invMassUp_ele_EB_in_Low");
    TH1F * up_EBin_high = (TH1F*)files[0]->Get("invMassUp_ele_EB_in_High");
    TH1F * up_EBout_low = (TH1F*)files[0]->Get("invMassUp_ele_EB_out_Low");
    TH1F * up_EBout_high = (TH1F*)files[0]->Get("invMassUp_ele_EB_out_High");
    TH1F * up_EEin_low = (TH1F*)files[0]->Get("invMassUp_ele_EE_in_Low");
    TH1F * up_EEin_high = (TH1F*)files[0]->Get("invMassUp_ele_EE_in_High");
    TH1F * up_EEout_low = (TH1F*)files[0]->Get("invMassUp_ele_EE_out_Low");
    TH1F * up_EEout_high = (TH1F*)files[0]->Get("invMassUp_ele_EE_out_High");

    TH1F * mc_EBin_low = (TH1F*)files[1]->Get("invMass_ele_EB_in_Low");
    TH1F * mc_EBin_high = (TH1F*)files[1]->Get("invMass_ele_EB_in_High");
    TH1F * mc_EBout_low = (TH1F*)files[1]->Get("invMass_ele_EB_out_Low");
    TH1F * mc_EBout_high = (TH1F*)files[1]->Get("invMass_ele_EB_out_High");
    TH1F * mc_EEin_low = (TH1F*)files[1]->Get("invMass_ele_EE_in_Low");
    TH1F * mc_EEin_high = (TH1F*)files[1]->Get("invMass_ele_EE_in_High");
    TH1F * mc_EEout_low = (TH1F*)files[1]->Get("invMass_ele_EE_out_Low");
    TH1F * mc_EEout_high = (TH1F*)files[1]->Get("invMass_ele_EE_out_High");

    TH1F * down_mc_EBin_low = (TH1F*)files[1]->Get("invMassDown_ele_EB_in_Low");
    TH1F * down_mc_EBin_high = (TH1F*)files[1]->Get("invMassDown_ele_EB_in_High");
    TH1F * down_mc_EBout_low = (TH1F*)files[1]->Get("invMassDown_ele_EB_out_Low");
    TH1F * down_mc_EBout_high = (TH1F*)files[1]->Get("invMassDown_ele_EB_out_High");
    TH1F * down_mc_EEin_low = (TH1F*)files[1]->Get("invMassDown_ele_EE_in_Low");
    TH1F * down_mc_EEin_high = (TH1F*)files[1]->Get("invMassDown_ele_EE_in_High");
    TH1F * down_mc_EEout_low = (TH1F*)files[1]->Get("invMassDown_ele_EE_out_Low");
    TH1F * down_mc_EEout_high = (TH1F*)files[1]->Get("invMassDown_ele_EE_out_High");

    TH1F * up_mc_EBin_low = (TH1F*)files[1]->Get("invMassUp_ele_EB_in_Low");
    TH1F * up_mc_EBin_high = (TH1F*)files[1]->Get("invMassUp_ele_EB_in_High");
    TH1F * up_mc_EBout_low = (TH1F*)files[1]->Get("invMassUp_ele_EB_out_Low");
    TH1F * up_mc_EBout_high = (TH1F*)files[1]->Get("invMassUp_ele_EB_out_High");
    TH1F * up_mc_EEin_low = (TH1F*)files[1]->Get("invMassUp_ele_EE_in_Low");
    TH1F * up_mc_EEin_high = (TH1F*)files[1]->Get("invMassUp_ele_EE_in_High");
    TH1F * up_mc_EEout_low = (TH1F*)files[1]->Get("invMassUp_ele_EE_out_Low");
    TH1F * up_mc_EEout_high = (TH1F*)files[1]->Get("invMassUp_ele_EE_out_High");
    // want to print out data table:
    //  eta across top, R9 down
    
    vector< errors* > data_highR9;
    vector< errors* > data_lowR9;
    vector< errors* > mc_highR9;
    vector< errors* > mc_lowR9;


    data_highR9.push_back( statistic::getHistStatistic(EBin_high, "median", N_PERCENT_HIST, 500, 0.95));
    data_highR9.push_back( statistic::getHistStatistic(EBout_high, "median", N_PERCENT_HIST, 500, 0.95));
    data_highR9.push_back( statistic::getHistStatistic(EEin_high, "median", N_PERCENT_HIST, 500, 0.95));
    data_highR9.push_back( statistic::getHistStatistic(EEout_high, "median", N_PERCENT_HIST, 500, 0.95));

    data_lowR9.push_back( statistic::getHistStatistic(EBin_low, "median", N_PERCENT_HIST, 500, 0.95));
    data_lowR9.push_back( statistic::getHistStatistic(EBout_low, "median", N_PERCENT_HIST, 500, 0.95));
    data_lowR9.push_back( statistic::getHistStatistic(EEin_low, "median", N_PERCENT_HIST, 500, 0.95));
    data_lowR9.push_back( statistic::getHistStatistic(EEout_low, "median", N_PERCENT_HIST, 500, 0.95));

    mc_highR9.push_back( statistic::getHistStatistic(mc_EBin_high, "median", N_PERCENT_HIST, 500, 0.95));
    mc_highR9.push_back( statistic::getHistStatistic(mc_EBout_high, "median", N_PERCENT_HIST, 500, 0.95));
    mc_highR9.push_back( statistic::getHistStatistic(mc_EEin_high, "median", N_PERCENT_HIST, 500, 0.95));
    mc_highR9.push_back( statistic::getHistStatistic(mc_EEout_high, "median", N_PERCENT_HIST, 500, 0.95));

    mc_lowR9.push_back( statistic::getHistStatistic(mc_EBin_low, "median", N_PERCENT_HIST, 500, 0.95));
    mc_lowR9.push_back( statistic::getHistStatistic(mc_EBout_low, "median", N_PERCENT_HIST, 500, 0.95));
    mc_lowR9.push_back( statistic::getHistStatistic(mc_EEin_low, "median", N_PERCENT_HIST, 500, 0.95));
    mc_lowR9.push_back( statistic::getHistStatistic(mc_EEout_low, "median", N_PERCENT_HIST, 500, 0.95));

    //want bins of Eta, combine R9 plots
    TH1F * EBin = (TH1F*)EBin_low->Clone();
    TH1F * EBout = (TH1F*)EBin_high->Clone();
    TH1F * EEin = (TH1F*)EEin_low->Clone();
    TH1F * EEout = (TH1F*)EEin_high->Clone();

    TH1F * down_EBin = (TH1F*)down_EBin_low->Clone();
    TH1F * down_EBout = (TH1F*)down_EBin_high->Clone();
    TH1F * down_EEin = (TH1F*)down_EEin_low->Clone();
    TH1F * down_EEout = (TH1F*)down_EEin_high->Clone();

    TH1F * up_EBin = (TH1F*)up_EBin_low->Clone();
    TH1F * up_EBout = (TH1F*)up_EBin_high->Clone();
    TH1F * up_EEin = (TH1F*)up_EEin_low->Clone();
    TH1F * up_EEout = (TH1F*)up_EEin_high->Clone();

    TH1F * mc_EBin = (TH1F*)mc_EBin_low->Clone();
    TH1F * mc_EBout = (TH1F*)mc_EBin_high->Clone();
    TH1F * mc_EEin = (TH1F*)mc_EEin_low->Clone();
    TH1F * mc_EEout = (TH1F*)mc_EEin_high->Clone();

    TH1F * down_mc_EBin = (TH1F*)down_mc_EBin_low->Clone();
    TH1F * down_mc_EBout = (TH1F*)down_mc_EBin_high->Clone();
    TH1F * down_mc_EEin = (TH1F*)down_mc_EEin_low->Clone();
    TH1F * down_mc_EEout = (TH1F*)down_mc_EEin_high->Clone();

    TH1F * up_mc_EBin = (TH1F*)up_mc_EBin_low->Clone();
    TH1F * up_mc_EBout = (TH1F*)up_mc_EBin_high->Clone();
    TH1F * up_mc_EEin = (TH1F*)up_mc_EEin_low->Clone();
    TH1F * up_mc_EEout = (TH1F*)up_mc_EEin_high->Clone();

    EBin->Add(EBout_low);
    up_EBin->Add(up_EBout_low);
    down_EBin->Add(down_EBout_low);
    EBout->Add(EBout_high);
    up_EBout->Add(up_EBout_high);
    down_EBout->Add(down_EBout_high);
    EEin->Add(EEout_low);
    up_EEin->Add(up_EEout_low);
    down_EEin->Add(down_EEout_low);
    EEout->Add(EEout_high);
    up_EEout->Add(up_EEout_high);
    down_EEout->Add(down_EEout_high);
    
    mc_EBin->Add(mc_EBout_low);
    mc_EBout->Add(mc_EBout_high);
    mc_EEin->Add(mc_EEout_low);
    mc_EEout->Add(mc_EEout_high);
    up_mc_EBin->Add(up_mc_EBout_low);
    up_mc_EBout->Add(up_mc_EBout_high);
    up_mc_EEin->Add(up_mc_EEout_low);
    up_mc_EEout->Add(up_mc_EEout_high);
    down_mc_EBin->Add(down_mc_EBout_low);
    down_mc_EBout->Add(down_mc_EBout_high);
    down_mc_EEin->Add(down_mc_EEout_low);
    down_mc_EEout->Add(down_mc_EEout_high);

    std::vector<TH1F*> data_hists;
    std::vector<TH1F*> data_hists_up;
    std::vector<TH1F*> data_hists_down;

    std::vector<TH1F*> mc_hists;
    std::vector<TH1F*> mc_hists_up;
    std::vector<TH1F*> mc_hists_down;

    data_hists.push_back(EBin);
    data_hists.push_back(EBout);
    data_hists.push_back(EEin);
    data_hists.push_back(EEout);

    data_hists_down.push_back(down_EBin);
    data_hists_down.push_back(down_EBout);
    data_hists_down.push_back(down_EEin);
    data_hists_down.push_back(down_EEout);

    data_hists_up.push_back(up_EBin);
    data_hists_up.push_back(up_EBout);
    data_hists_up.push_back(up_EEin);
    data_hists_up.push_back(up_EEout);

    mc_hists.push_back(mc_EBin);
    mc_hists.push_back(mc_EBout);
    mc_hists.push_back(mc_EEin);
    mc_hists.push_back(mc_EEout);

    mc_hists_down.push_back(down_mc_EBin);
    mc_hists_down.push_back(down_mc_EBout);
    mc_hists_down.push_back(down_mc_EEin);
    mc_hists_down.push_back(down_mc_EEout);

    mc_hists_up.push_back(up_mc_EBin);
    mc_hists_up.push_back(up_mc_EBout);
    mc_hists_up.push_back(up_mc_EEin);
    mc_hists_up.push_back(up_mc_EEout);

    if(_flag_invMassDist){
        for(int i = 0; i < 4; i ++){
            auto lastBins = 9999999999;
            auto theseBins = data_hists[i]->GetNbinsX();
            while(data_hists[i]->GetNbinsX() > 2*(data_hists[i]->GetXaxis()->GetXmax() - data_hists[i]->GetXaxis()->GetXmin())){
                if(data_hists[i]->GetNbinsX()%2 == 0){
                    data_hists[i]->Rebin(2, "", 0);
                }
                else{
                    int tryAgain = 3;
                    while(theseBins >= lastBins){
                        data_hists[i]->Rebin(tryAgain, "", 0);
                        theseBins = data_hists[i]->GetNbinsX();
                        tryAgain++;
                    }
                }
                lastBins = theseBins;
            }
            data_hists[i]->Scale(1/data_hists[i]->Integral());

            lastBins = 9999999999;
            theseBins = mc_hists[i]->GetNbinsX();
            while(mc_hists[i]->GetNbinsX() > 2*(mc_hists[i]->GetXaxis()->GetXmax() - mc_hists[i]->GetXaxis()->GetXmin())){
                if(mc_hists[i]->GetNbinsX()%2 == 0){
                    mc_hists[i]->Rebin(2, "", 0);
                }
                else{
                    int tryAgain = 3;
                    while(theseBins >= lastBins){
                        mc_hists[i]->Rebin(tryAgain, "", 0);
                        theseBins = mc_hists[i]->GetNbinsX();
                        tryAgain++;
                    }
                }
                lastBins = theseBins;
            }
            mc_hists[i]->Scale(1/mc_hists[i]->Integral());
        }
        utilities::PlotDataMC(EBin, dataTitle, mc_EBin, mcTitle, "standardSet_"+outputFile+"_meeDist_EB_lowR9", "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", -1, -1, "");
        utilities::PlotDataMC(EBout, dataTitle, mc_EBout, mcTitle, "standardSet_"+outputFile+"_meeDist_EB_highR9", "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", -1, -1, "");
        utilities::PlotDataMC(EEin, dataTitle, mc_EEin, mcTitle, "standardSet_"+outputFile+"_meeDist_EE_lowR9", "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", -1, -1, "");
        utilities::PlotDataMC(EEout, dataTitle, mc_EEout, mcTitle, "standardSet_"+outputFile+"_meeDist_EE_highR9", "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", -1, -1, "");
    }

    double etaBins [6] = {0., 1., 1.4442, 1.566, 2., 2.5};
    //TH1F * data_loose = new TH1F("data_loose", "", 5, etaBins);
    //TH1F * mc_loose = new TH1F("mc_loose", "", 5, etaBins);

    TH1F * data_loose = new TH1F("data_loose", "", 4, 0, 1);
    TH1F * dataDown_loose = new TH1F("dataDown_loose", "", 4, 0, 1);
    TH1F * dataUp_loose = new TH1F("dataUp_loose", "", 4, 0, 1);

    TH1F * mc_loose = new TH1F("mc_loose", "", 4, 0, 1);
    TH1F * mcDown_loose = new TH1F("mcDown_loose", "", 4, 0, 1);
    TH1F * mcUp_loose = new TH1F("mcUp_loose", "", 4, 0, 1);
    
    errors* median;
    std::string stat = "median";
    for(int i = 1; i < 5; i++){
        median = statistic::getHistStatistic(data_hists[i-1], stat, N_PERCENT_HIST, 500, 0.95);
        data_loose->SetBinContent(i, median->x);
        data_loose->SetBinError(i, max(median->errLow, median->errHigh));

        median = statistic::getHistStatistic(data_hists_down[i-1], stat, N_PERCENT_HIST, 500, 0.95);
        dataDown_loose->SetBinContent(i, median->x);
        dataDown_loose->SetBinError(i, max(median->errLow, median->errHigh));

        median = statistic::getHistStatistic(data_hists_up[i-1], stat, N_PERCENT_HIST, 500, 0.95);
        dataUp_loose->SetBinContent(i, median->x);
        dataUp_loose->SetBinError(i, max(median->errLow, median->errHigh));

        median = statistic::getHistStatistic(mc_hists[i-1], stat, N_PERCENT_HIST, 500, 0.95);
        mc_loose->SetBinContent(i, median->x);
        mc_loose->SetBinError(i, max(median->errLow, median->errHigh));

        median = statistic::getHistStatistic(mc_hists_down[i-1], stat, N_PERCENT_HIST, 500, 0.95);
        mcDown_loose->SetBinContent(i, median->x);
        mcDown_loose->SetBinError(i, max(median->errLow, median->errHigh));

        median = statistic::getHistStatistic(mc_hists_up[i-1], stat, N_PERCENT_HIST, 500, 0.95);
        mcUp_loose->SetBinContent(i, median->x);
        mcUp_loose->SetBinError(i, max(median->errLow, median->errHigh));
    }

    TH1F * scalesUncertainties = new TH1F("scalesUnc", "", 4,0,1);
    TH1F * smearsUncertainties = new TH1F("smearsUnc", "", 4,0,1);
    TH1F * systUnc = new TH1F("systUnc", "", 4,0,1);

    int numBins = 0;
    numBins = 5;
    for(int i = 1; i < numBins; i++){
        scalesUncertainties->SetBinContent(i, 1.);
        scalesUncertainties->SetBinError(i, max( fabs(data_loose->GetBinContent(i)-dataUp_loose->GetBinContent(i)), fabs(data_loose->GetBinContent(i)-dataDown_loose->GetBinContent(i))));
        smearsUncertainties->SetBinContent(i, 1.);
        smearsUncertainties->SetBinError(i, max( fabs(mc_loose->GetBinContent(i)-mcUp_loose->GetBinContent(i)), fabs(mc_loose->GetBinContent(i)-mcDown_loose->GetBinContent(i))));
        systUnc->SetBinContent(i, 1.);
        systUnc->SetBinError(i, (data_loose->GetBinContent(i)/mc_loose->GetBinContent(i))* sqrt( pow( scalesUncertainties->GetBinError(i)/data_loose->GetBinContent(i) , 2) + pow( smearsUncertainties->GetBinError(i)/mc_loose->GetBinContent(i), 2)));
    }

    return utilities::PlotDataMC(data_loose, dataTitle, mc_loose, mcTitle, systUnc, "standardSet_"+outputFile+"_cat_median_noStatErr", "", "Median Invariant Mass [GeV]", -1, -1, 89, 92, false, true, "#bf{ Data / MC }", -1, -1, "categories");
};

TCanvas * standardSet::evaluate_EtPlots( string dataFile, string mcFile){
    //files
    vector <TFile*> files;
    files.push_back(new TFile(dataFile.c_str(), "READ"));
    files.push_back(new TFile(mcFile.c_str(), "READ"));

    vector<TH1F*> data_Et_EBin;
    vector<TH1F*> data_Et_EBout;
    vector<TH1F*> data_Et_EEin;
    vector<TH1F*> data_Et_EEout;
    vector<TH1F*> mc_Et_EBin;
    vector<TH1F*> mc_Et_EBout;
    vector<TH1F*> mc_Et_EEin;
    vector<TH1F*> mc_Et_EEout;


    vector<TH1F*> data_Et;
    vector<TH1F*> dataUp_Et;
    vector<TH1F*> dataDown_Et;
    vector<TH1F*> mc_Et;
    vector<TH1F*> mcUp_Et;
    vector<TH1F*> mcDown_Et;

/*
    data_Et_EBin.push_back((TH1F*)files[0]->Get("invMass_Et0_EBin"));
    data_Et_EBin.push_back((TH1F*)files[0]->Get("invMass_Et1_EBin"));
    data_Et_EBin.push_back((TH1F*)files[0]->Get("invMass_Et2_EBin"));
    data_Et_EBin.push_back((TH1F*)files[0]->Get("invMass_Et3_EBin"));
    data_Et_EBin.push_back((TH1F*)files[0]->Get("invMass_Et4_EBin"));
 
    data_Et_EBout.push_back((TH1F*)files[0]->Get("invMass_Et0_EBout"));
    data_Et_EBout.push_back((TH1F*)files[0]->Get("invMass_Et1_EBout"));
    data_Et_EBout.push_back((TH1F*)files[0]->Get("invMass_Et2_EBout"));
    data_Et_EBout.push_back((TH1F*)files[0]->Get("invMass_Et3_EBout"));
    data_Et_EBout.push_back((TH1F*)files[0]->Get("invMass_Et4_EBout"));

    data_Et_EEin.push_back((TH1F*)files[0]->Get("invMass_Et0_EEin"));
    data_Et_EEin.push_back((TH1F*)files[0]->Get("invMass_Et1_EEin"));
    data_Et_EEin.push_back((TH1F*)files[0]->Get("invMass_Et2_EEin"));
    data_Et_EEin.push_back((TH1F*)files[0]->Get("invMass_Et3_EEin"));
    data_Et_EEin.push_back((TH1F*)files[0]->Get("invMass_Et4_EEin"));

    data_Et_EEout.push_back((TH1F*)files[0]->Get("invMass_Et0_EEout"));
    data_Et_EEout.push_back((TH1F*)files[0]->Get("invMass_Et1_EEout"));
    data_Et_EEout.push_back((TH1F*)files[0]->Get("invMass_Et2_EEout"));
    data_Et_EEout.push_back((TH1F*)files[0]->Get("invMass_Et3_EEout"));
    data_Et_EEout.push_back((TH1F*)files[0]->Get("invMass_Et4_EEout"));
*/

    data_Et.push_back((TH1F*)files[0]->Get("invMass_Et0"));
    data_Et.push_back((TH1F*)files[0]->Get("invMass_Et1"));
    data_Et.push_back((TH1F*)files[0]->Get("invMass_Et2"));
    data_Et.push_back((TH1F*)files[0]->Get("invMass_Et3"));
    data_Et.push_back((TH1F*)files[0]->Get("invMass_Et4"));
    data_Et.push_back((TH1F*)files[0]->Get("invMass_Et5"));

    dataUp_Et.push_back((TH1F*)files[0]->Get("invMassUp_Et0"));
    dataUp_Et.push_back((TH1F*)files[0]->Get("invMassUp_Et1"));
    dataUp_Et.push_back((TH1F*)files[0]->Get("invMassUp_Et2"));
    dataUp_Et.push_back((TH1F*)files[0]->Get("invMassUp_Et3"));
    dataUp_Et.push_back((TH1F*)files[0]->Get("invMassUp_Et4"));
    dataUp_Et.push_back((TH1F*)files[0]->Get("invMassUp_Et5"));

    dataDown_Et.push_back((TH1F*)files[0]->Get("invMassDown_Et0"));
    dataDown_Et.push_back((TH1F*)files[0]->Get("invMassDown_Et1"));
    dataDown_Et.push_back((TH1F*)files[0]->Get("invMassDown_Et2"));
    dataDown_Et.push_back((TH1F*)files[0]->Get("invMassDown_Et3"));
    dataDown_Et.push_back((TH1F*)files[0]->Get("invMassDown_Et4"));
    dataDown_Et.push_back((TH1F*)files[0]->Get("invMassDown_Et5"));

    /*
    mc_Et_EBin.push_back((TH1F*)files[1]->Get("invMass_Et0_EBin"));
    mc_Et_EBin.push_back((TH1F*)files[1]->Get("invMass_Et1_EBin"));
    mc_Et_EBin.push_back((TH1F*)files[1]->Get("invMass_Et2_EBin"));
    mc_Et_EBin.push_back((TH1F*)files[1]->Get("invMass_Et3_EBin"));
    mc_Et_EBin.push_back((TH1F*)files[1]->Get("invMass_Et4_EBin"));

    mc_Et_EBout.push_back((TH1F*)files[1]->Get("invMass_Et0_EBout"));
    mc_Et_EBout.push_back((TH1F*)files[1]->Get("invMass_Et1_EBout"));
    mc_Et_EBout.push_back((TH1F*)files[1]->Get("invMass_Et2_EBout"));
    mc_Et_EBout.push_back((TH1F*)files[1]->Get("invMass_Et3_EBout"));
    mc_Et_EBout.push_back((TH1F*)files[1]->Get("invMass_Et4_EBout"));

    mc_Et_EEin.push_back((TH1F*)files[1]->Get("invMass_Et0_EEin"));
    mc_Et_EEin.push_back((TH1F*)files[1]->Get("invMass_Et1_EEin"));
    mc_Et_EEin.push_back((TH1F*)files[1]->Get("invMass_Et2_EEin"));
    mc_Et_EEin.push_back((TH1F*)files[1]->Get("invMass_Et3_EEin"));
    mc_Et_EEin.push_back((TH1F*)files[1]->Get("invMass_Et4_EEin"));

    mc_Et_EEout.push_back((TH1F*)files[1]->Get("invMass_Et0_EEout"));
    mc_Et_EEout.push_back((TH1F*)files[1]->Get("invMass_Et1_EEout"));
    mc_Et_EEout.push_back((TH1F*)files[1]->Get("invMass_Et2_EEout"));
    mc_Et_EEout.push_back((TH1F*)files[1]->Get("invMass_Et3_EEout"));
    mc_Et_EEout.push_back((TH1F*)files[1]->Get("invMass_Et4_EEout"));
    */

    mc_Et.push_back((TH1F*)files[1]->Get("invMass_Et0"));
    mc_Et.push_back((TH1F*)files[1]->Get("invMass_Et1"));
    mc_Et.push_back((TH1F*)files[1]->Get("invMass_Et2"));
    mc_Et.push_back((TH1F*)files[1]->Get("invMass_Et3"));
    mc_Et.push_back((TH1F*)files[1]->Get("invMass_Et4"));
    mc_Et.push_back((TH1F*)files[1]->Get("invMass_Et5"));

    mcUp_Et.push_back((TH1F*)files[1]->Get("invMassUp_Et0"));
    mcUp_Et.push_back((TH1F*)files[1]->Get("invMassUp_Et1"));
    mcUp_Et.push_back((TH1F*)files[1]->Get("invMassUp_Et2"));
    mcUp_Et.push_back((TH1F*)files[1]->Get("invMassUp_Et3"));
    mcUp_Et.push_back((TH1F*)files[1]->Get("invMassUp_Et4"));
    mcUp_Et.push_back((TH1F*)files[1]->Get("invMassUp_Et5"));

    mcDown_Et.push_back((TH1F*)files[1]->Get("invMassDown_Et0"));
    mcDown_Et.push_back((TH1F*)files[1]->Get("invMassDown_Et1"));
    mcDown_Et.push_back((TH1F*)files[1]->Get("invMassDown_Et2"));
    mcDown_Et.push_back((TH1F*)files[1]->Get("invMassDown_Et3"));
    mcDown_Et.push_back((TH1F*)files[1]->Get("invMassDown_Et4"));
    mcDown_Et.push_back((TH1F*)files[1]->Get("invMassDown_Et5"));

    /*
    for(int i = 0; i < 5; i++){
        data_Et_EBin[i]->Add(data_Et_EBout[i]);
        data_Et_EBin[i]->Add(data_Et_EEin[i]);
        data_Et_EBin[i]->Add(data_Et_EEout[i]);
        mc_Et_EBin[i]->Add(data_Et_EBout[i]);
        mc_Et_EBin[i]->Add(data_Et_EEin[i]);
        mc_Et_EBin[i]->Add(data_Et_EEout[i]);
    }
    */

    double etBins [6] = { 32., 40, 50., 80, 120., 200};
    double etBins2 [7] = {32., 40., 50., 80., 120., 300., 500.};

    TH1F * data = new TH1F("data", "", 6, etBins2);
    TH1F * dataUp = new TH1F("dataUp", "", 6, etBins2);
    TH1F * dataDown = new TH1F("dataDown", "", 6, etBins2);
    TH1F * mc = new TH1F("mc", "", 6, etBins2);
    TH1F * mcUp = new TH1F("mcUp", "", 6, etBins2);
    TH1F * mcDown = new TH1F("mcDown", "", 6, etBins2);

    errors* median;
    errors* stddev;
    for(int i = 0; i < 6; i++){
        median = statistic::getHistStatistic(data_Et[i], "median", N_PERCENT_HIST, 500, 0.95);
        data->SetBinContent(i+1, median->x);
        data->SetBinError(i+1, max(median->errLow, median->errHigh));

        median = statistic::getHistStatistic(dataUp_Et[i], "median", N_PERCENT_HIST, 500, 0.95);
        dataUp->SetBinContent(i+1, median->x);
        dataUp->SetBinError(i+1, max(median->errLow, median->errHigh) );

        median = statistic::getHistStatistic(dataDown_Et[i], "median", N_PERCENT_HIST, 500, 0.95);
        dataDown->SetBinContent(i+1, median->x);
        dataDown->SetBinError(i+1, max(median->errLow, median->errHigh) );

        median = statistic::getHistStatistic(mc_Et[i], "median", N_PERCENT_HIST, 500, 0.95);
        mc->SetBinContent(i+1, median->x);
        mc->SetBinError(i+1, max(median->errLow, median->errHigh) );

        median = statistic::getHistStatistic(mcUp_Et[i], "median", N_PERCENT_HIST, 500, 0.95);
        mcUp->SetBinContent(i+1, median->x);
        mcUp->SetBinError(i+1, max(median->errLow, median->errHigh) );

        median = statistic::getHistStatistic(mcDown_Et[i], "median", N_PERCENT_HIST, 500, 0.95);
        mcDown->SetBinContent(i+1, median->x);
        mcDown->SetBinError(i+1, max(median->errLow, median->errHigh) );
    }

    TH1F * scalesUncertainties = new TH1F("scalesUnc", "", 6, etBins2);
    TH1F * smearsUncertainties = new TH1F("smearsUnc", "", 6, etBins2);
    TH1F * totalSyst = new TH1F("systUnc", "", 6, etBins2);
    for(int i = 1; i < 7; i++){
        scalesUncertainties->SetBinContent(i, 1.);
        scalesUncertainties->SetBinError(i, max( fabs(data->GetBinContent(i)-dataUp->GetBinContent(i)), fabs(data->GetBinContent(i)-dataDown->GetBinContent(i))));
        smearsUncertainties->SetBinContent(i, 1.);
        smearsUncertainties->SetBinError(i, max( fabs(mc->GetBinContent(i)-mcUp->GetBinContent(i)), fabs(mc->GetBinContent(i)-mcDown->GetBinContent(i))));
        totalSyst->SetBinContent(i, 1);
        totalSyst->SetBinError(i, (data->GetBinContent(i)/mc->GetBinContent(i))*sqrt( pow(scalesUncertainties->GetBinError(i)/data->GetBinContent(i), 2) + pow( smearsUncertainties->GetBinError(i)/mc->GetBinContent(i), 2)));

    }
    string dataTitle;
    string mcTitle = "MC: Run-fineEtaR9-Et";
    string plotTitle;
    if( dataFile.find("scaledEt") != std::string::npos){
        dataTitle = "Data: Run-FineEtaR9-Et";
        plotTitle = "invMass_mee_Et_Et";
    }
    if( dataFile.find("scaledNewEt") != std::string::npos){
        dataTitle = "Data: Run-FineEtaR9-Et UncV2";
        plotTitle = "invMass_mee_Et_newEt";
    }
    if( dataFile.find("scaledEtGain") != std::string::npos){
        dataTitle = "Data: Run-FineEtaR9-Et FixGain12";
        plotTitle = "invMass_mee_Et_fixGain";
    }
    if( dataFile.find("mvaID") != std::string::npos){
        dataTitle = "Data: Run-FineEtaR9-Et";
        plotTitle = "invMass_mee_Et_mvaID";
    }


    plotTitle = "invMass_mee2018_Et_ratio";
    utilities::PlotDataMC(data, dataTitle, mc, mcTitle, totalSyst, plotTitle, "E_{T} [GeV]", "Median Invariant Mass [GeV]", 32, 299.99999, 88, 94, true, true, "#bf{ Data / MC }", 0.99, 1.01, "et");
    plotTitle = "invMass_mee2018_Et_noRatio";
    return utilities::PlotDataMC(data, dataTitle, mc, mcTitle, totalSyst, plotTitle, "E_{T} [GeV]", "Median Invariant Mass [GeV]", 32, 299.99999, 88, 94, true, false, "#bf{ Data / MC }", 0.99, 1.01, "et");
    //utilities::PlotOne( data5 , "Et Scaled Data", "invMass_relativeRes_EE", "E_{T} [GeV]", "#sigma_{M_{ee}}/M_{ee} [%]", 20, 199.99999, 2, 6, true);
};
#endif
