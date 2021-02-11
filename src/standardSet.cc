#ifndef STANDARD_SET
#define STANDARD_SET

#include "../interface/utilities.h"
#include "../interface/statistic.h"
#include "../interface/standardSet.h"
#include <TH1.h>
#include <TH2.h>
#include <TLegend.h>
#include <TProfile.h>
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
#include <fstream>

#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooBreitWigner.h"
#include "RooCBShape.h"
#include "RooFFTConvPdf.h"
#include "RooFitResult.h"

using namespace std;
using namespace RooFit;

extern string dataTitle;
extern string mcTitle;
extern string outputFile;
extern string lumiLabel;
extern bool _flag_invMassDist;
extern double N_PERCENT_HIST;

utilities util;

Double_t breitGausFun(Double_t *x, Double_t *par){
    double Zwidth = 2.4952;
    return (par[2] * TMath::Voigt(x[0] - par[0], par[1], Zwidth));
}

TF1 * fitBreitGaus_old(TH1 * hist, bool isData){
    TF1 * f1 = new TF1();
    if(hist){

        double xmin = hist->GetMean() - 2*hist->GetRMS();
        double xmax = hist->GetMean() + 2*hist->GetRMS();
        f1 = new TF1("f1", breitGausFun, xmin, xmax, 3);
        f1->SetParNames("BW mean", "Gauss Width", "Integral");

        double par[3];
        par[0] = hist->GetMean();
        par[1] = 2.5;
        par[2] = hist->Integral();

        f1->SetParameters(par[0], par[1], par[2]);
        f1->SetParLimits(0, 85, 95);
        f1->SetParLimits(1, 1.0, 5);
        if( isData) hist->Fit(f1,"mlre 0");
        else hist->Fit(f1,"mwlre 0");

        //need to check if fit was successful
    } else {
        std::cout << "##########################################################################" << std::endl;
        std::cout << "could not fit hist " << hist->GetName() << std::endl;
        std::cout << "##########################################################################" << std::endl;
    }
    return f1;
};

RooFitResult * fitBreitGaus(TH1 * hist, bool isData){
    if(!hist){
        std::cout << "no histogram" << std::endl;
        return (RooFitResult*)0;
    }
    RooRealVar invmass("invmass","invmass", 80, 100);
    invmass.setBins(hist->GetNbinsX(), "cache");
    invmass.setMin("cache", 80);
    invmass.setMax("cache", 100);
    RooDataHist roo_hist("roo_hist", "roo_hist", invmass, Import(*hist));

    // Breit-Wigner
    RooRealVar m0( "m0", "m0", 91.188 );
    RooRealVar width( "width", "width", 2.495 );
    RooBreitWigner bw( "bw", "bw", invmass, m0, width );

    // Crystal-Ball
    RooRealVar mean( "mean", "mean", hist->GetBinCenter(hist->GetMaximumBin())-91.188, -10, 4);
    RooRealVar sigma( "sigma", "sigma", hist->GetRMS()-1, 0, 25);
    RooRealVar alpha( "alpha", "alpha", 1.424, 0.01, 10);
    RooRealVar n( "n", "n", 1.86, 0.5, 100);
    RooCBShape cb( "cb", "cb", invmass, mean, sigma, alpha, n );

    RooFFTConvPdf * pdf = new RooFFTConvPdf("pdf", "pdf", invmass, bw, cb);
    RooFitResult * ret = pdf->fitTo(roo_hist,
            SumW2Error(true), Save(), PrintEvalErrors(-1),
            RooFit::Verbose(kFALSE), RooFit::PrintLevel(-1),
            RooFit::Warnings(kFALSE), RooFit::PrintEvalErrors(kFALSE) );
    return ret;
}

TCanvas * standardSet::runPlots(std::string _str_data, std::string _str_mc){

    TFile * data = new TFile(_str_data.c_str(), "READ");
    TFile * mc = new TFile(_str_mc.c_str(), "READ");

    TH2F * data0 = (TH2F*)data->Get("invMass_run2018_EBin");
    TH2F * data1 = (TH2F*)data->Get("invMass_run2018_EBout");
    TH2F * data2 = (TH2F*)data->Get("invMass_run2018_EEin");
    TH2F * data3 = (TH2F*)data->Get("invMass_run2018_EEout");

    TH2F * mc0 = (TH2F*)mc->Get("invMass_run2018_EBin");
    TH2F * mc1 = (TH2F*)mc->Get("invMass_run2018_EBout");
    TH2F * mc2 = (TH2F*)mc->Get("invMass_run2018_EEin");
    TH2F * mc3 = (TH2F*)mc->Get("invMass_run2018_EEout");

    TH1F * dat0 = new TH1F("dat0", "", data0->GetNbinsX(), data0->GetXaxis()->GetXbins()->GetArray());
    TH1F * dat1 = new TH1F("dat1", "", data0->GetNbinsX(), data0->GetXaxis()->GetXbins()->GetArray());
    TH1F * dat2 = new TH1F("dat2", "", data0->GetNbinsX(), data0->GetXaxis()->GetXbins()->GetArray());
    TH1F * dat3 = new TH1F("dat3", "", data0->GetNbinsX(), data0->GetXaxis()->GetXbins()->GetArray());

    TH1F * d0 = util.GetMedianProfilesWithError(data0);
    TH1F * d1 = util.GetMedianProfilesWithError(data1);
    TH1F * d2 = util.GetMedianProfilesWithError(data2);
    TH1F * d3 = util.GetMedianProfilesWithError(data3);

    for( int i = 1; i <= dat0->GetNbinsX(); i++){
        dat0->SetBinContent(i, d0->GetBinContent(i));
        dat0->SetBinError(i,d0->GetBinError(i));
        dat1->SetBinContent(i, d1->GetBinContent(i));
        dat1->SetBinError(i,d1->GetBinError(i));
        dat2->SetBinContent(i, d2->GetBinContent(i));
        dat2->SetBinError(i,d2->GetBinError(i));
        dat3->SetBinContent(i, d3->GetBinContent(i));
        dat3->SetBinError(i,d3->GetBinError(i));
    }

    TH1F * m0 = (TH1F*)mc0->ProjectionY("_py0",1,1,"");
    TH1F * m1 = (TH1F*)mc1->ProjectionY("_py1",1,1,"");
    TH1F * m2 = (TH1F*)mc2->ProjectionY("_py2",1,1,"");
    TH1F * m3 = (TH1F*)mc3->ProjectionY("_py3",1,1,"");

    double median = 0.5;
    double median0, median1, median2, median3;
    m0->GetQuantiles(1,&median0,&median);
    m1->GetQuantiles(1,&median1,&median);
    m2->GetQuantiles(1,&median2,&median);
    m3->GetQuantiles(1,&median3,&median);

    return util.plotRuns2018(dat0, dat1, dat2, dat3, median0, median1, median2, median3, 88, 92, outputFile+"_runs2017");
    
}

TCanvas * standardSet::etLinearityPlots(std::string _str_data, std::string _str_mc){
    TFile * data = new TFile(_str_data.c_str(), "READ");
    TFile * mc = new TFile(_str_mc.c_str(), "READ");

    TH2F * data_et_eb = (TH2F*) data->Get("mee_et_eb");
    TH2F * mc_et_eb = (TH2F*) mc->Get("mee_et_eb");
    TH2F * data_et_ee = (TH2F*) data->Get("mee_et_ee");
    TH2F * mc_et_ee = (TH2F*) mc->Get("mee_et_ee");

    TH2F * dataUp_et_eb = (TH2F*) data->Get("meeUp_et_eb");
    TH2F * mcUp_et_eb = (TH2F*) mc->Get("meeUp_et_eb");
    TH2F * dataUp_et_ee = (TH2F*) data->Get("meeUp_et_ee");
    TH2F * mcUp_et_ee = (TH2F*) mc->Get("meeUp_et_ee");

    TH2F * dataDown_et_eb = (TH2F*) data->Get("meeDown_et_eb");
    TH2F * mcDown_et_eb = (TH2F*) mc->Get("meeDown_et_eb");
    TH2F * dataDown_et_ee = (TH2F*) data->Get("meeDown_et_ee");
    TH2F * mcDown_et_ee = (TH2F*) mc->Get("meeDown_et_ee");

    float stat_et_bins [7] = {20, 39, 50, 65, 80, 100, 1000};
    TH1F * peakDiff_eb = new TH1F("peakDiff_eb", "", 6, stat_et_bins);
    TH1F * peakDiff_ee = new TH1F("peakDiff_ee", "", 6, stat_et_bins);

    TH1F * peakDiffUp_eb = new TH1F("peakDiffUp_eb", "", 6, stat_et_bins);
    TH1F * peakDiffUp_ee = new TH1F("peakDiffUp_ee", "", 6, stat_et_bins);

    TH1F * peakDiffDown_eb = new TH1F("peakDiffDown_eb", "", 6, stat_et_bins);
    TH1F * peakDiffDown_ee = new TH1F("peakDiffDown_ee", "", 6, stat_et_bins);

    for( int i = 1; i <= data_et_eb->GetNbinsX(); i++){

       TH1F * dataUp_temp_eb = (TH1F*)dataUp_et_eb->ProjectionY("dataUp_temp", i, i,""); 
       TH1F * mcUp_temp_eb = (TH1F*)mcUp_et_eb->ProjectionY("mcUp_temp", i, i,""); 
       RooFitResult * rfr_dataUp = fitBreitGaus(dataUp_temp_eb, true);
       RooFitResult * rfr_mcUp = fitBreitGaus(mcUp_temp_eb, false);
       std::cout << dataUp_temp_eb->GetEntries() << std::endl;
       std::cout << mcUp_temp_eb->GetEntries() << std::endl;

       RooRealVar * dataUp_mean = (RooRealVar*)rfr_dataUp->floatParsFinal().find("mean");
       float dataUp_M = dataUp_mean->getValV()+91.188;
       float dataUp_M_err = dataUp_mean->getAsymErrorHi(); 
       RooRealVar * mcUp_mean = (RooRealVar*)rfr_mcUp->floatParsFinal().find("mean");
       float mcUp_M = mcUp_mean->getValV()+91.188;
       float mcUp_M_err = mcUp_mean->getAsymErrorHi(); 

       peakDiffUp_eb->SetBinContent(i, 100*(dataUp_M-mcUp_M)/mcUp_M);
       peakDiffUp_eb->SetBinError(i, 100*(dataUp_M/mcUp_M)*sqrt( pow(dataUp_M_err/dataUp_M,2)+pow(mcUp_M_err/mcUp_M,2)));
       if( peakDiffUp_eb->GetBinError(i) > peakDiffUp_eb->GetBinContent(i)) peakDiffUp_eb->SetBinError(i, peakDiffUp_eb->GetBinContent(i));

       TH1F * dataDown_temp_eb = (TH1F*)dataDown_et_eb->ProjectionY("dataDown_temp", i, i,""); 
       TH1F * mcDown_temp_eb = (TH1F*)mcDown_et_eb->ProjectionY("mcDown_temp", i, i,""); 
       RooFitResult * rfr_dataDown = fitBreitGaus(dataDown_temp_eb, true);
       RooFitResult * rfr_mcDown = fitBreitGaus(mcDown_temp_eb, false);
       std::cout << dataDown_temp_eb->GetEntries() << std::endl;
       std::cout << mcDown_temp_eb->GetEntries() << std::endl;

       RooRealVar * dataDown_mean = (RooRealVar*)rfr_dataDown->floatParsFinal().find("mean");
       float dataDown_M = dataDown_mean->getValV()+91.188;
       float dataDown_M_err = dataDown_mean->getAsymErrorHi(); 
       RooRealVar * mcDown_mean = (RooRealVar*)rfr_mcDown->floatParsFinal().find("mean");
       float mcDown_M = mcDown_mean->getValV()+91.188;
       float mcDown_M_err = mcDown_mean->getAsymErrorHi(); 

       peakDiffDown_eb->SetBinContent(i, 100*(dataDown_M-mcDown_M)/mcDown_M);
       peakDiffDown_eb->SetBinError(i, 100*(dataDown_M/mcDown_M)*sqrt( pow(dataDown_M_err/dataDown_M,2)+pow(mcDown_M_err/mcDown_M,2)));
       if( peakDiffDown_eb->GetBinError(i) > peakDiffDown_eb->GetBinContent(i)) peakDiffDown_eb->SetBinError(i, peakDiffDown_eb->GetBinContent(i));

       TH1F * data_temp_eb = (TH1F*)data_et_eb->ProjectionY("data_temp", i, i,""); 
       TH1F * mc_temp_eb = (TH1F*)mc_et_eb->ProjectionY("mc_temp", i, i,""); 
       RooFitResult * rfr_data = fitBreitGaus(data_temp_eb, true);
       RooFitResult * rfr_mc = fitBreitGaus(mc_temp_eb, false);
       std::cout << data_temp_eb->GetEntries() << std::endl;
       std::cout << mc_temp_eb->GetEntries() << std::endl;

       RooRealVar * data_mean = (RooRealVar*)rfr_data->floatParsFinal().find("mean");
       float data_M = data_mean->getValV()+91.188;
       float data_M_err = data_mean->getAsymErrorHi(); 
       RooRealVar * mc_mean = (RooRealVar*)rfr_mc->floatParsFinal().find("mean");
       float mc_M = mc_mean->getValV()+91.188;
       float mc_M_err = mc_mean->getAsymErrorHi(); 

       peakDiff_eb->SetBinContent(i, 100*(data_M-mc_M)/mc_M);
       peakDiff_eb->SetBinError(i, 100*(data_M/mc_M)*sqrt( pow(data_M_err/data_M,2)+pow(mc_M_err/mc_M,2)));
       peakDiff_eb->SetBinError(i, sqrt(pow(peakDiff_eb->GetBinContent(i),2)+pow(max(abs(peakDiff_eb->GetBinContent(i) - peakDiffUp_eb->GetBinContent(i)),abs(peakDiff_eb->GetBinContent(i) - peakDiffDown_eb->GetBinContent(i))),2)));
       if( peakDiff_eb->GetBinError(i) > peakDiff_eb->GetBinContent(i)) peakDiff_eb->SetBinError(i, peakDiff_eb->GetBinContent(i));

       TH1F * data_temp_ee = (TH1F*)data_et_ee->ProjectionY("data_temp_ee", i, i,""); 
       TH1F * mc_temp_ee = (TH1F*)mc_et_ee->ProjectionY("mc_temp_ee", i, i,""); 
       rfr_data = fitBreitGaus(data_temp_ee, true);
       rfr_mc = fitBreitGaus(mc_temp_ee, false);

       data_mean = (RooRealVar*)rfr_data->floatParsFinal().find("mean");
       data_M = data_mean->getValV()+91.188;
       data_M_err = data_mean->getAsymErrorHi(); 
       mc_mean = (RooRealVar*)rfr_mc->floatParsFinal().find("mean");
       mc_M = mc_mean->getValV()+91.188;
       mc_M_err = mc_mean->getAsymErrorHi(); 
       std::cout << i << " " << data_M << " " << data_M_err << std::endl;
       std::cout << i << " " << mc_M << " " << mc_M_err << std::endl;

       peakDiff_ee->SetBinContent(i, 100*(data_M-mc_M)/mc_M);
       peakDiff_ee->SetBinError(i, 100*(data_M/mc_M)*sqrt( pow(data_M_err/data_M,2)+pow(mc_M_err/mc_M,2)));
       if( peakDiff_ee->GetBinError(i) > peakDiff_ee->GetBinContent(i)) peakDiff_ee->SetBinError(i, peakDiff_ee->GetBinContent(i));

    }

    TCanvas * c = new TCanvas("c", "", 900, 900);
    gStyle->SetOptStat(0);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    gPad->SetTopMargin(0.091);
    gPad->SetLeftMargin(0.13);
    gPad->SetRightMargin(0.03);

    peakDiff_eb->SetXTitle("Electron E_{T} [GeV]");
    peakDiff_eb->SetYTitle("( m_{peak}^{data} - m_{peak}^{mc} ) / mc_{peak}^{mc} [%]");

    peakDiff_eb->SetLineColor(kBlack);
    peakDiff_eb->SetLineWidth(3);
    peakDiff_eb->SetMarkerStyle(kFullCircle);
    peakDiff_eb->SetMarkerSize(1.2);
    peakDiff_eb->SetMarkerColor(kBlack);

    peakDiff_eb->SetAxisRange(20, 99, "X");

    peakDiff_eb->Draw("pe");

    TLatex* pave = new TLatex(0.13, 0.92, "#font[42]{#bf{CMS} #it{Preliminary}}");
    TLatex* lumi_label = new TLatex(0.97, 0.92, lumiLabel.c_str());

    pave->SetTextSize(0.035);
    lumi_label->SetTextSize(0.035);
    lumi_label->SetTextAlign(31);
    pave->SetNDC();
    pave->Draw();
    lumi_label->SetNDC();
    lumi_label->Draw();

    c->Modified();
    string macro_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_macros/";
    string macro_post = ".C";
    string pdf_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pdfs/";
    string pdf_post = ".pdf";
    string png_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pngs/";
    string png_post = ".png";
    string title = macro_pre + outputFile + "_peakDiff_EB" + macro_post;
    c->SaveAs(title.c_str());
    title = pdf_pre + outputFile+"_peakDiff_EB" + pdf_post;
    c->SaveAs(title.c_str());
    title = png_pre + outputFile+"_peakDiff_EB" + png_post;
    c->SaveAs(title.c_str(), "");

    peakDiff_ee->SetXTitle("Electron E_{T} [GeV]");
    peakDiff_ee->SetYTitle("( m_{peak}^{data} - m_{peak}^{mc} ) / mc_{peak}^{mc} [%]");

    peakDiff_ee->SetLineColor(kBlack);
    peakDiff_ee->SetLineWidth(3);
    peakDiff_ee->SetMarkerStyle(kFullCircle);
    peakDiff_ee->SetMarkerSize(1.2);
    peakDiff_ee->SetMarkerColor(kBlack);

    peakDiff_ee->SetAxisRange(20, 99, "X");

    peakDiff_ee->Draw("pe");

    pave->SetTextSize(0.035);
    lumi_label->SetTextSize(0.035);
    lumi_label->SetTextAlign(31);
    pave->SetNDC();
    pave->Draw();
    lumi_label->SetNDC();
    lumi_label->Draw();

    c->Modified();
    macro_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_macros/";
    macro_post = ".C";
    pdf_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pdfs/";
    pdf_post = ".pdf";
    png_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pngs/";
    png_post = ".png";
    title = macro_pre + outputFile + "_peakDiff_EE" + macro_post;
    c->SaveAs(title.c_str());
    title = pdf_pre + outputFile+"_peakDiff_EE" + pdf_post;
    c->SaveAs(title.c_str());
    title = png_pre + outputFile+"_peakDiff_EE" + png_post;
    c->SaveAs(title.c_str(), "");

    return c;
}

TCanvas * standardSet::dataData(std::string _str_data, std::string _str_mc){

    TFile * data = new TFile(_str_data.c_str(), "READ");
    TFile * mc = new TFile(_str_mc.c_str(), "READ");

    TH1F * data_EbEb = (TH1F*)data->Get("invMass_ele_EbEb_high");
    TH1F * data_NotEbEb = (TH1F*)data->Get("invMass_ele_NotEbEb");
    TH1F * data_gain6_EB = (TH1F*)data->Get("invMass_gain6_EB");
    TH1F * data_gain6_EE = (TH1F*)data->Get("invMass_gain6_EE");
    TH1F * data_gain1_EB = (TH1F*)data->Get("invMass_gain1_EB");
    TH1F * data_gain1_EE = (TH1F*)data->Get("invMass_gain1_EE");

    TH1F * data_pt0 = (TH1F*)data->Get("pt_40_55");
    TH1F * data_pt1 = (TH1F*)data->Get("pt_55_65");
    TH1F * data_ptDiag1 = (TH1F*)data->Get("ptDiag_55_65");
    TH1F * data_pt2 = (TH1F*)data->Get("pt_65_90");
    TH1F * data_pt3 = (TH1F*)data->Get("pt_90_inf");

    TH1F * mc_EbEb = (TH1F*)mc->Get("invMass_ele_EbEb_high");
    TH1F * mc_NotEbEb = (TH1F*)mc->Get("invMass_ele_NotEbEb");

    TH1F * mc_gain6_EB = (TH1F*)mc->Get("invMass_gain6_EB");
    TH1F * mc_gain6_EE = (TH1F*)mc->Get("invMass_gain6_EE");
    TH1F * mc_gain1_EB = (TH1F*)mc->Get("invMass_gain1_EB");
    TH1F * mc_gain1_EE = (TH1F*)mc->Get("invMass_gain1_EE");

    TH1F * mc_pt0 = (TH1F*)mc->Get("pt_40_55");
    TH1F * mc_pt1 = (TH1F*)mc->Get("pt_55_65");
    TH1F * mc_ptDiag1 = (TH1F*)mc->Get("ptDiag_55_65");
    TH1F * mc_pt2 = (TH1F*)mc->Get("pt_65_90");
    TH1F * mc_pt3 = (TH1F*)mc->Get("pt_90_inf");

    TH1F * data_ebeb = (TH1F*)data->Get("invMass_EBEB");
    TH1F * data_eeee = (TH1F*)data->Get("invMass_EEEE");
    TH1F * mc_ebeb = (TH1F*)mc->Get("invMass_EBEB");
    TH1F * mc_eeee = (TH1F*)mc->Get("invMass_EEEE");

    mc_EbEb->Scale(data_EbEb->Integral()/mc_EbEb->Integral());
    mc_gain6_EB->Scale(data_gain6_EB->Integral()/mc_gain6_EB->Integral());
    mc_gain6_EE->Scale(data_gain6_EE->Integral()/mc_gain6_EE->Integral());
    mc_gain1_EB->Scale(data_gain1_EB->Integral()/mc_gain1_EB->Integral());
    mc_gain1_EE->Scale(data_gain1_EE->Integral()/mc_gain1_EE->Integral());
    mc_NotEbEb->Scale(data_NotEbEb->Integral()/mc_NotEbEb->Integral());
    mc_pt0->Scale(data_pt0->Integral()/mc_pt0->Integral());
    mc_pt1->Scale(data_pt1->Integral()/mc_pt1->Integral());
    mc_ptDiag1->Scale(data_ptDiag1->Integral()/mc_ptDiag1->Integral());
    mc_pt2->Scale(data_pt2->Integral()/mc_pt2->Integral());
    mc_pt3->Scale(data_pt3->Integral()/mc_pt3->Integral());
    mc_ebeb->Scale(data_ebeb->Integral()/mc_ebeb->Integral());
    mc_eeee->Scale(data_eeee->Integral()/mc_eeee->Integral());

    data_pt0->Rebin(15, "", 0);
    data_pt1->Rebin(15, "", 0);
    data_ptDiag1->Rebin(15, "", 0);
    data_pt2->Rebin(15, "", 0);
    data_pt3->Rebin(15, "", 0);
    data_EbEb->Rebin(15, "", 0);
    data_gain6_EB->Rebin(15, "", 0);
    data_gain6_EE->Rebin(15, "", 0);
    data_gain1_EB->Rebin(15, "", 0);
    data_gain1_EE->Rebin(15, "", 0);
    data_NotEbEb->Rebin(15, "", 0);
    data_ebeb->Rebin(15,"",0);
    data_eeee->Rebin(15,"",0);
    mc_pt0->Rebin(15, "", 0);
    mc_pt1->Rebin(15, "", 0);
    mc_ptDiag1->Rebin(15, "", 0);
    mc_pt2->Rebin(15, "", 0);
    mc_pt3->Rebin(15, "", 0);
    mc_EbEb->Rebin(15, "", 0);
    mc_NotEbEb->Rebin(15, "", 0);
    mc_ebeb->Rebin(15,"",0);
    mc_eeee->Rebin(15,"",0);

    ofstream out;
    out.open(string("chiSquared/chiSquared_"+outputFile+".txt").c_str(), std::ofstream::app);
    out << "Barrel-Barrel           " << util.EvaluateChiSquare(data_ebeb, mc_ebeb, 80, 100) << std::endl;
    out << "Barrel-Barrel, High R9  " << util.EvaluateChiSquare(data_EbEb, mc_EbEb, 80, 100) << std::endl;
    out << "Barrel-!Barrel, High R9 " << util.EvaluateChiSquare(data_NotEbEb, mc_NotEbEb, 80, 100) << std::endl;
    out << "Endcap-Endcap           " << util.EvaluateChiSquare(data_eeee, mc_eeee, 80, 100) << std::endl;
    out << "40 < pt < 55            " << util.EvaluateChiSquare(data_pt0, mc_pt0, 80, 100) <<  std::endl;
    out << "55 < pt < 65            " << util.EvaluateChiSquare(data_pt1, mc_pt1, 80, 100) <<  std::endl;
    out << "65 < pt < 90            " << util.EvaluateChiSquare(data_pt2, mc_pt2, 80, 100) <<  std::endl;
    out << "90 > pt                 " << util.EvaluateChiSquare(data_pt0, mc_pt0, 80, 100) <<  std::endl;
    out.close();

    //plot(data_pt, mc_pt, total_pt);   
    std::cout << dataTitle << " " << mcTitle << std::endl;
    util.dataDataPlot_wRatio(data_EbEb, mc_EbEb, outputFile, 0, dataTitle, mcTitle);   
    util.dataDataPlot_wRatio(data_NotEbEb, mc_NotEbEb, outputFile, 2, dataTitle, mcTitle);   
    util.dataDataPlot_wRatio(data_ebeb, mc_ebeb, outputFile, 3, dataTitle, mcTitle);   
    util.dataDataPlot_wRatio(data_eeee, mc_eeee, outputFile, 4, dataTitle, mcTitle);   
    util.dataDataPlot_wRatio(data_pt0, mc_pt0, outputFile, 5, dataTitle, mcTitle);   
    util.dataDataPlot_wRatio(data_pt1, mc_pt1, outputFile, 1, dataTitle, mcTitle);   
    util.dataDataPlot_wRatio(data_ptDiag1, mc_ptDiag1, outputFile, 7, dataTitle, mcTitle);   
    util.dataDataPlot_wRatio(data_pt2, mc_pt2, outputFile, 6, dataTitle, mcTitle);   
    return util.dataDataPlot_wRatio(data_pt3, mc_pt3, outputFile, 8, dataTitle, mcTitle);   

};

TCanvas * standardSet::moneyPlots(std::string _str_data, std::string _str_mc){

    TFile * data = new TFile(_str_data.c_str(), "READ");
    TFile * mc = new TFile(_str_mc.c_str(), "READ");

    TH1F * data_EbEb = (TH1F*)data->Get("invMass_ele_EbEb_high");
    TH1F * data_EbEb_up = (TH1F*)data->Get("invMassUp_ele_EbEb_high");
    TH1F * data_EbEb_down = (TH1F*)data->Get("invMassDown_ele_EbEb_high");
    TH1F * data_NotEbEb = (TH1F*)data->Get("invMass_ele_NotEbEb");
    TH1F * data_NotEbEb_up = (TH1F*)data->Get("invMassUp_ele_NotEbEb");
    TH1F * data_NotEbEb_down = (TH1F*)data->Get("invMassDown_ele_NotEbEb");

    TH1F * data_gain6_EB = (TH1F*)data->Get("invMass_gain6_EB");
    TH1F * data_gain6_EB_up = (TH1F*)data->Get("invMassUp_gain6_EB");
    TH1F * data_gain6_EB_down = (TH1F*)data->Get("invMassDown_gain6_EB");
    TH1F * data_gain6_EE = (TH1F*)data->Get("invMass_gain6_EE");
    TH1F * data_gain6_EE_up = (TH1F*)data->Get("invMassUp_gain6_EE");
    TH1F * data_gain6_EE_down = (TH1F*)data->Get("invMassDown_gain6_EE");
    TH1F * data_gain1_EB = (TH1F*)data->Get("invMass_gain1_EB");
    TH1F * data_gain1_EB_up = (TH1F*)data->Get("invMassUp_gain1_EB");
    TH1F * data_gain1_EB_down = (TH1F*)data->Get("invMassDown_gain1_EB");
    TH1F * data_gain1_EE = (TH1F*)data->Get("invMass_gain1_EE");
    TH1F * data_gain1_EE_up = (TH1F*)data->Get("invMassUp_gain1_EE");
    TH1F * data_gain1_EE_down = (TH1F*)data->Get("invMassDown_gain1_EE");

    TH1F * data_EbEb_ptz200 = (TH1F*)data->Get("invMass_EbEb_ptz200");
    TH1F * data_EbEb_ptz200_up = (TH1F*)data->Get("invMassUp_EbEb_ptz200");
    TH1F * data_EbEb_ptz200_down = (TH1F*)data->Get("invMassDown_EbEb_ptz200");
    TH1F * data_EeEe_ptz200 = (TH1F*)data->Get("invMass_EeEe_ptz200");
    TH1F * data_EeEe_ptz200_up = (TH1F*)data->Get("invMassUp_EeEe_ptz200");
    TH1F * data_EeEe_ptz200_down = (TH1F*)data->Get("invMassDown_EeEe_ptz200");
    TH1F * mc_EbEb_ptz200 = (TH1F*)mc->Get("invMass_EbEb_ptz200");
    TH1F * mc_EbEb_ptz200_up = (TH1F*)mc->Get("invMassUp_EbEb_ptz200");
    TH1F * mc_EbEb_ptz200_down = (TH1F*)mc->Get("invMassDown_EbEb_ptz200");
    TH1F * mc_EeEe_ptz200 = (TH1F*)mc->Get("invMass_EeEe_ptz200");
    TH1F * mc_EeEe_ptz200_up = (TH1F*)mc->Get("invMassUp_EeEe_ptz200");
    TH1F * mc_EeEe_ptz200_down = (TH1F*)mc->Get("invMassDown_EeEe_ptz200");

    TH1F * data_pt0 = (TH1F*)data->Get("pt_40_55");
    TH1F * data_pt0_up = (TH1F*)data->Get("pt_40_55_up");
    TH1F * data_pt0_down = (TH1F*)data->Get("pt_40_55_down");
    TH1F * data_pt1 = (TH1F*)data->Get("pt_55_65");
    TH1F * data_pt1_up = (TH1F*)data->Get("pt_55_65_up");
    TH1F * data_pt1_down = (TH1F*)data->Get("pt_55_65_down");
    TH1F * data_ptDiag1 = (TH1F*)data->Get("ptDiag_55_65");
    TH1F * data_ptDiag1_up = (TH1F*)data->Get("ptDiag_55_65_up");
    TH1F * data_ptDiag1_down = (TH1F*)data->Get("ptDiag_55_65_down");
    TH1F * data_pt2 = (TH1F*)data->Get("pt_65_90");
    TH1F * data_pt2_up = (TH1F*)data->Get("pt_65_90_up");
    TH1F * data_pt2_down = (TH1F*)data->Get("pt_65_90_down");
    TH1F * data_pt3 = (TH1F*)data->Get("pt_90_inf");
    TH1F * data_pt3_up = (TH1F*)data->Get("pt_90_inf_up");
    TH1F * data_pt3_down = (TH1F*)data->Get("pt_90_inf_down");

    TH1F * mc_EbEb = (TH1F*)mc->Get("invMass_ele_EbEb_high");
    TH1F * mc_EbEb_up = (TH1F*)mc->Get("invMassUp_ele_EbEb_high");
    TH1F * mc_EbEb_down = (TH1F*)mc->Get("invMassDown_ele_EbEb_high");
    TH1F * mc_NotEbEb = (TH1F*)mc->Get("invMass_ele_NotEbEb");
    TH1F * mc_NotEbEb_up = (TH1F*)mc->Get("invMassUp_ele_NotEbEb");
    TH1F * mc_NotEbEb_down = (TH1F*)mc->Get("invMassDown_ele_NotEbEb");

    TH1F * mc_gain6_EB = (TH1F*)mc->Get("invMass_gain6_EB");
    TH1F * mc_gain6_EB_up = (TH1F*)mc->Get("invMassUp_gain6_EB");
    TH1F * mc_gain6_EB_down = (TH1F*)mc->Get("invMassDown_gain6_EB");
    TH1F * mc_gain6_EE = (TH1F*)mc->Get("invMass_gain6_EE");
    TH1F * mc_gain6_EE_up = (TH1F*)mc->Get("invMassUp_gain6_EE");
    TH1F * mc_gain6_EE_down = (TH1F*)mc->Get("invMassDown_gain6_EE");
    TH1F * mc_gain1_EB = (TH1F*)mc->Get("invMass_gain1_EB");
    TH1F * mc_gain1_EB_up = (TH1F*)mc->Get("invMassUp_gain1_EB");
    TH1F * mc_gain1_EB_down = (TH1F*)mc->Get("invMassDown_gain1_EB");
    TH1F * mc_gain1_EE = (TH1F*)mc->Get("invMass_gain1_EE");
    TH1F * mc_gain1_EE_up = (TH1F*)mc->Get("invMassUp_gain1_EE");
    TH1F * mc_gain1_EE_down = (TH1F*)mc->Get("invMassDown_gain1_EE");

    TH1F * mc_pt0 = (TH1F*)mc->Get("pt_40_55");
    TH1F * mc_pt0_up = (TH1F*)mc->Get("pt_40_55_up");
    TH1F * mc_pt0_down = (TH1F*)mc->Get("pt_40_55_down");
    TH1F * mc_pt1 = (TH1F*)mc->Get("pt_55_65");
    TH1F * mc_pt1_up = (TH1F*)mc->Get("pt_55_65_up");
    TH1F * mc_pt1_down = (TH1F*)mc->Get("pt_55_65_down");
    TH1F * mc_ptDiag1 = (TH1F*)mc->Get("ptDiag_55_65");
    TH1F * mc_ptDiag1_up = (TH1F*)mc->Get("ptDiag_55_65_up");
    TH1F * mc_ptDiag1_down = (TH1F*)mc->Get("ptDiag_55_65_down");
    TH1F * mc_pt2 = (TH1F*)mc->Get("pt_65_90");
    TH1F * mc_pt2_up = (TH1F*)mc->Get("pt_65_90_up");
    TH1F * mc_pt2_down = (TH1F*)mc->Get("pt_65_90_down");
    TH1F * mc_pt3 = (TH1F*)mc->Get("pt_90_inf");
    TH1F * mc_pt3_up = (TH1F*)mc->Get("pt_90_inf_up");
    TH1F * mc_pt3_down = (TH1F*)mc->Get("pt_90_inf_down");

    TH1F * data_ebeb = (TH1F*)data->Get("invMass_EBEB");
    TH1F * data_ebeb_up = (TH1F*)data->Get("invMassUp_EBEB");
    TH1F * data_ebeb_down = (TH1F*)data->Get("invMassDown_EBEB");

    TH1F * data_eeee = (TH1F*)data->Get("invMass_EEEE");
    TH1F * data_eeee_up = (TH1F*)data->Get("invMassUp_EEEE");
    TH1F * data_eeee_down = (TH1F*)data->Get("invMassDown_EEEE");

    TH1F * mc_ebeb = (TH1F*)mc->Get("invMass_EBEB");
    TH1F * mc_ebeb_up = (TH1F*)mc->Get("invMassUp_EBEB");
    TH1F * mc_ebeb_down = (TH1F*)mc->Get("invMassDown_EBEB");

    TH1F * mc_eeee = (TH1F*)mc->Get("invMass_EEEE");
    TH1F * mc_eeee_up = (TH1F*)mc->Get("invMassUp_EEEE");
    TH1F * mc_eeee_down = (TH1F*)mc->Get("invMassDown_EEEE");

    mc_EbEb->Scale(data_EbEb->Integral()/mc_EbEb->Integral());
    mc_EbEb_up->Scale(data_EbEb_up->Integral()/mc_EbEb_up->Integral());
    mc_EbEb_down->Scale(data_EbEb_down->Integral()/mc_EbEb_down->Integral());

    mc_gain6_EB->Scale(data_gain6_EB->Integral()/mc_gain6_EB->Integral());
    mc_gain6_EB_up->Scale(data_gain6_EB_up->Integral()/mc_gain6_EB_up->Integral());
    mc_gain6_EB_down->Scale(data_gain6_EB_down->Integral()/mc_gain6_EB_down->Integral());
    mc_gain6_EE->Scale(data_gain6_EE->Integral()/mc_gain6_EE->Integral());
    mc_gain6_EE_up->Scale(data_gain6_EE_up->Integral()/mc_gain6_EE_up->Integral());
    mc_gain6_EE_down->Scale(data_gain6_EE_down->Integral()/mc_gain6_EE_down->Integral());

    mc_EbEb_ptz200->Scale(data_EbEb_ptz200->Integral()/mc_EbEb_ptz200->Integral());
    mc_EbEb_ptz200_up->Scale(data_EbEb_ptz200_up->Integral()/mc_EbEb_ptz200_up->Integral());
    mc_EbEb_ptz200_down->Scale(data_EbEb_ptz200_down->Integral()/mc_EbEb_ptz200_down->Integral());
    mc_EeEe_ptz200->Scale(data_EeEe_ptz200->Integral()/mc_EeEe_ptz200->Integral());
    mc_EeEe_ptz200_up->Scale(data_EeEe_ptz200_up->Integral()/mc_EeEe_ptz200_up->Integral());
    mc_EeEe_ptz200_down->Scale(data_EeEe_ptz200_down->Integral()/mc_EeEe_ptz200_down->Integral());

    mc_gain1_EB->Scale(data_gain1_EB->Integral()/mc_gain1_EB->Integral());
    mc_gain1_EB_up->Scale(data_gain1_EB_up->Integral()/mc_gain1_EB_up->Integral());
    mc_gain1_EB_down->Scale(data_gain1_EB_down->Integral()/mc_gain1_EB_down->Integral());
    mc_gain1_EE->Scale(data_gain1_EE->Integral()/mc_gain1_EE->Integral());
    mc_gain1_EE_up->Scale(data_gain1_EE_up->Integral()/mc_gain1_EE_up->Integral());
    mc_gain1_EE_down->Scale(data_gain1_EE_down->Integral()/mc_gain1_EE_down->Integral());

    mc_NotEbEb->Scale(data_NotEbEb->Integral()/mc_NotEbEb->Integral());
    mc_NotEbEb_up->Scale(data_NotEbEb_up->Integral()/mc_NotEbEb_up->Integral());
    mc_NotEbEb_down->Scale(data_NotEbEb_down->Integral()/mc_NotEbEb_down->Integral());
    
    mc_pt0->Scale(data_pt0->Integral()/mc_pt0->Integral());
    mc_pt0_up->Scale(data_pt0_up->Integral()/mc_pt0_up->Integral());
    mc_pt0_down->Scale(data_pt0_down->Integral()/mc_pt0_down->Integral());
    mc_pt1->Scale(data_pt1->Integral()/mc_pt1->Integral());
    mc_pt1_up->Scale(data_pt1_up->Integral()/mc_pt1_up->Integral());
    mc_pt1_down->Scale(data_pt1_down->Integral()/mc_pt1_down->Integral());
    mc_ptDiag1->Scale(data_ptDiag1->Integral()/mc_ptDiag1->Integral());
    mc_ptDiag1_up->Scale(data_ptDiag1_up->Integral()/mc_ptDiag1_up->Integral());
    mc_ptDiag1_down->Scale(data_ptDiag1_down->Integral()/mc_ptDiag1_down->Integral());
    mc_pt2->Scale(data_pt2->Integral()/mc_pt2->Integral());
    mc_pt2_up->Scale(data_pt2_up->Integral()/mc_pt2_up->Integral());
    mc_pt2_down->Scale(data_pt2_down->Integral()/mc_pt2_down->Integral());
    mc_pt3->Scale(data_pt3->Integral()/mc_pt3->Integral());
    mc_pt3_up->Scale(data_pt3_up->Integral()/mc_pt3_up->Integral());
    mc_pt3_down->Scale(data_pt3_down->Integral()/mc_pt3_down->Integral());

    mc_ebeb->Scale(data_ebeb->Integral()/mc_ebeb->Integral());
    mc_ebeb_up->Scale(data_ebeb_up->Integral()/mc_ebeb_up->Integral());
    mc_ebeb_down->Scale(data_ebeb_down->Integral()/mc_ebeb_down->Integral());

    mc_eeee->Scale(data_eeee->Integral()/mc_eeee->Integral());
    mc_eeee_up->Scale(data_eeee_up->Integral()/mc_eeee_up->Integral());
    mc_eeee_down->Scale(data_eeee_down->Integral()/mc_eeee_down->Integral());

    data_pt0->Rebin(15, "", 0);
    data_pt0_up->Rebin(15, "", 0);
    data_pt0_down->Rebin(15, "", 0);
    data_pt1->Rebin(15, "", 0);
    data_pt1_up->Rebin(15, "", 0);
    data_pt1_down->Rebin(15, "", 0);
    data_ptDiag1->Rebin(15, "", 0);
    data_ptDiag1_up->Rebin(15, "", 0);
    data_ptDiag1_down->Rebin(15, "", 0);
    data_pt2->Rebin(15, "", 0);
    data_pt2_up->Rebin(15, "", 0);
    data_pt2_down->Rebin(15, "", 0);
    data_pt3->Rebin(15, "", 0);
    data_pt3_up->Rebin(15, "", 0);
    data_pt3_down->Rebin(15, "", 0);

    data_EbEb->Rebin(15, "", 0);
    data_EbEb_up->Rebin(15, "", 0);
    data_EbEb_down->Rebin(15, "", 0);

    data_gain6_EB->Rebin(15, "", 0);
    data_gain6_EB_up->Rebin(15, "", 0);
    data_gain6_EB_down->Rebin(15, "", 0);
    data_gain6_EE->Rebin(15, "", 0);
    data_gain6_EE_up->Rebin(15, "", 0);
    data_gain6_EE_down->Rebin(15, "", 0);

    data_EbEb_ptz200->Rebin(15, "", 0);
    data_EbEb_ptz200_up->Rebin(15, "", 0);
    data_EbEb_ptz200_down->Rebin(15, "", 0);
    data_EeEe_ptz200->Rebin(15, "", 0);
    data_EeEe_ptz200_up->Rebin(15, "", 0);
    data_EeEe_ptz200_down->Rebin(15, "", 0);
    mc_EbEb_ptz200->Rebin(15, "", 0);
    mc_EbEb_ptz200_up->Rebin(15, "", 0);
    mc_EbEb_ptz200_down->Rebin(15, "", 0);
    mc_EeEe_ptz200->Rebin(15, "", 0);
    mc_EeEe_ptz200_up->Rebin(15, "", 0);
    mc_EeEe_ptz200_down->Rebin(15, "", 0);

    data_gain1_EB->Rebin(15, "", 0);
    data_gain1_EB_up->Rebin(15, "", 0);
    data_gain1_EB_down->Rebin(15, "", 0);
    data_gain1_EE->Rebin(15, "", 0);
    data_gain1_EE_up->Rebin(15, "", 0);
    data_gain1_EE_down->Rebin(15, "", 0);

    data_NotEbEb->Rebin(15, "", 0);
    data_NotEbEb_up->Rebin(15, "", 0);
    data_NotEbEb_down->Rebin(15, "", 0);

    data_ebeb->Rebin(15,"",0);
    data_ebeb_up->Rebin(15,"",0);
    data_ebeb_down->Rebin(15,"",0);

    data_eeee->Rebin(15,"",0);
    data_eeee_up->Rebin(15,"",0);
    data_eeee_down->Rebin(15,"",0);

    mc_pt0->Rebin(15, "", 0);
    mc_pt0_up->Rebin(15, "", 0);
    mc_pt0_down->Rebin(15, "", 0);
    mc_pt1->Rebin(15, "", 0);
    mc_pt1_up->Rebin(15, "", 0);
    mc_pt1_down->Rebin(15, "", 0);
    mc_ptDiag1->Rebin(15, "", 0);
    mc_ptDiag1_up->Rebin(15, "", 0);
    mc_ptDiag1_down->Rebin(15, "", 0);
    mc_pt2->Rebin(15, "", 0);
    mc_pt2_up->Rebin(15, "", 0);
    mc_pt2_down->Rebin(15, "", 0);
    mc_pt3->Rebin(15, "", 0);
    mc_pt3_up->Rebin(15, "", 0);
    mc_pt3_down->Rebin(15, "", 0);

    mc_EbEb->Rebin(15, "", 0);
    mc_EbEb_up->Rebin(15, "", 0);
    mc_EbEb_down->Rebin(15, "", 0);

    mc_NotEbEb->Rebin(15, "", 0);
    mc_NotEbEb_up->Rebin(15, "", 0);
    mc_NotEbEb_down->Rebin(15, "", 0);

    mc_ebeb->Rebin(15,"",0);
    mc_ebeb_up->Rebin(15,"",0);
    mc_ebeb_down->Rebin(15,"",0);

    mc_eeee->Rebin(15,"",0);
    mc_eeee_up->Rebin(15,"",0);
    mc_eeee_down->Rebin(15,"",0);

    TH1F * syst_pt0 = new TH1F("systData_pt0", "", mc_pt0->GetNbinsX(), 55, 65);
    TH1F * syst_pt1 = new TH1F("systData_pt1", "", mc_pt1->GetNbinsX(), 55, 65);
    TH1F * syst_ptDiag1 = new TH1F("systData_ptDiag1", "", mc_ptDiag1->GetNbinsX(), 55, 65);
    TH1F * syst_pt2 = new TH1F("systData_pt2", "", mc_pt2->GetNbinsX(), 55, 65);
    TH1F * syst_pt3 = new TH1F("systData_pt3", "", mc_pt3->GetNbinsX(), 55, 65);
    TH1F * syst_EB = new TH1F("systData_EB", "", mc_EbEb->GetNbinsX(), 80, 100);
    TH1F * syst_NotEB = new TH1F("systData_NotEB", "", mc_NotEbEb->GetNbinsX(), 80, 100);
    TH1F * syst_ebeb = new TH1F("systData_ebeb", "", mc_ebeb->GetNbinsX(), 80, 100);
    TH1F * syst_eeee = new TH1F("systData_eeee", "", mc_eeee->GetNbinsX(), 80, 100);
    TH1F * syst_gain6_EB = new TH1F("systData_gain6_EB", "", mc_gain6_EB->GetNbinsX(), 80, 100);
    TH1F * syst_gain6_EE = new TH1F("systData_gain6_EE", "", mc_gain6_EE->GetNbinsX(), 80, 100);
    TH1F * syst_gain1_EB = new TH1F("systData_gain1_EB", "", mc_gain1_EB->GetNbinsX(), 80, 100);
    TH1F * syst_gain1_EE = new TH1F("systData_gain1_EE", "", mc_gain1_EE->GetNbinsX(), 80, 100);
    TH1F * syst_EbEb_ptz200 = new TH1F("systData_EbEb_ptz200", "", mc_EbEb_ptz200->GetNbinsX(), 80, 100);
    TH1F * syst_EeEe_ptz200 = new TH1F("systData_EeEe_ptz200", "", mc_EeEe_ptz200->GetNbinsX(), 80, 100);

    for(int i = 0; i < syst_EB->GetNbinsX(); i++){
        syst_EB->SetBinContent(i+1, sqrt( pow( max(fabs(data_EbEb_up->GetBinContent(i+1) - data_EbEb->GetBinContent(i+1)), fabs(data_EbEb_down->GetBinContent(i+1) - data_EbEb->GetBinContent(i+1))) ,2) + pow( max( fabs( mc_EbEb_up->GetBinContent(i+1) - mc_EbEb->GetBinContent(i+1)), fabs( mc_EbEb_down->GetBinContent(i+1) - mc_EbEb->GetBinContent(i+1)))  ,2)));
        syst_ebeb->SetBinContent(i+1, sqrt( pow( max(fabs(data_ebeb_up->GetBinContent(i+1) - data_ebeb->GetBinContent(i+1)), fabs(data_ebeb_down->GetBinContent(i+1) - data_ebeb->GetBinContent(i+1))) ,2) + pow( max( fabs( mc_ebeb_up->GetBinContent(i+1) - mc_ebeb->GetBinContent(i+1)), fabs( mc_ebeb_down->GetBinContent(i+1) - mc_ebeb->GetBinContent(i+1)))  ,2)));
        syst_eeee->SetBinContent(i+1, sqrt( pow( max(fabs(data_eeee_up->GetBinContent(i+1) - data_eeee->GetBinContent(i+1)), fabs(data_eeee_down->GetBinContent(i+1) - data_eeee->GetBinContent(i+1))) ,2) + pow( max( fabs( mc_eeee_up->GetBinContent(i+1) - mc_eeee->GetBinContent(i+1)), fabs( mc_eeee_down->GetBinContent(i+1) - mc_eeee->GetBinContent(i+1)))  ,2)));

        syst_NotEB->SetBinContent(i+1, sqrt( pow( max(fabs(data_NotEbEb_up->GetBinContent(i+1) - data_NotEbEb->GetBinContent(i+1)), fabs(data_NotEbEb_down->GetBinContent(i+1) - data_NotEbEb->GetBinContent(i+1))) ,2) + pow( max( fabs( mc_NotEbEb_up->GetBinContent(i+1) - mc_NotEbEb->GetBinContent(i+1)), fabs( mc_NotEbEb_down->GetBinContent(i+1) - mc_NotEbEb->GetBinContent(i+1)))  ,2)));

        syst_pt0->SetBinContent(i+1, sqrt( pow( max(fabs(data_pt0->GetBinContent(i+1) - data_pt0_up->GetBinContent(i+1)), fabs(data_pt0->GetBinContent(i+1) - data_pt0_down->GetBinContent(i+1))), 2) + pow( max(fabs(mc_pt0->GetBinContent(i+1) - mc_pt0_up->GetBinContent(i+1)), fabs(mc_pt0->GetBinContent(i+1) - mc_pt0_down->GetBinContent(i+1))), 2)));
        syst_pt1->SetBinContent(i+1, sqrt( pow( max(fabs(data_pt1->GetBinContent(i+1) - data_pt1_up->GetBinContent(i+1)), fabs(data_pt1->GetBinContent(i+1) - data_pt1_down->GetBinContent(i+1))), 2) + pow( max(fabs(mc_pt1->GetBinContent(i+1) - mc_pt1_up->GetBinContent(i+1)), fabs(mc_pt1->GetBinContent(i+1) - mc_pt1_down->GetBinContent(i+1))), 2)));
        syst_ptDiag1->SetBinContent(i+1, sqrt( pow( max(fabs(data_ptDiag1->GetBinContent(i+1) - data_ptDiag1_up->GetBinContent(i+1)), fabs(data_ptDiag1->GetBinContent(i+1) - data_ptDiag1_down->GetBinContent(i+1))), 2) + pow( max(fabs(mc_ptDiag1->GetBinContent(i+1) - mc_ptDiag1_up->GetBinContent(i+1)), fabs(mc_ptDiag1->GetBinContent(i+1) - mc_ptDiag1_down->GetBinContent(i+1))), 2)));
        syst_pt2->SetBinContent(i+1, sqrt( pow( max(fabs(data_pt2->GetBinContent(i+1) - data_pt2_up->GetBinContent(i+1)), fabs(data_pt2->GetBinContent(i+1) - data_pt2_down->GetBinContent(i+1))), 2) + pow( max(fabs(mc_pt2->GetBinContent(i+1) - mc_pt2_up->GetBinContent(i+1)), fabs(mc_pt2->GetBinContent(i+1) - mc_pt2_down->GetBinContent(i+1))), 2)));
        syst_pt3->SetBinContent(i+1, sqrt( pow( max(fabs(data_pt3->GetBinContent(i+1) - data_pt3_up->GetBinContent(i+1)), fabs(data_pt3->GetBinContent(i+1) - data_pt3_down->GetBinContent(i+1))), 2) + pow( max(fabs(mc_pt3->GetBinContent(i+1) - mc_pt3_up->GetBinContent(i+1)), fabs(mc_pt3->GetBinContent(i+1) - mc_pt3_down->GetBinContent(i+1))), 2)));

        syst_gain6_EB->SetBinContent(i+1, sqrt( pow( max(fabs(data_gain6_EB_up->GetBinContent(i+1) - data_gain6_EB->GetBinContent(i+1)), fabs(data_gain6_EB_down->GetBinContent(i+1) - data_gain6_EB->GetBinContent(i+1))) ,2) + pow( max( fabs( mc_gain6_EB_up->GetBinContent(i+1) - mc_gain6_EB->GetBinContent(i+1)), fabs( mc_gain6_EB_down->GetBinContent(i+1) - mc_gain6_EB->GetBinContent(i+1)))  ,2)));
        syst_gain6_EE->SetBinContent(i+1, sqrt( pow( max(fabs(data_gain6_EE_up->GetBinContent(i+1) - data_gain6_EE->GetBinContent(i+1)), fabs(data_gain6_EE_down->GetBinContent(i+1) - data_gain6_EE->GetBinContent(i+1))) ,2) + pow( max( fabs( mc_gain6_EE_up->GetBinContent(i+1) - mc_gain6_EE->GetBinContent(i+1)), fabs( mc_gain6_EE_down->GetBinContent(i+1) - mc_gain6_EE->GetBinContent(i+1)))  ,2)));
        syst_gain1_EB->SetBinContent(i+1, sqrt( pow( max(fabs(data_gain1_EB_up->GetBinContent(i+1) - data_gain1_EB->GetBinContent(i+1)), fabs(data_gain1_EB_down->GetBinContent(i+1) - data_gain1_EB->GetBinContent(i+1))) ,2) + pow( max( fabs( mc_gain1_EB_up->GetBinContent(i+1) - mc_gain1_EB->GetBinContent(i+1)), fabs( mc_gain1_EB_down->GetBinContent(i+1) - mc_gain1_EB->GetBinContent(i+1)))  ,2)));
        syst_gain1_EE->SetBinContent(i+1, sqrt( pow( max(fabs(data_gain1_EE_up->GetBinContent(i+1) - data_gain1_EE->GetBinContent(i+1)), fabs(data_gain1_EE_down->GetBinContent(i+1) - data_gain1_EE->GetBinContent(i+1))) ,2) + pow( max( fabs( mc_gain1_EE_up->GetBinContent(i+1) - mc_gain1_EE->GetBinContent(i+1)), fabs( mc_gain1_EE_down->GetBinContent(i+1) - mc_gain1_EE->GetBinContent(i+1)))  ,2)));
        syst_EbEb_ptz200->SetBinContent(i+1, sqrt( pow( max(fabs(data_EbEb_ptz200_up->GetBinContent(i+1) - data_EbEb_ptz200->GetBinContent(i+1)), fabs(data_EbEb_ptz200_down->GetBinContent(i+1) - data_EbEb_ptz200->GetBinContent(i+1))) ,2) + pow( max( fabs( mc_EbEb_ptz200_up->GetBinContent(i+1) - mc_EbEb_ptz200->GetBinContent(i+1)), fabs( mc_EbEb_ptz200_down->GetBinContent(i+1) - mc_EbEb_ptz200->GetBinContent(i+1)))  ,2)));
        syst_EeEe_ptz200->SetBinContent(i+1, sqrt( pow( max(fabs(data_EeEe_ptz200_up->GetBinContent(i+1) - data_EeEe_ptz200->GetBinContent(i+1)), fabs(data_EeEe_ptz200_down->GetBinContent(i+1) - data_EeEe_ptz200->GetBinContent(i+1))) ,2) + pow( max( fabs( mc_EeEe_ptz200_up->GetBinContent(i+1) - mc_EeEe_ptz200->GetBinContent(i+1)), fabs( mc_EeEe_ptz200_down->GetBinContent(i+1) - mc_EeEe_ptz200->GetBinContent(i+1)))  ,2)));

    }

    TH1F * total_EB = new TH1F("total_EB", "",mc_EbEb->GetNbinsX(), 80, 100);
    TH1F * total_ebeb = new TH1F("total_ebeb", "",mc_EbEb->GetNbinsX(), 80, 100);
    TH1F * total_eeee = new TH1F("total_eeee", "",mc_EbEb->GetNbinsX(), 80, 100);
    TH1F * total_NotEB = new TH1F("total_EE", "",mc_NotEbEb->GetNbinsX(), 80, 100);
    TH1F * total_pt0 = new TH1F("total_pt0", "", mc_pt0->GetNbinsX(), 80, 100);
    TH1F * total_pt1 = new TH1F("total_pt1", "", mc_pt1->GetNbinsX(), 80, 100);
    TH1F * total_ptDiag1 = new TH1F("total_ptDiag1", "", mc_ptDiag1->GetNbinsX(), 80, 100);
    TH1F * total_pt2 = new TH1F("total_pt2", "", mc_pt2->GetNbinsX(), 80, 100);
    TH1F * total_pt3 = new TH1F("total_pt3", "", mc_pt3->GetNbinsX(), 80, 100);
    TH1F * total_gain6_EB = new TH1F("total_gain6_EB", "",mc_gain6_EB->GetNbinsX(), 80, 100);
    TH1F * total_gain6_EE = new TH1F("total_gain6_EE", "",mc_gain6_EE->GetNbinsX(), 80, 100);
    TH1F * total_gain1_EB = new TH1F("total_gain1_EB", "",mc_gain1_EB->GetNbinsX(), 80, 100);
    TH1F * total_gain1_EE = new TH1F("total_gain1_EE", "",mc_gain1_EE->GetNbinsX(), 80, 100);
    TH1F * total_EbEb_ptz200 = new TH1F("total_EbEb_ptz200", "",mc_EbEb_ptz200->GetNbinsX(), 80, 100);
    TH1F * total_EeEe_ptz200 = new TH1F("total_EeEe_ptz200", "",mc_EeEe_ptz200->GetNbinsX(), 80, 100);

    TH1F * tot_EB = (TH1F*)mc_EbEb->Clone();
    TH1F * tot_NotEB = (TH1F*)mc_NotEbEb->Clone();
    TH1F * tot_ebeb = (TH1F*)mc_ebeb->Clone();
    TH1F * tot_eeee = (TH1F*)mc_eeee->Clone();
    TH1F * tot_pt0 = (TH1F*)mc_pt0->Clone();
    TH1F * tot_pt1 = (TH1F*)mc_pt1->Clone();
    TH1F * tot_pt2 = (TH1F*)mc_pt2->Clone();
    TH1F * tot_pt3 = (TH1F*)mc_pt3->Clone();

    for(int i = 0; i < total_EB->GetNbinsX(); i++){
        total_EB->SetBinContent(i+1, sqrt( pow(syst_EB->GetBinContent(i+1), 2) + pow(mc_EbEb->GetBinError(i+1), 2)));
        total_ebeb->SetBinContent(i+1, sqrt( pow(syst_ebeb->GetBinContent(i+1), 2) + pow(mc_ebeb->GetBinError(i+1), 2)));
        total_eeee->SetBinContent(i+1, sqrt( pow(syst_eeee->GetBinContent(i+1), 2) + pow(mc_eeee->GetBinError(i+1), 2)));
        total_NotEB->SetBinContent(i+1, sqrt( pow(syst_NotEB->GetBinContent(i+1), 2) + pow(mc_NotEbEb->GetBinError(i+1), 2)));
        total_pt0->SetBinContent(i+1, sqrt( pow(syst_pt0->GetBinContent(i+1),2) + pow(mc_pt0->GetBinError(i+1), 2)));
        total_pt1->SetBinContent(i+1, sqrt( pow(syst_pt1->GetBinContent(i+1),2) + pow(mc_pt1->GetBinError(i+1), 2)));
        total_ptDiag1->SetBinContent(i+1, sqrt( pow(syst_ptDiag1->GetBinContent(i+1),2) + pow(mc_ptDiag1->GetBinError(i+1), 2)));
        total_pt2->SetBinContent(i+1, sqrt( pow(syst_pt2->GetBinContent(i+1),2) + pow(mc_pt2->GetBinError(i+1), 2)));
        total_pt3->SetBinContent(i+1, sqrt( pow(syst_pt3->GetBinContent(i+1),2) + pow(mc_pt3->GetBinError(i+1), 2)));

        total_gain6_EB->SetBinContent(i+1, sqrt( pow(syst_gain6_EB->GetBinContent(i+1), 2) + pow(mc_gain6_EB->GetBinError(i+1), 2)));
        total_gain6_EE->SetBinContent(i+1, sqrt( pow(syst_gain6_EE->GetBinContent(i+1), 2) + pow(mc_gain6_EE->GetBinError(i+1), 2)));
        total_gain1_EB->SetBinContent(i+1, sqrt( pow(syst_gain1_EB->GetBinContent(i+1), 2) + pow(mc_gain1_EB->GetBinError(i+1), 2)));
        total_gain1_EE->SetBinContent(i+1, sqrt( pow(syst_gain1_EE->GetBinContent(i+1), 2) + pow(mc_gain1_EE->GetBinError(i+1), 2)));
        total_EbEb_ptz200->SetBinContent(i+1, sqrt( pow(syst_EbEb_ptz200->GetBinContent(i+1), 2) + pow(mc_EbEb_ptz200->GetBinError(i+1), 2)));
        total_EeEe_ptz200->SetBinContent(i+1, sqrt( pow(syst_EeEe_ptz200->GetBinContent(i+1), 2) + pow(mc_EeEe_ptz200->GetBinError(i+1), 2)));

        tot_EB->SetBinError(i+1, sqrt( pow(syst_EB->GetBinError(i+1), 2) + pow(mc_EbEb->GetBinError(i+1), 2)));
        tot_ebeb->SetBinError(i+1, sqrt( pow(syst_ebeb->GetBinError(i+1), 2) + pow(mc_ebeb->GetBinError(i+1), 2)));
        tot_eeee->SetBinError(i+1, sqrt( pow(syst_eeee->GetBinError(i+1), 2) + pow(mc_eeee->GetBinError(i+1), 2)));
        tot_NotEB->SetBinError(i+1, sqrt( pow(syst_NotEB->GetBinError(i+1), 2) + pow(mc_NotEbEb->GetBinError(i+1), 2)));
        tot_pt0->SetBinError(i+1, sqrt( pow(syst_pt0->GetBinError(i+1),2) + pow(mc_pt0->GetBinError(i+1), 2)));
        tot_pt1->SetBinError(i+1, sqrt( pow(syst_pt1->GetBinError(i+1),2) + pow(mc_pt1->GetBinError(i+1), 2)));
        tot_pt2->SetBinError(i+1, sqrt( pow(syst_pt2->GetBinError(i+1),2) + pow(mc_pt2->GetBinError(i+1), 2)));
        tot_pt3->SetBinError(i+1, sqrt( pow(syst_pt3->GetBinError(i+1),2) + pow(mc_pt3->GetBinError(i+1), 2)));
    }


    ofstream out;
    out.open(string("chiSquared/chiSquared_"+outputFile+".txt").c_str(), std::ofstream::app);
    out << "Barrel-Barrel           " << util.EvaluateChiSquare(data_ebeb, mc_ebeb, 80, 100) << " " << util.EvaluateChiSquare(data_ebeb,tot_ebeb,80,100) << std::endl;
    out << "Barrel-Barrel, High R9  " << util.EvaluateChiSquare(data_EbEb, mc_EbEb, 80, 100) << " " << util.EvaluateChiSquare(data_EbEb,tot_EB,80,100) << std::endl;
    out << "Barrel-!Barrel, High R9 " << util.EvaluateChiSquare(data_NotEbEb, mc_NotEbEb, 80, 100) << " " << util.EvaluateChiSquare(data_NotEbEb,tot_NotEB,80,100) << std::endl;
    out << "Endcap-Endcap           " << util.EvaluateChiSquare(data_eeee, mc_eeee, 80, 100) << " " << util.EvaluateChiSquare(data_eeee, tot_eeee,80,100) << std::endl;
    out << "40 < pt < 55            " << util.EvaluateChiSquare(data_pt0, mc_pt0, 80, 100) << " " << util.EvaluateChiSquare(data_pt0,tot_pt0,80,100) <<  std::endl;
    out << "55 < pt < 65            " << util.EvaluateChiSquare(data_pt1, mc_pt1, 80, 100) << " " << util.EvaluateChiSquare(data_pt1,tot_pt1,80,100) <<  std::endl;
    out << "65 < pt < 90            " << util.EvaluateChiSquare(data_pt2, mc_pt2, 80, 100) << " " << util.EvaluateChiSquare(data_pt2,tot_pt2,80,100) <<  std::endl;
    out << "90 > pt                 " << util.EvaluateChiSquare(data_pt0, mc_pt0, 80, 100) << " " << util.EvaluateChiSquare(data_pt0,tot_pt0,80,100) <<  std::endl;
    out.close();

    //plot(data_pt, mc_pt, total_pt);   
    util.moneyPlot_wRatio(data_EbEb, mc_EbEb, total_EB, outputFile, 0);   
    util.moneyPlot_wRatio(data_NotEbEb, mc_NotEbEb, total_NotEB, outputFile, 2);   
    util.moneyPlot_wRatio(data_ebeb, mc_ebeb, total_ebeb, outputFile, 3);   
    util.moneyPlot_wRatio(data_eeee, mc_eeee, total_eeee, outputFile, 4);   
    util.moneyPlot_wRatio(data_pt0, mc_pt0, total_pt0, outputFile, 5);   
    util.moneyPlot_wRatio(data_pt1, mc_pt1, total_pt1, outputFile, 1);   
    util.moneyPlot_wRatio(data_ptDiag1, mc_ptDiag1, total_ptDiag1, outputFile, 7);   
    util.moneyPlot_wRatio(data_pt2, mc_pt2, total_pt2, outputFile, 6);   
    //util.moneyPlot_wRatio(data_gain6_EB, mc_gain6_EB, total_gain6_EB, outputFile, 9);   
    //util.moneyPlot_wRatio(data_gain6_EE, mc_gain6_EE, total_gain6_EE, outputFile, 10);   
    //util.moneyPlot_wRatio(data_gain1_EB, mc_gain1_EB, total_gain1_EB, outputFile, 11);   
    //util.moneyPlot_wRatio(data_gain1_EE, mc_gain1_EE, total_gain1_EE, outputFile, 12);   
    util.moneyPlot_wRatio(data_EbEb_ptz200, mc_EbEb_ptz200, total_EbEb_ptz200, outputFile, 13);   
    util.moneyPlot_wRatio(data_EeEe_ptz200, mc_EeEe_ptz200, total_EeEe_ptz200, outputFile, 14);   
    return util.moneyPlot_wRatio(data_pt3, mc_pt3, total_pt3, outputFile, 8);   

};

TCanvas * standardSet::eval_CoarseEtaPlots(string data, string mc1){

    double NP_STDDEV = 60.;
    double NP_MEDIAN = 100.;

    vector <TFile*> files;
    files.push_back(new TFile(data.c_str(), "READ"));
    files.push_back(new TFile(mc1.c_str(), "READ"));

    TH1F * dat_EbLow = (TH1F*)files[0]->Get("invMass_EBEB_low");
    TH1F * dat_EbHigh = (TH1F*)files[0]->Get("invMass_EBEB_high");
    TH1F * dat_EeLow = (TH1F*)files[0]->Get("invMass_EEEE_low");
    TH1F * dat_EeHigh = (TH1F*)files[0]->Get("invMass_EEEE_high");

    TH1F * mc_EbLow = (TH1F*)files[1]->Get("invMass_EBEB_low");
    TH1F * mc_EbHigh = (TH1F*)files[1]->Get("invMass_EBEB_high");
    TH1F * mc_EeLow = (TH1F*)files[1]->Get("invMass_EEEE_low");
    TH1F * mc_EeHigh = (TH1F*)files[1]->Get("invMass_EEEE_high");

    std::vector<TH1F*> dataHists;
    std::vector<TH1F*> mcHists;

    dataHists.push_back(dat_EbLow);
    dataHists.push_back(dat_EbHigh);
    dataHists.push_back(dat_EeLow);
    dataHists.push_back(dat_EeHigh);

    mcHists.push_back(mc_EbLow);
    mcHists.push_back(mc_EbHigh);
    mcHists.push_back(mc_EeLow);
    mcHists.push_back(mc_EeHigh);

    //invariant mass distributions
    dat_EbLow->Rebin(15,"",0);
    dat_EbHigh->Rebin(15,"",0);
    dat_EeLow->Rebin(15,"",0);
    dat_EeHigh->Rebin(15,"",0);
    mc_EbLow->Rebin(15,"",0);
    mc_EbHigh->Rebin(15,"",0);
    mc_EeLow->Rebin(15,"",0);
    mc_EeHigh->Rebin(15,"",0);

    dat_EbLow->Scale(1/dat_EbLow->Integral());
    dat_EbHigh->Scale(1/dat_EbHigh->Integral());
    dat_EeLow->Scale(1/dat_EeLow->Integral());
    dat_EeHigh->Scale(1/dat_EeHigh->Integral());
    mc_EbLow->Scale(1/mc_EbLow->Integral());
    mc_EbHigh->Scale(1/mc_EbHigh->Integral());
    mc_EeLow->Scale(1/mc_EeLow->Integral());
    mc_EeHigh->Scale(1/mc_EeHigh->Integral());

    //util.PlotDataMC(dat_EbLow, dataTitle, mc_EbLow, mcTitle, "standardSet_"+outputFile+"_meeDist_EB_LowR9", "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", 0.5, 1.5, "");
    //util.PlotDataMC(dat_EbHigh, dataTitle, mc_EbHigh, mcTitle, "standardSet_"+outputFile+"_meeDist_EB_HighR9", "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", 0.5, 1.5, "");
    //util.PlotDataMC(dat_EeLow, dataTitle, mc_EeLow, mcTitle, "standardSet_"+outputFile+"_meeDist_EE_LowR9", "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", 0.5, 1.5, "");
    //util.PlotDataMC(dat_EeHigh, dataTitle, mc_EeHigh, mcTitle, "standardSet_"+outputFile+"_meeDist_EE_HighR9", "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", 0.5, 1.5, "");

    TH1F * data_stddev = new TH1F("data_stddev", "", 4, 0, 1);
    TH1F * mc_stddev = new TH1F("mc_stddev", "", 4, 0, 1);

    TH1F * data_median = new TH1F("data_median", "", 4, 0, 1);
    TH1F * mc_median = new TH1F("mc_median", "", 4, 0, 1);

    for( int i = 0; i < 4; i++){
        errors* dat_med = statistic::getHistStatistic(dataHists[i], "median", NP_MEDIAN, 500, 0.95);
        errors* mc_med = statistic::getHistStatistic(mcHists[i], "median", NP_MEDIAN, 500, 0.95);
        errors* dat_std = statistic::getHistStatistic(dataHists[i], "stddev", NP_STDDEV, 500, 0.95);
        errors* mc_std = statistic::getHistStatistic(mcHists[i], "stddev", NP_STDDEV, 500, 0.95);

        RooFitResult * myFitData = fitBreitGaus(dataHists[i], true);
        RooFitResult * myFitMC = fitBreitGaus(mcHists[i], false);

        RooRealVar * data_mean = (RooRealVar*)myFitData->floatParsFinal().find("mean");
        RooRealVar * data_sigma = (RooRealVar*)myFitData->floatParsFinal().find("sigma");
        float data_M = data_mean->getValV()+91.188;
        float data_M_err = sqrt( pow(data_mean->getAsymErrorHi(), 2) + pow(data_mean->getAsymErrorHi(),2)); 
        float data_sig = data_sigma->getValV();
        float data_sig_err = data_sigma->getAsymErrorHi();
        float data_res = 100*data_sig/data_M;
        float data_err = data_res * sqrt(pow(data_sig_err/data_sig, 2) + pow(data_M/data_M_err,2));
        
        RooRealVar * mc_mean = (RooRealVar*)myFitData->floatParsFinal().find("mean");
        RooRealVar * mc_sigma = (RooRealVar*)myFitData->floatParsFinal().find("sigma");
        float mc_M = mc_mean->getValV()+91.188;
        float mc_M_err = sqrt(pow(mc_mean->getAsymErrorHi(),2)); 
        float mc_sig = mc_sigma->getValV();
        float mc_sig_err = mc_sigma->getAsymErrorHi();

        data_median->SetBinContent(i+1, dat_med->x);
        data_median->SetBinError(i+1, max(dat_med->errLow, dat_med->errHigh));
        data_stddev->SetBinContent(i+1, data_sig);
        data_stddev->SetBinError(i+1, data_sig_err);

        mc_median->SetBinContent(i+1, mc_med->x);
        mc_median->SetBinError(i+1, max(mc_med->errLow, mc_med->errHigh));
        mc_stddev->SetBinContent(i+1, mc_sig);
        mc_stddev->SetBinError(i+1, mc_sig_err);
    }

    util.PlotDataMC(data_median, dataTitle, mc_median, mcTitle, "standardSet_"+outputFile+"_cat_median_noStatErr", "", "Median M_{ee} [GeV]", -1, -1, 89, 92, false, true, "#bf{ Data / MC }", -1, -1, "categories");
    return util.PlotDataMC(data_stddev, dataTitle, mc_stddev, mcTitle, "standardSet_"+outputFile+"_cat_stddev_noStatErr", "", "#sigma^{60}_{M_{ee}} [GeV]", -1, -1, 1, 3, false, true, "#bf{ Data / MC }", -1, -1, "categories");

    
};

TCanvas * standardSet::evaluate_systPlots(vector<string> dat_files, int kWhichR9){
    vector <TFile*> files;
    for( int i = 0; i < dat_files.size(); i++){
        files.push_back(new TFile(dat_files[i].c_str(), "READ"));
    }

    TH1F * EBinSyst_low = (TH1F*)files[0]->Get("invMass_ele_EB_in_Low");
    TH1F * EBinSyst_high = (TH1F*)files[0]->Get("invMass_ele_EB_in_High");
    TH1F * EBoutSyst_low = (TH1F*)files[0]->Get("invMass_ele_EB_out_Low");
    TH1F * EBoutSyst_high = (TH1F*)files[0]->Get("invMass_ele_EB_out_High");
    TH1F * EEinSyst_low = (TH1F*)files[0]->Get("invMass_ele_EE_in_Low");
    TH1F * EEinSyst_high = (TH1F*)files[0]->Get("invMass_ele_EE_in_High");
    TH1F * EEoutSyst_low = (TH1F*)files[0]->Get("invMass_ele_EE_out_Low");
    TH1F * EEoutSyst_high = (TH1F*)files[0]->Get("invMass_ele_EE_out_High");

    TH1F * mcSyst_EBin_low = (TH1F*)files[1]->Get("invMass_ele_EB_in_Low");
    TH1F * mcSyst_EBin_high = (TH1F*)files[1]->Get("invMass_ele_EB_in_High");
    TH1F * mcSyst_EBout_low = (TH1F*)files[1]->Get("invMass_ele_EB_out_Low");
    TH1F * mcSyst_EBout_high = (TH1F*)files[1]->Get("invMass_ele_EB_out_High");
    TH1F * mcSyst_EEin_low = (TH1F*)files[1]->Get("invMass_ele_EE_in_Low");
    TH1F * mcSyst_EEin_high = (TH1F*)files[1]->Get("invMass_ele_EE_in_High");
    TH1F * mcSyst_EEout_low = (TH1F*)files[1]->Get("invMass_ele_EE_out_Low");
    TH1F * mcSyst_EEout_high = (TH1F*)files[1]->Get("invMass_ele_EE_out_High");

    TH1F * EBin_low = (TH1F*)files[2]->Get("invMass_ele_EB_in_Low");
    TH1F * EBin_high = (TH1F*)files[2]->Get("invMass_ele_EB_in_High");
    TH1F * EBout_low = (TH1F*)files[2]->Get("invMass_ele_EB_out_Low");
    TH1F * EBout_high = (TH1F*)files[2]->Get("invMass_ele_EB_out_High");
    TH1F * EEin_low = (TH1F*)files[2]->Get("invMass_ele_EE_in_Low");
    TH1F * EEin_high = (TH1F*)files[2]->Get("invMass_ele_EE_in_High");
    TH1F * EEout_low = (TH1F*)files[2]->Get("invMass_ele_EE_out_Low");
    TH1F * EEout_high = (TH1F*)files[2]->Get("invMass_ele_EE_out_High");

    TH1F * mc_EBin_low = (TH1F*)files[3]->Get("invMass_ele_EB_in_Low");
    TH1F * mc_EBin_high = (TH1F*)files[3]->Get("invMass_ele_EB_in_High");
    TH1F * mc_EBout_low = (TH1F*)files[3]->Get("invMass_ele_EB_out_Low");
    TH1F * mc_EBout_high = (TH1F*)files[3]->Get("invMass_ele_EB_out_High");
    TH1F * mc_EEin_low = (TH1F*)files[3]->Get("invMass_ele_EE_in_Low");
    TH1F * mc_EEin_high = (TH1F*)files[3]->Get("invMass_ele_EE_in_High");
    TH1F * mc_EEout_low = (TH1F*)files[3]->Get("invMass_ele_EE_out_Low");
    TH1F * mc_EEout_high = (TH1F*)files[3]->Get("invMass_ele_EE_out_High");

    // want to print out data table:
    //  eta across top, R9 down

    vector< errors* > dataSyst_highR9;
    vector< errors* > dataSyst_lowR9;
    vector< errors* > mcSyst_highR9;
    vector< errors* > mcSyst_lowR9;

    vector< errors* > data_highR9;
    vector< errors* > data_lowR9;
    vector< errors* > mc_highR9;
    vector< errors* > mc_lowR9;

    dataSyst_highR9.push_back( statistic::getHistStatistic(EBinSyst_high, "median", N_PERCENT_HIST, 500, 0.95));
    dataSyst_highR9.push_back( statistic::getHistStatistic(EBoutSyst_high, "median", N_PERCENT_HIST, 500, 0.95));
    dataSyst_highR9.push_back( statistic::getHistStatistic(EEinSyst_high, "median", N_PERCENT_HIST, 500, 0.95));
    dataSyst_highR9.push_back( statistic::getHistStatistic(EEoutSyst_high, "median", N_PERCENT_HIST, 500, 0.95));

    dataSyst_lowR9.push_back( statistic::getHistStatistic(EBinSyst_low, "median", N_PERCENT_HIST, 500, 0.95));
    dataSyst_lowR9.push_back( statistic::getHistStatistic(EBoutSyst_low, "median", N_PERCENT_HIST, 500, 0.95));
    dataSyst_lowR9.push_back( statistic::getHistStatistic(EEinSyst_low, "median", N_PERCENT_HIST, 500, 0.95));
    dataSyst_lowR9.push_back( statistic::getHistStatistic(EEoutSyst_low, "median", N_PERCENT_HIST, 500, 0.95));

    mcSyst_highR9.push_back( statistic::getHistStatistic(mcSyst_EBin_high, "median", N_PERCENT_HIST, 500, 0.95));
    mcSyst_highR9.push_back( statistic::getHistStatistic(mcSyst_EBout_high, "median", N_PERCENT_HIST, 500, 0.95));
    mcSyst_highR9.push_back( statistic::getHistStatistic(mcSyst_EEin_high, "median", N_PERCENT_HIST, 500, 0.95));
    mcSyst_highR9.push_back( statistic::getHistStatistic(mcSyst_EEout_high, "median", N_PERCENT_HIST, 500, 0.95));

    mcSyst_lowR9.push_back( statistic::getHistStatistic(mcSyst_EBin_low, "median", N_PERCENT_HIST, 500, 0.95));
    mcSyst_lowR9.push_back( statistic::getHistStatistic(mcSyst_EBout_low, "median", N_PERCENT_HIST, 500, 0.95));
    mcSyst_lowR9.push_back( statistic::getHistStatistic(mcSyst_EEin_low, "median", N_PERCENT_HIST, 500, 0.95));
    mcSyst_lowR9.push_back( statistic::getHistStatistic(mcSyst_EEout_low, "median", N_PERCENT_HIST, 500, 0.95));

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

    TH1F* EBinSysts;   TH1F* mc_EBinSysts; 
    TH1F* EBoutSysts;  TH1F* mc_EBoutSysts;
    TH1F* EEinSysts;   TH1F* mc_EEinSysts; 
    TH1F* EEoutSysts;  TH1F* mc_EEoutSysts;

    TH1F* EBin;   TH1F* mc_EBin;  
    TH1F* EBout;  TH1F* mc_EBout; 
    TH1F* EEin;   TH1F* mc_EEin;  
    TH1F* EEout;  TH1F* mc_EEout; 

    //want bins of Eta, combine R9 plots
    if(kWhichR9 == -1 || kWhichR9 == 0){

        EBinSysts = (TH1F*)EBinSyst_low->Clone();
        EBoutSysts = (TH1F*)EBoutSyst_low->Clone();
        EEinSysts = (TH1F*)EEinSyst_low->Clone();
        EEoutSysts = (TH1F*)EEoutSyst_low->Clone();

        mc_EBinSysts = (TH1F*)mcSyst_EBin_low->Clone();
        mc_EBoutSysts = (TH1F*)mcSyst_EBout_low->Clone();
        mc_EEinSysts = (TH1F*)mcSyst_EEin_low->Clone();
        mc_EEoutSysts = (TH1F*)mcSyst_EEout_low->Clone();

        EBin = (TH1F*)EBin_low->Clone();
        EBout = (TH1F*)EBout_low->Clone();
        EEin = (TH1F*)EEin_low->Clone();
        EEout = (TH1F*)EEout_low->Clone();

        mc_EBin = (TH1F*)mc_EBin_low->Clone();
        mc_EBout = (TH1F*)mc_EBout_low->Clone();
        mc_EEin = (TH1F*)mc_EEin_low->Clone();
        mc_EEout = (TH1F*)mc_EEout_low->Clone();
    }
    if(kWhichR9 == 1){

        EBinSysts = (TH1F*)EBinSyst_high->Clone();
        EBoutSysts = (TH1F*)EBoutSyst_high->Clone();
        EEinSysts = (TH1F*)EEinSyst_high->Clone();
        EEoutSysts = (TH1F*)EEoutSyst_high->Clone();

        mc_EBinSysts = (TH1F*)mcSyst_EBin_high->Clone();
        mc_EBoutSysts = (TH1F*)mcSyst_EBout_high->Clone();
        mc_EEinSysts = (TH1F*)mcSyst_EEin_high->Clone();
        mc_EEoutSysts = (TH1F*)mcSyst_EEout_high->Clone();

        EBin = (TH1F*)EBin_high->Clone();
        EBout = (TH1F*)EBout_high->Clone();
        EEin = (TH1F*)EEin_high->Clone();
        EEout = (TH1F*)EEout_high->Clone();

        mc_EBin = (TH1F*)mc_EBin_high->Clone();
        mc_EBout = (TH1F*)mc_EBout_high->Clone();
        mc_EEin = (TH1F*)mc_EEin_high->Clone();
        mc_EEout = (TH1F*)mc_EEout_high->Clone();
    }

    EBinSysts->Scale(1/EBinSysts->Integral());
    EBoutSysts->Scale(1/EBoutSysts->Integral());
    EEinSysts->Scale(1/EEinSysts->Integral());
    EEoutSysts->Scale(1/EEoutSysts->Integral());
    mc_EBinSysts->Scale(1/mc_EBinSysts->Integral());
    mc_EBoutSysts->Scale(1/mc_EBoutSysts->Integral());
    mc_EEinSysts->Scale(1/mc_EEinSysts->Integral());
    mc_EEoutSysts->Scale(1/mc_EEoutSysts->Integral());
    EBinSysts->Rebin(15, "", 0);
    EBoutSysts->Rebin(15, "", 0);
    EEinSysts->Rebin(15, "", 0);
    EEoutSysts->Rebin(15, "", 0);
    mc_EBinSysts->Rebin(15, "", 0);
    mc_EBoutSysts->Rebin(15, "", 0);
    mc_EEinSysts->Rebin(15, "", 0);
    mc_EEoutSysts->Rebin(15, "", 0);

    EEinSysts->Rebin(2,"",0);
    mc_EEinSysts->Rebin(2,"",0);
    EBoutSysts->Rebin(2,"",0);
    mc_EBoutSysts->Rebin(2,"",0);
    EEinSysts->Rebin(2,"",0);
    mc_EEinSysts->Rebin(2,"",0);
    EEoutSysts->Rebin(2,"",0);
    mc_EEoutSysts->Rebin(2,"",0);

    EBin->Scale(1/EBin->Integral());
    EBout->Scale(1/EBout->Integral());
    EEin->Scale(1/EEin->Integral());
    EEout->Scale(1/EEout->Integral());
    mc_EBin->Scale(1/mc_EBin->Integral());
    mc_EBout->Scale(1/mc_EBout->Integral());
    mc_EEin->Scale(1/mc_EEin->Integral());
    mc_EEout->Scale(1/mc_EEout->Integral());
    EBin->Rebin(15, "", 0);
    EBout->Rebin(15, "", 0);
    EEin->Rebin(15, "", 0);
    EEout->Rebin(15, "", 0);
    mc_EBin->Rebin(15, "", 0);
    mc_EBout->Rebin(15, "", 0);
    mc_EEin->Rebin(15, "", 0);
    mc_EEout->Rebin(15, "", 0);

    EEin->Rebin(2,"",0);
    mc_EEin->Rebin(2,"",0);
    EBout->Rebin(2,"",0);
    mc_EBout->Rebin(2,"",0);
    EEin->Rebin(2,"",0);
    mc_EEin->Rebin(2,"",0);
    EEout->Rebin(2,"",0);
    mc_EEout->Rebin(2,"",0);

    std::vector<TH1F*> dataSysts_hists;
    std::vector<TH1F*> mcSysts_hists;

    std::vector<TH1F*> data_hists;
    std::vector<TH1F*> mc_hists;

    data_hists.push_back(EBin);
    data_hists.push_back(EBout);
    data_hists.push_back((TH1F*)0);
    data_hists.push_back(EEin);
    data_hists.push_back(EEout);

    mc_hists.push_back(mc_EBin);
    mc_hists.push_back(mc_EBout);
    mc_hists.push_back((TH1F*)0);
    mc_hists.push_back(mc_EEin);
    mc_hists.push_back(mc_EEout);

    dataSysts_hists.push_back(EBinSysts);
    dataSysts_hists.push_back(EBoutSysts);
    dataSysts_hists.push_back((TH1F*)0);
    dataSysts_hists.push_back(EEinSysts);
    dataSysts_hists.push_back(EEoutSysts);

    mcSysts_hists.push_back(mc_EBinSysts);
    mcSysts_hists.push_back(mc_EBoutSysts);
    mcSysts_hists.push_back((TH1F*)0);
    mcSysts_hists.push_back(mc_EEinSysts);
    mcSysts_hists.push_back(mc_EEoutSysts);

    string thisR9String = (kWhichR9 == 1) ? "_highR9" : (kWhichR9 == 0) ? "" : "_lowR9";

    double etaBins [6] = {0., 1., 1.4442, 1.566, 2., 2.5};

    TH1F * data_loose_median = new TH1F("data_loose_median", "", 5, etaBins);
    TH1F * mc_loose_median = new TH1F("mc_loose_median", "", 5, etaBins);

    TH1F * dataSysts_loose_median = new TH1F("dataSysts_loose_median", "", 5, etaBins);
    TH1F * mcSysts_loose_median = new TH1F("mcSysts_loose_median", "", 5, etaBins);

    errors* median;
    for(int i = 1; i < 6; i++){
        if( i != 3 ){

            median = statistic::getHistStatistic(dataSysts_hists[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            dataSysts_loose_median->SetBinContent(i, median->x);
            dataSysts_loose_median->SetBinError(i, max(median->errLow, median->errHigh));

            median = statistic::getHistStatistic(mcSysts_hists[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            mcSysts_loose_median->SetBinContent(i, median->x);
            mcSysts_loose_median->SetBinError(i, max(median->errLow, median->errHigh));

            median = statistic::getHistStatistic(data_hists[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            data_loose_median->SetBinContent(i, median->x);
            data_loose_median->SetBinError(i, max(median->errLow, median->errHigh));

            median = statistic::getHistStatistic(mc_hists[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            mc_loose_median->SetBinContent(i, median->x);
            mc_loose_median->SetBinError(i, max(median->errLow, median->errHigh));
        }
    }

    TH1F * ratio = new TH1F("ratio", "", 5, etaBins);
    TH1F * ratioSysts = new TH1F("ratioSysts", "", 5, etaBins);

    int numBins = 0;
    numBins = 6;
    for(int i = 1; i < numBins; i++){
        if( i != 3){
            ratio->SetBinContent(i, data_loose_median->GetBinContent(i)/mc_loose_median->GetBinContent(i));
            ratioSysts->SetBinContent(i, dataSysts_loose_median->GetBinContent(i)/mcSysts_loose_median->GetBinContent(i));
        }
    }

    double yMin = 89; double yMax = 92;
    std::string yTitle_median = "Median M_{ee} Ratio";  
    yMin = -1;
    yMax = -1;

    util.PlotDataMC(ratioSysts, dataTitle, ratio, mcTitle, "standardSet_"+outputFile+"_eta_median"+thisR9String+"_noRatio", "|#eta|", yTitle_median, 0, 2.4999, 0.99, 1.01, false, false, "#bf{Systematics Ratio / Ratio}", 0.99, 1.01, "eta");
    return util.PlotDataMC(ratioSysts, dataTitle, ratio, mcTitle, "standardSet_"+outputFile+"_eta_median"+thisR9String, "|#eta|", yTitle_median, 0, 2.4999, 0.99, 1.01, false, true, "#bf{Systematics Ratio / Ratio}", 0.99, 1.01, "eta");
};


TCanvas * standardSet::evaluate_EtaPlots(string data, string mc1, int kWhichR9, string stat){
    //files
    std::cout << data << " " << mc1 << " " << kWhichR9 << " " << stat << std::endl;
    std::cout << N_PERCENT_HIST << std::endl;
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

    double etaBins [6] = {0., 1., 1.4442, 1.566, 2., 2.5};

    TH1F * data_loose_stddev = new TH1F("data_loose_stddev", "", 5, etaBins);
    TH1F * dataDown_loose_stddev = new TH1F("dataDown_loose_stddev", "", 5, etaBins);
    TH1F * dataUp_loose_stddev = new TH1F("dataUp_loose_stddev", "", 5, etaBins);

    TH1F * mc_loose_stddev = new TH1F("mc_loose_stddev", "", 5, etaBins);
    TH1F * mcDown_loose_stddev = new TH1F("mcDown_loose_stddev", "", 5, etaBins);
    TH1F * mcUp_loose_stddev = new TH1F("mcUp_loose_stddev", "", 5, etaBins);

    TH1F * data_loose_mean = new TH1F("data_loose_mean", "", 5, etaBins);
    TH1F * dataDown_loose_mean = new TH1F("dataDown_loose_mean", "", 5, etaBins);
    TH1F * dataUp_loose_mean = new TH1F("dataUp_loose_mean", "", 5, etaBins);

    TH1F * mc_loose_mean = new TH1F("mc_loose_mean", "", 5, etaBins);
    TH1F * mcDown_loose_mean = new TH1F("mcDown_loose_mean", "", 5, etaBins);
    TH1F * mcUp_loose_mean = new TH1F("mcUp_loose_mean", "", 5, etaBins);

    TH1F * data_loose_median = new TH1F("data_loose_median", "", 5, etaBins);
    TH1F * dataDown_loose_median = new TH1F("dataDown_loose_median", "", 5, etaBins);
    TH1F * dataUp_loose_median = new TH1F("dataUp_loose_median", "", 5, etaBins);

    TH1F * mc_loose_median = new TH1F("mc_loose_median", "", 5, etaBins);
    TH1F * mcDown_loose_median = new TH1F("mcDown_loose_median", "", 5, etaBins);
    TH1F * mcUp_loose_median = new TH1F("mcUp_loose_median", "", 5, etaBins);

    TH1F * data_res = new TH1F("data_res", "", 5, etaBins);
    TH1F * mc_res = new TH1F("mc_res", "", 5, etaBins);

    errors* median;
    for(int i = 1; i < 6; i++){
        if( i != 3 ){
            std::string catName = "";
            switch(i){
                case 1:
                    catName = "_EBin";
                    break;
                case 2:
                    catName = "_EBout";
                    break;
                case 4:
                    catName = "_EEin";
                    break;
                case 5:
                    catName = "_EEout";
                    break;
            }
            if(kWhichR9 == -1) catName += "_LowR9";
            if(kWhichR9 == 1) catName += "_HighR9";

            std::cout << data_hists[i-1]->GetName() << std::endl;
            data_hists[i-1]->Rebin(15,"",0);
            mc_hists[i-1]->Rebin(15,"", 0);
            RooFitResult * myFitData = fitBreitGaus(data_hists[i-1], true);
            median = statistic::getHistStatistic(data_hists[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            util.plotRooFitResult(data_hists[i-1], myFitData, "standardSet_"+outputFile+"_dataWithFit"+catName, outputFile+catName);
    
            RooRealVar * data_mean = (RooRealVar*)myFitData->floatParsFinal().find("mean");
            RooRealVar * data_sigma = (RooRealVar*)myFitData->floatParsFinal().find("sigma");
            float data_M = data_mean->getValV()+91.188;
            float data_M_err = sqrt(pow(data_mean->getAsymErrorHi(),2)); 
            float data_sig = data_sigma->getValV();
            float data_sig_err = data_sigma->getAsymErrorHi();

            data_loose_stddev->SetBinContent(i, data_sig);
            data_loose_stddev->SetBinError(i, data_sig_err);
            data_loose_mean->SetBinContent(i, data_M);
            data_loose_mean->SetBinError(i, data_M_err);
            data_loose_median->SetBinContent(i, median->x);
            data_loose_median->SetBinError(i, max(median->errLow, median->errHigh));

            myFitData = fitBreitGaus(data_hists_down[i-1], true);
            median = statistic::getHistStatistic(data_hists_down[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            data_mean = (RooRealVar*)myFitData->floatParsFinal().find("mean");
            data_sigma = (RooRealVar*)myFitData->floatParsFinal().find("sigma");
            data_M = data_mean->getValV()+91.188;
            data_M_err = sqrt(pow(data_mean->getAsymErrorHi(),2)); 
            data_sig = data_sigma->getValV();
            data_sig_err = data_sigma->getAsymErrorHi();

            dataDown_loose_stddev->SetBinContent(i, data_sig);
            dataDown_loose_stddev->SetBinError(i, data_sig_err);
            dataDown_loose_mean->SetBinContent(i, data_M);
            dataDown_loose_mean->SetBinError(i, data_M_err);
            dataDown_loose_median->SetBinContent(i, median->x);
            dataDown_loose_median->SetBinError(i, max(median->errLow, median->errHigh));

            std::cout << data_hists_up[i-1]->GetName() << std::endl;
            myFitData = fitBreitGaus(data_hists_up[i-1], true);
            median = statistic::getHistStatistic(data_hists_up[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            data_mean = (RooRealVar*)myFitData->floatParsFinal().find("mean");
            data_sigma = (RooRealVar*)myFitData->floatParsFinal().find("sigma");
            data_M = data_mean->getValV()+91.188;
            data_M_err = sqrt(pow(data_mean->getAsymErrorHi(),2)); 
            data_sig = data_sigma->getValV();
            data_sig_err = data_sigma->getAsymErrorHi();

            dataUp_loose_stddev->SetBinContent(i, data_sig);
            dataUp_loose_stddev->SetBinError(i, data_sig_err);
            dataUp_loose_mean->SetBinContent(i, data_M);
            dataUp_loose_mean->SetBinError(i, data_M_err);
            dataUp_loose_median->SetBinContent(i, median->x);
            dataUp_loose_median->SetBinError(i, max(median->errLow, median->errHigh));

            std::cout << mc_hists[i-1]->GetName() << std::endl;
            RooFitResult * myFitMC = fitBreitGaus(mc_hists[i-1], false);
            median = statistic::getHistStatistic(mc_hists[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            util.plotRooFitResult(mc_hists[i-1], myFitMC, "standardSet_"+outputFile+"_mcWithFit"+catName, outputFile+catName);

            RooRealVar * mc_mean = (RooRealVar*)myFitMC->floatParsFinal().find("mean");
            RooRealVar * mc_sigma = (RooRealVar*)myFitMC->floatParsFinal().find("sigma");
            float mc_M = mc_mean->getValV()+91.188;
            float mc_M_err = sqrt(pow(mc_mean->getAsymErrorHi(),2)); 
            float mc_sig = mc_sigma->getValV();
            float mc_sig_err = mc_sigma->getAsymErrorHi();

            mc_loose_stddev->SetBinContent(i, mc_sig);
            mc_loose_stddev->SetBinError(i, mc_sig_err);
            mc_loose_mean->SetBinContent(i, mc_M);
            mc_loose_mean->SetBinError(i, mc_M_err);
            mc_loose_median->SetBinContent(i, median->x);
            mc_loose_median->SetBinError(i, max(median->errLow, median->errHigh));

            std::cout << mc_hists_down[i-1]->GetName() << std::endl;
            myFitMC = fitBreitGaus(mc_hists_down[i-1], false);
            median = statistic::getHistStatistic(mc_hists_down[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            mc_mean = (RooRealVar*)myFitMC->floatParsFinal().find("mean");
            mc_sigma = (RooRealVar*)myFitMC->floatParsFinal().find("sigma");
            mc_M = mc_mean->getValV()+91.188;
            mc_M_err = sqrt(pow(mc_mean->getAsymErrorHi(),2)); 
            mc_sig = mc_sigma->getValV();
            mc_sig_err = mc_sigma->getAsymErrorHi();

            mcDown_loose_stddev->SetBinContent(i, mc_sig);
            mcDown_loose_stddev->SetBinError(i, mc_sig_err);
            mcDown_loose_mean->SetBinContent(i, mc_M);
            mcDown_loose_mean->SetBinError(i, mc_M_err);
            mcDown_loose_median->SetBinContent(i, median->x);
            mcDown_loose_median->SetBinError(i, max(median->errLow, median->errHigh));

            std::cout << mc_hists_up[i-1]->GetName() << std::endl;
            myFitMC = fitBreitGaus(mc_hists_up[i-1], false);
            median = statistic::getHistStatistic(mc_hists_up[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            mc_mean = (RooRealVar*)myFitMC->floatParsFinal().find("mean");
            mc_sigma = (RooRealVar*)myFitMC->floatParsFinal().find("sigma");
            mc_M = mc_mean->getValV()+91.188;
            mc_M_err = sqrt(pow(mc_mean->getAsymErrorHi(),2)); 
            mc_sig = mc_sigma->getValV();
            mc_sig_err = mc_sigma->getAsymErrorHi();

            mcUp_loose_stddev->SetBinContent(i, mc_sig);
            mcUp_loose_stddev->SetBinError(i, mc_sig_err);
            mcUp_loose_mean->SetBinContent(i, mc_M);
            mcUp_loose_mean->SetBinError(i, mc_M_err);
            mcUp_loose_median->SetBinContent(i, median->x);
            mcUp_loose_median->SetBinError(i, max(median->errLow, median->errHigh));

            data_res->SetBinContent(i, 100*data_loose_stddev->GetBinContent(i)/data_loose_mean->GetBinContent(i));
            data_res->SetBinError(i, data_res->GetBinContent(i)*sqrt( pow(data_loose_stddev->GetBinError(i)/data_loose_stddev->GetBinContent(i),2) + pow(data_loose_mean->GetBinError(i)/data_loose_mean->GetBinContent(i),2)));
            mc_res->SetBinContent(i, 100*mc_loose_stddev->GetBinContent(i)/mc_loose_mean->GetBinContent(i));
            mc_res->SetBinError(i, mc_res->GetBinContent(i)*sqrt( pow(mc_loose_stddev->GetBinError(i)/mc_loose_stddev->GetBinContent(i),2) + pow(mc_loose_mean->GetBinError(i)/mc_loose_mean->GetBinContent(i),2)));

            /*
            TF1 * myFitData = fitBreitGaus_old(data_hists[i-1], true);
            median = statistic::getHistStatistic(data_hists[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            float data_M = myFitData->GetParameter(0);
            float data_M_err = myFitData->GetParError(0); 
            float data_sig = myFitData->GetParameter(1);
            float data_sig_err = myFitData->GetParError(1);

            data_loose_stddev->SetBinContent(i, data_sig);
            data_loose_stddev->SetBinError(i, data_sig_err);
            data_loose_mean->SetBinContent(i, data_M);
            data_loose_mean->SetBinError(i, data_M_err);
            data_loose_median->SetBinContent(i, median->x);
            data_loose_median->SetBinError(i, max(median->errLow, median->errHigh));
            data_res->SetBinContent(i, 100*data_sig/data_M);
            data_res->SetBinError(i, data_res->GetBinContent(i)*sqrt(pow(data_sig_err/data_sig, 2) + pow(data_M_err/data_M, 2)));

            myFitData = fitBreitGaus_old(data_hists_up[i-1], true);
            median = statistic::getHistStatistic(data_hists_up[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            data_M = myFitData->GetParameter(0);
            data_M_err = myFitData->GetParError(0); 
            data_sig = myFitData->GetParameter(1);
            data_sig_err = myFitData->GetParError(1);

            dataUp_loose_stddev->SetBinContent(i, data_sig);
            dataUp_loose_stddev->SetBinError(i, data_sig_err);
            dataUp_loose_mean->SetBinContent(i, data_M);
            dataUp_loose_mean->SetBinError(i, data_M_err);
            dataUp_loose_median->SetBinContent(i, median->x);
            dataUp_loose_median->SetBinError(i, max(median->errLow, median->errHigh));

            myFitData = fitBreitGaus_old(data_hists_down[i-1], true);
            median = statistic::getHistStatistic(data_hists_down[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            data_M = myFitData->GetParameter(0);
            data_M_err = myFitData->GetParError(0); 
            data_sig = myFitData->GetParameter(1);
            data_sig_err = myFitData->GetParError(1);

            dataDown_loose_stddev->SetBinContent(i, data_sig);
            dataDown_loose_stddev->SetBinError(i, data_sig_err);
            dataDown_loose_mean->SetBinContent(i, data_M);
            dataDown_loose_mean->SetBinError(i, data_M_err);
            dataDown_loose_median->SetBinContent(i, median->x);
            dataDown_loose_median->SetBinError(i, max(median->errLow, median->errHigh));

            TF1 * myFitMC = fitBreitGaus_old(mc_hists[i-1], true);
            median = statistic::getHistStatistic(mc_hists[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            float mc_M = myFitMC->GetParameter(0);
            float mc_M_err = myFitMC->GetParError(0); 
            float mc_sig = myFitMC->GetParameter(1);
            float mc_sig_err = myFitMC->GetParError(1);

            mc_loose_stddev->SetBinContent(i, mc_sig);
            mc_loose_stddev->SetBinError(i, mc_sig_err);
            mc_loose_mean->SetBinContent(i, mc_M);
            mc_loose_mean->SetBinError(i, mc_M_err);
            mc_loose_median->SetBinContent(i, median->x);
            mc_loose_median->SetBinError(i, max(median->errLow, median->errHigh));
            mc_res->SetBinContent(i, 100*mc_sig/mc_M);
            mc_res->SetBinError(i, mc_res->GetBinContent(i)*sqrt(pow(mc_sig_err/mc_sig, 2) + pow(mc_M_err/mc_M, 2)));

            myFitMC = fitBreitGaus_old(mc_hists_up[i-1], true);
            median = statistic::getHistStatistic(mc_hists_up[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            mc_M = myFitMC->GetParameter(0);
            mc_M_err = myFitMC->GetParError(0); 
            mc_sig = myFitMC->GetParameter(1);
            mc_sig_err = myFitMC->GetParError(1);

            mcUp_loose_stddev->SetBinContent(i, mc_sig);
            mcUp_loose_stddev->SetBinError(i, mc_sig_err);
            mcUp_loose_mean->SetBinContent(i, mc_M);
            mcUp_loose_mean->SetBinError(i, mc_M_err);
            mcUp_loose_median->SetBinContent(i, median->x);
            mcUp_loose_median->SetBinError(i, max(median->errLow, median->errHigh));

            myFitMC = fitBreitGaus_old(mc_hists_down[i-1], true);
            median = statistic::getHistStatistic(mc_hists_down[i-1], "median", N_PERCENT_HIST, 500, 0.95);

            mc_M = myFitMC->GetParameter(0);
            mc_M_err = myFitMC->GetParError(0); 
            mc_sig = myFitMC->GetParameter(1);
            mc_sig_err = myFitMC->GetParError(1);

            mcDown_loose_stddev->SetBinContent(i, mc_sig);
            mcDown_loose_stddev->SetBinError(i, mc_sig_err);
            mcDown_loose_mean->SetBinContent(i, mc_M);
            mcDown_loose_mean->SetBinError(i, mc_M_err);
            mcDown_loose_median->SetBinContent(i, median->x);
            mcDown_loose_median->SetBinError(i, max(median->errLow, median->errHigh));
            */



        }
    }

    std::vector<TH1F*> invMassSysts;
    for( int iii = 0; iii < 5; iii++){
        if( iii != 2 ){
            invMassSysts.push_back((TH1F*)mc_hists[iii]->Clone());
            for( int jjj = 1; jjj <= invMassSysts[iii]->GetNbinsX(); jjj++){

                float data_syst = sqrt(max( fabs(data_hists[iii]->GetBinContent(jjj) - data_hists_down[iii]->GetBinContent(jjj)), fabs(data_hists[iii]->GetBinContent(jjj) - data_hists_up[iii]->GetBinContent(jjj))));
                float mc_syst = sqrt(max( fabs(mc_hists[iii]->GetBinContent(jjj) - mc_hists_down[iii]->GetBinContent(jjj)), fabs(mc_hists[iii]->GetBinContent(jjj) - mc_hists_up[iii]->GetBinContent(jjj))));
                invMassSysts[iii]->SetBinError(jjj, sqrt( pow( data_syst/data_hists[iii]->GetBinContent(jjj), 2) + pow( mc_syst/mc_hists[iii]->GetBinContent(jjj), 2)));
                invMassSysts[iii]->SetBinContent(jjj, 1.);
            }
            data_hists[iii]->Scale(1/data_hists[iii]->Integral());
            mc_hists[iii]->Scale(1/mc_hists[iii]->Integral());
        }       
        else invMassSysts.push_back((TH1F*)0);
    }

    std::ofstream out;
    out.open(string("chiSquared_"+outputFile+".txt").c_str(), std::ofstream::app);
    if(_flag_invMassDist){
        util.PlotDataMC(EBin, dataTitle, mc_EBin, mcTitle, invMassSysts[0], "standardSet_"+outputFile+"_meeDist_EBin"+thisR9String, "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", 0.5, 1.5, "");
        out << "EBin"+thisR9String << " " << util.EvaluateChiSquare(EBin, mc_EBin, 80, 100) << std::endl;
        util.PlotDataMC(EBout, dataTitle, mc_EBout, mcTitle, invMassSysts[1], "standardSet_"+outputFile+"_meeDist_EBout"+thisR9String, "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", 0.5, 1.5, "");
        out << "EBout"+thisR9String << " " << util.EvaluateChiSquare(EBout, mc_EBout, 80, 100) << std::endl;
        util.PlotDataMC(EEin, dataTitle, mc_EEin, mcTitle, invMassSysts[3], "standardSet_"+outputFile+"_meeDist_EEin"+thisR9String, "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", 0.5, 1.5, "");
        out << "EEin"+thisR9String << " " << util.EvaluateChiSquare(EEin, mc_EEin, 80, 100) << std::endl;
        util.PlotDataMC(EEout, dataTitle, mc_EEout, mcTitle, invMassSysts[4], "standardSet_"+outputFile+"_meeDist_EEout"+thisR9String, "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", 0.5, 1.5, "");
        out << "EEout"+thisR9String << " " << util.EvaluateChiSquare(EEout, mc_EEout, 80, 100) << std::endl;
    }
    out.close();

    TH1F * scalesUncertainties_stddev = new TH1F("scalesUnc_stddev", "", 5,etaBins);
    TH1F * scalesUncertainties_mean = new TH1F("scalesUnc_mean", "", 5,etaBins);
    TH1F * scalesUncertainties_median = new TH1F("scalesUnc_median", "", 5,etaBins);
    TH1F * smearsUncertainties_stddev = new TH1F("smearsUnc_stddev", "", 5,etaBins);
    TH1F * smearsUncertainties_mean = new TH1F("smearsUnc_mean", "", 5,etaBins);
    TH1F * smearsUncertainties_median = new TH1F("smearsUnc_median", "", 5,etaBins);
    TH1F * systUnc_stddev = new TH1F("systUnc_stddev", "", 5,etaBins);
    TH1F * systUnc_mean = new TH1F("systUnc_mean", "", 5,etaBins);
    TH1F * systUnc_median = new TH1F("systUnc_median", "", 5,etaBins);

    int numBins = 0;
    numBins = 6;
    for(int i = 1; i < numBins; i++){
        scalesUncertainties_stddev->SetBinContent(i, 1.);
        scalesUncertainties_stddev->SetBinError(i, max( fabs(data_loose_stddev->GetBinContent(i)-dataUp_loose_stddev->GetBinContent(i)), fabs(data_loose_stddev->GetBinContent(i)-dataDown_loose_stddev->GetBinContent(i))));
        smearsUncertainties_stddev->SetBinContent(i, 1.);
        smearsUncertainties_stddev->SetBinError(i, max( fabs(mc_loose_stddev->GetBinContent(i)-mcUp_loose_stddev->GetBinContent(i)), fabs(mc_loose_stddev->GetBinContent(i)-mcDown_loose_stddev->GetBinContent(i))));
        systUnc_stddev->SetBinContent(i, 1.);
        systUnc_stddev->SetBinError(i, (data_loose_stddev->GetBinContent(i)/mc_loose_stddev->GetBinContent(i))* sqrt( pow( scalesUncertainties_stddev->GetBinError(i)/data_loose_stddev->GetBinContent(i) , 2) + pow( smearsUncertainties_stddev->GetBinError(i)/mc_loose_stddev->GetBinContent(i), 2)));

        scalesUncertainties_mean->SetBinContent(i, 1.);
        scalesUncertainties_mean->SetBinError(i, max( fabs(data_loose_mean->GetBinContent(i)-dataUp_loose_mean->GetBinContent(i)), fabs(data_loose_mean->GetBinContent(i)-dataDown_loose_mean->GetBinContent(i))));
        smearsUncertainties_mean->SetBinContent(i, 1.);
        smearsUncertainties_mean->SetBinError(i, max( fabs(mc_loose_mean->GetBinContent(i)-mcUp_loose_mean->GetBinContent(i)), fabs(mc_loose_mean->GetBinContent(i)-mcDown_loose_mean->GetBinContent(i))));
        systUnc_mean->SetBinContent(i, 1.);
        systUnc_mean->SetBinError(i, (data_loose_mean->GetBinContent(i)/mc_loose_mean->GetBinContent(i))* sqrt( pow( scalesUncertainties_mean->GetBinError(i)/data_loose_mean->GetBinContent(i) , 2) + pow( smearsUncertainties_mean->GetBinError(i)/mc_loose_mean->GetBinContent(i), 2)));

        scalesUncertainties_median->SetBinContent(i, 1.);
        scalesUncertainties_median->SetBinError(i, max( fabs(data_loose_median->GetBinContent(i)-dataUp_loose_median->GetBinContent(i)), fabs(data_loose_median->GetBinContent(i)-dataDown_loose_median->GetBinContent(i))));
        smearsUncertainties_median->SetBinContent(i, 1.);
        smearsUncertainties_median->SetBinError(i, max( fabs(mc_loose_median->GetBinContent(i)-mcUp_loose_median->GetBinContent(i)), fabs(mc_loose_median->GetBinContent(i)-mcDown_loose_median->GetBinContent(i))));
        systUnc_median->SetBinContent(i, 1.);
        systUnc_median->SetBinError(i, (data_loose_median->GetBinContent(i)/mc_loose_median->GetBinContent(i))* sqrt( pow( scalesUncertainties_median->GetBinError(i)/data_loose_median->GetBinContent(i) , 2) + pow( smearsUncertainties_median->GetBinError(i)/mc_loose_median->GetBinContent(i), 2)));
    }
    systUnc_stddev->SetBinError(3, 0.);
    systUnc_mean->SetBinError(3, 0.);
    systUnc_median->SetBinError(3, 0.);

    double yMin = 89; double yMax = 92;
    std::string yTitle_stddev = "#sigma_{M_{ee}} [GeV]";
    std::string yTitle_mean = "<M_{ee}> [GeV]";  
    std::string yTitle_median = "Median M_{ee} [GeV]";  
    yMin = -1;
    yMax = -1;

    util.PlotDataMC(data_res, dataTitle, mc_res, mcTitle, "standardSet_"+outputFile+"_eta_res"+thisR9String, "|#eta|", "#sigma/#mu [%]", 0, 2.4999, -1, -1, true, true, "Data/MC", 0.9, 1.1, "eta");
    util.PlotDataMC(data_res, dataTitle, mc_res, mcTitle, "standardSet_"+outputFile+"_eta_res"+thisR9String+"_noRatio", "|#eta|", "#sigma/#mu [%]", 0, 2.4999, -1, -1, true, false, "Data/MC", 0.9, 1.1, "eta");

    util.PlotDataMC(data_loose_stddev, dataTitle, mc_loose_stddev, mcTitle, systUnc_stddev, "standardSet_"+outputFile+"_eta_stddev"+thisR9String, "|#eta|", yTitle_stddev, 0, 2.4999, yMin, yMax, true, true, "#bf{Data / MC}", -1, -1, "eta");
    util.PlotDataMC(data_loose_stddev, dataTitle, mc_loose_stddev, mcTitle, systUnc_stddev, "standardSet_"+outputFile+"_eta_stddev"+thisR9String+"_noRatio", "|#eta|", yTitle_stddev, 0, 2.4999, yMin, yMax, true, false, "#bf{Data / MC}", -1, -1, "eta");
    util.PlotDataMC(data_loose_mean, dataTitle, mc_loose_mean, mcTitle, systUnc_mean, "standardSet_"+outputFile+"_eta_mean"+thisR9String, "|#eta|", yTitle_mean, 0, 2.4999, 89, 92, true, true, "#bf{Data / MC}", -1, -1, "eta");
    util.PlotDataMC(data_loose_mean, dataTitle, mc_loose_mean, mcTitle, systUnc_mean, "standardSet_"+outputFile+"_eta_mean"+thisR9String+"_noRatio", "|#eta|", yTitle_mean, 0, 2.4999, 89, 92, true, false, "#bf{Data / MC}", -1, -1, "eta");
    util.PlotDataMC(data_loose_median, dataTitle, mc_loose_median, mcTitle, systUnc_median, "standardSet_"+outputFile+"_eta_median"+thisR9String, "|#eta|", yTitle_median, 0, 2.4999, 89, 92, true, true, "#bf{Data / MC}", -1, -1, "eta");
    util.PlotDataMC(data_loose_median, dataTitle, mc_loose_median, mcTitle, systUnc_median, "standardSet_"+outputFile+"_eta_median"+thisR9String+"_noRatio", "|#eta|", yTitle_median, 0, 2.4999, 89, 92, true, false, "#bf{Data / MC}", -1, -1, "eta");
    util.PlotDataMC(data_loose_stddev, dataTitle, mc_loose_stddev, mcTitle, systUnc_stddev, "standardSet_"+outputFile+"_eta_stddev"+thisR9String, "|#eta|", yTitle_stddev, 0, 2.4999, yMin, yMax, false, true, "#bf{Data / MC}", -1, -1, "eta");
    util.PlotDataMC(data_loose_stddev, dataTitle, mc_loose_stddev, mcTitle, systUnc_stddev, "standardSet_"+outputFile+"_eta_stddev"+thisR9String+"_noRatio", "|#eta|", yTitle_stddev, 0, 2.4999, yMin, yMax, false, false, "#bf{Data / MC}", -1, -1, "eta");
    util.PlotDataMC(data_loose_mean, dataTitle, mc_loose_mean, mcTitle, systUnc_mean, "standardSet_"+outputFile+"_eta_mean"+thisR9String, "|#eta|", yTitle_mean, 0, 2.4999, 89, 92, false, true, "#bf{Data / MC}", -1, -1, "eta");
    util.PlotDataMC(data_loose_mean, dataTitle, mc_loose_mean, mcTitle, systUnc_mean, "standardSet_"+outputFile+"_eta_mean"+thisR9String+"_noRatio", "|#eta|", yTitle_mean, 0, 2.4999, 89, 92, false, false, "#bf{Data / MC}", -1, -1, "eta");
    util.PlotDataMC(data_loose_median, dataTitle, mc_loose_median, mcTitle, systUnc_median, "standardSet_"+outputFile+"_eta_median"+thisR9String+"_noRatio", "|#eta|", yTitle_median, 0, 2.4999, 89, 92, false, false, "#bf{Data / MC}", -1, -1, "eta");
    return util.PlotDataMC(data_loose_median, dataTitle, mc_loose_median, mcTitle, systUnc_median, "standardSet_"+outputFile+"_eta_median"+thisR9String, "|#eta|", yTitle_median, 0, 2.4999, 89, 92, false, true, "#bf{Data / MC}", -1, -1, "eta");
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

    return util.PlotDataMC(ratio1, dataTitle, ratio2, mcTitle, plotTitle, "|#eta|", "Median M_{ee} Data/MC", 0, 2.4999, 0.99, 1.01, false, true, "#bf{Nominal / High Et Threshold}", 0.998, 1.002, "eta");
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
    return util.PlotDataMC(data_correctedDist, dataTitle, mc_correctedDist, mcTitle, plotTitle, "R_{9}", "Median Invariant Mass [GeV]", 0.5, 0.99999, 89, 92, true, true, "#bf{ Uncorrected / Corrected }", 0.995, 1.005, "r90");
    //plotTitle = "invMass_mee2018_R9_noRatio";
    //util.PlotDataMC(data_correctedDist, dataTitle, mc_correctedDist, mcTitle, totalUnc, plotTitle, "R_{9} [GeV]", "Median Invariant Mass [GeV]", 0.5, 0.99999, 88, 94, true, false, "#bf{ Data / MC }", 0.99, 1.01, "r90");
    //util.PlotDataMC(data_loose, dataTitle, mc_loose, mcTitle, systUnc, plotTitle, "", "Median Invariant Mass [GeV]", 0, 0.999, 89, 92, false, true, "#bf{ Uncorrected / Corrected }", 0.995, 1.005, "categories");


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
        util.PlotDataMC(EBin, dataTitle, mc_EBin, mcTitle, "standardSet_"+outputFile+"_meeDist_EB_lowR9", "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", -1, -1, "");
        util.PlotDataMC(EBout, dataTitle, mc_EBout, mcTitle, "standardSet_"+outputFile+"_meeDist_EB_highR9", "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", -1, -1, "");
        util.PlotDataMC(EEin, dataTitle, mc_EEin, mcTitle, "standardSet_"+outputFile+"_meeDist_EE_lowR9", "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", -1, -1, "");
        util.PlotDataMC(EEout, dataTitle, mc_EEout, mcTitle, "standardSet_"+outputFile+"_meeDist_EE_highR9", "M_{ee} [GeV]", "Fraction", 80, 99.9999, 0, -1, true, true, "#bf{Data / MC}", -1, -1, "");
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


    return util.PlotDataMC(data_loose, dataTitle, mc_loose, mcTitle, systUnc, "standardSet_"+outputFile+"_cat_median_noStatErr", "", "Median Invariant Mass [GeV]", -1, -1, 89, 92, false, true, "#bf{ Data / MC }", -1, -1, "categories");
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
    util.PlotDataMC(data, dataTitle, mc, mcTitle, totalSyst, plotTitle, "E_{T} [GeV]", "Median Invariant Mass [GeV]", 32, 299.99999, 88, 94, true, true, "#bf{ Data / MC }", 0.99, 1.01, "et");
    plotTitle = "invMass_mee2018_Et_noRatio";
    return util.PlotDataMC(data, dataTitle, mc, mcTitle, totalSyst, plotTitle, "E_{T} [GeV]", "Median Invariant Mass [GeV]", 32, 299.99999, 88, 94, true, false, "#bf{ Data / MC }", 0.99, 1.01, "et");
    //util.PlotOne( data5 , "Et Scaled Data", "invMass_relativeRes_EE", "E_{T} [GeV]", "#sigma_{M_{ee}}/M_{ee} [%]", 20, 199.99999, 2, 6, true);
};

TCanvas * standardSet::eval_EtResolution(string data, string mc){
    TFile * file_data = new TFile(data.c_str(), "READ");
    TFile * file_mc = new TFile(mc.c_str(), "READ");

    TH2F * et_data = (TH2F*)file_data->Get("mee_et_ee");
    TH2F * et_mc = (TH2F*)file_mc->Get("mee_et_ee");

    float new_et_bins [8] = {30, 40, 50, 60, 75, 90, 110, 130};
    TH1F * res_data = new TH1F("res_data", "", 7, new_et_bins);
    TH1F * res_mc = new TH1F("res_mc", "", 7, new_et_bins);

    for(int i = 0; i < et_data->GetNbinsX(); i++){
        TH1F * temp_data = (TH1F*)et_data->ProjectionY("data", i+1, i+2, "");
        TH1F * temp_mc = (TH1F*)et_mc->ProjectionY("mc", i+1, i+2, "");
        //temp_data->Rebin(5, "", 0);
        //temp_mc->Rebin(5, "", 0);
        temp_mc->Scale(temp_data->Integral()/temp_mc->Integral());
        //util.PlotDataMC(temp_data, dataTitle, temp_mc, mcTitle, string("invMass_Et_"+std::to_string(i)).c_str(), "E_{T} [GeV]", "Events/0.25 [GeV]", 80,100, 1, -1, true, true, "#bf{ Data / MC }", 0.99, 1.01, "");
        
        if( temp_data->Integral() != 0){
            std::cout << "data events: " << temp_data->Integral() << std::endl;
            std::string tmp_str = "temp_et_data_"+std::to_string(i);

            TF1 * myFitData = fitBreitGaus_old(temp_data, true);
            float data_sig = myFitData->GetParameter(1);
            float data_sig_err = myFitData->GetParError(1);
            float data_M = myFitData->GetParameter(0);
            float data_M_err = myFitData->GetParError(0);
            
            /*
            RooFitResult * myFitData = fitBreitGaus(temp_data, true);
            RooRealVar * data_mean = (RooRealVar*)myFitData->floatParsFinal().find("mean");
            RooRealVar * data_sigma = (RooRealVar*)myFitData->floatParsFinal().find("sigma");
            float data_M = data_mean->getValV()+91.188;
            float data_M_err = sqrt(pow(data_mean->getAsymErrorHi(),2)); 
            float data_sig = data_sigma->getValV();
            std::cout << data_sig << std::endl;
            float data_sig_err = data_sigma->getAsymErrorHi();
            std::cout << data_sig << " " << data_M << " " << data_sig/data_M << std::endl;
            */

            res_data->SetBinContent(i+1, 100*data_sig/data_M);
            //res_data->SetBinContent(i+1, 100*temp_data->GetRMS()/temp_data->GetMean());
            res_data->SetBinError(i+1, res_data->GetBinContent(i+1)*sqrt( pow(data_sig_err/data_sig,2) + pow(data_M_err/data_M, 2)));
            //res_data->SetBinError(i+1, res_data->GetBinContent(i+1)*sqrt( pow(temp_data->GetRMSError()/temp_data->GetRMS(),2) + pow(temp_data->GetMeanError()/temp_data->GetMean(), 2)));
            delete myFitData;
        }
        if( temp_mc->Integral() != 0){
            std::cout << "mc events: " << temp_mc->Integral() << std::endl;
            std::string tmp_str = "temp_et_mc_"+std::to_string(i);

            TF1 * myFitMC = fitBreitGaus_old(temp_mc, true);
            float mc_sig = myFitMC->GetParameter(1);
            float mc_sig_err = myFitMC->GetParError(1);
            float mc_M = myFitMC->GetParameter(0);
            float mc_M_err = myFitMC->GetParError(0);

            /*
            RooFitResult * myFitMC = fitBreitGaus(temp_mc, false);
            RooRealVar * mc_mean = (RooRealVar*)myFitMC->floatParsFinal().find("mean");
            RooRealVar * mc_sigma = (RooRealVar*)myFitMC->floatParsFinal().find("sigma");
            float mc_M = mc_mean->getValV()+91.188;
            float mc_M_err = sqrt(pow(mc_mean->getAsymErrorHi(),2)); 
            float mc_sig = mc_sigma->getValV();
            std::cout << mc_sig << std::endl;
            float mc_sig_err = mc_sigma->getAsymErrorHi();
            */

            res_mc->SetBinContent(i+1, 100*mc_sig/mc_M);
            //res_mc->SetBinContent(i+1, 100*temp_mc->GetRMS()/temp_mc->GetMean());
            res_mc->SetBinError(i+1, res_mc->GetBinContent(i+1)*sqrt( pow(mc_sig_err/mc_sig,2) + pow(mc_M_err/mc_M, 2)));
            //res_mc->SetBinError(i+1, res_mc->GetBinContent(i+1)*sqrt( pow(temp_mc->GetRMSError()/temp_mc->GetRMS(),2) + pow(temp_mc->GetMeanError()/temp_mc->GetMean(), 2)));
            delete myFitMC;
        }
        delete temp_data;
        delete temp_mc;
    }

    return util.PlotDataMC(res_data, dataTitle, res_mc, mcTitle, "standardSet_"+outputFile+"_EtResolution_fit", "E_{T} [GeV]", "#sigma/#mu [%]", 30, 129, -1, -1, true, true, "Data/MC", 0.5, 1.5, "et3");
    
};

#endif
