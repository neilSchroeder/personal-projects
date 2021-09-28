#ifndef UTILITIES
#define UTILITIES
//contains utilties for plotting and manipulating histograms
//

#include "../interface/utilities.h"
#include "../interface/statistic.h"
#include <TF1.h>
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
#include <TGaxis.h>

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
#include <TMath.h>

#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooBreitWigner.h"
#include "RooCBShape.h"
#include "RooFFTConvPdf.h"
#include "RooPlot.h"

using namespace std;
using namespace RooFit;
extern string lumiLabel;

/*
Double_t breitGausFun(Double_t *x, Double_t *par){
    double Zwidth = 2.4952;
    return (par[2] * TMath::Voigt(x[0] - par[0], par[1], Zwidth));
}
*/

RooFitResult * utilities::fitBreitGaus(TH1 * hist, bool isData){
    if(!hist) return (RooFitResult*)0;
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
        RooRealVar mean( "mean", "mean", hist->GetBinCenter(hist->GetMaximumBin()), 85, 95 );
        RooRealVar sigma( "sigma", "sigma", hist->GetRMS(), 0, 10 );
        RooRealVar alpha( "alpha", "alpha", 0, 5);
        RooRealVar n( "n", "n", 0.81, 0, 5 );
        RooCBShape cb( "cb", "cb", invmass, mean, sigma, alpha, n );

        RooFFTConvPdf * pdf = new RooFFTConvPdf("pdf", "pdf", invmass, bw, cb);
        RooFitResult * ret = pdf->fitTo(roo_hist, "q");

        return ret;

    //need to check if fit was successful
};

/*
TF1 * fitBreitGaus_old(TH1 * hist, bool isData){
    TF1 * f1 = new TF1();
    if(hist){

        double xmin = hist->GetMean() - 2*hist->GetRMS();
        double xmax = hist->GetMean() + 2*hist->GetRMS();
        f1 = new TF1("f1", breitGausFun, xmin, xmax, 3);
        f1->SetParNames("BW mean", "Gauss Width", "Integral");

        double par[3];
        par[0] = hist->GetMean();
        par[1] = hist->GetRMS()+1;
        par[2] = hist->Integral();

        f1->SetParameters(par[0], par[1], par[2]);
        f1->SetParLimits(0, 85, 95);
        f1->SetParLimits(1, 1, 5);
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
*/

TCanvas * utilities::PlotHistWithFit(TH1F* data, string dataTitle, RooFitResult* fit, string fitTitle , string histTitle){
    if( ! data){
        std::cout << "[ERROR] data hist not viable" << std::endl;
    }
    if( ! fit){
        std::cout << "[ERROR] mc hist not viable" << std::endl;
    }
    const char * buff = histTitle.c_str();
    string temp = "";
    TCanvas * a;
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    TLine * one;
    
    a = new TCanvas(buff, "", 900, 900);
    TLatex* pave = new TLatex(0.10, 0.92, "#font[42]{#bf{CMS} #it{Preliminary}}");
    TLatex* lumi_label = new TLatex(0.97, 0.92, lumiLabel.c_str());
    lumi_label->SetTextAlign(31);
    pave->SetTextSize(0.07);
    lumi_label->SetTextSize(0.07);
	gPad->SetBottomMargin(0.03);
	gPad->SetTopMargin(0.09);
	gPad->SetLeftMargin(0.10);
	gPad->SetRightMargin(0.03);
	gPad->SetGridy(1);
	gPad->SetGridx(1);
	data->SetLineColor(kBlack);
    data->SetMarkerStyle(kFullCircle);
    data->SetMarkerSize(1.25);
    data->SetMarkerColor(kBlack);
	data->SetLineWidth(3);
	data->SetLabelSize(0.03, "Y");
	data->SetLabelSize(0.03, "X");
	data->SetYTitle("Events");
	data->SetTitleSize(0.07, "Y");
	data->SetTitleOffset(0.7, "Y");
    data->SetXTitle("M_{ee} [GeV]");
    data->SetTitleSize(0.055, "X");
    std::vector<float> vals;
    for(int i = 1; i <= data->GetNbinsX(); i++){
        if( data->GetBinContent(i)){
            vals.push_back(data->GetBinContent(i));
        }
    }

    float maxVal = *std::max_element(vals.begin(), vals.end());
    float minVal = *std::min_element(vals.begin(), vals.end());

    float yMax = 1.2*maxVal;
	data->SetAxisRange(1, yMax, "Y");
    data->GetYaxis()->SetNdivisions(510);
	data->Draw("P");
    fit->Draw("SAME");
    TLegend * legend0 = new TLegend(0.10, 0.7, 0.97, 0.91);
    legend0->AddEntry(data, dataTitle.c_str(), "p");
    legend0->AddEntry(fit, fitTitle.c_str());
    RooRealVar * mean = (RooRealVar*)fit->floatParsFinal().find("mean");
    RooRealVar * m0 = (RooRealVar*)fit->floatParsFinal().find("m0");
    RooRealVar * sigma = (RooRealVar*)fit->floatParsFinal().find("sigma");
    float M = mean->getValV() + m0->getValV();
    float M_err = sqrt( pow(mean->getAsymErrorHi(), 2) + pow(m0->getAsymErrorHi(),2)); 
    float sig = sigma->getValV();
    float sig_err = sigma->getAsymErrorHi();
    float res = 100*sig/M;
    float err = res * sqrt(pow(sig_err/sig, 2) + pow(M/M_err,2));
    legend0->AddEntry((TObject*)0, "#sigma / #mu = " + TString::Format("%4.3f #pm %4.3f", res, err) +" %", "");
    legend0->AddEntry((TObject*)0, "#sigma = " + TString::Format("%4.3f #pm %4.3f", sig, sig_err) +" %", "");
    legend0->AddEntry((TObject*)0, "#mu = " + TString::Format("%4.3f #pm %4.3f", M, M_err) +" %", "");
    legend0->Draw();
    pave->SetNDC();
    pave->Draw();
    lumi_label->SetNDC();
    lumi_label->Draw();

    a->Modified();
    string macro_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_macros/";
    string macro_post = ".C";
    string pdf_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pdfs/";
    string pdf_post = ".pdf";
    string png_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pngs/";
    string png_post = ".png";
    string title = macro_pre + histTitle + macro_post;
    a->SaveAs(title.c_str());
    title = pdf_pre + histTitle + pdf_post;
    a->SaveAs(title.c_str());
    title = png_pre + histTitle + png_post;
    a->SaveAs(title.c_str(), "");
    return a;
};

TCanvas * utilities::PlotDataMC(TH1F* data, string dataTitle, TH1F* mc, string mcTitle , string histTitle, string xTitle, string yTitle, double xMin, double xMax, double yMin, double yMax, bool kDrawErrors, bool kDrawRatioPad, string ratioTitle, double ratioMin, double ratioMax, string whichPlot){
    if( ! data){
        std::cout << "[ERROR] data hist not viable" << std::endl;
    }
    if( ! mc){
        std::cout << "[ERROR] mc hist not viable" << std::endl;
    }
    const char * buff = histTitle.c_str();
    string temp = "";
    TCanvas * a;

    bool kCategories = (whichPlot.find("categories") != std::string::npos) ? true : false;
    bool kR90 = (whichPlot.find("r90") != std::string::npos) ? true : false;
    bool kR91 = (whichPlot.find("r91") != std::string::npos) ? true : false;
    bool kR92 = (whichPlot.find("r92") != std::string::npos) ? true : false;
    bool kR93 = (whichPlot.find("r93") != std::string::npos) ? true : false;
    bool kEta = (whichPlot.find("eta") != std::string::npos) ? true : false;
    bool kEta2 = (whichPlot.find("eta2") != std::string::npos) ? true : false;
    bool kEt1 = (whichPlot.find("et1") != std::string::npos) ? true : false;
    bool kEt2 = (whichPlot.find("et2") != std::string::npos) ? true : false;
    bool kEt3 = (whichPlot.find("et3") != std::string::npos) ? true : false;

    double r9Bins0 [6] = {0.5, 0.8, 0.9, 0.92, 0.96, 1.};
    double r9Bins1 [5] = {0.5, 0.8, 0.9, 0.94, 1.};
    double r9Bins2 [11] = {0., 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.92, 0.94, 0.96, 1.};
    double r9Bins3 [17] = {0., 0.5, 0.6, 0.65, 0.7, 0.74, 0.77, 0.82, 0.84, 0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.};
    double etaBins [6] = {0., 1., 1.4442, 1.566, 2., 2.5};
    double etaBins2 [8] = {0., 0.5, 1., 1.2, 1.4442, 1.566, 2., 2.5};
    double etBins [7] = {32., 40., 50., 80., 120., 300., 500};
    double etBins2 [6] = {32., 40, 50., 80., 120., 200.};
    float new_et_bins [8] = {30, 40, 50, 60, 75, 90, 110, 130};

    TH1F* ratio;
    if(kCategories) ratio = new TH1F("ratio", "", data->GetNbinsX(), data->GetXaxis()->GetXmin(), data->GetXaxis()->GetXmax());
    else if(kR90) ratio = new TH1F("ratio", "", 5, r9Bins0);
    else if(kR91) ratio = new TH1F("ratio", "", 5, r9Bins1);
    else if(kR92) ratio = new TH1F("ratio", "", 10, r9Bins2);
    else if(kR93) ratio = new TH1F("ratio", "", 16, r9Bins3);
    else if(kEta) ratio = new TH1F("ratio", "", 5, etaBins);
    else if(kEta2) ratio = new TH1F("ratio", "", 7, etaBins2);
    else if(kEt1) ratio = new TH1F("ratio", "", 6, etBins);
    else if(kEt2) ratio = new TH1F("ratio", "", 5, etBins2);
    else if(kEt3) ratio = new TH1F("ratio", "", 7, new_et_bins);
    else ratio = new TH1F("ratio", "", data->GetNbinsX(), data->GetXaxis()->GetXmin(), data->GetXaxis()->GetXmax());

    for(int i = 1; i <= data->GetNbinsX()+1; i++){
        if(data->GetBinContent(i) != 0 && mc->GetBinContent(i) != 0 ) ratio->SetBinContent(i, data->GetBinContent(i) / mc->GetBinContent(i));
        if(kDrawErrors){
            ratio->SetBinError(i, ratio->GetBinContent(i)*sqrt( pow( data->GetBinError(i)/data->GetBinContent(i), 2)+ pow( mc->GetBinError(i)/mc->GetBinContent(i), 2)));
        }
        else{
            ratio->SetBinError(i,0.000000000000001);
            data->SetBinError(i, 0.000000000000001);
            mc->SetBinError(i, 0.000000000000001);
        }
    }


    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    TLine * one;
    
    if( kDrawRatioPad){
    a = new TCanvas(buff, "", 900, 900);
    TLatex* pave = new TLatex(0.10, 0.92, "#font[42]{#bf{CMS} #it{Preliminary}}");
    //TLatex* lumi_label = new TLatex(0.87, 0.92, "#font[42]{13 TeV}");
    //TLatex* lumi_label = new TLatex(0.65, 0.92, "#font[42]{58.8 fb^{-1} 13 TeV (2018)}");
    TLatex* lumi_label = new TLatex(0.97, 0.92, lumiLabel.c_str());
    lumi_label->SetTextAlign(31);
    pave->SetTextSize(0.07);
    lumi_label->SetTextSize(0.07);
	a->Divide(1,2);
	a->cd(1);
	gPad->SetPad("p1", "p1", 0, 0.5, 1, 1, kWhite, 0, 0);
	gPad->SetBottomMargin(0.03);
	gPad->SetTopMargin(0.09);
	gPad->SetLeftMargin(0.10);
	gPad->SetRightMargin(0.03);
	gPad->SetGridy(1);
	gPad->SetGridx(1);
	data->SetLineColor(kBlack);
    data->SetMarkerStyle(kFullCircle);
    data->SetMarkerSize(1.25);
    data->SetMarkerColor(kBlack);
	data->SetLineWidth(3);
	data->SetLabelSize(0.055, "Y");
	data->SetLabelSize(0.00, "X");
    temp = yTitle;
	data->SetYTitle(temp.c_str());
	data->SetTitleSize(0.07, "Y");
	data->SetTitleOffset(0.7, "Y");
    std::vector<float> vals;
    for(int i = 1; i <= data->GetNbinsX(); i++){
        if( data->GetBinContent(i) != 0 && mc->GetBinContent(i) != 0){
            vals.push_back(data->GetBinContent(i));
            vals.push_back(mc->GetBinContent(i));
        }
    }

    float maxVal = max(data->GetMaximum(),mc->GetMaximum());
    if( vals.size() != 0) maxVal = *std::max_element(vals.begin(), vals.end());
    float minVal = min(data->GetMinimum(), mc->GetMinimum());
    if(vals.size() != 0) minVal = *std::min_element(vals.begin(), vals.end());

    if( yMin == -1) yMin = (2./3.)*minVal;
    if( yMax == -1) yMax = 1.6*maxVal;
	data->SetAxisRange(yMin, yMax, "Y");
    data->GetYaxis()->SetNdivisions(510);
    data->SetAxisRange(xMin, xMax, "X");
    mc->SetAxisRange(xMin, xMax, "X");
    mc->SetLineColor(kAzure+3);
    mc->SetLineWidth(4);
    mc->SetFillColor(kAzure-7);
    mc->SetFillStyle(3001);
	data->Draw("P");
    mc->Draw("SAME HIST mE2");
	data->Draw("SAME E");
    TLegend * legend0 = new TLegend(0.10, 0.7, 0.97, 0.91);
    legend0->AddEntry(data, dataTitle.c_str(), "p");
    legend0->AddEntry(mc, mcTitle.c_str());
    legend0->Draw();
    pave->SetNDC();
    pave->Draw();
    lumi_label->SetNDC();
    lumi_label->Draw();
	a->cd(2);
	gPad->SetPad("p2", "p2", 0, 0, 1, 0.5, kWhite, 0, 0);
	gPad->SetBottomMargin(0.15);
	gPad->SetTopMargin(0.03);
	gPad->SetLeftMargin(0.10);
	gPad->SetRightMargin(0.03);
    gPad->SetGridy(1);
    gPad->SetGridx(1);
    one = new TLine(xMin, 1, xMax, 1);
    one->SetLineColor(kBlack);
    one->SetLineStyle(9);
    one->SetLineWidth(2);
    /////////////////////////////////
    // set ratio axis range
    std::vector<double> possibleMinima;
    std::vector<double> possibleMaxima;
    if(ratioMin == -1){
        for(int i = 1; i <= ratio->GetNbinsX(); i++){ if(mc->GetBinContent(i) != 0) possibleMinima.push_back(1-ratio->GetBinError(i));}
        possibleMinima.push_back(ratio->GetMinimum());
        ratioMin = *std::min_element(possibleMinima.begin(),possibleMinima.end());
        ratioMin = ratioMin - 0.05*(1-ratioMin);
    }
    if(ratioMax == -1){
        for(int i = 1; i <= ratio->GetNbinsX(); i++){ if(mc->GetBinContent(i) != 0) possibleMaxima.push_back(1+ratio->GetBinError(i));}
        possibleMaxima.push_back(ratio->GetMaximum());
        ratioMax = *std::max_element(possibleMaxima.begin(), possibleMaxima.end());
        ratioMax = ratioMax + 0.05*(ratioMax-1);
    }
    if (ratioMax -1 > 1-ratioMin){
        ratioMin = 2-ratioMax;
    }
    else{
        ratioMax = 2-ratioMin;
    }
    ratio->SetAxisRange(ratioMin, ratioMax, "Y");
    /////////////////////////////////
	ratio->SetLineColor(kBlack);
    ratio->SetMarkerStyle(kFullCircle);
    ratio->SetMarkerColor(kBlack);
    ratio->SetMarkerSize(1.25);
	ratio->SetLineWidth(3);
	ratio->SetYTitle(ratioTitle.c_str());
    temp = xTitle;
	ratio->SetXTitle(temp.c_str());
	ratio->SetTitleSize(0.09, "X");
    ratio->SetTitleOffset(0.7, "X");
	ratio->SetLabelSize(0.06, "X");
    ratio->SetTitleSize(0.07, "Y");
    ratio->SetTitleOffset(0.7, "Y");
    ratio->SetLabelSize(0.045, "Y");
    ratio->SetLabelFont(42);
    ratio->GetYaxis()->SetNdivisions(5, 3, 0);
    if(kCategories){
    ratio->GetXaxis()->SetBinLabel(1, "EB, Low R_{9}");
    ratio->GetXaxis()->SetBinLabel(2, "EB, High R_{9}");
    ratio->GetXaxis()->SetBinLabel(3, "EE, Low R_{9}");
    ratio->GetXaxis()->SetBinLabel(4, "EE, High R_{9}");
	ratio->SetLabelSize(0.09, "X");
    }
	ratio->Draw("E");
    ratio->Draw("same E");
    one->Draw();

    }
    else{
    a = new TCanvas(buff, "", 900, 900);
    TLatex* pave = new TLatex(0.10, 0.92, "#font[42]{#bf{CMS} #it{Preliminary}}");
    TLatex* lumi_label = new TLatex(0.52, 0.92, "#font[42]{35.9 fb^{-1} 13 TeV (2016)}");
    pave->SetTextSize(0.05);
    lumi_label->SetTextSize(0.05);
	gPad->SetBottomMargin(0.09);
	gPad->SetTopMargin(0.09);
	gPad->SetLeftMargin(0.10);
	gPad->SetRightMargin(0.03);
	gPad->SetGridy(1);
	gPad->SetGridx(1);
	data->SetLineColor(kBlack);
    data->SetMarkerStyle(kFullCircle);
    data->SetMarkerSize(1.25);
    data->SetMarkerColor(kBlack);
	data->SetLineWidth(3);
	data->SetLabelSize(0.03, "Y");
	data->SetLabelSize(0.04, "X");
    temp = yTitle;
	data->SetYTitle(temp.c_str());
    temp = xTitle;
    data->SetXTitle(temp.c_str());
	data->SetTitleSize(0.04, "Y");
    data->SetTitleSize(0.04, "X");
    data->SetTitleOffset(0.85,"X");
	data->SetTitleOffset(1.1, "Y");
	data->SetAxisRange(yMin, yMax, "Y");
    data->GetYaxis()->SetNdivisions(510);
    std::vector<float> vals;
    for(int i = 1; i <= data->GetNbinsX(); i++){
        if( data->GetBinContent(i) != 0 && mc->GetBinContent(i) != 0){
            vals.push_back(data->GetBinContent(i));
            vals.push_back(mc->GetBinContent(i));
        }
    }

    float maxVal = max(data->GetMaximum(),mc->GetMaximum());
    if( vals.size() != 0) maxVal = *std::max_element(vals.begin(), vals.end());
    float minVal = min(data->GetMinimum(), mc->GetMinimum());
    if(vals.size() != 0) minVal = *std::min_element(vals.begin(), vals.end());

    if( yMin == -1) yMin = (2./3.)*minVal;
    if( yMax == -1) yMax = 1.6*maxVal;
    data->SetAxisRange(yMin, yMax, "Y");
    mc->SetAxisRange(yMin, yMax, "Y");
    data->SetAxisRange(xMin, xMax, "X");
    mc->SetAxisRange(xMin, xMax, "X");
    mc->SetLineColor(kAzure+3);
    mc->SetLineWidth(4);
    mc->SetFillColor(kAzure-7);
    mc->SetFillStyle(3001);
    if( kDrawErrors ){
	data->Draw("P");
    mc->Draw("SAME HIST E2");
    data->Draw("SAME E P");
    }
    else{
        data->Draw("P");
        mc->Draw("SAME HIST");
        data->Draw("SAME P");
    }
    TLegend * legend0 = new TLegend(0.10, 0.7, 0.97, 0.91);
    legend0->AddEntry(data, dataTitle.c_str(), "p");
    legend0->AddEntry(mc, mcTitle.c_str());
    legend0->Draw();
    pave->SetNDC();
    pave->Draw();
    lumi_label->SetNDC();
    lumi_label->Draw();
    }
    a->Modified();
    string macro_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_macros/";
    string macro_post = ".C";
    string pdf_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pdfs/";
    string pdf_post = ".pdf";
    string png_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pngs/";
    string png_post = ".png";
    string title = macro_pre + histTitle + macro_post;
    a->SaveAs(title.c_str());
    title = pdf_pre + histTitle + pdf_post;
    a->SaveAs(title.c_str());
    title = png_pre + histTitle + png_post;
    a->SaveAs(title.c_str(), "");
    delete ratio;
    return a;
};

TCanvas * utilities::PlotDataMC(TH1F* data, string dataTitle, TH1F* mc, string mcTitle , TH1F* systs, string histTitle, string xTitle, string yTitle, double xMin, double xMax, double yMin, double yMax, bool kDrawErrors, bool kDrawRatioPad, string ratioTitle, double ratioMin, double ratioMax, string whichPlot){
    std::cout << "[utilities::INFO] running the function utilities::PlotDataMC @line 346" << std::endl;
    if(!kDrawErrors) std::cout << "[utilities::INFO] neglecting error bars" << std::endl;
    const char * buff = histTitle.c_str();
    string temp = "";
    TCanvas * a;

    bool kCategories = (whichPlot.find("categories") != std::string::npos) ? true : false;
    bool kR90 = (whichPlot.find("r90") != std::string::npos) ? true : false;
    bool kR91 = (whichPlot.find("r91") != std::string::npos) ? true : false;
    bool kR92 = (whichPlot.find("r92") != std::string::npos) ? true : false;
    bool kR93 = (whichPlot.find("r93") != std::string::npos) ? true : false;
    bool kEta = (whichPlot.find("eta") != std::string::npos) ? true : false;
    bool kEta2 = (whichPlot.find("eta2") != std::string::npos) ? true : false;
    bool kEt1 = (whichPlot.find("et") != std::string::npos) ? true : false;
    bool kEt2 = (whichPlot.find("et2") != std::string::npos) ? true : false;

    double r9Bins0 [6] = {0.5, 0.8, 0.9, 0.92, 0.96, 1.};
    double r9Bins1 [5] = {0., 0.8, 0.9, 0.94, 1.};
    double r9Bins2 [11] = {0., 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.92, 0.94, 0.96, 1.};
    double r9Bins3 [17] = {0., 0.5, 0.6, 0.65, 0.7, 0.74, 0.77, 0.82, 0.84, 0.86, 0.88, 0.90, 0.92, 0.94, 0.96, 0.98, 1.};
    double etaBins [6] = {0., 1., 1.4442, 1.566, 2., 2.5};
    double etaBins2 [8] = {0., 0.5, 1., 1.2, 1.4442, 1.566, 2., 2.5};
    double etBins [7] = {32., 40., 50., 80., 120., 300., 500};
    double etBins2 [6] = {32., 40, 50., 80., 120., 200.};
    TH1F* ratio;
    if(kCategories) ratio = new TH1F("ratio", "", data->GetNbinsX(), data->GetXaxis()->GetXmin(), data->GetXaxis()->GetXmax());
    else if(kR90) ratio = new TH1F("ratio", "", 5, r9Bins0);
    else if(kR91) ratio = new TH1F("ratio", "", 5, r9Bins1);
    else if(kR92) ratio = new TH1F("ratio", "", 10, r9Bins2);
    else if(kR93) ratio = new TH1F("ratio", "", 16, r9Bins3);
    else if(kEta) ratio = new TH1F("ratio", "", 5, etaBins);
    else if(kEta2) ratio = new TH1F("ratio", "", 7, etaBins2);
    else if(kEt1) ratio = new TH1F("ratio", "", 6, etBins);
    else if(kEt2) ratio = new TH1F("ratio", "", 5, etBins2);
    else ratio = new TH1F("ratio", "", data->GetNbinsX(), data->GetXaxis()->GetXmin(), data->GetXaxis()->GetXmax());
    for(int i = 1; i <= data->GetNbinsX()+1; i++){
        if(data->GetBinContent(i) != 0 && mc->GetBinContent(i) != 0 ){
            ratio->SetBinContent(i, data->GetBinContent(i) / mc->GetBinContent(i));
            if( kDrawErrors){
                ratio->SetBinError(i, ratio->GetBinContent(i)*sqrt( pow( data->GetBinError(i)/data->GetBinContent(i), 2) + pow( mc->GetBinError(i)/mc->GetBinContent(i), 2)));
            }
            else{
                ratio->SetBinError(i,0.000000000000001);
                data->SetBinError(i, 0.000000000000001);
                mc->SetBinError(i, 0.000000000000001);
            }
        }
    }
    

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    TLine * one;
    
    if( kDrawRatioPad){
        a = new TCanvas(buff, "", 900, 900);
        TLatex* pave = new TLatex(0.10, 0.92, "#font[42]{#bf{CMS} #it{Preliminary}}");
        //TLatex* lumi_label = new TLatex(0.87, 0.92, "#font[42]{13 TeV}");
        TLatex* lumi_label = new TLatex(0.97, 0.92, lumiLabel.c_str());
        lumi_label->SetTextAlign(31);
        pave->SetTextSize(0.07);
        lumi_label->SetTextSize(0.07);
        a->Divide(1,2);
        a->cd(1);
        gPad->SetPad("p1", "p1", 0, 0.5, 1, 1, kWhite, 0, 0);
        gPad->SetBottomMargin(0.03);
        gPad->SetTopMargin(0.09);
        gPad->SetLeftMargin(0.10);
        gPad->SetRightMargin(0.03);
        gPad->SetGridy(1);
        gPad->SetGridx(1);
        data->SetLineColor(kBlack);
        data->SetMarkerStyle(kFullCircle);
        data->SetMarkerSize(1.25);
        data->SetMarkerColor(kBlack);
        data->SetLineWidth(3);
        data->SetLabelSize(0.055, "Y");
        data->SetLabelSize(0.00, "X");
        temp = yTitle;
        data->SetYTitle(temp.c_str());
        data->SetTitleSize(0.07, "Y");
        data->SetTitleOffset(0.7, "Y");
        //get boundaries:
        std::vector<float> vals;
        for(int i = 1; i <= data->GetNbinsX(); i++){
            if( data->GetBinContent(i) != 0 && mc->GetBinContent(i) != 0){
                vals.push_back(data->GetBinContent(i));
                vals.push_back(mc->GetBinContent(i));
            }
        }

        float maxVal = *std::max_element(vals.begin(), vals.end());
        float minVal = *std::min_element(vals.begin(), vals.end());

        if( yMin == -1) yMin = (2./3.)*minVal;
        if( yMax == -1) yMax = 1.6*maxVal;
        data->SetAxisRange(yMin, yMax, "Y");
        data->GetYaxis()->SetNdivisions(510);
        data->SetAxisRange(xMin, xMax, "X");
        mc->SetAxisRange(xMin, xMax, "X");
        mc->SetLineColor(kAzure+3);
        mc->SetLineWidth(4);
        mc->SetMarkerStyle(43);
        mc->SetMarkerSize(2.5);
        mc->SetMarkerColor(kAzure+3);
        mc->SetFillColor(kAzure-7);
        mc->SetFillStyle(3001);
        ratio->SetAxisRange(xMin, xMax, "X");
        data->Draw("P");
        mc->Draw("SAME HIST E2");
        data->Draw("SAME E");
        TLegend * legend0 = new TLegend(0.10, 0.7, 0.97, 0.91);
        legend0->AddEntry(data, dataTitle.c_str(), "p");
        legend0->AddEntry(mc, mcTitle.c_str());
        legend0->Draw();
        pave->SetNDC();
        pave->Draw();
        lumi_label->SetNDC();
        lumi_label->Draw();
        a->cd(2);
        gPad->SetPad("p2", "p2", 0, 0, 1, 0.5, kWhite, 0, 0);
        gPad->SetBottomMargin(0.15);
        gPad->SetTopMargin(0.03);
        gPad->SetLeftMargin(0.10);
        gPad->SetRightMargin(0.03);
        gPad->SetGridy(1);
        gPad->SetGridx(1);
        one = new TLine(xMin, 1, xMax, 1);
        one->SetLineColor(kBlack);
        one->SetLineStyle(9);
        one->SetLineWidth(2);
        /////////////////////////////////
        // set ratio axis range
        if(ratioMin == -1){
            std::vector<double> possibleMinima;
            for(int i = 1; i <= systs->GetNbinsX(); i++){ 
                if(mc->GetBinContent(i) != 0){
                    possibleMinima.push_back(1-systs->GetBinError(i));
                    possibleMinima.push_back(ratio->GetBinContent(i));
                }
            }
            ratioMin = *std::min_element(possibleMinima.begin(),possibleMinima.end());
            ratioMin = ratioMin - 0.05*(1-ratioMin);
        }
        if(ratioMax == -1){
            std::vector<double> possibleMaxima;
            for(int i = 1; i <= systs->GetNbinsX(); i++){ 
                if(mc->GetBinContent(i) != 0){
                    possibleMaxima.push_back(1+systs->GetBinError(i));
                    possibleMaxima.push_back(ratio->GetBinContent(i));
                }
            }
            ratioMax = *std::max_element(possibleMaxima.begin(), possibleMaxima.end());
            ratioMax = ratioMax + 0.05*(ratioMax-1);
        }
        if (ratioMax -1 > 1-ratioMin){
            ratioMin = 2-ratioMax;
        }
        else{
            ratioMax = 2-ratioMin;
        }
        ratio->SetAxisRange(ratioMin, ratioMax, "Y");
        /////////////////////////////////
        ratio->SetLineColor(kBlack);
        ratio->SetMarkerStyle(kFullCircle);
        ratio->SetMarkerColor(kBlack);
        ratio->SetMarkerSize(1.25);
        ratio->SetLineWidth(3);
        ratio->SetYTitle(ratioTitle.c_str());
        temp = xTitle;
        ratio->SetXTitle(temp.c_str());
        ratio->SetTitleSize(0.09, "X");
        ratio->SetTitleOffset(0.7, "X");
        ratio->SetLabelSize(0.06, "X");
        ratio->SetTitleSize(0.07, "Y");
        ratio->SetTitleOffset(0.7, "Y");
        ratio->SetLabelSize(0.045, "Y");
        ratio->SetLabelFont(42);
        ratio->GetYaxis()->SetNdivisions(5, 3, 0);
        if(kCategories){
            ratio->GetXaxis()->SetBinLabel(1, "EB, Low R_{9}");
            ratio->GetXaxis()->SetBinLabel(2, "EB, High R_{9}");
            ratio->GetXaxis()->SetBinLabel(3, "EE, Low R_{9}");
            ratio->GetXaxis()->SetBinLabel(4, "EE, High R_{9}");
            ratio->SetLabelSize(0.09, "X");
        }
        systs->SetLineColor(kBlack);
        systs->SetLineWidth(0);
        systs->SetFillColor(kBlack);
        systs->SetFillStyle(3002);
        TLegend * legend1 = new TLegend(0.1, 0.87, 0.45, 0.97);
        legend1->AddEntry(systs, "Systematic Uncertainty", "f");
        ratio->Draw("E");
        systs->Draw("same E2");
        ratio->Draw("same E");
        one->Draw();
        legend1->Draw();
    }
    else{
        a = new TCanvas(buff, "", 900, 900);
        TLatex* pave = new TLatex(0.10, 0.92, "#font[42]{#bf{CMS} #it{Preliminary}}");
        TLatex* lumi_label = new TLatex(0.52, 0.92, "#font[42]{35.9 fb^{-1} 13 TeV (2016)}");
        pave->SetTextSize(0.05);
        lumi_label->SetTextSize(0.05);
        gPad->SetBottomMargin(0.09);
        gPad->SetTopMargin(0.09);
        gPad->SetLeftMargin(0.10);
        gPad->SetRightMargin(0.03);
        gPad->SetGridy(1);
        gPad->SetGridx(1);
        data->SetLineColor(kBlack);
        data->SetMarkerStyle(kFullCircle);
        data->SetMarkerSize(1.25);
        data->SetMarkerColor(kBlack);
        data->SetLineWidth(3);
        data->SetLabelSize(0.035, "Y");
        data->SetLabelSize(0.04, "X");
        temp = yTitle;
        data->SetYTitle(temp.c_str());
        temp = xTitle;
        data->SetXTitle(temp.c_str());
        data->SetTitleSize(0.04, "Y");
        data->SetTitleSize(0.04, "X");
        data->SetTitleOffset(0.75,"X");
        data->SetTitleOffset(1.0, "Y");
        data->SetAxisRange(yMin, yMax, "Y");
        data->GetYaxis()->SetNdivisions(510);
        data->SetAxisRange(xMin, xMax, "X");
        mc->SetAxisRange(xMin, xMax, "X");
        mc->SetLineColor(kAzure+3);
        mc->SetLineWidth(4);
        mc->SetFillColor(kAzure-7);
        mc->SetFillStyle(3001);
        if( kDrawErrors ){
            data->Draw("P");
            mc->Draw("SAME HIST E2");
            data->Draw("SAME E P");
        }
        else{
            data->Draw("P");
            mc->Draw("SAME HIST");
            data->Draw("SAME P");
        }
        TLegend * legend0 = new TLegend(0.10, 0.7, 0.97, 0.91);
        legend0->AddEntry(data, dataTitle.c_str(), "p");
        legend0->AddEntry(mc, mcTitle.c_str());
        legend0->Draw();
        pave->SetNDC();
        pave->Draw();
        lumi_label->SetNDC();
        lumi_label->Draw();
    }
    a->Modified();
    string macro_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_macros/";
    string macro_post = ".C";
    string pdf_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pdfs/";
    string pdf_post = ".pdf";
    string png_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pngs/";
    string png_post = ".png";
    string title = macro_pre + histTitle + macro_post;
    a->SaveAs(title.c_str());
    title = pdf_pre + histTitle + pdf_post;
    a->SaveAs(title.c_str());
    title = png_pre + histTitle + png_post;
    a->SaveAs(title.c_str(), "");
    std::cout << std::endl;
    delete ratio;
    return a;
};

TCanvas * utilities::PlotData(TH1F* data, string dataTitle, string histTitle, string xTitle, string yTitle, double xMin, double xMax, bool kDrawErrors){
    std::cout << "[utilities::INFO] running the function utilities::PlotDataMC @line 289" << std::endl;
    if(!kDrawErrors) std::cout << "[utilities::INFO] neglecting error bars" << std::endl;
    const char * buff = histTitle.c_str();
    string temp = "";
    TCanvas * a;

    for(int i = 1; i <= data->GetNbinsX()+1; i++){
        if( !kDrawErrors){
            data->SetBinError(i, 0.000000000000001);
        }
    }
    
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    TLine * one;
    
    a = new TCanvas(buff, "", 900, 900);
    TLatex* pave = new TLatex(0.10, 0.92, "#font[42]{#bf{CMS} #it{Preliminary}}");
    TLatex* lumi_label = new TLatex(0.52, 0.92, "#font[42]{35.9 fb^{-1} 13 TeV (2016)}");
    pave->SetTextSize(0.05);
    lumi_label->SetTextSize(0.05);
    gPad->SetBottomMargin(0.09);
    gPad->SetTopMargin(0.09);
    gPad->SetLeftMargin(0.10);
    gPad->SetRightMargin(0.03);
    gPad->SetGridy(1);
    gPad->SetGridx(1);
    data->SetLineColor(kBlack);
    data->SetMarkerStyle(kFullCircle);
    data->SetMarkerSize(1.25);
    data->SetMarkerColor(kBlack);
    data->SetLineWidth(3);
    data->SetLabelSize(0.035, "Y");
    data->SetLabelSize(0.04, "X");
    temp = yTitle;
    data->SetYTitle(temp.c_str());
    temp = xTitle;
    data->SetXTitle(temp.c_str());
    data->SetTitleSize(0.04, "Y");
    data->SetTitleSize(0.04, "X");
    data->SetTitleOffset(0.75,"X");
    data->SetTitleOffset(1.0, "Y");
    data->SetAxisRange(0, data->GetMaximum()*1.5, "Y");
    data->GetYaxis()->SetNdivisions(510);
    data->SetAxisRange(xMin, xMax, "X");
    if( kDrawErrors ){
        data->Draw("P");
        data->Draw("SAME E P");
    }
    else{
        data->Draw("P");
        data->Draw("SAME P");
    }
    TLegend * legend0 = new TLegend(0.10, 0.8, 0.97, 0.91);
    legend0->AddEntry(data, dataTitle.c_str(), "p");
    legend0->Draw();
    pave->SetNDC();
    pave->Draw();
    lumi_label->SetNDC();
    lumi_label->Draw();

    a->Modified();
    string macro_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/data_linearity/plots_macros/";
    string macro_post = ".C";
    string png_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/data_linearity/plots_pngs/";
    string png_post = ".png";
    string title = macro_pre + histTitle + macro_post;
    a->SaveAs(title.c_str());
    title = png_pre + histTitle + png_post;
    a->SaveAs(title.c_str(), "");
    std::cout << std::endl;
    return a;
};

TH1F * utilities::GetMedianProfilesWithError(TH2F* data){
    int thisId = rand();
    char name [50];
    sprintf( name, "%i", thisId);
    TH1F * ret = new TH1F(name, "", data->GetNbinsX(), data->GetXaxis()->GetXmin(), data->GetXaxis()->GetXmax());
    for(int i = 0; i < data->GetNbinsX(); i++){
        TH1F * temp = (TH1F*)data->ProjectionY("", i, i+1, "");

        if( temp->Integral() != 0){
            errors * error = statistic::getHistStatistic(temp, "median", 100, 500, 0.95);
            ret->SetBinContent(i, error->x);
            ret->SetBinError(i, temp->GetMeanError());
        } else {
            ret->SetBinContent(i, 0.);
            ret->SetBinError(i, 0.);
        }
        delete temp;
    }
    return ret;
};

TH1F * utilities::GetMedianProfilesWithError(TH2F* data, string dataTitle, string histTitle, string xTitle, string yTitle, double xMin, double xMax, double yMin, double yMax, bool kDrawErrors){
    int thisId = rand();
    char name [50];
    sprintf( name, "%i", thisId);
    TH1F * ret = new TH1F(name, "", data->GetNbinsX(), data->GetXaxis()->GetXmin(), data->GetXaxis()->GetXmax());
    for(int i = 0; i < data->GetNbinsX(); i++){
        TH1F * temp = (TH1F*)data->ProjectionY("", i, i+1, "");
        std::cout << i << std::endl;
        
        if( temp->Integral() != 0){
            std::string tmp_str = histTitle + "_" + std::to_string(i);
            if(i < 10){
                tmp_str = histTitle + "_0" + std::to_string(i);
            }
            PlotData(temp, dataTitle, tmp_str, xTitle, yTitle, xMin, xMax, kDrawErrors);
            //errors * error = statistic::getHistStatistic(temp, "median", 100, 500, 0.95);
            //ret->SetBinContent(i, error->x);
            //ret->SetBinError(i, max(error->errLow, error->errHigh));
        } else {
            ret->SetBinContent(i, 0.);
            ret->SetBinError(i, 0.);
        }
        delete temp;
    }
    return ret;
};


TH1F * utilities::GetMedianProfilesWithError(TH2F* mc1, TH2F* mc2){

    int thisId = rand();
    char name [50];
    sprintf( name, "%i", thisId);
    TH1F * ret = new TH1F(name, "", mc1->GetNbinsX(), mc1->GetXaxis()->GetXmin(), mc1->GetXaxis()->GetXmax());
    TH1F * temp1;
    TH1F * temp2;
    for(int i = 1; i <= mc1->GetNbinsX(); i++){
        temp1 = (TH1F*)mc1->ProjectionY("temp1", i-1, i, "");
        temp2 = (TH1F*)mc2->ProjectionY("temp2", i-1, i, "");
        double integral1 = temp1->GetEntries(); 
        double integral2 = temp2->GetEntries();
        if( temp1->Integral() != 0 && temp2->Integral() != 0 ) {
            errors * err1 = statistic::getHistStatistic(temp1, "median", 100, 200, 0.95);
            errors * err2 = statistic::getHistStatistic(temp2, "median", 100, 200, 0.95);
            double error1 = temp1->GetRMS()/sqrt(temp1->GetEntries()); 
            double error2 = temp2->GetRMS()/sqrt(temp2->GetEntries());
            ret->SetBinContent(i, (err1->x*error1+err2->x*error2)/(error1+error2));
            ret->SetBinError(i, sqrt(pow(pow(error1, 2)/(error1+error2),2) + pow(pow(error2,2)/(error1+error2),2)));
        }
    }
    ret->ComputeIntegral(0);
    return ret;

};

double utilities::EvaluateChiSquare(TH1F * data, TH1F * mc, double xMin, double xMax){
    double chi_square = 0;
    double countBins = 0;
    for(int i = data->FindBin(xMin); i <= data->FindBin(xMax); i++){
        countBins++;
        if( data->GetBinError(i) != 0 && mc->GetBinError(i) != 0){
            chi_square += pow( (data->GetBinContent(i) - mc->GetBinContent(i)),2)/(pow(data->GetBinError(i), 2) + pow(mc->GetBinError(i), 2));
            /*
               cout << chi_square << endl;
               cout << pow( (data->GetBinContent(i) - mc->GetBinContent(i)),2) << " " << data->GetBinContent(i) << " " << mc->GetBinContent(i) << endl;
               cout << pow(data->GetBinError(i), 2) + pow(mc->GetBinError(i), 2) << " " << data->GetBinError(i) << " " << mc->GetBinError(i) << endl << endl;
               */
        }
    }
    return chi_square/(countBins-1);
};

TCanvas * utilities::dataDataPlot_wRatio( TH1F * data, TH1F * mc, std::string out, int EB_EE, std::string dataTitle, std::string mcTitle){
    out = "dataData_"+out;
    std::string histTitle = "";
    std::string plot_label = "";
    switch( EB_EE ){
        case 0:
            histTitle = out+"_EB_highR9_wRatioPad";
            plot_label = "#font[42]{Barrel-Barrel}";
            break;
        case 1:
            histTitle = out+"_pt_55_65_wRatioPad";
            plot_label = "#font[42]{55 < p_{T, lead}^{e} < 65}";
            break;
        case 2:
            histTitle = out+"_notEbEb_wRatioPad";
            plot_label = "#font[42]{Not Barrel-Barrel}";
            break;
        case 3:
            histTitle = out+"_EbEb_inclusive_wRatioPad";
            plot_label = "#font[42]{Barrel-Barrel}";
            break;
        case 4:
            histTitle = out+"_EeEe_inclusive_wRatioPad";
            plot_label = "#font[42]{Endcap-Endcap}";
            break;
        case 5:
            histTitle = out+"_pt_40_55_wRatioPad";
            plot_label = "#font[42]{40 < p_{T, lead}^{e} < 55}";
            break;
        case 6:
            histTitle = out+"_pt_65_90_wRatioPad";
            plot_label = "#font[42]{65 < p_{T, lead}^{e} < 90}";
            break;
        case 7:
            histTitle = out+"_ptDiag_55_65_wRatioPad";
            plot_label = "#font[42]{55 < p_{T, lead}^{e} < 65 + |#eta| < 1.4442}";
            break;
        case 8:
            histTitle = out+"_pt_90_inf_wRatioPad";
            plot_label = "#font[42]{90 < p_{T, lead}^{e}}";
            break;
    }
    const char * buff = histTitle.c_str();
    string temp = "";
    TCanvas * a;

    if(!data){
        std::cout << "[ERROR] invalid data hist" << std::endl;
        return a;
    }
    if(!mc){
        std::cout << "[ERROR] invalid data hist" << std::endl;
        return a;
    }

    a = new TCanvas(buff, "", 900, 900);
    a->Divide(1,2);
    a->cd(1);
    gPad->SetPad("p1", "p1", 0, 0.35, 1, 1, kWhite, 0, 0);
    gPad->SetBottomMargin(0.01);
    gPad->SetTopMargin(0.09);
    gPad->SetLeftMargin(0.1);
    gPad->SetRightMargin(0.03);
    gPad->SetGridx(1);
    gPad->SetGridy(1);


    TH1F * ratio = new TH1F("ratio", "", mc->GetNbinsX(), 80, 100);
    for(int i = 1; i <= mc->GetNbinsX(); i++){
        ratio->SetBinContent(i, data->GetBinContent(i)/mc->GetBinContent(i));
        ratio->SetBinError(i, data->GetBinError(i)/data->GetBinContent(i));
    }

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    TLine * one;
    TLegend * legend0 = new TLegend(0.5,0.7,0.97,0.91,NULL,"brNDC");
    legend0->SetBorderSize(1);
    legend0->SetTextSize(0.0475);
    legend0->SetLineColor(1);
    legend0->SetLineStyle(1);
    legend0->SetLineWidth(1);
    legend0->SetFillColor(0);
    legend0->SetFillStyle(1001);

    TLatex* pave = new TLatex(0.10, 0.92, "#font[42]{#bf{CMS} #it{Preliminary}}");
    TLatex* lumi_label = new TLatex(0.97, 0.92, lumiLabel.c_str());
    TLatex* barrel = new TLatex(0.13, 0.8, plot_label.c_str());
    TLatex* r9 = new TLatex(0.13, 0.72, "#font[42]{R9 > 0.96}");
    pave->SetTextSize(0.075);
    lumi_label->SetTextSize(0.075);
    lumi_label->SetTextAlign(31);
    barrel->SetTextSize(0.05);
    r9->SetTextSize(0.05);
    gPad->SetTicks();
	data->SetLineColor(kBlack);
    data->SetMarkerStyle(kFullCircle);
    data->SetMarkerSize(0.95);
    data->SetMarkerColor(kBlack);
	data->SetLineWidth(2);
    data->SetXTitle("m_{ee} (GeV)");
    data->SetYTitle("Events / 0.25 GeV");
	data->SetLabelSize(0.05, "Y");
	data->SetLabelSize(0.00, "X");
	data->SetTitleSize(0.06, "Y");
	data->SetTitleSize(0.07, "X");
	data->SetTitleOffset(0.8, "Y");
	data->SetTitleOffset(0.75, "X");
    float myMax = data->GetMaximum();
    //for some unknown reason the following line changes the maximum of the data
	data->SetAxisRange(1, data->GetMaximum()*1.53, "Y");
    data->GetYaxis()->SetNdivisions(510);
    double xMin = 80;
    double xMax = 99.999999999;
    data->SetAxisRange(xMin, xMax, "X");
    mc->SetAxisRange(xMin, xMax, "X");
    mc->SetLineColor(kBlack);
    mc->SetLineWidth(2);
    Int_t ci = 1185;
    TColor * color = new TColor(ci, 0.619, 0.619, 0.773, " ", 0.5);
    mc->SetFillColor(ci);
    mc->SetFillStyle(3001);
    if( data->GetMaximum() < 10000) TGaxis::SetMaxDigits(2);
    else TGaxis::SetMaxDigits(4);
    TGaxis::SetExponentOffset(-0.05, 1, "y");
    TLatex * tex = new TLatex(0.050,0.91,"#scale[0.6]{#times10^{3}}");
    tex->SetNDC();
    tex->SetTextAlign(12);
    tex->SetLineWidth(2);
	data->Draw("P");

    mc->Draw("SAME HIST E");
	data->Draw("SAME E");
    std::cout << dataTitle << " " << mcTitle << std::endl;
    std::cout << dataTitle.c_str() << " " << mcTitle.c_str() << std::endl;
    legend0->AddEntry(data, dataTitle.c_str(), "pe");
    legend0->AddEntry(mc, mcTitle.c_str(), "f");
    legend0->Draw();
    pave->SetNDC();
    pave->Draw();
    lumi_label->SetNDC();
    lumi_label->Draw();
    barrel->SetNDC();
    barrel->Draw();
    r9->SetNDC();
    tex->Draw();
    if( EB_EE == 0 ) r9->Draw();
    data->Draw("same e");

    //pad 2 ( ratio pad )
    a->cd(2);
	gPad->SetPad("p2", "p2", 0, 0, 1, 0.35, kWhite, 0, 0);
	gPad->SetBottomMargin(0.20);
	gPad->SetTopMargin(0.01);
	gPad->SetLeftMargin(0.10);
	gPad->SetRightMargin(0.03);
    gPad->SetGridy(1);
    gPad->SetGridx(1);
    one = new TLine(xMin, 1, xMax, 1);
    one->SetLineColor(kBlack);
    one->SetLineStyle(9);
    one->SetLineWidth(2);

	ratio->SetLineColor(kBlack);
    ratio->SetMarkerStyle(kFullCircle);
    ratio->SetMarkerColor(kBlack);
    ratio->SetMarkerSize(1.25);
	ratio->SetLineWidth(2);
	ratio->SetYTitle("Data / MC");
	ratio->SetXTitle("m_{ee} (GeV)");
	ratio->SetTitleSize(0.111, "X");
    ratio->SetTitleOffset(0.8, "X");
	ratio->SetLabelSize(0.075, "X");
    ratio->SetLabelOffset(0.01, "X");
    ratio->SetTitleSize(0.111, "Y");
    ratio->SetTitleOffset(0.4, "Y");
    ratio->SetLabelSize(0.06, "Y");
    ratio->SetLabelFont(42);
    ratio->GetYaxis()->SetNdivisions(5, 3, 0);
    ratio->GetYaxis()->SetRangeUser(0.75, 1.25);
	ratio->Draw("E");
    one->Draw();

    TLegend * legend1 = new TLegend(0.5, 0.85, 0.97, 0.99);
    legend1->AddEntry(ratio, "Ratio", "pl");
	ratio->Draw("E");
    one->Draw();
    legend1->Draw();

    a->Modified();
    string macro_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_macros/";
    string macro_post = ".C";
    string pdf_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pdfs/";
    string pdf_post = ".pdf";
    string png_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pngs/";
    string png_post = ".png";
    string title = macro_pre + histTitle + macro_post;
    a->SaveAs(title.c_str());
    title = pdf_pre + histTitle + pdf_post;
    a->SaveAs(title.c_str());
    title = png_pre + histTitle + png_post;
    a->SaveAs(title.c_str(), "");
    
    //reset the max for the sake of any following functions
    data->SetMaximum(myMax);
    return a;
};

TCanvas * utilities::moneyPlot_wRatio( TH1F * data, TH1F * mc, TH1F * systematics, std::string out, int EB_EE){
    out = "money_"+out;
    std::string histTitle = "";
    std::string plot_label = "";
    switch( EB_EE ){
        case 0:
            histTitle = out+"_EB_highR9_wRatioPad";
            plot_label = "#font[42]{Barrel-Barrel}";
            break;
        case 1:
            histTitle = out+"_pt_55_65_wRatioPad";
            plot_label = "#font[42]{55 < p_{T, lead}^{e} < 65}";
            break;
        case 2:
            histTitle = out+"_notEbEb_wRatioPad";
            plot_label = "#font[42]{Not Barrel-Barrel}";
            break;
        case 3:
            histTitle = out+"_EbEb_inclusive_wRatioPad";
            plot_label = "#font[42]{Barrel-Barrel}";
            break;
        case 4:
            histTitle = out+"_EeEe_inclusive_wRatioPad";
            plot_label = "#font[42]{Endcap-Endcap}";
            break;
        case 5:
            histTitle = out+"_pt_40_55_wRatioPad";
            plot_label = "#font[42]{40 < p_{T, lead}^{e} < 55}";
            break;
        case 6:
            histTitle = out+"_pt_65_90_wRatioPad";
            plot_label = "#font[42]{65 < p_{T, lead}^{e} < 90}";
            break;
        case 7:
            histTitle = out+"_ptDiag_55_65_wRatioPad";
            plot_label = "#font[42]{55 < p_{T, lead}^{e} < 65 + |#eta| < 1.4442}";
            break;
        case 8:
            histTitle = out+"_pt_90_inf_wRatioPad";
            plot_label = "#font[42]{90 < p_{T, lead}^{e}}";
            break;
        case 9:
            histTitle = out+"_gain6_EB_wRatioPad";
            plot_label = "#font[42]{gain 6, Barrel-Barrel}";
            break;
        case 10:
            histTitle = out+"_gain6_EE_wRatioPad";
            plot_label = "#font[42]{gain 6, Endcap-Endcap}";
            break;
        case 11:
            histTitle = out+"_gain1_EB_wRatioPad";
            plot_label = "#font[42]{gain 1, Barrel-Barrel}";
            break;
        case 12:
            histTitle = out+"_gain1_EE_wRatioPad";
            plot_label = "#font[42]{gain 1, Endcap-Endcap}";
            break;
        case 13:
            histTitle = out+"_EbEb_ptz200_wRatioPad";
            plot_label = "#scale[0.8]{#font[42]{Barrel-Barrel, P_{T}(Z) > 200 GeV}}";
            break;
        case 14:
            histTitle = out+"_EeEe_ptz200_wRatioPad";
            plot_label = "#scale[0.8]{#font[42]{Endcap-Endcap, P_{T}(Z) > 200 GeV}}";
            break;
    }
    const char * buff = histTitle.c_str();
    string temp = "";
    TCanvas * a;

    if(!data){
        std::cout << "[ERROR] invalid data hist" << std::endl;
        return a;
    }
    if(!mc){
        std::cout << "[ERROR] invalid data hist" << std::endl;
        return a;
    }

    a = new TCanvas(buff, "", 900, 900);
    a->Divide(1,2);
    a->cd(1);
    gPad->SetPad("p1", "p1", 0, 0.35, 1, 1, kWhite, 0, 0);
    gPad->SetBottomMargin(0.01);
    gPad->SetTopMargin(0.09);
    gPad->SetLeftMargin(0.1);
    gPad->SetRightMargin(0.03);
    gPad->SetGridx(1);
    gPad->SetGridy(1);


    TH1F* errors = (TH1F*)mc->Clone();
    TH1F * systs = (TH1F*)systematics->Clone();
    TH1F * ratio = new TH1F("ratio", "", systematics->GetNbinsX(), 80, 100);
    for(int i = 1; i <= systematics->GetNbinsX(); i++){
        errors->SetBinError(i, systematics->GetBinContent(i));
        ratio->SetBinContent(i, data->GetBinContent(i)/mc->GetBinContent(i));
        ratio->SetBinError(i, data->GetBinError(i)/data->GetBinContent(i));
        systs->SetBinError(i, systematics->GetBinContent(i)/mc->GetBinContent(i));
        systs->SetBinContent(i, 1.);
    }

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    TLine * one;
    TLegend * legend0 = new TLegend(0.5,0.7,0.97,0.91,NULL,"brNDC");
    legend0->SetBorderSize(1);
    legend0->SetTextSize(0.0475);
    legend0->SetLineColor(1);
    legend0->SetLineStyle(1);
    legend0->SetLineWidth(1);
    legend0->SetFillColor(0);
    legend0->SetFillStyle(1001);

    TLatex* pave = new TLatex(0.10, 0.92, "#font[42]{#bf{CMS} #it{Preliminary}}");
    TLatex* lumi_label = new TLatex(0.97, 0.92, lumiLabel.c_str());
    TLatex* barrel = new TLatex(0.13, 0.8, plot_label.c_str());
    TLatex* r9 = new TLatex(0.13, 0.72, "#font[42]{R9 > 0.96}");
    pave->SetTextSize(0.075);
    lumi_label->SetTextSize(0.075);
    lumi_label->SetTextAlign(31);
    barrel->SetTextSize(0.05);
    r9->SetTextSize(0.05);
    gPad->SetTicks();
	data->SetLineColor(kBlack);
    data->SetMarkerStyle(kFullCircle);
    data->SetMarkerSize(0.95);
    data->SetMarkerColor(kBlack);
	data->SetLineWidth(2);
    data->SetXTitle("m_{ee} (GeV)");
    data->SetYTitle("Events / 0.25 GeV");
	data->SetLabelSize(0.05, "Y");
	data->SetLabelSize(0.00, "X");
	data->SetTitleSize(0.06, "Y");
	data->SetTitleSize(0.07, "X");
	data->SetTitleOffset(0.8, "Y");
	data->SetTitleOffset(0.75, "X");
    float myMax = data->GetMaximum();
    //for some unknown reason the following line changes the maximum of the data
	data->SetAxisRange(1, data->GetMaximum()*1.53, "Y");
    data->GetYaxis()->SetNdivisions(510);
    double xMin = 80;
    double xMax = 99.999999999;
    data->SetAxisRange(xMin, xMax, "X");
    mc->SetAxisRange(xMin, xMax, "X");
    mc->SetLineColor(kBlack);
    mc->SetLineWidth(2);
    Int_t ci = 1185;
    TColor * color = new TColor(ci, 0.619, 0.619, 0.773, " ", 0.5);
    mc->SetFillColor(ci);
    mc->SetFillStyle(3001);
    systematics->SetAxisRange(xMin, xMax, "X");
    if( data->GetMaximum() < 10000) TGaxis::SetMaxDigits(2);
    else TGaxis::SetMaxDigits(4);
    TGaxis::SetExponentOffset(-0.05, 1, "y");
    TLatex * tex = new TLatex(0.050,0.91,"#scale[0.6]{#times10^{3}}");
    tex->SetNDC();
    tex->SetTextAlign(12);
    tex->SetLineWidth(2);
	data->Draw("P");

    mc->Draw("SAME HIST E");
	data->Draw("SAME E");
    legend0->AddEntry(data, "Data", "pe");
    legend0->AddEntry(mc, "Z #rightarrow ee simulation", "f");
    legend0->AddEntry(errors, "Simulation stat. #oplus syst. unc.", "f");
    legend0->Draw();
    pave->SetNDC();
    pave->Draw();
    lumi_label->SetNDC();
    lumi_label->Draw();
    barrel->SetNDC();
    barrel->Draw();
    r9->SetNDC();
    tex->Draw();
    //if(EB_EE) r9->Draw();
    if( EB_EE == 0 ) r9->Draw();
    //tex->Draw();
    /////////////////////////////////
    // set ratio axis range
    systematics->SetAxisRange(0.99, 1.01, "Y");
    /////////////////////////////////
    errors->SetLineColor(kBlack);
    errors->SetLineWidth(0);
    errors->SetFillColor(kRed);
    errors->SetFillStyle(3002);
    errors->Draw("e2 same");
    data->Draw("same e");
    //systematics->Draw("e3same");
    //
    a->cd(2);
	gPad->SetPad("p2", "p2", 0, 0, 1, 0.35, kWhite, 0, 0);
	gPad->SetBottomMargin(0.20);
	gPad->SetTopMargin(0.01);
	gPad->SetLeftMargin(0.10);
	gPad->SetRightMargin(0.03);
    gPad->SetGridy(1);
    gPad->SetGridx(1);
    one = new TLine(xMin, 1, xMax, 1);
    one->SetLineColor(kBlack);
    one->SetLineStyle(9);
    one->SetLineWidth(2);

	ratio->SetLineColor(kBlack);
    ratio->SetMarkerStyle(kFullCircle);
    ratio->SetMarkerColor(kBlack);
    ratio->SetMarkerSize(1.25);
	ratio->SetLineWidth(2);
	ratio->SetYTitle("Data / MC");
	ratio->SetXTitle("m_{ee} (GeV)");
	ratio->SetTitleSize(0.111, "X");
    ratio->SetTitleOffset(0.8, "X");
	ratio->SetLabelSize(0.075, "X");
    ratio->SetLabelOffset(0.01, "X");
    ratio->SetTitleSize(0.111, "Y");
    ratio->SetTitleOffset(0.4, "Y");
    ratio->SetLabelSize(0.06, "Y");
    ratio->SetLabelFont(42);
    ratio->GetYaxis()->SetNdivisions(5, 3, 0);
    ratio->GetYaxis()->SetRangeUser(0.75, 1.25);
	ratio->Draw("E");
    one->Draw();

    systs->SetLineColor(kBlack);
    systs->SetLineWidth(0);
    systs->SetFillColor(kRed);
    systs->SetFillStyle(3002);
    TLegend * legend1 = new TLegend(0.5, 0.85, 0.97, 0.99);
    legend1->AddEntry(errors, "Simulation stat. #oplus syst. unc.", "f");
	ratio->Draw("E");
    systs->Draw("same E2");
    ratio->Draw("same E");
    one->Draw();
    legend1->Draw();

    a->Modified();
    string macro_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_macros/";
    string macro_post = ".C";
    string pdf_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pdfs/";
    string pdf_post = ".pdf";
    string png_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pngs/";
    string png_post = ".png";
    string title = macro_pre + histTitle + macro_post;
    a->SaveAs(title.c_str());
    title = pdf_pre + histTitle + pdf_post;
    a->SaveAs(title.c_str());
    title = png_pre + histTitle + png_post;
    a->SaveAs(title.c_str(), "");
    
    //resent the max for the sake of any following functions
    data->SetMaximum(myMax);
    return a;
};

TCanvas * utilities::moneyPlot( TH1F * data, TH1F * mc, TH1F * systematics, std::string out, int EB_EE){
    out = "money_"+out;
    std::string histTitle = "";
    std::string plot_label = "";
    switch( EB_EE ){
        case 0:
           histTitle = out+"_EB_highR9";
           plot_label = "#font[42]{Barrel-Barrel}";
           break;
        case 1:
           histTitle = out+"_pt_55_65";
           plot_label = "#font[42]{55 < p_{T, lead}^{e} < 65}";
           break;
        case 2:
           histTitle = out+"_notEbEb_highR9";
           plot_label = "#font[42]{Not Barrel-Barrel}";
           break;
    }
    const char * buff = histTitle.c_str();
    string temp = "";
    TCanvas * a;

    if(!data){
        std::cout << "[ERROR] invalid data hist" << std::endl;
        return a;
    }
    if(!mc){
        std::cout << "[ERROR] invalid data hist" << std::endl;
        return a;
    }

    TH1F* errors = (TH1F*)mc->Clone();
    TH1F * systs = (TH1F*)systematics->Clone();
    for(int i = 1; i <= systematics->GetNbinsX(); i++){
        errors->SetBinError(i, systematics->GetBinContent(i));
        systs->SetBinError(i, systematics->GetBinContent(i)/mc->GetBinContent(i));
        systs->SetBinContent(i, 1.);
    }

    a = new TCanvas(buff, "", 900, 900);
    gPad->SetTopMargin(0.09);
    gPad->SetLeftMargin(0.1);
    gPad->SetRightMargin(0.03);
    gPad->SetGridx(1);
    gPad->SetGridy(1);

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    TLine * one;
    TLegend * legend0 = new TLegend(0.5,0.7,0.97,0.91,NULL,"brNDC");
    legend0->SetBorderSize(1);
    legend0->SetLineColor(1);
    legend0->SetLineStyle(1);
    legend0->SetLineWidth(1);
    legend0->SetFillColor(0);
    legend0->SetFillStyle(1001);

    //TLatex* pave = new TLatex(0.10, 0.92, "#font[42]{#bf{CMS} #it{2017 Preliminary}}");
    TLatex* pave = new TLatex(0.10, 0.92, "#font[42]{#bf{CMS} #it{Preliminary}}");
    pave->SetNDC();
    TLatex* lumi_label = new TLatex(0.97, 0.92, lumiLabel.c_str());
    //TLatex* lumi_label = new TLatex(0.97, 0.92, "#font[42]{35.9 fb^{-1} (13 TeV)}");
    TLatex* barrel = new TLatex(0.13, 0.84, plot_label.c_str());
    TLatex* r9 = new TLatex(0.13, 0.79, "#font[42]{R9 > 0.96}");
    pave->SetTextSize(0.05);
    lumi_label->SetTextSize(0.05);
    lumi_label->SetTextAlign(31);
    barrel->SetTextSize(0.035);
    r9->SetTextSize(0.035);
    gPad->SetTicks();
	data->SetLineColor(kBlack);
    data->SetMarkerStyle(kFullCircle);
    data->SetMarkerSize(0.95);
    data->SetMarkerColor(kBlack);
	data->SetLineWidth(2);
    data->SetXTitle("m_{ee} (GeV)");
    data->SetYTitle("Events / 0.25 GeV");
	data->SetLabelSize(0.035, "Y");
	data->SetLabelSize(0.035, "X");
	data->SetTitleSize(0.05, "Y");
	data->SetTitleSize(0.05, "X");
	data->SetTitleOffset(0.9, "Y");
	data->SetTitleOffset(0.75, "X");
    //distance from top of canv to plot border is 0.09
    //distance from bottom of canv to plot border is 0.1
    //distance from top of plot border to bottom of legend is 0.21
    //distance from bottom of border to max of data/mc/systs should be 1 - 0.09 - 0.1 - 0.21 - 0.01 (for some room) = 0.59
    float absYScale = max(data->GetMaximum(), mc->GetMaximum());
    float newYScale = absYScale/0.69;
	data->SetAxisRange(1, newYScale, "Y");
	mc->SetAxisRange(1, newYScale, "Y");
	systs->SetAxisRange(1, newYScale, "Y");
    data->GetYaxis()->SetNdivisions(510);
    double xMin = 80;
    double xMax = 99.999999999;
    data->SetAxisRange(xMin, xMax, "X");
    mc->SetAxisRange(xMin, xMax, "X");
    mc->SetLineColor(kBlack);
    mc->SetLineWidth(2);
    Int_t ci = 1185;
    TColor * color = new TColor(ci, 0.619, 0.619, 0.773, " ", 0.5);
    mc->SetFillColor(ci);
    mc->SetFillStyle(3001);
    systematics->SetAxisRange(xMin, xMax, "X");
    if( data->GetMaximum() < 10000) TGaxis::SetMaxDigits(2);
    else TGaxis::SetMaxDigits(4);
    TGaxis::SetExponentOffset(-0.05, 1, "y");
    TLatex * tex = new TLatex(0.050,0.91,"#scale[0.6]{#times10^{3}}");
    tex->SetNDC();
    tex->SetTextAlign(12);
    tex->SetLineWidth(2);

	data->Draw("P");
    mc->Draw("SAME HIST E");
	data->Draw("SAME E");
    legend0->AddEntry(data, "Data", "pe");
    legend0->AddEntry(mc, "Z #rightarrow ee simulation", "f");
    legend0->AddEntry(errors, "Simulation stat. #oplus syst. unc.", "f");
    legend0->Draw();
    pave->Draw();
    lumi_label->SetNDC();
    lumi_label->Draw();
    tex->Draw();
    barrel->SetNDC();
    barrel->Draw();
    r9->SetNDC();
    //if(EB_EE) r9->Draw();
    if( EB_EE != 1) r9->Draw();
    //tex->Draw();
    /////////////////////////////////
    // set ratio axis range
    systematics->SetAxisRange(0.99, 1.01, "Y");
    /////////////////////////////////
    errors->SetLineColor(kBlack);
    errors->SetLineWidth(0);
    errors->SetFillColor(kRed);
    errors->SetFillStyle(3002);
    errors->Draw("e2 same");
    data->Draw("same e");
    //systematics->Draw("e3same");
    //
    a->Modified();
    string macro_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_macros/";
    string macro_post = ".C";
    string pdf_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pdfs/";
    string pdf_post = ".pdf";
    string png_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pngs/";
    string png_post = ".png";
    string title = macro_pre + histTitle + macro_post;
    a->SaveAs(title.c_str());
    title = pdf_pre + histTitle + pdf_post;
    a->SaveAs(title.c_str());
    title = png_pre + histTitle + png_post;
    a->SaveAs(title.c_str(), "");
    return a;
};

TCanvas * utilities::plotRooFitResult(TH1F * hist, RooFitResult * res, std::string histTitle, std::string dataTitle){
    //build the Breit-Wigner convoluted with a Crystal Ball
    RooRealVar invmass("invMass","M_{ee} [GeV]", 80,100);
    invmass.setBins(hist->GetNbinsX(), "cache");
    invmass.setMin("cache", 80);
    invmass.setMax("cache", 100);
    RooDataHist roo_hist("roo_hist", "roo_hist", invmass, Import(*hist));

    RooRealVar m0 ("m0", "m0", 91.188);
    RooRealVar width ("width", "width", 2.495);
    RooBreitWigner bw ("bw", "bw", invmass, m0, width);

    RooRealVar * rrv_mean = (RooRealVar*)res->floatParsFinal().find("mean");
    RooRealVar * rrv_sigma = (RooRealVar*)res->floatParsFinal().find("sigma");
    RooRealVar * rrv_alpha = (RooRealVar*)res->floatParsFinal().find("alpha");
    RooRealVar * rrv_n = (RooRealVar*)res->floatParsFinal().find("n");

    float res_mean = rrv_mean->getValV();
    float res_sigma = rrv_sigma->getValV();
    float res_alpha = rrv_alpha->getValV();
    float res_n = rrv_n->getValV();

    float M = res_mean + 91.118;
    float M_err = rrv_mean->getAsymErrorHi(); 
    float sig_err = rrv_sigma->getAsymErrorHi();
    float resolution = 100*res_sigma/M;
    float err =  resolution * sqrt(pow(sig_err/res_sigma, 2) + pow(M_err/M,2));

    RooRealVar mean ("mean", "mean", res_mean);
    RooRealVar sigma ("sigma", "sigma", res_sigma);
    RooRealVar alpha ("alpha", "alpha", res_alpha);
    RooRealVar n ("n", "n", res_n);
    RooCBShape cb( "cb", "cb", invmass, mean, sigma, alpha, n);

    RooFFTConvPdf * pdf = new RooFFTConvPdf("pdf", "pdf", invmass, bw, cb);

    TCanvas * c = new TCanvas( "c", "c", 900, 900);
	gPad->SetBottomMargin(0.1);
	gPad->SetTopMargin(0.2);
	gPad->SetLeftMargin(0.13);
	gPad->SetRightMargin(0.03);
	gPad->SetGridy(1);
	gPad->SetGridx(1);

    RooPlot* xFrame = invmass.frame();
    xFrame->SetAxisRange(80, 100);
    roo_hist.plotOn(xFrame, MarkerStyle(1), FillColor(kAzure-7), LineColor(kAzure+3), DrawOption("BXF") );
    pdf->plotOn(xFrame, LineColor(kRed+2));
    xFrame->Draw();
    c->Modified();

    TLegend * leg = new TLegend(0.13, 0.8, 0.97, 0.99);
    leg->AddEntry("roo_hist", dataTitle.c_str(), "lf");
    leg->AddEntry(pdf, "Breit-Wigner * Crystal Ball Fit", "l");
    leg->AddEntry((TObject*)0,"#sigma / #mu = " + TString::Format("%4.3f #pm %4.3f", resolution, err) +" %", "");
    leg->AddEntry((TObject*)0,"#sigma = " + TString::Format("%4.3f #pm %4.3f", res_sigma, sig_err) +" GeV", "");
    leg->AddEntry((TObject*)0,"#mu = " + TString::Format("%4.3f #pm %4.3f", M, M_err)+ " GeV", "");
    leg->Draw();

    TLatex* pave = new TLatex(0.10, 0.92, "#font[42]{#bf{CMS} #scale[0.85]{#it{Preliminary}}}");
    TLatex* lumi_label = new TLatex(0.97, 0.92, lumiLabel.c_str());
    lumi_label->SetTextAlign(31);
    pave->SetTextSize(0.07);
    lumi_label->SetTextSize(0.07);
    pave->Draw();
    lumi_label->Draw();
    c->Modified();

    string macro_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_macros/";
    string macro_post = ".C";
    string pdf_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pdfs/";
    string pdf_post = ".pdf";
    string png_pre = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/plots_pngs/";
    string png_post = ".png";
    string title = macro_pre + histTitle + macro_post;
    c->SaveAs(title.c_str());
    title = pdf_pre + histTitle + pdf_post;
    c->SaveAs(title.c_str());
    title = png_pre + histTitle + png_post;
    c->SaveAs(title.c_str(), "");
    return c;
};

TCanvas * utilities::plotRuns2018(TH1F * d0, TH1F * d1, TH1F * d2, TH1F * d3, float m0, float m1, float m2, float m3, float yMin, float yMax, string histTitle){

    TCanvas * c = new TCanvas("c", "", 1600, 900);
    gStyle->SetOptStat(0);
    c->SetTopMargin(0.09);
    c->SetRightMargin(0.05);

    TLine * l0 = new TLine(297049.5, m0, 306460.5, m0);
    TLine * l1 = new TLine(297049.5, m1, 306460.5, m1);
    TLine * l2 = new TLine(297049.5, m2, 306460.5, m2);
    TLine * l3 = new TLine(297049.5, m3, 306460.5, m3);

    l0->SetLineColor(kRed);
    l1->SetLineColor(kBlue);
    l2->SetLineColor(kGreen+1);
    l3->SetLineColor(kBlack);

    l0->SetLineWidth(3);
    l1->SetLineWidth(3);
    l2->SetLineWidth(3);
    l3->SetLineWidth(3);

    l0->SetLineStyle(9);
    l1->SetLineStyle(9);
    l2->SetLineStyle(9);
    l3->SetLineStyle(9);

    d0->SetLineWidth(1);
    d1->SetLineWidth(1);
    d2->SetLineWidth(1);
    d3->SetLineWidth(1);

    d0->SetLineColor(kRed);
    d1->SetLineColor(kBlue);
    d2->SetLineColor(kGreen+1);
    d3->SetLineColor(kBlack);

    d0->SetMarkerColor(kRed);
    d1->SetMarkerColor(kBlue);
    d2->SetMarkerColor(kGreen+1);
    d3->SetMarkerColor(kBlack);

    d0->SetMarkerStyle(kFullSquare);
    d1->SetMarkerStyle(kFullSquare);
    d2->SetMarkerStyle(kFullSquare);
    d3->SetMarkerStyle(kFullSquare);

    d0->SetXTitle("Run Number");
    d0->SetYTitle("Median M_{ee} [GeV]");
    d0->SetAxisRange(yMin, yMax, "Y");
    d0->SetAxisRange(297049.6,306460.45, "X");

    TLegend * leg = new TLegend(0.1, 0.8, 0.95, 0.91);
    leg->SetNColumns(4);
    leg->AddEntry(d0, "Data, |#eta| < 1", "lp");
    leg->AddEntry(d1, "Data, 1 < |#eta| < 1.4442", "lp");
    leg->AddEntry(d2, "Data, 1.566 < |#eta| < 2", "lp");
    leg->AddEntry(d3, "Data, 2 < |#eta| < 2.5", "lp");
    leg->AddEntry(l0, "MC, |#eta| < 1", "l");
    leg->AddEntry(l1, "MC, 1 < |#eta| < 1.4442", "l");
    leg->AddEntry(l2, "MC, 1.566 < |#eta| < 2", "l");
    leg->AddEntry(l3, "MC, 2 < |#eta| < 2.5", "l");

    d0->Draw("pe");
    d1->Draw("pe same");
    d2->Draw("pe same");
    d3->Draw("pe same");
    l0->Draw("same");
    l1->Draw("same");
    l2->Draw("same");
    l3->Draw("same");
    leg->Draw();

    TLatex* pave = new TLatex(0.10, 0.92, "#font[42]{#bf{CMS} #scale[0.85]{#it{Preliminary}}}");
    TLatex* lumi_label = new TLatex(0.95, 0.92, lumiLabel.c_str());
    lumi_label->SetTextAlign(31);
    pave->SetTextSize(0.05);
    lumi_label->SetTextSize(0.05);
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
    string title = macro_pre + histTitle + macro_post;
    c->SaveAs(title.c_str());
    title = pdf_pre + histTitle + pdf_post;
    c->SaveAs(title.c_str());
    title = png_pre + histTitle + png_post;
    c->SaveAs(title.c_str(), "");
    return c;

};

#endif
