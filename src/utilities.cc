#ifndef UTILITIES
#define UTILITIES
//contains utilties for plotting and manipulating histograms
//

#include "../interface/utilities.h"
#include "../interface/statistic.h"
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

TCanvas * utilities::PlotDataMC(TH1F* data, string dataTitle, TH1F* mc, string mcTitle , string histTitle, string xTitle, string yTitle, double xMin, double xMax, double yMin, double yMax, bool kDrawErrors, bool kDrawRatioPad, string ratioTitle, double ratioMin, double ratioMax, string whichPlot){
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
    double r9Bins1 [5] = {0.5, 0.8, 0.9, 0.94, 1.};
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
            if( kDrawErrors) ratio->SetBinError(i, ratio->GetBinContent(i)*sqrt( pow( data->GetBinError(i)/data->GetBinContent(i), 2)+ pow( mc->GetBinError(i)/mc->GetBinContent(i), 2)));
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
    //TLatex* lumi_label = new TLatex(0.65, 0.92, "#font[42]{58.8 fb^{-1} 13 TeV (2018)}");
    TLatex* lumi_label = new TLatex(0.65, 0.92, "#font[42]{35.9 fb^{-1} 13 TeV (2016)}");
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
    temp = "#bf{" + yTitle + "}";
	data->SetYTitle(temp.c_str());
	data->SetTitleSize(0.07, "Y");
	data->SetTitleOffset(0.7, "Y");
    if(yMax == -1) yMax = data->GetMaximum()*1.5;
	data->SetAxisRange(yMin, yMax, "Y");
    data->GetYaxis()->SetNdivisions(510);
    data->SetAxisRange(xMin, xMax, "X");
    mc->SetAxisRange(xMin, xMax, "X");
    mc->SetLineColor(kAzure+3);
    mc->SetLineWidth(4);
    mc->SetFillColor(kAzure-7);
    mc->SetFillStyle(3001);
    ratio->SetAxisRange(xMin, xMax, "X");
	data->Draw("P");
    mc->Draw("SAME HIST E");
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
        ratioMin = ratio->GetMinimum() - ratio->GetBinError(ratio->GetMinimumBin())*1.05;
    }
    if(ratioMax == -1){
        ratioMax = ratio->GetMaximum() + ratio->GetBinError(ratio->GetMaximumBin())*1.05;
    }
    ratio->SetAxisRange(ratioMin, ratioMax, "Y");
    /////////////////////////////////
	ratio->SetLineColor(kBlack);
    ratio->SetMarkerStyle(kFullCircle);
    ratio->SetMarkerColor(kBlack);
    ratio->SetMarkerSize(1.25);
	ratio->SetLineWidth(3);
	ratio->SetYTitle(ratioTitle.c_str());
    temp = "#bf{"+ xTitle + "}";
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
    temp = "#bf{" + yTitle + "}";
	data->SetYTitle(temp.c_str());
    temp = "#bf{" + xTitle + "}";
    data->SetXTitle(temp.c_str());
	data->SetTitleSize(0.04, "Y");
    data->SetTitleSize(0.04, "X");
    data->SetTitleOffset(0.85,"X");
	data->SetTitleOffset(1.1, "Y");
	data->SetAxisRange(yMin, yMax, "Y");
    data->GetYaxis()->SetNdivisions(510);
    if( yMin == -1) yMin = (2./3.)*min(data->GetMaximum(), mc->GetMaximum());
    if( yMax == -1) yMax = 1.6*max(data->GetMaximum(), mc->GetMaximum());
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
    mc->Draw("SAME HIST E");
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
    std::cout << "[utilities::INFO] running the function utilities::PlotDataMC @line 576" << std::endl;
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
        TLatex* lumi_label = new TLatex(0.65, 0.92, "#font[42]{35.9 fb^{-1} 13 TeV (2016)}");
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
        temp = "#bf{" + yTitle + "}";
        data->SetYTitle(temp.c_str());
        data->SetTitleSize(0.07, "Y");
        data->SetTitleOffset(0.7, "Y");
        //double yMin = 0.9*min(data->GetBinContent(2), mc->GetBinContent(2));
        //double yMax = 1.2*max(data->GetBinContent(data->GetMaximumBin()), mc->GetBinContent(mc->GetMaximumBin()));
        if( yMin == -1) yMin = (2./3.)*min(data->GetMaximum(), mc->GetMaximum());
        if( yMax == -1) yMax = 1.6*max(data->GetMaximum(), mc->GetMaximum());
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
        mc->Draw("SAME HIST E");
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
            for(int i = 1; i <= systs->GetNbinsX(); i++){ possibleMinima.push_back(1-systs->GetBinError(i));}
            possibleMinima.push_back(ratio->GetMinimum());
            ratioMin = *std::min_element(possibleMinima.begin(),possibleMinima.end());
            ratioMin = ratioMin - 0.05*(1-ratioMin);
        }
        if(ratioMax == -1){
            std::vector<double> possibleMaxima;
            for(int i = 1; i <= systs->GetNbinsX(); i++){ possibleMaxima.push_back(1+systs->GetBinError(i));}
            possibleMaxima.push_back(ratio->GetMaximum());
            ratioMax = *std::max_element(possibleMaxima.begin(), possibleMaxima.end());
            ratioMax = ratioMax + 0.05*(ratioMax-1);
        }
        ratio->SetAxisRange(ratioMin, ratioMax, "Y");
        /////////////////////////////////
        ratio->SetLineColor(kBlack);
        ratio->SetMarkerStyle(kFullCircle);
        ratio->SetMarkerColor(kBlack);
        ratio->SetMarkerSize(1.25);
        ratio->SetLineWidth(3);
        ratio->SetYTitle(ratioTitle.c_str());
        temp = "#bf{"+ xTitle + "}";
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
        temp = "#bf{" + yTitle + "}";
        data->SetYTitle(temp.c_str());
        temp = "#bf{" + xTitle + "}";
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
            mc->Draw("SAME HIST E");
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

TH1F * utilities::GetMedianProfilesWithError(TH2F* data){
    int thisId = rand();
    char name [50];
    sprintf( name, "%i", thisId);
    TH1F * ret = new TH1F(name, "", data->GetNbinsX(), data->GetXaxis()->GetXmin(), data->GetXaxis()->GetXmax());
    for(int i = 0; i <= data->GetNbinsX(); i++){
        TH1F * temp = (TH1F*)data->ProjectionY("", i-1, i, "");

        if( temp->Integral() < 0 ) temp->Scale(-1);
        if( data->Integral() != 0){
            errors * error = statistic::getHistStatistic(temp, "median", 100, 500, 0.95);
            ret->SetBinContent(i, error->x);
            ret->SetBinError(i, max(error->errLow, error->errHigh));
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
        if(mc1->Integral(i, i+1) != 0 && mc2->Integral(i,i+1) !=0){
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

#endif
