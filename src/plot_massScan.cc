/*
 * Author: Neil Schroeder
 * gitHub: neilSchroeder@github.com
 *
 * Description: 
 *   script which uses ROOT to plot the mass scan plots from paper HIG-19-004
 */

#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TPad.h>
#include <TLine.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TLatex.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "boost/program_options.hpp"
#include "boost/algorithm/string/replace.hpp"

//#define debug

int plot_massScan(std::string, std::string);

bool notObs = false;
bool noStat = false;
bool singleScan = false;
bool _flag_lumiLabel;
std::string lumiLabel;

int main(int argc, char **argv){
    ////////////////////////////////////
    // boost for command line options //
    ////////////////////////////////////

    using namespace boost;
    namespace opts = boost::program_options;

    std::string fileList;
    std::string plotTitle;

    opts::options_description desc("Main Options");

    desc.add_options()
        ("help", "print help message")
        ("fileList", opts::value<std::string>(&fileList), "List of root files to process, do not include stat files as the program will attempt to include them if available")
        ("plotTitle", opts::value<std::string>(&plotTitle), "Title of plot to be created as *.{pdf,png,C}")
        ("lumiLabel", opts::value<std::string>(&lumiLabel), "lumi label (top right corner label)")
        ("singleScan", opts::bool_switch(&singleScan), "use this option if you are only plotting one likelihood scan")
        ("noStat", opts::bool_switch(&noStat), "use this option if you do not want to include the stat likelihood scans \n Default: False")
        ("exp", opts::bool_switch(&notObs), "use this option if you are processing expected likelihood scans \n Default: False");
    ;

    opts::variables_map v_map;
    opts::store(opts::parse_command_line(argc, argv, desc), v_map);
    opts::notify(v_map);

    if(v_map.count("help")){ 
        std::cout << desc << std::endl;
        return 0;
    }

    if(v_map.count("lumiLabel")) _flag_lumiLabel = true;

    return plot_massScan(fileList, plotTitle);
}

int plot_massScan(std::string thisFile, std::string plotName){

    std::string expObs = "_Obs";
    if(notObs){ expObs = "_Exp";}

    std::cout << "[INFO] this is plot_massScan which will plot likelihood profiles" << std::endl;
    std::cout << "[INFO] the intput file you have provided is: " << thisFile << std::endl;
    std::cout << "[INFO] the output files you will obtain are: " << "./plots/"+plotName+expObs+".{pdf,png,C}" << std::endl;
    std::cout << std::endl;

    std::vector<TFile*> vec_filesTot;
    std::vector<TFile*> vec_filesStat;
    std::vector<TTree*> vec_treesTot;
    std::vector<TTree*> vec_treesStat;
    std::vector<std::vector<Float_t>> vec_xValsTot;
    std::vector<std::vector<Float_t>> vec_yValsTot;
    std::vector<std::vector<Float_t>> vec_xValsStat;
    std::vector<std::vector<Float_t>> vec_yValsStat;
    std::vector<std::string> title_Tot;

    /////////////////////////////
    // process the input files //
    /////////////////////////////

    std::ifstream inStream (thisFile.c_str()); 

    double myMass, myTotalErr, myStatErr;
    if(inStream.is_open()){

        if(inStream.peek() == std::ifstream::traits_type::eof()){
            std::cout << "[ERROR FATAL] the file: " << thisFile << " is empty. " << std::endl;
            std::cout << "[EXIT ON FAILURE] exiting" << std::endl;
            return 1;
        }

        std::string thisRootPath;
        std::string thisTitle;
        if(!singleScan){
            while(inStream >> thisRootPath >> thisTitle){
                std::cout << "[INFO] file: " << thisRootPath << std::endl;
                if(TFile::Open(thisRootPath.c_str(), "READ")){
                    vec_filesTot.push_back(new TFile(thisRootPath.c_str(), "READ"));
                }
                else{
                    std::cout << "[ERROR] Could not read file: " << thisRootPath << std::endl;
                    return 1;
                }
                boost::replace_all(thisTitle, "_", " ");
                title_Tot.push_back(thisTitle);
                if(!noStat){
                    boost::replace_all(thisRootPath, "MHScan", "MHScanStat");
                    std::cout << "[INFO] stat file: " << thisRootPath << std::endl;
                    if(TFile::Open(thisRootPath.c_str(), "READ")){
                        vec_filesStat.push_back(new TFile(thisRootPath.c_str(), "READ"));
                    }
                    else{
                        std::cout << "[ERROR] could not read file: " << thisRootPath << std::endl;
                        vec_filesStat.push_back(new TFile());
                        std::cout << "[ERROR RECOVERED] continuing without stat file" << std::endl;
                    }
                }
            }
        }
        else{
            while(inStream >> thisRootPath >> thisTitle >> myMass >> myTotalErr >> myStatErr){

                std::cout << "[INFO] file: " << thisRootPath << std::endl;
                if(TFile::Open(thisRootPath.c_str())) vec_filesTot.push_back(new TFile(thisRootPath.c_str(), "READ"));
                else{
                    std::cout << "[ERROR FATAL] could not read file: " << thisRootPath << std::endl;
                    return 1;
                }
                boost::replace_all(thisTitle, "_", " ");
                title_Tot.push_back(thisTitle);
                if(!noStat){
                    boost::replace_all(thisRootPath, "MHScan", "MHScanStat");
                    std::cout << "[INFO] stat file: " << thisRootPath << std::endl;
                    if(TFile::Open(thisRootPath.c_str(), "READ")){
                        vec_filesStat.push_back(new TFile(thisRootPath.c_str(), "READ"));
                    }
                    else vec_filesStat.push_back(new TFile());
                }
            }
        }
    }//end if file is open
    else{
        std::cout << "[ERROR FATAL] could not open file: " << thisFile << std::endl;
        std::cout << "[EXIT ON FAILURE]" << std::endl;
        return 1;
    }
    inStream.close();

    //something gets passed by in the earlier error handling letting an empty file be processed

    std::cout << std::endl << "[INFO] obtaining trees" << std::endl;
    for(auto &iFile : vec_filesTot){
        vec_treesTot.push_back((TTree*)iFile->Get("limit"));
    }
    if(!noStat){
        for(auto &iFile : vec_filesStat){
            if(iFile) vec_treesStat.push_back((TTree*)iFile->Get("limit"));
            else vec_treesStat.push_back((TTree*)0);
        }
    }

    Float_t deltanll;
    Float_t mh;

    std::vector<std::pair<Float_t,Float_t>> vec_minNllTot;
    std::vector<std::pair<Float_t,Float_t>> vec_minNllStat;

    /////////////////////////////////////////////////////////////////////////////////////////
    // loop over the trees, grab the likelihood scan vals, and find the center of the scan //
    /////////////////////////////////////////////////////////////////////////////////////////

    std::cout << "[INFO] initiating loop over trees" << std::endl;
    for(auto &iTree : vec_treesTot){
        if(iTree){
            std::vector<Float_t> vec_xVals;
            std::vector<Float_t> vec_yVals;
            std::pair<Float_t,Float_t> iPair = {999, 999};
#ifdef debug
            std::cout << iPair.first << " " << iPair.second << " check" << std::endl;
#endif
            iTree->SetBranchAddress("deltaNLL", &deltanll);
            iTree->SetBranchAddress("MH", &mh);
            for( Long64_t i = 0; i < iTree->GetEntries(); i++){
                iTree->GetEntry(i);
                vec_xVals.push_back(mh);
                vec_yVals.push_back(2*deltanll);
                if(2*deltanll < iPair.first){ iPair.first = 2*deltanll; iPair.second = mh;}
            }
            vec_minNllTot.push_back(iPair);
            vec_xValsTot.push_back(vec_xVals);
            vec_yValsTot.push_back(vec_yVals);
        }
    }

#ifdef debug
    std::cout << vec_minNllTot[0].first << " " << vec_minNllTot[0].second << " check" << std::endl;
#endif

    if(!noStat){
        //stat loop
        for(auto &iTree : vec_treesStat){
            std::vector<Float_t> vec_xVals;
            std::vector<Float_t> vec_yVals;
            std::pair<Float_t,Float_t> iPair = {999, 999};
#ifdef debug
            std::cout << iPair.first << " " << iPair.second << " check" << std::endl;
#endif
            if(iTree){
                iTree->SetBranchAddress("deltaNLL", &deltanll);
                iTree->SetBranchAddress("MH", &mh);
                for( Long64_t i = 0; i < iTree->GetEntries(); i++){
                    iTree->GetEntry(i);
                    vec_xVals.push_back(mh);
                    vec_yVals.push_back(2*deltanll);
                    if(2*deltanll < iPair.first){ iPair.first = 2*deltanll; iPair.second = mh;}
                }
            }
            vec_minNllStat.push_back(iPair);
            vec_xValsStat.push_back(vec_xVals);
            vec_yValsStat.push_back(vec_yVals);
        }
    }

#ifdef debug
    std::cout << vec_minNllStat[0].first << " " << vec_minNllStat[0].second << " check" << std::endl;
#endif

    ///////////////////////////////////////////////
    // center the stat scans on the total scans  //
    // adjust the height of both scans : min = 0 //
    /////////////////////////////////////////////// 

    std::cout << "[INFO] centering stat scan on total scan and adjusting heights" << std::endl;
    for(int i = 0; i < vec_yValsTot.size(); i++){
        for(int j = 0; j < vec_yValsTot[i].size(); j++){
            if(vec_minNllTot[i].first != 0) vec_yValsTot[i][j] -= vec_minNllTot[i].first;
            if(notObs){
                if(vec_minNllTot[i].second != 125.) vec_xValsTot[i][j] -= (vec_minNllTot[i].second - 125.);
            }
        }
    }
    if(!noStat){
        for(int i = 0; i < vec_yValsStat.size(); i++){
            for(int j = 0; j < vec_yValsStat[i].size(); j++){
                if(vec_minNllStat[i].first != 0) vec_yValsStat[i][j] -= vec_minNllStat[i].first;
                if(notObs){
                    if(vec_minNllStat[i].second != 125.) vec_xValsStat[i][j] -= (vec_minNllStat[i].second - 125.);
                }
                else{
                    if(vec_minNllStat[i].second != vec_minNllTot[i].second) vec_xValsStat[i][j] -= (vec_minNllStat[i].second - vec_minNllTot[i].second);
                }
            }
        }
    }

    std::vector<TGraph*> graphs_Total;
    std::vector<TGraph*> graphs_Stat;

    //////////////////////////////
    // sort and name the graphs //
    //////////////////////////////

    std::cout << "[INFO] sorting and naming" << std::endl;
    for(int i = 0; i < vec_xValsTot.size(); i++){
        graphs_Total.push_back(new TGraph(vec_xValsTot[i].size(), &(vec_xValsTot[i][0]), &(vec_yValsTot[i][0])));
        graphs_Total[i]->Sort();
    }
    if(!noStat){
        for(int i = 0; i < vec_xValsStat.size(); i++){
            graphs_Stat.push_back(new TGraph(vec_xValsStat[i].size(), &(vec_xValsStat[i][0]), &(vec_yValsStat[i][0])));
            char total [50];
            sprintf( total, "Stat_%i", i);
            graphs_Stat[i]->SetName(total);
            graphs_Stat[i]->Sort();
        }
    }

    /////////////////////
    // labels and such //
    /////////////////////

    TLatex * cms_prelim = new TLatex(0.1, 0.92, "#font[42]{#bf{CMS} #it{Preliminary}}");
    cms_prelim->SetTextSize(0.035);
    cms_prelim->SetNDC();
    TLatex * label = new TLatex(0.95, 0.92, lumiLabel.c_str());
    label->SetTextAlign(31);
    label->SetTextSize(0.035);
    label->SetNDC();
    TLatex * combine = new TLatex(0.12, 0.875, "H#rightarrow#gamma#gamma + H#rightarrow ZZ*#rightarrow 4l Combination");
    combine->SetTextSize(0.0325);
    combine->SetNDC();

    TLatex * one_sig = new TLatex(0.9, 0.215, "#font[42]{1#sigma}");
    TLatex * two_sig = new TLatex(0.9, 0.564, "#font[42]{2#sigma}");
    one_sig->SetTextSize(0.03);
    one_sig->SetNDC();
    one_sig->SetTextColor(kGray+1);
    two_sig->SetTextSize(0.03);
    two_sig->SetNDC();
    two_sig->SetTextColor(kGray+1);

    std::vector<std::pair<int, int>> topAndBottomTot;
    std::vector<std::pair<int, int>> topAndBottomStat;
    std::vector<double> xmin;
    std::vector<double> xmax;

    ///////////////////////////////////////
    // find the new limits for the plots //
    ///////////////////////////////////////

    std::cout << "[INFO] adjusting limits on graphs" << std::endl;
    for(int i = 0; i < graphs_Total.size(); i++){
        std::pair<int, int> temp_topBottom = {0, 0};
        double * entry_x = graphs_Total[i]->GetX();
        double * entry_y = graphs_Total[i]->GetY();
        for(int j = 0; j < graphs_Total[i]->GetN(); j++){
            if(j > temp_topBottom.first && entry_y[j] > 5. && j < (int)graphs_Total[i]->GetN()/2){
                temp_topBottom.first = j;
            }
            if(j > temp_topBottom.second && entry_y[j] < 5.001 && j > (int)graphs_Total[i]->GetN()/2){
                temp_topBottom.second = j;
            }
        }
#ifdef debug
        std::cout << temp_topBottom.first << " " << temp_topBottom.second << std::endl;
#endif
        topAndBottomTot.push_back(temp_topBottom);
        xmin.push_back(entry_x[temp_topBottom.first]-0.35);
        xmax.push_back(entry_x[temp_topBottom.second]+0.35);
    }


#ifdef debug
    std::cout << "[DEBUG] finished finding limits on total likelihood scans" << std::endl;
#endif
    if(!noStat){
        for(int i = 0; i < graphs_Stat.size(); i++){
            std::pair<int, int> temp_topBottom = {0, 0};
            double * entry_x = graphs_Stat[i]->GetX();
            double * entry_y = graphs_Stat[i]->GetY();
            for(int j = 0; j < graphs_Stat[i]->GetN(); j++){
                if(j > temp_topBottom.first && entry_y[j] > 5. && j < (int)graphs_Stat[i]->GetN()/2) temp_topBottom.first = j;
                if(j > temp_topBottom.second && entry_y[j] < 5.001 && j > (int)graphs_Stat[i]->GetN()/2) temp_topBottom.second = j;
            }
#ifdef debug
        std::cout << temp_topBottom.first << " " << temp_topBottom.second << std::endl;
#endif
            topAndBottomStat.push_back(temp_topBottom);
        }
    }

#ifdef debug
    std::cout << "[DEBUG] finished finding limits on stat likelihood scans" << std::endl;
#endif

    ////////////////////////////////////////
    // force the plots to be deltaNLL < 5 //
    ////////////////////////////////////////

    for(int i = 0; i < graphs_Total.size(); i++){ 
        std::vector<float> new_x;
        std::vector<float> new_y;
        double * entry_y = graphs_Total[i]->GetY();
        double * entry_x = graphs_Total[i]->GetX();
        for(int j = 0; j < graphs_Total[i]->GetN(); j++){
            if(j >= topAndBottomTot[i].first && j <= topAndBottomTot[i].second){
                new_x.push_back(entry_x[j]);
                new_y.push_back(entry_y[j]);
            }
        }
        graphs_Total[i] = new TGraph(new_x.size(), &(new_x[0]), &(new_y[0]));
        char total [50];
        sprintf( total, "Total_%i", i);
        graphs_Total[i]->SetName(total);
        if(!singleScan) graphs_Total[i]->GetYaxis()->SetRangeUser(0.0001, 7);
        else graphs_Total[i]->GetYaxis()->SetRangeUser(0.0001, 6);
        new_x.clear();
        new_y.clear();
    }
#ifdef debug
    std::cout << "[DEBUG] finished setting new limits on total likelihood scans" << std::endl;
#endif
    if(!noStat){
        for(int i = 0; i < graphs_Stat.size(); i++){ 
            std::vector<float> new_x;
            std::vector<float> new_y;
            double * entry_y = graphs_Stat[i]->GetY();
            double * entry_x = graphs_Stat[i]->GetX();
            for(int j = 0; j < graphs_Stat[i]->GetN(); j++){
                if(j >= topAndBottomStat[i].first && j <= topAndBottomStat[i].second){
                    new_x.push_back(entry_x[j]);
                    new_y.push_back(entry_y[j]);
                }
            }
            graphs_Stat[i] = new TGraph(new_x.size(), &(new_x[0]), &(new_y[0]));
            char total [50];
            sprintf( total, "Stat_%i", i);
            graphs_Stat[i]->SetName(total);
            if(!singleScan) graphs_Stat[i]->GetYaxis()->SetRangeUser(0.0001, 7);
            else graphs_Stat[i]->GetYaxis()->SetRangeUser(0.0001, 6);
#ifdef debug 
            std::cout << graphs_Stat[i]->GetName() << " " << graphs_Stat[i]->GetMean() << std::endl;
            std::cout << graphs_Total[i]->GetName() << " " << graphs_Total[i]->GetMean() << std::endl;
#endif
        }
    }
#ifdef debug
    std::cout << "[DEBUG] finished setting new limits on total likelihood scans" << std::endl;
#endif

    //////////////////////////////
    // do the colors and styles //
    //////////////////////////////
    
    std::cout << "[INFO] Styling likelihood scans" << std::endl;
    for(int i = 0; i < graphs_Total.size(); i++){
        graphs_Total[i]->SetLineWidth(3);
        if(!noStat){
            graphs_Stat[i]->SetLineStyle(9);
            graphs_Stat[i]->SetLineWidth(3);
        }
        switch(i){
            case 0 :
                graphs_Total[i]->SetLineColor(kBlue);
                if(!noStat){
                    graphs_Stat[i]->SetLineColor(kBlue);
                }
                break;
            case 1 : 
                graphs_Total[i]->SetLineColor(kRed+1);
                if(!noStat){
                    graphs_Stat[i]->SetLineColor(kRed+1);
                }
                break;
            case 2 : 
                graphs_Total[i]->SetLineColor(kBlack);
                if(!noStat){
                    graphs_Stat[i]->SetLineColor(kBlack);
                }
                break;
            case 3 :
                graphs_Total[i]->SetLineColor(kGreen);
                if(!noStat){
                    graphs_Stat[i]->SetLineColor(kGreen);
                }
                break;
            case 4 :
                graphs_Total[i]->SetLineColor(kOrange+7);
                if(!noStat){
                    graphs_Stat[i]->SetLineColor(kOrange+7);
                }
                break;
        }
    }

    graphs_Total[0]->GetYaxis()->SetTitle("#bf{-2#Delta NLL}");
    graphs_Total[0]->GetXaxis()->SetTitle("#bf{m_{#gamma#gamma} (GeV)}");

    //////////////////////
    // draw the plot(s) //
    //////////////////////


    TCanvas * c = new TCanvas("c", "", 900, 900);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    std::cout << "[INFO] begin drawing graphs" << std::endl;
    for(int i = 0; i < graphs_Total.size(); i++){
        graphs_Total[i]->GetXaxis()->SetLimits(*std::min_element(xmin.begin(),xmin.end()), *std::max_element(xmax.begin(), xmax.end()));
        if(!noStat) graphs_Stat[i]->GetXaxis()->SetLimits(*std::min_element(xmin.begin(),xmin.end()), *std::max_element(xmax.begin(), xmax.end()));
        graphs_Total[i]->GetXaxis()->SetNdivisions(505);
        if(i!=0){
            graphs_Total[i]->Draw("c same");
            if(!noStat) graphs_Stat[i]->Draw("c same");
        }
        else{
            graphs_Total[i]->Draw("ac");
            if(!noStat) graphs_Stat[i]->Draw("c same");
            graphs_Total[i]->Draw("ac");
            if(!noStat) graphs_Stat[i]->Draw("c same");
            c->Update();
        }
    }

    TLegend * leg;
    TLatex * massLabel;
    if(!singleScan){
        leg = new TLegend(0.6, 0.65, 0.945, 0.909);
        gPad->SetRightMargin(0.05);
        gPad->SetTopMargin(0.09);
        gPad->SetBottomMargin(0.09);
        leg->SetBorderSize(0);
        leg->SetTextSize(0.03);
        for(int i = 0; i < graphs_Total.size(); i++){
            leg->AddEntry(graphs_Total[i]->GetName(), title_Tot[i].c_str(), "l");
            if(!noStat) leg->AddEntry(graphs_Stat[i]->GetName(), "Stat. Only", "l");
        }
    }
    else{
        leg = new TLegend(0.125, 0.75, 0.94, 0.909);
        gPad->SetRightMargin(0.05);
        gPad->SetTopMargin(0.09);
        gPad->SetBottomMargin(0.09);
        leg->SetBorderSize(0);
        leg->SetTextSize(0.035);
        leg->SetNColumns(3);
        if(!noStat){
        leg->AddEntry((TObject*)0, "     ", "");
        }
        for(int i = 0; i < graphs_Total.size(); i++){
            leg->AddEntry(graphs_Total[i]->GetName(), title_Tot[i].c_str(), "l");
            if(!noStat) leg->AddEntry(graphs_Stat[i]->GetName(), "Stat. Only", "l");
        }
        if(!noStat) leg->AddEntry((TObject*)0, "", "");
        if(!noStat){
            massLabel = new TLatex(0.525, 0.79, TString::Format("#font[42]{M_{H} = %6.2f #pm %3.2f (#pm %3.2f) GeV}", myMass, myTotalErr, myStatErr)); 
            massLabel->SetTextAlign(21);
        }
        else{
            massLabel = new TLatex(0.94, 0.82, TString::Format("#font[42]{M_{H} = %6.2f #pm %3.2f (#pm %3.2f) GeV}", myMass, myTotalErr, myStatErr));
            massLabel->SetTextAlign(31);
        }
        massLabel->SetTextSize(0.035);
        massLabel->SetNDC();
        std::cout << "[INFO] label is " << massLabel->GetTitle() << " " << massLabel->GetName() << std::endl;
    }


    TLegend * leg1 = new TLegend(0.12, 0.65, 0.6, 0.909);
    leg1->SetBorderSize(0);

    TLine * gg_sig_one = new TLine(graphs_Total[0]->GetXaxis()->GetXmin(), 1, graphs_Total[0]->GetXaxis()->GetXmax(), 1);
    gg_sig_one->SetLineColor(kGray+1);
    gg_sig_one->SetLineWidth(3);
    gg_sig_one->SetLineStyle(9);
    TLine * gg_sig_two = new TLine(graphs_Total[0]->GetXaxis()->GetXmin(), 4, graphs_Total[0]->GetXaxis()->GetXmax(), 4);
    gg_sig_two->SetLineColor(kGray+1);
    gg_sig_two->SetLineWidth(3);
    gg_sig_two->SetLineStyle(9);

    leg->Draw();
    if(!singleScan) leg1->Draw();
    cms_prelim->Draw();
    label->Draw();
    if(!singleScan) combine->Draw();
    else{
        massLabel->Draw();
    }
    one_sig->Draw();
    two_sig->Draw();
    gg_sig_one->Draw("same");
    gg_sig_two->Draw("same");

    std::string pdfName = "./plots/"+plotName+expObs+".pdf";
    std::string pngName = "./plots/"+plotName+expObs+".png";
    std::string cName = "./plots/"+plotName+expObs+".C";
    c->SaveAs(pdfName.c_str());
    c->SaveAs(pngName.c_str());
    c->SaveAs(cName.c_str());

    std::cout << std::endl << "[INFO] finished plotting" << std::endl;

    return 0;
}
