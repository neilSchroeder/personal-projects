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
#include "/usr/include/boost/program_options.hpp"

#include "TFile.h"
#include <TH1.h>
#include <TH2.h>

#include "../interface/utilities.h"
#include "../interface/statistic.h"
#include "../interface/standardSet.h"

//#define DEBUG
using namespace std;

float N_PERCENT_HIST = 100;

bool _flag_eta = false;
bool _flag_r9 = false;
bool _flag_et = false;
bool _flag_cat = false;
bool _flag_dr_eta = false;
bool _flag_invMassDist = false;

std::string outputFile = "";
std::string dataTitle = "";
std::string mcTitle = "";

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
        ("outputFile,o", opts::value<std::string>(&outputFile), "name of file to write")
        ("dataTitle,d", opts::value<std::string>(&dataTitle), "Legend entry for the data")
        ("mcTitle,m", opts::value<std::string>(&mcTitle), "Legend entry for the mc")
        ("eta", opts::bool_switch(&_flag_eta), "Turns off plotting the standard set, plots eta set")
        ("r9", opts::bool_switch(&_flag_r9), "Turns off plotting the standard set, plots r9 set")
        ("et", opts::bool_switch(&_flag_et), "Turns off plotting the standard set, plots Et set")
        ("cat", opts::bool_switch(&_flag_cat), "Turns off plotting the standard set, plots category set")
        ("etaDoubleRatio", opts::bool_switch(&_flag_dr_eta), "Turns off plotting the standard set, plots category set")
        ("invMassDist", opts::bool_switch(&_flag_invMassDist), "Additionally plots the invariant mass distributions for the plots chosen")
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
    std::cout << std::endl;

    if(_flag_eta || _flag_r9 || _flag_et || _flag_cat || _flag_dr_eta){
        if(_flag_eta){
            std::cout << "[INFO] the flag `eta` was provided" << std::endl;
            std::cout << "[INFO] plotting the `eta` set of plots" << std::endl;
            for(int i = -1; i < 2; i++){
                standardSet::evaluate_EtaPlots(rootFiles[0], rootFiles[1], i, quantity);
            }
            std::cout << std::endl;
        }
        if(_flag_cat){
            std::cout << "[INFO] the flag `cat` was provided" << std::endl;
            std::cout << "[INFO] plotting the `cat` set of plots" << std::endl;
            standardSet::evaluate_CatPlots(rootFiles[0], rootFiles[1]);   
            std::cout << std::endl;
        }
        if(_flag_dr_eta){

        }
        if(_flag_r9){

        }
        if(_flag_et){

        }
    }
    else{
        std::cout << "[INFO] no additional options were provided. begin plotting the standard set: " << std::endl;
        for(int i = -1; i < 2; i++){
            standardSet::evaluate_EtaPlots(rootFiles[0], rootFiles[1], i, quantity);
            //standardSet::evaluate_EtPlots(rootFiles[0], rootFiles[1], i, quantity);
        }
        standardSet::evaluate_CatPlots(rootFiles[0], rootFiles[1]);
    }
    
    return 0;
}
