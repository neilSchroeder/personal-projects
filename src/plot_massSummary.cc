/*
 * Author: Neil Schroeder
 * gitHub: neilSchroeder@github.com
 * 
 * Description:
 *      script which uses ROOT to plot the mass summary plots for HIG-19-004.
 *
 */


#include <iomanip>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "boost/program_options.hpp"
#include "boost/algorithm/string/replace.hpp"

#include "TFile.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TH2.h"
#include "TPad.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TString.h"
#include "TLatex.h"
#include "TLine.h"
#include "TROOT.h"
#include "TGaxis.h"
#include "TAxis.h"

int plot_categorySummaryHgg(std::string, std::string, std::string);

bool _flag_lumiLabel = false;
std::string lumiLabel;

int main(int argc, char **argv){
    using namespace boost;
    namespace opts = boost::program_options;
    std::string inputFile;
    std::string plotName;
    std::string additionalLines = "";

    opts::options_description desc("Main Options");

    desc.add_options()
        ("help", "produce help message")
        ("inputFile", opts::value<std::string>(&inputFile), "Input File")
        ("plotName", opts::value<std::string>(&plotName), "name of file to write")
        ("additionalLines", opts::value<std::string>(&additionalLines), "List of categories after which to add an additional line, i.e. `1 ` or `4 5 3 `")
        ("lumiLabel", opts::value<std::string>(&lumiLabel), "lumi label (top right corner of plots)")
    ;

    opts::variables_map v_map;
    opts::store(opts::parse_command_line(argc, argv, desc), v_map);
    opts::notify(v_map);

    if( v_map.count("help")){
        std::cout << desc << std::endl;
        return 1;
    }

    if( v_map.count("lumiLabel") ) _flag_lumiLabel = true;

    return plot_categorySummaryHgg(inputFile, plotName, additionalLines);
}

int plot_categorySummaryHgg(std::string datFile, std::string plotFile, std::string lines) {
    std::cout << std::endl << "[INFO] creating mass summary plot from information in " << datFile << std::endl;
    std::cout << "[INFO] the resulting plots will be created in `" << plotFile <<".{pdf,png,C}'" << std::endl;

    //variable declaration
    std::vector<std::string> categories;
    std::vector<Double_t> centralValues;
    std::vector<Double_t> errorTotal;
    std::vector<Double_t> errorStat;

    std::ifstream myFile (datFile.c_str());

    //open the file and handle possible datFile errors
    if(myFile.is_open()){
        std::string cat;
        Double_t centralVal;
        Double_t errTot;
        Double_t errStat;
        while(myFile >> cat >> centralVal >> errTot >> errStat){
            categories.push_back(cat);
            centralValues.push_back(centralVal);
            errorTotal.push_back(errTot);
            errorStat.push_back(errStat);
        }
        myFile.close();
    }
    else{
        std::cout << "[ERROR] could not open " << datFile << std::endl;
        std::cout << "[ERROR] ending operation" << std::endl;
        return 1;
    }

    if( categories.size() != centralValues.size()){
        std::cout << "[ERROR] number of categories isn't compatible with number of central values" << std::endl;
        std::cout << "[ERROR] ending operation, please review your dat file" << std::endl;
        return 2;
    }
    if( centralValues.size() != errorTotal.size()){
        std::cout << "[ERROR] number of central values isn't compatible with number of total errors" << std::endl;
        std::cout << "[ERROR] ending operation, please review your dat file" << std::endl;
        return 2;
    }

    std::cout << std::endl;

    //correct string
    for(auto& cat : categories){
        boost::replace_all(cat, "_", " ");
    }
    std::cout << "[INFO] the categories are: " << std::endl;
    for(int i = 0; i < categories.size(); i++){ std::cout << "[INFO] category: " << categories[i] << std::endl;}
    std::cout << std::endl;

    // create TGraph objects
    Double_t yPlace[1] = {0};
    Double_t ePlace[1] = {0.0001};
    Double_t ePlaceSyst[1] = {0.1};

    std::vector<TGraphErrors> myGraphsTotal;
    std::vector<TGraphErrors> myGraphsStat;

    for(int i = 0; i < categories.size(); i++){
        yPlace[0] = categories.size()-i;
        myGraphsTotal.push_back(TGraphErrors(1, &(centralValues[i]), yPlace, &(errorTotal[i]), ePlace));
        myGraphsStat.push_back(TGraphErrors(1, &(centralValues[i]), yPlace, &(errorStat[i]), ePlaceSyst));
    }

    //canvas and pad style
    gROOT->SetStyle("Plain");
    gStyle->SetOptStat(0);
    gStyle->SetEndErrorSize(6);
    TCanvas *Csigma = new TCanvas("Csigma","comparisons",10,10, 1200, 900);
    gPad->SetLeftMargin(0.05);
    gPad->SetRightMargin(0.05);
    Csigma->SetGridx(0);
    Csigma->SetGridy(0);
    gPad->SetTicks(1,0);

    //variable min and max of x-axis
    Double_t xmin = 999;
    Double_t xmax = 0;
    for(int i = 0; i < centralValues.size(); i++){
        if(centralValues[i]-errorTotal[i] < xmin) xmin = centralValues[i]-errorTotal[i];
        if(centralValues[i]+errorTotal[i] > xmax) xmax = centralValues[i]+errorTotal[i];
    }

    //dummy TH2 to hold the spaces of the categories
    TH2D *dummy;
    if( categories.size() > 6){
        dummy = new TH2D("dummy","; Higgs mass measurement [GeV]", 40, (Double_t)((int)(xmin-2)-0.5), (Double_t)((int)(xmax+4)+0.5),categories.size(), 0, categories.size()+2);
    }
    else{
        dummy = new TH2D("dummy","; Higgs mass measurement [GeV]", 40, (Double_t)((int)(xmin-2)-0.5), (Double_t)((int)(xmax+4)+0.5),categories.size(), 0, categories.size()+1);
    }

    //create the x axis
    TGaxis * axisX = new TGaxis(dummy->GetXaxis()->GetXmin(), 0, dummy->GetXaxis()->GetXmax(),0, dummy->GetXaxis()->GetXmin(), dummy->GetXaxis()->GetXmax(), 509, "");
    dummy->Draw("atext");

    // create box corresponding to the last entry in the datFile
    Double_t x1 = centralValues[centralValues.size()-1] - errorTotal[centralValues.size()-1];
    Double_t y1 = 0.5;
    Double_t x2 = centralValues[centralValues.size()-1] + errorTotal[centralValues.size()-1];
    Double_t y2 = centralValues.size()+0.5;
    TBox *BCMS = new TBox(x1,y1,x2,y2);
    TLine* TL1 = new TLine(centralValues[centralValues.size()-1], y1, centralValues[centralValues.size()-1], y2);
    BCMS->SetLineColor(0);
    BCMS->SetFillColor(kGray);
    BCMS->Draw();
    TL1->SetLineWidth(3);
    TL1->SetLineColor(2);
    TLine* TL_Divide1 = new TLine(axisX->GetWmin(), 1.5, axisX->GetWmax(), 1.5);
    TL_Divide1->SetLineWidth(3);
    TL_Divide1->SetLineColor(kGray+1);
    TL_Divide1->SetLineStyle(9);
    //TL_Divide1->Draw("same");
    TL1->Draw("same");

    //Handle addition of extra lines on plot
    std::vector<TLine*> additional_lines;
    std::vector<int> locationOfLines;
    std::string delimiter = " ";
    if( lines.compare("") != 0){
        std::cout << "[INFO] adding additional lines" << std::endl;
        size_t pos = 0;
        std::string token;
        while((pos = lines.find(delimiter)) != std::string::npos){
            token = lines.substr(0, pos);
            int thisLocation;
            std::istringstream iss (token);
            iss >> thisLocation;
            locationOfLines.push_back(thisLocation);
            lines.erase(0, pos + delimiter.length());
        }

        for(int i = 0; i < locationOfLines.size(); i++){
            additional_lines.push_back(new TLine(axisX->GetWmin(), categories.size()-locationOfLines[i]+0.5, axisX->GetWmax(), categories.size()-locationOfLines[i]+0.5));
            additional_lines[i]->SetLineWidth(3);
            additional_lines[i]->SetLineColor(kGray+1);
            additional_lines[i]->SetLineStyle(9);
            additional_lines[i]->Draw("same");
            std::cout << "[INFO] additional line added after category titled `" << categories[locationOfLines[i]-1] <<"`" << std::endl;
        }
    }

    //Draw all the TGraphs
    for(int i = 0; i < myGraphsStat.size(); i++){     
        myGraphsStat[i].SetMarkerStyle(20);
        myGraphsStat[i].SetMarkerColor(4);
        myGraphsStat[i].SetMarkerSize(10);
        myGraphsStat[i].SetLineColor(kOrange-3);
        myGraphsStat[i].SetLineWidth(2);
        myGraphsStat[i].SetFillColor(kYellow-9);
        myGraphsStat[i].SetFillStyle(1001);
        myGraphsStat[i].Draw("e5");

        myGraphsTotal[i].SetMarkerStyle(20);
        myGraphsTotal[i].SetMarkerColor(kBlack);
        myGraphsTotal[i].SetLineColor(kBlack);
        myGraphsTotal[i].SetLineWidth(3);
        myGraphsTotal[i].SetFillStyle(3001);
        myGraphsTotal[i].Draw("p same");
    }

    //Add Legend
    TLegend * legend0 = new TLegend(0.6,0.825,0.945,0.89,NULL,"brNDC");
    legend0->SetBorderSize(0);
    legend0->SetTextSize(0.0375);
    legend0->SetLineColor(1);
    legend0->SetLineStyle(1);
    legend0->SetLineWidth(1);
    legend0->SetFillColor(0);
    legend0->SetFillStyle(1001);
    legend0->SetNColumns(2);
    legend0->AddEntry(&(myGraphsTotal[0]), "Total", "lp");
    legend0->AddEntry(&(myGraphsStat[0]), "Stat. Only","f");

    dummy->Draw("same a");
    axisX->Draw();

    //Draw the "CMS preliminary" text
    TLatex LTOP_Left;
    LTOP_Left.SetTextAlign(12);
    LTOP_Left.SetTextSize(0.04);
    LTOP_Left.DrawLatexNDC(0.05,0.92,"CMS (2016) #it{Preliminary}");
    LTOP_Left.SetTextAlign(32);
    LTOP_Left.SetTextSize(0.035);
    //  LTOP.DrawLatexNDC(0.9,0.92,"19.7 fb^{-1}  (8 TeV)");

    //Draw the "35.9 fb^{-1} (13 TeV)" text
    TLatex LTOP_Right;
    LTOP_Right.SetTextAlign(32);
    LTOP_Right.SetTextSize(0.04);
    if(_flag_lumiLabel) LTOP_Right.DrawLatexNDC(0.95, 0.92, lumiLabel.c_str());

    //draw the x-axis title
    TLatex LBOT;
    LBOT.SetTextAlign(32);
    LBOT.SetTextSize(0.04);
    LBOT.DrawLatexNDC(0.95,0.03,"m_{#gamma#gamma} (GeV)");

    //format the TLatex for the mass texts
    TLatex LSRC;
    LSRC.SetTextAlign(12);
    LSRC.SetTextSize(0.035);
    LSRC.SetTextColor(kBlack);

    Double_t latex_left = axisX->GetWmin()+.5;
    Double_t latex_right = axisX->GetWmax()-3.5;

    for(int i = 0; i < categories.size(); i++){
        LSRC.DrawLatex(latex_left, categories.size()-i, categories[i].c_str());
        LSRC.DrawLatex(latex_right, categories.size()-i, TString::Format("%6.2f #pm %3.2f ( #pm %3.2f) GeV", centralValues[i], errorTotal[i], errorStat[i]));
    }

    legend0->Draw();
    std::string plotFile_pdf = "./plots/"+plotFile+".pdf";
    std::string plotFile_png = "./plots/"+plotFile+".png";
    std::string plotFile_C = "./plots/"+plotFile+".C";
    std::cout << std::endl;
    Csigma->SaveAs(plotFile_pdf.c_str());
    Csigma->SaveAs(plotFile_png.c_str());
    Csigma->SaveAs(plotFile_C.c_str());

    return 0;
}
