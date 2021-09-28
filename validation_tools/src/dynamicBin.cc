/* 
 * Author: Neil Schroeder
 * gitHub: neilSchroeder@github.com
 *
 * Description:
 *   script which uses ROOT to find a set of variable bins.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "/usr/include/boost/program_options.hpp"

#include "TFile.h"
#include <TH1.h>
#include <TH2.h>
#include <TTree.h>

#include "../interface/utilities.h"
#include "../interface/statistic.h"
#include "../interface/standardSet.h"

//#define DEBUG
using namespace std;

std::string outputFile = "";
double minVal;
double _range;
double smallestBinSize;
int vectorLength = -999;
double maxPercent = 0.005;
bool _flag_vectorBranch = false;


void createBins(TTree *, string );

int main(int argc, char **argv){


    using namespace boost;
    namespace opts = boost::program_options;
    std::string inputFile;
    std::string branchName;

    opts::options_description desc("Main Options");

    desc.add_options()
        ("help", "produce help message")
        ("inputFile,i", opts::value<std::string>(&inputFile), "Input File")
        ("outputFile,o", opts::value<std::string>(&outputFile), "name of file to write")
        ("branchName,b", opts::value<std::string>(&branchName), "name of the branch to optimize bins over")
        ("vectorBranch,v", opts::bool_switch(&_flag_vectorBranch), "activate this option if the branch is a vector")
        ("vectorLength", opts::value<int>(&vectorLength), "length of vector needed for branch")
        ("minVal,m", opts::value<double>(&minVal), "starting value of the x axis")
        ("range,r", opts::value<double>(&_range), "range of the x axis to consider")
        ("smallestBinSize,s", opts::value<double>(&smallestBinSize), "smallest bin size") 
        ("maxPercent", opts::value<double>(&maxPercent), "maximum relative percent error on a bin (Default is 0.5%)")
    ;

    opts::variables_map v_map;
    opts::store(opts::parse_command_line(argc, argv, desc), v_map);
    opts::notify(v_map);

    if(v_map.count("help")){
        std::cout << desc << std::endl;
        return 0;
    }

    ////////////////////////////////////////////
    // print out the options for this program //
    ////////////////////////////////////////////

    std::cout << "###################################################" << std::endl;
    std::cout << "[INFO] dynamic bin script" << std::endl << std::endl;
    std::cout << "[OPTION] inputFile: " << inputFile << std::endl;
    std::cout << "[OPTION] outputFile: " << outputFile << std::endl;
    std::cout << "[OPTION] branchName: " << branchName << std::endl;
    if(_flag_vectorBranch){
        std::cout << "[OPTION] --vectorBranch activated" << std::endl;
        std::cout << "[OPTION] vectorLength: " << vectorLength << std::endl;
    }
    std::cout << "[OPTION] minVal: " << minVal << std::endl;
    std::cout << "[OPTION] range: " << _range << std::endl;
    std::cout << "[OPTION] smallestBinSize: " << smallestBinSize << std::endl;
    std::cout << "[OPTION] maxPercent: " << maxPercent << std::endl;
    std::cout << "###################################################" << std::endl << std::endl;

    if(v_map.count("vectorBranch") && vectorLength == -999){
        std::cout << "[ERROR] you MUST provide the length of the vector" << std::endl;
        return 1;
    }
    if(v_map.count("maxPercent")) maxPercent /=100.;

    std::string fileName;
    std::string treeName;
    ifstream in(inputFile.c_str());
    if( in.peek() == std::ifstream::traits_type::eof()){
        std::cout << "[ERROR] file: " << inputFile << " is empty or does not exist" << std::endl;
        return 1;
    }
    while(in >> fileName >> treeName){
        if(!TFile::Open(fileName.c_str(), "READ")){
            std::cout << "[ERROR] root file: " << fileName << " did not open" << std::endl;
            return 1;
        }
        TFile * myFile = TFile::Open(fileName.c_str(), "READ");
        if(!myFile){
            std::cout << "[ERROR] root file: " << fileName << " has a bad pointer" << std::endl;
            return 1;
        }
        if(myFile->IsZombie()){
            std::cout << "[ERROR] root file: " << fileName << " is a zombie" << std::endl;
            return 1;
        }
        TTree * myTree = (TTree*)myFile->Get(treeName.c_str());
        createBins(myTree, branchName);
    }


    return 0;
}

void createBins(TTree* thisTree, string branchName){

    if(!thisTree){
        std::cout << "[ERROR] tree has a bad pointer" << std::endl;
        return;
    }

    bool notDone = true;
    std::vector<double> bins;
    bins.push_back(minVal);
    
    double binBottomFirst = minVal;
    double binBottom = minVal;
    double binTop = minVal+smallestBinSize;
    double binTopLast = minVal+_range;

    std::cout << binBottomFirst << " " << binBottom << " " << binTop << " " << binTopLast << std::endl;

    while(notDone){
        bool thisBin = true;
        while(thisBin){
            int numEntriesHere = 0;
            if(_flag_vectorBranch){
                char thisCut [100];
                for(int i = 0; i < vectorLength; i++){
                    sprintf(thisCut, std::string("("+branchName+"[%i] > %lf)*("+branchName+"[%i] < %lf)").c_str(), i, binBottom, i, binTop);
                    char vectorBranch [100];
                    sprintf(vectorBranch, std::string(branchName+"[%i]").c_str(), i);
                    numEntriesHere += thisTree->Draw(vectorBranch, thisCut);
                }
            }
            else{
                char thisCut [100];
                sprintf(thisCut, std::string("("+branchName+" > %lf)*("+branchName+" < %lf)").c_str(), binBottom, binTop);
                numEntriesHere += thisTree->Draw(branchName.c_str(), thisCut);
            }
            if( numEntriesHere == 0){
                std::cout << "[ERROR] there are no events in this bin" << std::endl;
            }
            else{
#ifdef DEBUG
                std::cout << binBottom << " " << binTop << " " << sqrt(numEntriesHere) / (double)numEntriesHere << endl;
#endif
                if(sqrt(numEntriesHere) / (double)numEntriesHere < maxPercent){
                    ///done with this bin, exit the loop
                    thisBin = false;
                    bins.push_back(binTop);
                    binBottom = binTop;
                    binTop += smallestBinSize;
                }
                else{
                    binTop += smallestBinSize;
                }
            }
            if(binTop >= binTopLast){notDone = false; break;}
        }
    }
    bins.push_back(max(binTopLast,binTop));
    ofstream out;
    out.open("output_createBins.dat");
    for(int i = 0; i < bins.size()-1;i++){
        out << bins[i] << ", ";
    }
    out << bins[bins.size()-1];
    out.close();

    return;
}
