/* 
 * Author: Neil Schroeder
 * gitHub: neilSchroeder@github.com
 *
 * Description:
 *   script which uses ROOT to plot the standard suite of validation/cross-check plots for the Scales+Smearings in CMS
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "/cvmfs/cms.cern.ch/slc7_amd64_gcc700/external/boost/1.63.0-gnimlf/include/boost/program_options.hpp"
#include "/cvmfs/cms.cern.ch/slc7_amd64_gcc700/external/boost/1.63.0-gnimlf/include/boost/program_options/options_description.hpp"

#include "TFile.h"
#include <TH1.h>
#include <TH2.h>

#include "../interface/utilities.h"
#include "../interface/statistic.h"
#include "../interface/standardSet.h"

//#define DEBUG
using namespace std;

double N_PERCENT_HIST = 100.;

bool _flag_eta = false;
bool _flag_r9 = false;
bool _flag_et = false;
bool _flag_cat = false;
bool _flag_dr_eta = false;
bool _flag_invMassDist = false;
bool _flag_moneyPlot = false;
bool _flag_systUnc = false;
bool _flag_dataData = false;

std::string outputFile = "";
std::string dataTitle = "";
std::string mcTitle = "";
std::string lumiLabel = "";

int main(int argc, char **argv){

    using namespace boost;
    namespace opts = boost::program_options;
    std::string inputFile;
    std::string quantity = "median";

    opts::options_description desc("Main Options");

    desc.add_options()
        ("help", "produce help message")
        ("inputFile,i", opts::value<std::string>(&inputFile), "Input File")
        ("quantity,q", opts::value<std::string>(&quantity), "Quantity you wish to plot, default is median invariant mass. Your options are \n`median` \n`mean` \n`stddev` \n`resolution`")
        ("minHist", opts::value<double>(&N_PERCENT_HIST)->default_value(60.), "Use this value to determine the histogram's min range containing x \% of events") 
        ("outputFile,o", opts::value<std::string>(&outputFile), "name of file to write")
        ("dataTitle,d", opts::value<std::string>(&dataTitle), "Legend entry for the data")
        ("mcTitle,m", opts::value<std::string>(&mcTitle), "Legend entry for the mc")
        ("eta", opts::bool_switch(&_flag_eta), "Turns off plotting the standard set, plots eta set")
        ("r9", opts::bool_switch(&_flag_r9), "Turns off plotting the standard set, plots r9 set")
        ("et", opts::bool_switch(&_flag_et), "Turns off plotting the standard set, plots Et set")
        ("cat", opts::bool_switch(&_flag_cat), "Turns off plotting the standard set, plots category set")
        ("etaDoubleRatio", opts::bool_switch(&_flag_dr_eta), "Turns off plotting the standard set, plots category set")
        ("invMassDist", opts::bool_switch(&_flag_invMassDist), "Additionally plots the invariant mass distributions for the plots chosen")
        ("moneyPlot", opts::bool_switch(&_flag_moneyPlot), "Turns off plotting the standard set and plots the moneyPlots")
        ("lumiLabel", opts::value<std::string>(&lumiLabel), "String for the luminosity label")
        ("systUncPlots", opts::bool_switch(&_flag_systUnc), "Systematic uncertainty plots (takes 4 root files)")
        ("dataData", opts::bool_switch(&_flag_dataData), "Plot Data on Data")
    ;

    opts::variables_map v_map;
    opts::store(opts::parse_command_line(argc, argv, desc), v_map);
    opts::notify(v_map);

    if(v_map.count("help")){
        std::cout << desc << std::endl;
        return 0;
    }

    std::cout << "[INFO] welcome to the standard set plotter" << std::endl;
    std::cout << "[INFO] the file you've provided is: " << inputFile << std::endl << std::endl;

    ifstream inFile (inputFile);
    if(inFile.peek() == std::ifstream::traits_type::eof()){
        std::cout << "[ERROR] the file: " << inputFile << " is empty, or doesn't exist" << std::endl;
        std::cout << "[EXIT ON FAILURE] " << std::endl;
        return 1;
    }
    
    std::vector<std::string> rootFiles;
    std::string _temp;
    while(inFile >> _temp){
        std::cout << "[INFO] root file: " << _temp << std::endl;
        rootFiles.push_back(_temp);
    } 
    for(int i = 0; i < rootFiles.size(); i++){
        if(!(TFile::Open(rootFiles[i].c_str(), "READ"))){
            std::cout << "[ERROR] file: " << rootFiles[i] << " did not open." <<std::endl;
            return -1;
        }
    }

    std::cout << std::endl;

    standardSet plt;
    if( _flag_systUnc ){
        std::cout << "[INFO] the flag `systUnc` was provided" << std::endl;
        std::cout << "[INFO] plotting the `systUnc` set of plots" << std::endl;
        plt.evaluate_systPlots(rootFiles, -1);
        plt.evaluate_systPlots(rootFiles, 1);
    }
    else if(_flag_eta || _flag_r9 || _flag_et || _flag_cat || _flag_dr_eta || _flag_moneyPlot || _flag_dataData){
        if(_flag_moneyPlot){
            plt.moneyPlots(rootFiles[0], rootFiles[1]);
            plt.etLinearityPlots(rootFiles[0], rootFiles[1]);
            plt.runPlots(rootFiles[0],rootFiles[1]);
        }
        if(_flag_dataData){
            plt.dataData(rootFiles[0], rootFiles[1]);
        }
        if(_flag_eta){
            std::cout << "[INFO] the flag `eta` was provided" << std::endl;
            std::cout << "[INFO] plotting the `eta` set of plots" << std::endl;
            for(int i = -1; i < 2; i++){
                plt.evaluate_EtaPlots(rootFiles[0], rootFiles[1], i, quantity);
            }
            plt.eval_CoarseEtaPlots(rootFiles[0], rootFiles[1]);
            std::cout << std::endl;
        }
        if(_flag_cat){
            std::cout << "[INFO] the flag `cat` was provided" << std::endl;
            std::cout << "[INFO] plotting the `cat` set of plots" << std::endl;
            plt.evaluate_CatPlots(rootFiles[0], rootFiles[1]);   
            std::cout << std::endl;
        }
        if(_flag_dr_eta){

        }
        if(_flag_r9){

        }
        if(_flag_et){
            plt.eval_EtResolution(rootFiles[0], rootFiles[1]);
        }
    }
    else{
        std::cout << "[INFO] no additional options were provided. begin plotting the standard set: " << std::endl;
        for(int i = -1; i < 2; i++){
            plt.evaluate_EtaPlots(rootFiles[0], rootFiles[1], i, quantity);
            //standardSet::evaluate_EtPlots(rootFiles[0], rootFiles[1], i, quantity);
        }
//        plt.evaluate_CatPlots(rootFiles[0], rootFiles[1]);
    }
    
    return 0;
}
