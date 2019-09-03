/*
 *
 *
 *
 *
 * This script creates a root file called r9CorrInvMass_fileName
 *   which contains histograms of 2D distributions (invariant mass by r9)
 *   in various eta regions
 *
 *
 *
 *
 *
 */

#include <TH2.h>
#include <TGraph.h>
#include <TFile.h>
#include <TTree.h>
#include <TTreeIndex.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TRandom.h>
#include <TKey.h>
#include <TCollection.h>
#include <TList.h>

#include <assert.h>
#include <string>
#include <fstream>
#include <iostream>

#include "/usr/include/boost/program_options.hpp"
#include "/afs/cern.ch/work/n/nschroed/ECALELF/CMSSW_9_4_9_cand1/src/RecoEgamma/EgammaTools/interface/EnergyScaleCorrection.h"

//#define DEBUG

using namespace std;

const string info = "[INFO] ";
const string error = "[ERROR] ";

void producer_scaleSmearHists(string, string, string, std::vector<string>);

bool _flag_tightID = false;
bool _flag_mediumID = false;
bool _flag_transformR9 = false;
bool _flag_makeTree = false;

std::string output = "";
std::string temp_output = "";

int main(int argc, char **argv){
    using namespace boost;
    namespace opts = boost::program_options;
    std::string configFile;
    std::string puConfigFile;
    std::string ssConfigFile;

    opts::options_description desc("Main Options");

    desc.add_options()
        ("help,h", "produce help message")
        ("configFile,f", opts::value<std::string>(&configFile), "Config File")
        ("output,o", opts::value<std::string>(&output), "string used to make the output file")
        ("makeTree,t", opts::bool_switch(&_flag_makeTree), "Make a root tree which contains all the necessary information to make the histograms in this file")
        ("mediumID", opts::bool_switch(&_flag_mediumID), "Apply the medium ID working point")
        ("tightID", opts::bool_switch(&_flag_tightID), "Apply the tight ID working point")
        ("transformR9", opts::bool_switch(&_flag_transformR9), "Apply R9 corrections")
        ;

    opts::variables_map v_map;
    opts::store(opts::parse_command_line(argc, argv, desc), v_map);
    opts::notify(v_map);

    if(v_map.count("help")){
        cout << desc << endl;
        return 0;
    }
    
    /////////////////////////////////////
    //    some quick error handling    //
    /////////////////////////////////////

    cout << info << "This is the producer of histograms (and a tree) to cross check the application of scales and smearings" << endl;

    if(v_map.count("configFile")){ //catch empty config file
        ifstream in(configFile);
        if(in.peek() == std::ifstream::traits_type::eof()){
            cout << error << "Config file: " << configFile << " is either empty or does not exist" << endl;
            cout << error << " cannot recover" << endl;
            cout << "[EXIT]"<< endl;
            return 1;
        }
    }

    cout << "#########################################################" << endl <<endl;

    string id = "";
    string transR9 = "";
    if(_flag_makeTree) cout << info << "[OPTION] 'makeTree' provided, a root tree will be written" << endl;
    if(_flag_tightID){
        cout << info << "[OPTION] 'tightID' provided, tight ID working point will be applied" << endl;
        id = "_tightID";
    }
    if(_flag_mediumID){
        cout << info << "[OPTION] 'mediumID' provided, medium ID working point will be applied" << endl;
        id = "_mediumID";
    }
    if(_flag_tightID && _flag_mediumID){
        cout << info << error << "both 'tightID' and 'mediumID' options were provided, defaulting to tightID" << endl;
        _flag_mediumID = false;
        id = "_tightID";
    }
    if(_flag_transformR9){
        cout << info << "[OPTION] 'transformR9' provided, r9 corrections will be applied" << endl;
        transR9 = "_transformR9";
    }

    std::vector<string> inputFiles;
    std::vector<string> fileTypes;
    std::vector<string> puFiles;
    std::vector<string> ssFiles;
    ifstream in(configFile.c_str());
    string type;
    string ntuples;
    string tpuFile;
    string ssFile;
    while( in >> ntuples >> type >> tpuFile >> ssFile ){
        fileTypes.push_back(type);
        inputFiles.push_back(ntuples);
        puFiles.push_back(tpuFile);
        ssFiles.push_back(ssFile);
    }
    in.close();

    //////////////////////////////////////////////////////////
    // now we'll create the output file for this production //
    //////////////////////////////////////////////////////////

    temp_output = "producer_scaleSmearHists_"+output;
    std::cout << "[INFO] output file: " << temp_output << std::endl;

    system( std::string("./scripts/check_output.sh "+temp_output+".dat").c_str());

    for(int i = 0; i < inputFiles.size(); i++){
        std::cout << "###########################################################" << std::endl;
        std::cout << info << "running producer_scaleSmearHists with: " << std::endl;
        std::cout << info << "type:      " << fileTypes[i] << std::endl;
        std::cout << info << "inputFile: " << inputFiles[i] << std::endl;
        std::cout << info << "ssFile:    " << ssFiles[i] << std::endl;
        producer_scaleSmearHists(fileTypes[i], inputFiles[i], ssFiles[i], puFiles);
    }
    
    return 0;
}


void producer_scaleSmearHists( string type, string fileName, string scaleSmearFile, std::vector<string> pileupFiles){
    std::ifstream check_in(fileName.c_str());
    if(check_in.peek() == std::ifstream::traits_type::eof()){
        std::cout << "[ERROR] input file: " << fileName << " is empty or does not exist." << std::endl;
        return;
    }

    /////////////////////////////////////////////////////
    // need a random number for applying the smearings //
    /////////////////////////////////////////////////////

    gRandom->SetSeed(0);

    //////////////////////////////////////////////////////////////////////////////
    // temp and tmep are the bin edges for the run ranges in 2016 and 2017/2018 //
    //////////////////////////////////////////////////////////////////////////////

    double temp [150] = {273158, 273446, 273494, 273725, 273728, 274094, 274172, 274241, 274244, 274251, 274316, 274317, 274336, 274344, 274388, 274422, 274440, 274442, 274968, 274969, 274970, 274998, 274999, 275000, 275068, 275073, 275124, 275282, 275310, 275311, 275337, 275344, 275375, 275376, 275657, 275776, 275782, 275832, 275835, 275847, 275886, 275911, 275913, 275920, 276242, 276244, 276282, 276283, 276315, 276361, 276363, 276384, 276437, 276454, 276501, 276525, 276527, 276543, 276582, 276585, 276587, 276655, 276659, 276776, 276794, 276808, 276811, 276831, 276870, 276935, 276950, 277069, 277076, 277087, 277096, 277112, 277148, 277168, 277194, 277305, 277981, 278018, 278167, 278175, 278240, 278308, 278310, 278345, 278349, 278406, 278509, 278769, 278801, 278808,   278820, 278822, 278873, 278923, 278962, 278969, 278975, 279024, 279115, 279479, 279654, 279667, 279694, 279715, 279716, 279760, 279766, 279794, 279841, 279844, 279931, 281613, 281693, 281707, 281726, 281797, 281976, 282037, 282092, 282708, 282735, 282800, 282814, 282842, 283042, 283052, 283270, 283283, 283305, 283308, 283353, 283358, 283408, 283416, 283478, 283548, 283820, 283830, 283865, 283876, 283877, 283885, 283934, 283946, 283964, 284025};
    double tmep [160] = {297050, 297099, 297113, 297178, 297219, 297224, 297411, 297424, 297432, 297484, 297486, 297488, 297504, 297558, 297599, 297604, 297620, 297666, 297722, 299061, 299067, 299178, 299368, 299380, 299479, 299481, 299593, 299594, 300087, 300122, 300155, 300156, 300226, 300236, 300238, 300280, 300284, 300389, 300401, 300461, 300466, 300497, 300516, 300560, 300576, 300631, 300636, 300742, 300785, 300806, 300812, 301142, 301165, 301298, 301323, 301330, 301391, 301397, 301447, 301461, 301475, 301529, 301627, 301665, 301959, 301960, 301987, 301998, 302031, 302042, 302159, 302225, 302229, 302262, 302277, 302328, 302350, 302393, 302448, 302472, 302473, 302484, 302492, 302526, 302573, 302596, 302597, 302635, 302651, 303825, 303832, 303838, 303885, 303948, 303998, 304062, 304125, 304144, 304158, 304169, 304170, 304204, 304292, 304333, 304366, 304447, 304451, 304508, 304562, 304616, 304654, 304655, 304661, 304671, 304738, 304739, 304778, 304797, 305044, 305046, 305062, 305081, 305112, 305113, 305186, 305202, 305204, 305207, 305237, 305248, 305252, 305311, 305314, 305351, 305364, 305366, 305376, 305406, 305440, 305517, 305589, 305590, 305636, 305766, 305814, 305821, 305832, 305842, 305898, 305902, 306041, 306049, 306092, 306125, 306126, 306138, 306154, 306155, 306169, 306456};
    double npuBins [36] = {0, 5, 7.5, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36 ,37, 38, 39, 40, 45, 50};
    double etBins [7] = { 32, 40, 50., 80., 120., 300, 99999};
    double etBins2 [6] = { 32., 40., 50., 80, 120., 200};
    double r9Bins [7] = {0., 0.5, 0.8, 0.9, 0.92, 0.96, 1.};
    double r9Bins2 [6] = {0., 0.8, 0.9, 0.92, 0.96, 1.};

    ///////////////////////////////////////
    // get and create the PURW histogram //
    ///////////////////////////////////////
    
    TFile * weightFile;
    TH1D * puWeights;
    TH1D * puWeights2;
    std::cout << "[INFO] initializing PURW histograms" << std::endl;
    if( type.compare("data") == 0 ){
        puWeights = new TH1D("puWeights", "", 100, 0, 100);
        for(int i = 1; i <= 100; i++){
            puWeights->SetBinContent(i, 1.);
        }
    }
    else if( pileupFiles[1].compare("x") == 0){
        std::cout << "########################################" << std::endl;
        std::cout << "[INFO] no pu reweighting will be applied" << std::endl;
        std::cout << "########################################" << std::endl << std::endl;
    }
    else{
        if(pileupFiles[0].compare("x") == 0){
            if( !TFile::Open(pileupFiles[1].c_str(), "READ") ){
                cout << error << "could not open file: " << pileupFiles[1] << endl;
            }
            else{
                weightFile = TFile::Open( pileupFiles[1].c_str(), "READ");
                TIter next(weightFile->GetListOfKeys());
                TKey *key;
                while( (key = (TKey*)next()) ){
                    puWeights = (TH1D*)weightFile->Get(key->GetName());
                }
                if(!puWeights){
                    std::cout << "[ERROR] could not find a suitable histogram" << std::endl;
                }
                if(puWeights->Integral() != 1) puWeights->Scale(1/puWeights->Integral());
            }
        }
        else{
            if( !TFile::Open(pileupFiles[0].c_str(), "READ") && !TFile::Open(pileupFiles[1].c_str()) ){
                if( !TFile::Open(pileupFiles[0].c_str(), "READ") ){
                    cout << error << "could not open file: " << pileupFiles[0] << endl;
                    return;
                }
                if( !TFile::Open(pileupFiles[1].c_str(), "READ") ){
                    cout << error << "could not open file: " << pileupFiles[1] << endl;
                    return;
                }
            }
            else{
                weightFile = TFile::Open(pileupFiles[0].c_str(), "READ");
                TIter next(weightFile->GetListOfKeys());
                TKey * key;
                while( (key = (TKey*)next()) ){
                    puWeights = (TH1D*)weightFile->Get(key->GetName());
                }
                TFile * weightFile2 = TFile::Open(pileupFiles[1].c_str(), "READ");
                TIter next2(weightFile2->GetListOfKeys());
                while( (key = (TKey*)next2()) ){
                    puWeights2 = (TH1D*)weightFile2->Get(key->GetName());
                }
                if(puWeights->Integral() != 1) puWeights->Scale(1/puWeights->Integral());
                if(puWeights2->Integral() != 1) puWeights2->Scale(1/puWeights2->Integral());
                for(int i = 1; i <= puWeights->GetNbinsX(); i++){
                    if( puWeights->GetBinContent(i) != 0) puWeights->SetBinContent(i, puWeights->GetBinContent(i)/puWeights2->GetBinContent(i));
                    else puWeights->SetBinContent(i, puWeights2->GetBinContent(i));
                }
                puWeights->Scale(1/puWeights->Integral());
            }
        }
    }

    if( !puWeights){ 
        cout << error << "failed to initialize pileup hist" << endl;
        return;
    }

    ///////////////////////////////////////////////
    // declare the energy scale correction class //
    ///////////////////////////////////////////////

    std::cout << "[INFO] Creating EnergyScaleCorrection scaleSmearer from: " << std::endl;
    EnergyScaleCorrection scalerSmearer;
    scalerSmearer = EnergyScaleCorrection(scaleSmearFile.c_str());
    std::cout << std::endl;

    ////////////////////////////////////////////////////////
    // declare the histograms we use for the cross checks //
    ////////////////////////////////////////////////////////

    std::cout << "[INFO] initializing histograms" << std::endl;
    TH2F * invMassVsNPU = new TH2F("invMassVsNPU", "", 35, npuBins, 400, 80, 100);
    TH2F * mee_npu_rawSC_low = new TH2F("mee_npu_rawSC_low", "", 35, npuBins, 400, 80, 100);
    TH2F * mee_npu_rawSC_high = new TH2F("mee_npu_rawSC_high", "", 35, npuBins, 400, 80, 100);
    TH2F * mee_npu_ele_low = new TH2F("mee_npu_ele_low", "", 35, npuBins, 400, 80, 100);
    TH2F * mee_npu_ele_high = new TH2F("mee_npu_ele_high", "", 35, npuBins, 400, 80, 100);
    
    TH2F * invMassVsRun_ele_EB_in_Low ; 
    TH2F * invMassVsRun_ele_EB_out_Low ; 
    TH2F * invMassVsRun_ele_EE_in_Low ; 
    TH2F * invMassVsRun_ele_EE_out_Low ; 
    TH2F * invMassVsRun_ele_EB_in_High ; 
    TH2F * invMassVsRun_ele_EB_out_High;
    TH2F * invMassVsRun_ele_EE_in_High ; 
    TH2F * invMassVsRun_ele_EE_out_High;
    TH2F * invMassVsRun_pho_EB_in_Low ; 
    TH2F * invMassVsRun_pho_EB_out_Low ; 
    TH2F * invMassVsRun_pho_EE_in_Low ; 
    TH2F * invMassVsRun_pho_EE_out_Low ; 
    TH2F * invMassVsRun_pho_EB_in_High ; 
    TH2F * invMassVsRun_pho_EB_out_High;
    TH2F * invMassVsRun_pho_EE_in_High ; 
    TH2F * invMassVsRun_pho_EE_out_High;

    TH1F * invMass = new TH1F("invMass", "", 400, 80, 100);
    TH1F * r9Dist = new TH1F("r9dist", "", 500, 0, 1);
    TH1F * r9Dist_EB = new TH1F("r9Dist_EB", "", 500, 0, 1);
    TH1F * r9Dist_EE = new TH1F("r9Dist_EE", "", 500, 0, 1);
    
    TH1F * invMass_Et0_EBin = new TH1F("invMass_Et0_EBin", "", 10000, 0, 500);
    TH1F * invMass_Et0_EBout = new TH1F("invMass_Et0_EBout", "", 10000, 0, 500);
    TH1F * invMass_Et0_EEin = new TH1F("invMass_Et0_EEin", "", 10000, 0, 500);
    TH1F * invMass_Et0_EEout = new TH1F("invMass_Et0_EEout", "", 10000, 0, 500);
    TH1F * invMass_Et1_EBin = new TH1F("invMass_Et1_EBin", "", 10000, 0, 500);
    TH1F * invMass_Et1_EBout = new TH1F("invMass_Et1_EBout", "", 10000, 0, 500);
    TH1F * invMass_Et1_EEin = new TH1F("invMass_Et1_EEin", "", 10000, 0, 500);
    TH1F * invMass_Et1_EEout = new TH1F("invMass_Et1_EEout", "", 10000, 0, 500);
    TH1F * invMass_Et2_EBin = new TH1F("invMass_Et2_EBin", "", 10000, 0, 500);
    TH1F * invMass_Et2_EBout = new TH1F("invMass_Et2_EBout", "", 10000, 0, 500);
    TH1F * invMass_Et2_EEin = new TH1F("invMass_Et2_EEin", "", 10000, 0, 500);
    TH1F * invMass_Et2_EEout = new TH1F("invMass_Et2_EEout", "", 10000, 0, 500);
    TH1F * invMass_Et3_EBin = new TH1F("invMass_Et3_EBin", "", 10000, 0, 500);
    TH1F * invMass_Et3_EBout = new TH1F("invMass_Et3_EBout", "", 10000, 0, 500);
    TH1F * invMass_Et3_EEin = new TH1F("invMass_Et3_EEin", "", 10000, 0, 500);
    TH1F * invMass_Et3_EEout = new TH1F("invMass_Et3_EEout", "", 10000, 0, 500);
    TH1F * invMass_Et4_EBin = new TH1F("invMass_Et4_EBin", "", 10000, 0, 500);
    TH1F * invMass_Et4_EBout = new TH1F("invMass_Et4_EBout", "", 10000, 0, 500);
    TH1F * invMass_Et4_EEin = new TH1F("invMass_Et4_EEin", "", 10000, 0, 500);
    TH1F * invMass_Et4_EEout = new TH1F("invMass_Et4_EEout", "", 10000, 0, 500);

    TH1F * invMass_Et0 = new TH1F("invMass_Et0", "", 10000, 0, 500);
    TH1F * invMass_Et1 = new TH1F("invMass_Et1", "", 10000, 0, 500);
    TH1F * invMass_Et2 = new TH1F("invMass_Et2", "", 10000, 0, 500);
    TH1F * invMass_Et3 = new TH1F("invMass_Et3", "", 10000, 0, 500);
    TH1F * invMass_Et4 = new TH1F("invMass_Et4", "", 10000, 0, 500);
    TH1F * invMass_Et5 = new TH1F("invMass_Et5", "", 10000, 0, 500);

    TH1F * invMassUp_Et0 = new TH1F("invMassUp_Et0", "", 10000, 0, 500);
    TH1F * invMassUp_Et1 = new TH1F("invMassUp_Et1", "", 10000, 0, 500);
    TH1F * invMassUp_Et2 = new TH1F("invMassUp_Et2", "", 10000, 0, 500);
    TH1F * invMassUp_Et3 = new TH1F("invMassUp_Et3", "", 10000, 0, 500);
    TH1F * invMassUp_Et4 = new TH1F("invMassUp_Et4", "", 10000, 0, 500);
    TH1F * invMassUp_Et5 = new TH1F("invMassUp_Et5", "", 10000, 0, 500);

    TH1F * invMassDown_Et0 = new TH1F("invMassDown_Et0", "", 10000, 0, 500);
    TH1F * invMassDown_Et1 = new TH1F("invMassDown_Et1", "", 10000, 0, 500);
    TH1F * invMassDown_Et2 = new TH1F("invMassDown_Et2", "", 10000, 0, 500);
    TH1F * invMassDown_Et3 = new TH1F("invMassDown_Et3", "", 10000, 0, 500);
    TH1F * invMassDown_Et4 = new TH1F("invMassDown_Et4", "", 10000, 0, 500);
    TH1F * invMassDown_Et5 = new TH1F("invMassDown_Et5", "", 10000, 0, 500);

    TH1F * invMass_ele_EB_in_Low = new TH1F("invMass_ele_EB_in_Low", "", 400, 80, 100);
    TH1F * invMass_ele_EB_out_Low = new TH1F("invMass_ele_EB_out_Low", "", 400, 80, 100);
    TH1F * invMass_ele_EE_in_Low = new TH1F("invMass_ele_EE_in_Low", "", 400, 80, 100);
    TH1F * invMass_ele_EE_out_Low = new TH1F("invMass_ele_EE_out_Low", "", 400, 80, 100);
    TH1F * invMass_ele_EB_in_High = new TH1F("invMass_ele_EB_in_High", "", 400, 80, 100);
    TH1F * invMass_ele_EB_out_High = new TH1F("invMass_ele_EB_out_High", "", 400, 80, 100);
    TH1F * invMass_ele_EE_in_High = new TH1F("invMass_ele_EE_in_High", "", 400, 80, 100);
    TH1F * invMass_ele_EE_out_High = new TH1F("invMass_ele_EE_out_High", "", 400, 80, 100);
    TH1F * invMass_ele_NotEbEb_high = new TH1F("invMass_ele_NotEbEb_high", "", 400, 80, 100);
    TH1F * invMassUp_ele_NotEbEb_high = new TH1F("invMassUp_ele_NotEbEb_high", "", 400, 80, 100);
    TH1F * invMassDown_ele_NotEbEb_high = new TH1F("invMassDown_ele_NotEbEb_high", "", 400, 80, 100);
    TH1F * invMass_pho_EB_in_Low = new TH1F("invMass_pho_EB_in_Low", "", 400, 80, 100);
    TH1F * invMass_pho_EB_out_Low = new TH1F("invMass_pho_EB_out_Low", "", 400, 80, 100);
    TH1F * invMass_pho_EE_in_Low = new TH1F("invMass_pho_EE_in_Low", "", 400, 80, 100);
    TH1F * invMass_pho_EE_out_Low = new TH1F("invMass_pho_EE_out_Low", "", 400, 80, 100);
    TH1F * invMass_pho_EB_in_High = new TH1F("invMass_pho_EB_in_High", "", 400, 80, 100);
    TH1F * invMass_pho_EB_out_High = new TH1F("invMass_pho_EB_out_High", "", 400, 80, 100);
    TH1F * invMass_pho_EE_in_High = new TH1F("invMass_pho_EE_in_High", "", 400, 80, 100);
    TH1F * invMass_pho_EE_out_High = new TH1F("invMass_pho_EE_out_High", "", 400, 80, 100);
    TH1F * invMassUp_ele_EB_in_Low = new TH1F("invMassUp_ele_EB_in_Low", "", 400, 80, 100);
    TH1F * invMassUp_ele_EB_out_Low = new TH1F("invMassUp_ele_EB_out_Low", "", 400, 80, 100);
    TH1F * invMassUp_ele_EE_in_Low = new TH1F("invMassUp_ele_EE_in_Low", "", 400, 80, 100);
    TH1F * invMassUp_ele_EE_out_Low = new TH1F("invMassUp_ele_EE_out_Low", "", 400, 80, 100);
    TH1F * invMassUp_ele_EB_in_High = new TH1F("invMassUp_ele_EB_in_High", "", 400, 80, 100);
    TH1F * invMassUp_ele_EB_out_High = new TH1F("invMassUp_ele_EB_out_High", "", 400, 80, 100);
    TH1F * invMassUp_ele_EE_in_High = new TH1F("invMassUp_ele_EE_in_High", "", 400, 80, 100);
    TH1F * invMassUp_ele_EE_out_High = new TH1F("invMassUp_ele_EE_out_High", "", 400, 80, 100);
    TH1F * invMassUp_pho_EB_in_Low = new TH1F("invMassUp_pho_EB_in_Low", "", 400, 80, 100);
    TH1F * invMassUp_pho_EB_out_Low = new TH1F("invMassUp_pho_EB_out_Low", "", 400, 80, 100);
    TH1F * invMassUp_pho_EE_in_Low = new TH1F("invMassUp_pho_EE_in_Low", "", 400, 80, 100);
    TH1F * invMassUp_pho_EE_out_Low = new TH1F("invMassUp_pho_EE_out_Low", "", 400, 80, 100);
    TH1F * invMassUp_pho_EB_in_High = new TH1F("invMassUp_pho_EB_in_High", "", 400, 80, 100);
    TH1F * invMassUp_pho_EB_out_High = new TH1F("invMassUp_pho_EB_out_High", "", 400, 80, 100);
    TH1F * invMassUp_pho_EE_in_High = new TH1F("invMassUp_pho_EE_in_High", "", 400, 80, 100);
    TH1F * invMassUp_pho_EE_out_High = new TH1F("invMassUp_pho_EE_out_High", "", 400, 80, 100);
    TH1F * invMassDown_ele_EB_in_Low = new TH1F("invMassDown_ele_EB_in_Low", "", 400, 80, 100);
    TH1F * invMassDown_ele_EB_out_Low = new TH1F("invMassDown_ele_EB_out_Low", "", 400, 80, 100);
    TH1F * invMassDown_ele_EE_in_Low = new TH1F("invMassDown_ele_EE_in_Low", "", 400, 80, 100);
    TH1F * invMassDown_ele_EE_out_Low = new TH1F("invMassDown_ele_EE_out_Low", "", 400, 80, 100);
    TH1F * invMassDown_ele_EB_in_High = new TH1F("invMassDown_ele_EB_in_High", "", 400, 80, 100);
    TH1F * invMassDown_ele_EB_out_High = new TH1F("invMassDown_ele_EB_out_High", "", 400, 80, 100);
    TH1F * invMassDown_ele_EE_in_High = new TH1F("invMassDown_ele_EE_in_High", "", 400, 80, 100);
    TH1F * invMassDown_ele_EE_out_High = new TH1F("invMassDown_ele_EE_out_High", "", 400, 80, 100);
    TH1F * invMassDown_pho_EB_in_Low = new TH1F("invMassDown_pho_EB_in_Low", "", 400, 80, 100);
    TH1F * invMassDown_pho_EB_out_Low = new TH1F("invMassDown_pho_EB_out_Low", "", 400, 80, 100);
    TH1F * invMassDown_pho_EE_in_Low = new TH1F("invMassDown_pho_EE_in_Low", "", 400, 80, 100);
    TH1F * invMassDown_pho_EE_out_Low = new TH1F("invMassDown_pho_EE_out_Low", "", 400, 80, 100);
    TH1F * invMassDown_pho_EB_in_High = new TH1F("invMassDown_pho_EB_in_High", "", 400, 80, 100);
    TH1F * invMassDown_pho_EB_out_High = new TH1F("invMassDown_pho_EB_out_High", "", 400, 80, 100);
    TH1F * invMassDown_pho_EE_in_High = new TH1F("invMassDown_pho_EE_in_High", "", 400, 80, 100);
    TH1F * invMassDown_pho_EE_out_High = new TH1F("invMassDown_pho_EE_out_High", "", 400, 80, 100);

    TH1F * invMass_R90 = new TH1F("invMass_R90", "", 400, 80, 100);
    TH1F * invMass_R91 = new TH1F("invMass_R91", "", 400, 80, 100);
    TH1F * invMass_R92 = new TH1F("invMass_R92", "", 400, 80, 100);
    TH1F * invMass_R93 = new TH1F("invMass_R93", "", 400, 80, 100);
    TH1F * invMass_R94 = new TH1F("invMass_R94", "", 400, 80, 100);
    TH1F * invMass_R95 = new TH1F("invMass_R95", "", 400, 80, 100);
    TH1F * invMassUp_R90 = new TH1F("invMassUp_R90", "", 400, 80, 100);
    TH1F * invMassUp_R91 = new TH1F("invMassUp_R91", "", 400, 80, 100);
    TH1F * invMassUp_R92 = new TH1F("invMassUp_R92", "", 400, 80, 100);
    TH1F * invMassUp_R93 = new TH1F("invMassUp_R93", "", 400, 80, 100);
    TH1F * invMassUp_R94 = new TH1F("invMassUp_R94", "", 400, 80, 100);
    TH1F * invMassUp_R95 = new TH1F("invMassUp_R95", "", 400, 80, 100);
    TH1F * invMassDown_R90 = new TH1F("invMassDown_R90", "", 400, 80, 100);
    TH1F * invMassDown_R91 = new TH1F("invMassDown_R91", "", 400, 80, 100);
    TH1F * invMassDown_R92 = new TH1F("invMassDown_R92", "", 400, 80, 100);
    TH1F * invMassDown_R93 = new TH1F("invMassDown_R93", "", 400, 80, 100);
    TH1F * invMassDown_R94 = new TH1F("invMassDown_R94", "", 400, 80, 100);
    TH1F * invMassDown_R95 = new TH1F("invMassDown_R95", "", 400, 80, 100);

    TH1F * leading_pt = new TH1F("leading_pt", "", 1000, 0, 100);
    TH1F * subleading_pt = new TH1F("subleading_pt", "", 1000, 0, 100);
    TH1F * npu_distribution = new TH1F("npu_dist", "", 100, 0, 100);

    TH1F * pt_55_65 = new TH1F("pt_55_65", "", 400, 80, 100);
    TH1F * pt_55_65_up = new TH1F("pt_55_65_up", "", 400, 80, 100);
    TH1F * pt_55_65_down = new TH1F("pt_55_65_down", "", 400, 80, 100);

    
    if( fileName.find("2016") != string::npos){
        invMassVsRun_ele_EB_in_Low =   new TH2F("invMassVsRun_ele_EB_in_Low", "",   149, temp, 400, 80, 100);
        invMassVsRun_ele_EB_out_Low =  new TH2F("invMassVsRun_ele_EB_out_Low", "",  149, temp, 400, 80, 100);
        invMassVsRun_ele_EE_in_Low =   new TH2F("invMassVsRun_ele_EE_in_Low", "",   149, temp, 400, 80, 100);
        invMassVsRun_ele_EE_out_Low =  new TH2F("invMassVsRun_ele_EE_out_Low", "",  149, temp, 400, 80, 100);
        invMassVsRun_ele_EB_in_High =  new TH2F("invMassVsRun_ele_EB_in_High", "",  149, temp, 400, 80, 100);
        invMassVsRun_ele_EB_out_High = new TH2F("invMassVsRun_ele_EB_out_High", "", 149, temp, 400, 80, 100);
        invMassVsRun_ele_EE_in_High =  new TH2F("invMassVsRun_ele_EE_in_High", "",  149, temp, 400, 80, 100);
        invMassVsRun_ele_EE_out_High = new TH2F("invMassVsRun_ele_EE_out_High", "", 149, temp, 400, 80, 100);
        invMassVsRun_pho_EB_in_Low =   new TH2F("invMassVsRun_pho_EB_in_Low", "",   149, temp, 400, 80, 100);
        invMassVsRun_pho_EB_out_Low =  new TH2F("invMassVsRun_pho_EB_out_Low", "",  149, temp, 400, 80, 100);
        invMassVsRun_pho_EE_in_Low =   new TH2F("invMassVsRun_pho_EE_in_Low", "",   149, temp, 400, 80, 100);
        invMassVsRun_pho_EE_out_Low =  new TH2F("invMassVsRun_pho_EE_out_Low", "",  149, temp, 400, 80, 100);
        invMassVsRun_pho_EB_in_High =  new TH2F("invMassVsRun_pho_EB_in_High", "",  149, temp, 400, 80, 100);
        invMassVsRun_pho_EB_out_High = new TH2F("invMassVsRun_pho_EB_out_High", "", 149, temp, 400, 80, 100);
        invMassVsRun_pho_EE_in_High =  new TH2F("invMassVsRun_pho_EE_in_High", "",  149, temp, 400, 80, 100);
        invMassVsRun_pho_EE_out_High = new TH2F("invMassVsRun_pho_EE_out_High", "", 149, temp, 400, 80, 100);
    }
    if( fileName.find("2018") != string::npos || fileName.find("2017") != string::npos){
        invMassVsRun_ele_EB_in_Low =   new TH2F("invMassVsRun_ele_EB_in_Low", "",   159, tmep, 400, 80, 100); 
        invMassVsRun_ele_EB_out_Low =  new TH2F("invMassVsRun_ele_EB_out_Low", "",  159, tmep, 400, 80, 100);
        invMassVsRun_ele_EE_in_Low =   new TH2F("invMassVsRun_ele_EE_in_Low", "",   159, tmep, 400, 80, 100);
        invMassVsRun_ele_EE_out_Low =  new TH2F("invMassVsRun_ele_EE_out_Low", "",  159, tmep, 400, 80, 100);
        invMassVsRun_ele_EB_in_High =  new TH2F("invMassVsRun_ele_EB_in_High", "",  159, tmep, 400, 80, 100);
        invMassVsRun_ele_EB_out_High = new TH2F("invMassVsRun_ele_EB_out_High", "", 159, tmep, 400, 80, 100);
        invMassVsRun_ele_EE_in_High =  new TH2F("invMassVsRun_ele_EE_in_High", "",  159, tmep, 400, 80, 100);
        invMassVsRun_ele_EE_out_High = new TH2F("invMassVsRun_ele_EE_out_High", "", 159, tmep, 400, 80, 100);
        invMassVsRun_pho_EB_in_Low =   new TH2F("invMassVsRun_pho_EB_in_Low", "",   159, tmep, 400, 80, 100);
        invMassVsRun_pho_EB_out_Low =  new TH2F("invMassVsRun_pho_EB_out_Low", "",  159, tmep, 400, 80, 100);
        invMassVsRun_pho_EE_in_Low =   new TH2F("invMassVsRun_pho_EE_in_Low", "",   159, tmep, 400, 80, 100);
        invMassVsRun_pho_EE_out_Low =  new TH2F("invMassVsRun_pho_EE_out_Low", "",  159, tmep, 400, 80, 100);
        invMassVsRun_pho_EB_in_High =  new TH2F("invMassVsRun_pho_EB_in_High", "",  159, tmep, 400, 80, 100);
        invMassVsRun_pho_EB_out_High = new TH2F("invMassVsRun_pho_EB_out_High", "", 159, tmep, 400, 80, 100);
        invMassVsRun_pho_EE_in_High =  new TH2F("invMassVsRun_pho_EE_in_High", "",  159, tmep, 400, 80, 100);
        invMassVsRun_pho_EE_out_High = new TH2F("invMassVsRun_pho_EE_out_High", "", 159, tmep, 400, 80, 100);
    }

    invMassVsNPU->Sumw2();
    mee_npu_rawSC_low->Sumw2();
    mee_npu_rawSC_high->Sumw2();
    mee_npu_ele_low->Sumw2();
    mee_npu_ele_high->Sumw2();
    invMassVsRun_ele_EB_in_Low->Sumw2();
    invMassVsRun_ele_EB_out_Low->Sumw2();
    invMassVsRun_ele_EE_in_Low->Sumw2();
    invMassVsRun_ele_EE_out_Low->Sumw2();
    invMassVsRun_ele_EB_in_High->Sumw2();
    invMassVsRun_ele_EB_out_High->Sumw2();
    invMassVsRun_ele_EE_in_High->Sumw2();
    invMassVsRun_ele_EE_out_High->Sumw2();
    invMassVsRun_pho_EB_in_Low->Sumw2();
    invMassVsRun_pho_EB_out_Low->Sumw2();
    invMassVsRun_pho_EE_in_Low->Sumw2();
    invMassVsRun_pho_EE_out_Low->Sumw2();
    invMassVsRun_pho_EB_in_High->Sumw2();
    invMassVsRun_pho_EB_out_High->Sumw2();
    invMassVsRun_pho_EE_in_High->Sumw2();
    invMassVsRun_pho_EE_out_High->Sumw2();
    invMass->Sumw2();
    invMass_Et0->Sumw2();
    invMassUp_Et0->Sumw2();
    invMassDown_Et0->Sumw2();
    invMass_Et1->Sumw2();
    invMassUp_Et1->Sumw2();
    invMassDown_Et1->Sumw2();
    invMass_Et2->Sumw2();
    invMassUp_Et2->Sumw2();
    invMassDown_Et2->Sumw2();
    invMass_Et3->Sumw2();
    invMassUp_Et3->Sumw2();
    invMassDown_Et3->Sumw2();
    invMass_Et4->Sumw2();
    invMassUp_Et4->Sumw2();
    invMassDown_Et4->Sumw2();
    invMass_Et5->Sumw2();
    invMassUp_Et5->Sumw2();
    invMassDown_Et5->Sumw2();
    invMass_Et0_EBin->Sumw2();
    invMass_Et0_EBout->Sumw2();
    invMass_Et0_EEin->Sumw2();
    invMass_Et0_EEout->Sumw2();
    invMass_Et1_EBin->Sumw2();
    invMass_Et1_EBout->Sumw2();
    invMass_Et1_EEin->Sumw2();
    invMass_Et1_EEout->Sumw2();
    invMass_Et2_EBin->Sumw2();
    invMass_Et2_EBout->Sumw2();
    invMass_Et2_EEin->Sumw2();
    invMass_Et2_EEout->Sumw2();
    invMass_Et3_EBin->Sumw2();
    invMass_Et3_EBout->Sumw2();
    invMass_Et3_EEin->Sumw2();
    invMass_Et3_EEout->Sumw2();
    invMass_Et4_EBin->Sumw2();
    invMass_Et4_EBout->Sumw2();
    invMass_Et4_EEin->Sumw2();
    invMass_Et4_EEout->Sumw2();
    r9Dist->Sumw2();
    r9Dist_EB->Sumw2();
    r9Dist_EE->Sumw2();
    invMass_R90->Sumw2();
    invMass_R91->Sumw2();
    invMass_R92->Sumw2();
    invMass_R93->Sumw2();
    invMass_R94->Sumw2();
    invMassUp_R90->Sumw2();
    invMassUp_R91->Sumw2();
    invMassUp_R92->Sumw2();
    invMassUp_R93->Sumw2();
    invMassUp_R94->Sumw2();
    invMassDown_R90->Sumw2();
    invMassDown_R91->Sumw2();
    invMassDown_R92->Sumw2();
    invMassDown_R93->Sumw2();
    invMassDown_R94->Sumw2();
    invMass_ele_NotEbEb_high->Sumw2();
    invMassUp_ele_NotEbEb_high->Sumw2();
    invMassDown_ele_NotEbEb_high->Sumw2();
    invMass_ele_EB_in_Low->Sumw2();
    invMass_ele_EB_out_Low->Sumw2();
    invMass_ele_EE_in_Low->Sumw2();
    invMass_ele_EE_out_Low->Sumw2();
    invMass_ele_EB_in_High->Sumw2();
    invMass_ele_EB_out_High->Sumw2();
    invMass_ele_EE_in_High->Sumw2();
    invMass_ele_EE_out_High->Sumw2();
    invMass_pho_EB_in_Low->Sumw2();
    invMass_pho_EB_out_Low->Sumw2();
    invMass_pho_EE_in_Low->Sumw2();
    invMass_pho_EE_out_Low->Sumw2();
    invMass_pho_EB_in_High->Sumw2();
    invMass_pho_EB_out_High->Sumw2();
    invMass_pho_EE_in_High->Sumw2();
    invMass_pho_EE_out_High->Sumw2();
    invMassUp_ele_EB_in_Low->Sumw2();
    invMassUp_ele_EB_out_Low->Sumw2();
    invMassUp_ele_EE_in_Low->Sumw2();
    invMassUp_ele_EE_out_Low->Sumw2();
    invMassUp_ele_EB_in_High->Sumw2();
    invMassUp_ele_EB_out_High->Sumw2();
    invMassUp_ele_EE_in_High->Sumw2();
    invMassUp_ele_EE_out_High->Sumw2();
    invMassUp_pho_EB_in_Low->Sumw2();
    invMassUp_pho_EB_out_Low->Sumw2();
    invMassUp_pho_EE_in_Low->Sumw2();
    invMassUp_pho_EE_out_Low->Sumw2();
    invMassUp_pho_EB_in_High->Sumw2();
    invMassUp_pho_EB_out_High->Sumw2();
    invMassUp_pho_EE_in_High->Sumw2();
    invMassUp_pho_EE_out_High->Sumw2();
    invMassDown_ele_EB_in_Low->Sumw2();
    invMassDown_ele_EB_out_Low->Sumw2();
    invMassDown_ele_EE_in_Low->Sumw2();
    invMassDown_ele_EE_out_Low->Sumw2();
    invMassDown_ele_EB_in_High->Sumw2();
    invMassDown_ele_EB_out_High->Sumw2();
    invMassDown_ele_EE_in_High     ->Sumw2();
    invMassDown_ele_EE_out_High    ->Sumw2();
    invMassDown_pho_EB_in_Low      ->Sumw2();
    invMassDown_pho_EB_out_Low     ->Sumw2();
    invMassDown_pho_EE_in_Low      ->Sumw2();
    invMassDown_pho_EE_out_Low     ->Sumw2();
    invMassDown_pho_EB_in_High     ->Sumw2();
    invMassDown_pho_EB_out_High    ->Sumw2();
    invMassDown_pho_EE_in_High     ->Sumw2();
    invMassDown_pho_EE_out_High    ->Sumw2();
    leading_pt->Sumw2();
    subleading_pt->Sumw2();
    pt_55_65->Sumw2();
    pt_55_65_up->Sumw2();
    pt_55_65_down->Sumw2();


    float R9Ele [3];
    float eta [3];
    float phi [3];
    float energy_ele [3];
    float energy_pho [3];
    float etaSCEle [3];
    float invMass_ele [3];
    float invMass_pho [3];
    float invMass_raw [3]; 
    float invMass_5x5 [3];
    float mcWeight;
    char NPU;
    char gain [3];
    float rho;
    unsigned int runNum;
    unsigned int eleID [3];
    
    ifstream in;
    in.open( fileName.c_str() );
    string thisFile;
    int count = 0;
    //open the TFile for R9 corrections
    TFile * corrR9_file = new TFile("/afs/cern.ch/work/n/nschroed/ECALELF_DATA/transformation_Legacy2016_Final.root", "READ");
    TGraph* transformR9_EB = (TGraph*) corrR9_file->Get("transffull5x5R9EB");
    TGraph* transformR9_EE = (TGraph*) corrR9_file->Get("transffull5x5R9EE");
    corrR9_file->Close();
    
    //Sumw2 the shit out of everything

    //loop over ntuples
    if(!in.is_open()){
        cout << "[ERROR] file: " << fileName << " did not open" << endl;
        return;
    }

    std::cout << "[INFO] begin processing files ... " << std::endl;
    while( in >> thisFile ){
        count++;
        cout << "[INFO] processing file: " << count << endl << thisFile << endl;
        TFile * thisRootFile = new TFile(thisFile.c_str(), "READ");
        TTree * thisTree = (TTree*)thisRootFile->Get("selected");
        thisTree->SetBranchAddress("R9Ele", R9Ele);
        thisTree->SetBranchAddress("etaSCEle", etaSCEle);
        thisTree->SetBranchAddress("invMass_ECAL_ele", invMass_ele);
        thisTree->SetBranchAddress("invMass_ECAL_pho", invMass_pho);
        thisTree->SetBranchAddress("invMass_rawSC", invMass_raw); 
        thisTree->SetBranchAddress("invMass_5x5SC", invMass_5x5);
        thisTree->SetBranchAddress("etaEle", eta);
        thisTree->SetBranchAddress("phiEle", phi);
        thisTree->SetBranchAddress("energy_ECAL_ele", energy_ele);
        thisTree->SetBranchAddress("energy_ECAL_pho", energy_pho);
        thisTree->SetBranchAddress("mcGenWeight", &mcWeight);
        thisTree->SetBranchAddress("nPU", &NPU);
        thisTree->SetBranchAddress("rho", &rho);
        thisTree->SetBranchAddress("runNumber", &runNum);
        thisTree->SetBranchAddress("eleID", eleID);
        thisTree->SetBranchAddress("gainSeedSC", gain);
        for(Long64_t i = 0; i < thisTree->GetEntries(); i++){
            //get this ntuple entry
            thisTree->GetEntry(i);

#ifdef DEBUG 
            cout << "invMass_ECAL_ele: " << invMass_ele[0] << endl;
            cout << "etaEle: " << eta[0] << " " << eta[1] << endl;
            cout << "R9Ele: " << R9Ele[0] << " " << R9Ele[1] << endl;
            cout << "mcGenWeight: " << mcWeight << endl;
            cout << "npu: " << NPU << endl;
            cout << "rho: " << rho << endl;
#endif

            //construct 4-vectors
            TLorentzVector  e_lead;
            TLorentzVector  e_sub;
            TLorentzVector  z;

            ////////////////////
            // r9 corrections //
            ////////////////////

            double thisR91 = R9Ele[0];
            double thisR92 = R9Ele[1];
            if( type.find("mc") != string::npos && _flag_transformR9){
                if( fabs( eta[0] ) < 1.4442 ) thisR91 = transformR9_EB->Eval(R9Ele[0]);
                else if( fabs( eta[0] ) > 1.566 && fabs( eta[0] ) && R9Ele[0] > 0.8) thisR91 = transformR9_EE->Eval(R9Ele[0]);
                if( fabs( eta[1] ) < 1.4442 ) thisR92 = transformR9_EB->Eval(R9Ele[1]);
                else if( fabs( eta[1] ) > 1.566 && fabs( eta[1] ) && R9Ele[1] > 0.8) thisR92 = transformR9_EE->Eval(R9Ele[1]);
            }

            ///////////////////////
            // scale corrections //
            ///////////////////////

            float correction = 1.;
            float correction_lead = 1.;
            float correction_sub = 1.;
            float correction_up = 1.;
            float correction_down = 1.;

            e_lead.SetPtEtaPhiE( energy_ele[0]*correction_lead/TMath::CosH(eta[0]), eta[0], phi[0], energy_ele[0]*correction_lead);
            e_sub.SetPtEtaPhiE( energy_ele[1]*correction_sub/TMath::CosH(eta[1]), eta[1], phi[1], energy_ele[1]*correction_sub);

            if( type.compare("data") == 0){
                int gain = 12;
                correction_lead = scalerSmearer.scaleCorr(runNum , e_lead.Et(), eta[0], thisR91, gain); //gain is gain
                correction_sub = scalerSmearer.scaleCorr(runNum , e_sub.Et(), eta[1], thisR92, gain);
                std::bitset<3> b1(EnergyScaleCorrection::kErrSystGain);
                auto correction_lead_unc = scalerSmearer.scaleCorrUncert(runNum, e_lead.Et(), eta[0], thisR91, gain, b1);
                auto correction_sub_unc = scalerSmearer.scaleCorrUncert(runNum, e_sub.Et(), eta[1], thisR92, gain, b1);
                correction = sqrt(correction_lead*correction_sub);
                correction_up = sqrt((correction_lead + correction_lead_unc)*(correction_sub + correction_sub_unc));
                correction_down = sqrt((correction_lead - correction_lead_unc)*(correction_sub - correction_sub_unc));
            }

            //////////////////////////
            // smearing corrections //
            //////////////////////////

            if( type.compare("mc") == 0){
                double sigma1 = scalerSmearer.smearingSigma( runNum, e_lead.Et(), eta[0], thisR91, 12, 0., 0.);
                double sigma2 = scalerSmearer.smearingSigma( runNum, e_sub.Et(), eta[1], thisR92, 12, 0., 0.);
                double sigma1_up = scalerSmearer.smearingSigma( runNum, e_lead.Et(), eta[0], thisR91, 12, 1, 0.);
                double sigma2_up = scalerSmearer.smearingSigma( runNum, e_sub.Et(), eta[1], thisR92, 12, 1, 0.);
                double sigma1_down = scalerSmearer.smearingSigma( runNum, e_lead.Et(), eta[0], thisR91, 12, -1, 0.);
                double sigma2_down = scalerSmearer.smearingSigma( runNum, e_sub.Et(), eta[1], thisR92, 12, -1, 0.);
                correction_lead = gRandom->Gaus(1, sigma1);
                correction_sub = gRandom->Gaus(1, sigma2);
                auto correction_lead_up = gRandom->Gaus(1, sigma1_up);
                auto correction_sub_up = gRandom->Gaus(1, sigma2_up);
                auto correction_lead_down = gRandom->Gaus(1, sigma1_down);
                auto correction_sub_down = gRandom->Gaus(1, sigma2_down);
                correction = sqrt(correction_lead*correction_sub);
                correction_up = sqrt(correction_lead_up*correction_sub_up);
                correction_down = sqrt(correction_lead_down*correction_sub_down);
            }

#ifdef DEBUG
            cout << correction << " " << correction_lead << " " << correction_sub << " " << correction_up << " " << correction_down << endl;
#endif

            //some workarounds for "corner" cases
            //   data doesn't have pileup so just make the NPU = 10 and read 1 from the hist
            //   mc isn't generated for specific runs so just assign it to the first bin
            double thisNPU;
            if(type.compare("data") == 0) NPU = 10;


            ////////////////////////
            // recreate 4-vectors //
            ////////////////////////

            z = e_lead + e_sub;
            e_lead.SetPtEtaPhiE( energy_ele[0]*correction_lead/TMath::CosH(eta[0]), eta[0], phi[0], energy_ele[0]*correction_lead);
            e_sub.SetPtEtaPhiE( energy_ele[1]*correction_sub/TMath::CosH(eta[1]), eta[1], phi[1], energy_ele[1]*correction_sub);

            /////////////////////////////
            // create cuts using bools //
            /////////////////////////////

            bool leadPt = e_lead.Pt() > 32;
            bool subPt = e_sub.Pt() > 20;
            bool exclusionRegion = (fabs(eta[0]) < 1.4442 || fabs(eta[0]) > 1.566) && (fabs(eta[1]) < 1.4442 || fabs(eta[1]) > 1.566) && fabs(eta[0]) < 2.5 && fabs(eta[1]) < 2.5;
            bool EB_IN = fabs(eta[0]) < 1 && fabs(eta[1]) < 1;
            bool EB_IN1 = fabs(eta[0]) < 1; bool EB_IN2 = fabs(eta[1]) < 1;
            bool EB_OUT = fabs(eta[0]) > 1 && fabs(eta[0]) < 1.4442 && fabs(eta[1]) > 1 && fabs(eta[1]) < 1.4442;
            bool EB_OUT1 = fabs(eta[0]) > 1 && fabs(eta[0]) < 1.4442; bool EB_OUT2 = fabs(eta[1]) > 1 && fabs(eta[1]) < 1.4442;
            bool EB1 = fabs(eta[0]) < 1.479; bool EB2 = fabs(eta[1]) < 1.479;
            bool EE1 = fabs(eta[0]) > 1.566; bool EE2 = fabs(eta[1]) > 1.566;
            bool EE_IN = fabs(eta[0]) > 1.566 && fabs(eta[0]) < 2.0 && fabs(eta[1]) > 1.566 && fabs(eta[1]) < 2.0;
            bool EE_IN1 = fabs(eta[0]) > 1.566 && fabs(eta[0]) < 2.0; bool EE_IN2 = fabs(eta[1]) && fabs(eta[1]) < 2.0;
            bool EE_OUT = fabs(eta[0]) > 2.0 && fabs(eta[1]) > 2.0;
            bool EE_OUT1 = fabs(eta[0]) > 2.0; bool EE_OUT2 = fabs(eta[1]) > 2.0;
            bool lowR9 = thisR91 < 0.94 && thisR92 < 0.94;
            bool lowR91 = thisR91 < 0.94; bool lowR92 = thisR92 < 0.94;
            bool highR9 = thisR91 > 0.94 && thisR92 > 0.94;
            bool highR91 = thisR91 > 0.94; bool highR92 = thisR92 > 0.94;
            bool mvaID = true;

            if(_flag_tightID) mvaID = (eleID[0] & 0x0010) && (eleID[1] & 0x0010);
            if(_flag_mediumID) mvaID = (eleID[0] & 0x0008) && (eleID[1] & 0x0008);

            /////////////////////////
            // apply cut on gain12 //
            /////////////////////////

            char kGain12 = 0;
            bool gain12_lead = (gain[0] == kGain12);
            bool gain12_sub = (gain[1] == kGain12);
            bool gainCut = gain12_lead && gain12_sub;

            ////////////////////////////////
            // set mcWeight to 1 for data //
            ////////////////////////////////

            if(fileName.find("data") != string::npos) mcWeight = 1.;

            /////////////////////////
            // fill the histograms //
            /////////////////////////

            if( exclusionRegion && leadPt && subPt && mvaID && gainCut ){

                ///////////////////////////////////////////////////////
                // set a mass window of 80 to 100 for invariant mass //
                ///////////////////////////////////////////////////////

                if( invMass_ele[0]*correction >= 80 && invMass_ele[0]*correction <= 100){
                    leading_pt->Fill(e_lead.Pt(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    subleading_pt->Fill(e_sub.Pt(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if( e_lead.Pt() > 55 && e_lead.Pt() < 65 ){
                        pt_55_65->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        pt_55_65_up->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        pt_55_65_down->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }

                    invMassVsNPU->Fill(thisNPU, invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));

                    if(lowR9){
                        mee_npu_rawSC_low->Fill(thisNPU, invMass_raw[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        mee_npu_ele_low->Fill(thisNPU, invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if(highR9){
                        mee_npu_rawSC_high->Fill(thisNPU, invMass_raw[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        mee_npu_ele_high->Fill(thisNPU, invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }

                    if( e_lead.Et() > etBins[0] && e_lead.Et() < etBins[1] && e_sub.Et() > etBins[0] && e_sub.Et()< etBins[1]){
                        invMass_Et0->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_Et0->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_Et0->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( e_lead.Et() > etBins[1] && e_lead.Et() < etBins[2] && e_sub.Et() > etBins[1] && e_sub.Et()< etBins[2]){
                        invMass_Et1->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_Et1->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_Et1->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( e_lead.Et() > etBins[2] && e_lead.Et() < etBins[3] && e_sub.Et() > etBins[2] && e_sub.Et()< etBins[3]){
                        invMass_Et2->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_Et2->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_Et2->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( e_lead.Et() > etBins[3] && e_lead.Et() < etBins[4] && e_sub.Et() > etBins[3] && e_sub.Et()< etBins[4]){
                        invMass_Et3->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_Et3->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_Et3->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( e_lead.Et() > etBins[4] && e_lead.Et() < etBins[5] && e_sub.Et() > etBins[4] && e_sub.Et()< etBins[5]){
                        invMass_Et4->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_Et4->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_Et4->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( e_lead.Et() > etBins[5] && e_lead.Et() < etBins[6] && e_sub.Et() > etBins[5] && e_sub.Et()< etBins[6]){
                        invMass_Et5->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_Et5->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_Et5->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( EB_IN ){
                        if( e_lead.Et() > etBins2[0] && e_lead.Et() < etBins2[1] && e_sub.Et() > etBins2[0] && e_sub.Et()< etBins2[1]){
                            invMass_Et0_EBin->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[1] && e_lead.Et() < etBins2[2] && e_sub.Et() > etBins2[1] && e_sub.Et()< etBins2[2]){
                            invMass_Et1_EBin->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[2] && e_lead.Et() < etBins2[3] && e_sub.Et() > etBins2[2] && e_sub.Et()< etBins2[3]){
                            invMass_Et2_EBin->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[3] && e_lead.Et() < etBins2[4] && e_sub.Et() > etBins2[3] && e_sub.Et()< etBins2[4]){
                            invMass_Et3_EBin->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[4] && e_lead.Et() < etBins2[5] && e_sub.Et() > etBins2[4] && e_sub.Et()< etBins2[5]){
                            invMass_Et4_EBin->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                    }
                    if( EB_OUT ){
                        if( e_lead.Et() > etBins2[0] && e_lead.Et() < etBins2[1] && e_sub.Et() > etBins2[0] && e_sub.Et()< etBins2[1]){
                            invMass_Et0_EBout->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[1] && e_lead.Et() < etBins2[2] && e_sub.Et() > etBins2[1] && e_sub.Et()< etBins2[2]){
                            invMass_Et1_EBout->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[2] && e_lead.Et() < etBins2[3] && e_sub.Et() > etBins2[2] && e_sub.Et()< etBins2[3]){
                            invMass_Et2_EBout->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[3] && e_lead.Et() < etBins2[4] && e_sub.Et() > etBins2[3] && e_sub.Et()< etBins2[4]){
                            invMass_Et3_EBout->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[4] && e_lead.Et() < etBins2[5] && e_sub.Et() > etBins2[4] && e_sub.Et()< etBins2[5]){
                            invMass_Et4_EBout->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                    }
                    if( EE_IN ){
                        if( e_lead.Et() > etBins2[0] && e_lead.Et() < etBins2[1] && e_sub.Et() > etBins2[0] && e_sub.Et()< etBins2[1]){
                            invMass_Et0_EEin->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[1] && e_lead.Et() < etBins2[2] && e_sub.Et() > etBins2[1] && e_sub.Et()< etBins2[2]){
                            invMass_Et1_EEin->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[2] && e_lead.Et() < etBins2[3] && e_sub.Et() > etBins2[2] && e_sub.Et()< etBins2[3]){
                            invMass_Et2_EEin->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[3] && e_lead.Et() < etBins2[4] && e_sub.Et() > etBins2[3] && e_sub.Et()< etBins2[4]){
                            invMass_Et3_EEin->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[4] && e_lead.Et() < etBins2[5] && e_sub.Et() > etBins2[4] && e_sub.Et()< etBins2[5]){
                            invMass_Et4_EEin->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                    }
                    if( EE_OUT ){
                        if( e_lead.Et() > etBins2[0] && e_lead.Et() < etBins2[1] && e_sub.Et() > etBins2[0] && e_sub.Et()< etBins2[1]){
                            invMass_Et0_EEout->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[1] && e_lead.Et() < etBins2[2] && e_sub.Et() > etBins2[1] && e_sub.Et()< etBins2[2]){
                            invMass_Et1_EEout->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[2] && e_lead.Et() < etBins2[3] && e_sub.Et() > etBins2[2] && e_sub.Et()< etBins2[3]){
                            invMass_Et2_EEout->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[3] && e_lead.Et() < etBins2[4] && e_sub.Et() > etBins2[3] && e_sub.Et()< etBins2[4]){
                            invMass_Et3_EEout->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( e_lead.Et() > etBins2[4] && e_lead.Et() < etBins2[5] && e_sub.Et() > etBins2[4] && e_sub.Et()< etBins2[5]){
                            invMass_Et4_EEout->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                    }

                    r9Dist->Fill(thisR91);
                    r9Dist->Fill(thisR92);
                    if(EB1) r9Dist_EB->Fill(thisR91);
                    if(EB2) r9Dist_EB->Fill(thisR92);
                    if(EE1) r9Dist_EE->Fill(thisR91);
                    if(EE2) r9Dist_EE->Fill(thisR92);
                    invMass->Fill( invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if( thisR91 > r9Bins[0] && thisR91 < r9Bins[1] && thisR92 > r9Bins[0] && thisR92 < r9Bins[1]){
                        invMass_R90->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_R90->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_R90->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( thisR91 > r9Bins[1] && thisR91 < r9Bins[2] && thisR92 > r9Bins[1] && thisR92 < r9Bins[2]){
                        invMass_R91->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_R91->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_R91->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( thisR91 > r9Bins[2] && thisR91 < r9Bins[3] && thisR92 > r9Bins[2] && thisR92 < r9Bins[3]){
                        invMass_R92->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_R92->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_R92->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( thisR91 > r9Bins[3] && thisR91 < r9Bins[4] && thisR92 > r9Bins[3] && thisR92 < r9Bins[4]){
                        invMass_R93->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_R93->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_R93->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( thisR91 > r9Bins[4] && thisR91 < r9Bins[5] && thisR92 > r9Bins[4] && thisR92 < r9Bins[5]){
                        invMass_R94->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_R94->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_R94->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if(thisR91 > r9Bins[5] && thisR91 < r9Bins[6] && thisR92 > r9Bins[5] && thisR92 < r9Bins[6]){
                        invMass_R95->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_R95->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_R95->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }

                    if( highR9 && !(EB1 && EB2)){
                        invMass_ele_NotEbEb_high->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_NotEbEb_high->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_NotEbEb_high->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }

                    if( EB_IN && lowR9){
                        invMassVsRun_ele_EB_in_Low->Fill(runNum, invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassVsRun_pho_EB_in_Low->Fill(runNum, invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_ele_EB_in_Low->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_pho_EB_in_Low->Fill(invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EB_in_Low->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_pho_EB_in_Low->Fill(invMass_pho[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EB_in_Low->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_pho_EB_in_Low->Fill(invMass_pho[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( EB_OUT && lowR9){
                        invMassVsRun_ele_EB_out_Low->Fill(runNum, invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassVsRun_pho_EB_out_Low->Fill(runNum, invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_ele_EB_out_Low->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_pho_EB_out_Low->Fill(invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EB_out_Low->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_pho_EB_out_Low->Fill(invMass_pho[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EB_out_Low->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_pho_EB_out_Low->Fill(invMass_pho[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    } 
                    if( EE_IN && lowR9){
                        invMassVsRun_ele_EE_in_Low->Fill(runNum, invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassVsRun_pho_EE_in_Low->Fill(runNum, invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_ele_EE_in_Low->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_pho_EE_in_Low->Fill(invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EE_in_Low->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_pho_EE_in_Low->Fill(invMass_pho[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EE_in_Low->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_pho_EE_in_Low->Fill(invMass_pho[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( EE_OUT && lowR9){
                        invMassVsRun_ele_EE_out_Low->Fill(runNum, invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassVsRun_pho_EE_out_Low->Fill(runNum, invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_ele_EE_out_Low->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_pho_EE_out_Low->Fill(invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EE_out_Low->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_pho_EE_out_Low->Fill(invMass_pho[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EE_out_Low->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_pho_EE_out_Low->Fill(invMass_pho[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    } 
                    if( EB_IN && highR9){
                        invMassVsRun_ele_EB_in_High->Fill(runNum, invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassVsRun_pho_EB_in_High->Fill(runNum, invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_ele_EB_in_High->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_pho_EB_in_High->Fill(invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EB_in_High->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_pho_EB_in_High->Fill(invMass_pho[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EB_in_High->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_pho_EB_in_High->Fill(invMass_pho[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( EB_OUT && highR9){
                        invMassVsRun_ele_EB_out_High->Fill(runNum, invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassVsRun_pho_EB_out_High->Fill(runNum, invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_ele_EB_out_High->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_pho_EB_out_High->Fill(invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EB_out_High->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_pho_EB_out_High->Fill(invMass_pho[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EB_out_High->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_pho_EB_out_High->Fill(invMass_pho[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    } 
                    if( EE_IN && highR9){
                        invMassVsRun_ele_EE_in_High->Fill(runNum, invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassVsRun_pho_EE_in_High->Fill(runNum, invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_ele_EE_in_High->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_pho_EE_in_High->Fill(invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EE_in_High->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_pho_EE_in_High->Fill(invMass_pho[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EE_in_High->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_pho_EE_in_High->Fill(invMass_pho[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( EE_OUT && highR9){
                        invMassVsRun_ele_EE_out_High->Fill(runNum, invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassVsRun_pho_EE_out_High->Fill(runNum, invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_ele_EE_out_High->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMass_pho_EE_out_High->Fill(invMass_pho[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EE_out_High->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_pho_EE_out_High->Fill(invMass_pho[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EE_out_High->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_pho_EE_out_High->Fill(invMass_pho[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    } 
                }
            }
        }//end for entries in the tree
        thisRootFile->Close();
        delete thisRootFile;
    }//end while files in
    delete transformR9_EB;
    delete transformR9_EE;

    in.close();

    string prefix = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/rootFiles/scaleSmearHists_";
    string fileOut = "";

    fileOut = prefix+output+"_"+type+".root";
    cout << info << "root file written to: " << fileOut << endl;
    system(std::string("echo "+fileOut+" >> "+temp_output+".dat").c_str());
    TFile * rootOut = new TFile(fileOut.c_str(), "RECREATE");

    invMassVsNPU->Write();
    mee_npu_rawSC_low->Write();
    mee_npu_rawSC_high->Write();
    mee_npu_ele_low->Write();
    mee_npu_ele_high->Write();
    invMassVsRun_ele_EB_in_Low->Write();
    invMassVsRun_ele_EB_out_Low->Write();
    invMassVsRun_ele_EE_in_Low->Write();
    invMassVsRun_ele_EE_out_Low->Write();
    invMassVsRun_ele_EB_in_High->Write();
    invMassVsRun_ele_EB_out_High->Write();
    invMassVsRun_ele_EE_in_High->Write();
    invMassVsRun_ele_EE_out_High->Write();
    invMassVsRun_pho_EB_in_Low->Write();
    invMassVsRun_pho_EB_out_Low->Write();
    invMassVsRun_pho_EE_in_Low->Write();
    invMassVsRun_pho_EE_out_Low->Write();
    invMassVsRun_pho_EB_in_High->Write();
    invMassVsRun_pho_EB_out_High->Write();
    invMassVsRun_pho_EE_in_High->Write();
    invMassVsRun_pho_EE_out_High->Write();
    invMass->Write();
    invMass_Et0->Write();
    invMassUp_Et0->Write();
    invMassDown_Et0->Write();
    invMass_Et1->Write();
    invMassUp_Et1->Write();
    invMassDown_Et1->Write();
    invMass_Et2->Write();
    invMassUp_Et2->Write();
    invMassDown_Et2->Write();
    invMass_Et3->Write();
    invMassUp_Et3->Write();
    invMassDown_Et3->Write();
    invMass_Et4->Write();
    invMassUp_Et4->Write();
    invMassDown_Et4->Write();
    invMass_Et5->Write();
    invMassUp_Et5->Write();
    invMassDown_Et5->Write();
    invMass_Et0_EBin->Write();
    invMass_Et0_EBout->Write();
    invMass_Et0_EEin->Write();
    invMass_Et0_EEout->Write();
    invMass_Et1_EBin->Write();
    invMass_Et1_EBout->Write();
    invMass_Et1_EEin->Write();
    invMass_Et1_EEout->Write();
    invMass_Et2_EBin->Write();
    invMass_Et2_EBout->Write();
    invMass_Et2_EEin->Write();
    invMass_Et2_EEout->Write();
    invMass_Et3_EBin->Write();
    invMass_Et3_EBout->Write();
    invMass_Et3_EEin->Write();
    invMass_Et3_EEout->Write();
    invMass_Et4_EBin->Write();
    invMass_Et4_EBout->Write();
    invMass_Et4_EEin->Write();
    invMass_Et4_EEout->Write();
    r9Dist->Write();
    r9Dist_EB->Write();
    r9Dist_EE->Write();
    invMass_R90->Write();
    invMass_R91->Write();
    invMass_R92->Write();
    invMass_R93->Write();
    invMass_R94->Write();
    invMass_R95->Write();
    invMassUp_R90->Write();
    invMassUp_R91->Write();
    invMassUp_R92->Write();
    invMassUp_R93->Write();
    invMassUp_R94->Write();
    invMassUp_R95->Write();
    invMassDown_R90->Write();
    invMassDown_R91->Write();
    invMassDown_R92->Write();
    invMassDown_R93->Write();
    invMassDown_R94->Write();
    invMassDown_R95->Write();
    invMass_ele_EB_in_Low->Write();
    invMass_ele_EB_out_Low->Write();
    invMass_ele_EE_in_Low->Write();
    invMass_ele_EE_out_Low->Write();
    invMass_ele_EB_in_High->Write();
    invMass_ele_EB_out_High->Write();
    invMass_ele_EE_in_High->Write();
    invMass_ele_EE_out_High->Write();
    invMass_ele_NotEbEb_high->Write();
    invMassUp_ele_NotEbEb_high->Write();
    invMassDown_ele_NotEbEb_high->Write();
    invMass_pho_EB_in_Low->Write();
    invMass_pho_EB_out_Low->Write();
    invMass_pho_EE_in_Low->Write();
    invMass_pho_EE_out_Low->Write();
    invMass_pho_EB_in_High->Write();
    invMass_pho_EB_out_High->Write();
    invMass_pho_EE_in_High->Write();
    invMass_pho_EE_out_High->Write();
    invMassUp_ele_EB_in_Low->Write();
    invMassUp_ele_EB_out_Low->Write();
    invMassUp_ele_EE_in_Low->Write();
    invMassUp_ele_EE_out_Low->Write();
    invMassUp_ele_EB_in_High->Write();
    invMassUp_ele_EB_out_High->Write();
    invMassUp_ele_EE_in_High->Write();
    invMassUp_ele_EE_out_High->Write();
    invMassUp_pho_EB_in_Low->Write();
    invMassUp_pho_EB_out_Low->Write();
    invMassUp_pho_EE_in_Low->Write();
    invMassUp_pho_EE_out_Low->Write();
    invMassUp_pho_EB_in_High->Write();
    invMassUp_pho_EB_out_High->Write();
    invMassUp_pho_EE_in_High->Write();
    invMassUp_pho_EE_out_High->Write();
    invMassDown_ele_EB_in_Low->Write();
    invMassDown_ele_EB_out_Low->Write();
    invMassDown_ele_EE_in_Low->Write();
    invMassDown_ele_EE_out_Low->Write();
    invMassDown_ele_EB_in_High->Write();
    invMassDown_ele_EB_out_High->Write();
    invMassDown_ele_EE_in_High->Write();
    invMassDown_ele_EE_out_High->Write();
    invMassDown_pho_EB_in_Low->Write();
    invMassDown_pho_EB_out_Low->Write();
    invMassDown_pho_EE_in_Low->Write();
    invMassDown_pho_EE_out_Low->Write();
    invMassDown_pho_EB_in_High->Write();
    invMassDown_pho_EB_out_High->Write();
    invMassDown_pho_EE_in_High->Write();
    invMassDown_pho_EE_out_High->Write();
    leading_pt->Write();
    subleading_pt->Write();
    pt_55_65->Write();
    pt_55_65_up->Write();
    pt_55_65_down->Write();

    rootOut->Close();

    delete invMassVsNPU;
    delete mee_npu_rawSC_low;
    delete mee_npu_rawSC_high;
    delete mee_npu_ele_low;
    delete mee_npu_ele_high;
    delete invMassVsRun_ele_EB_in_Low;
    delete invMassVsRun_ele_EB_out_Low;
    delete invMassVsRun_ele_EE_in_Low;
    delete invMassVsRun_ele_EE_out_Low;
    delete invMassVsRun_ele_EB_in_High;
    delete invMassVsRun_ele_EB_out_High;
    delete invMassVsRun_ele_EE_in_High;
    delete invMassVsRun_ele_EE_out_High;
    delete invMassVsRun_pho_EB_in_Low;
    delete invMassVsRun_pho_EB_out_Low;
    delete invMassVsRun_pho_EE_in_Low;
    delete invMassVsRun_pho_EE_out_Low;
    delete invMassVsRun_pho_EB_in_High;
    delete invMassVsRun_pho_EB_out_High;
    delete invMassVsRun_pho_EE_in_High;
    delete invMassVsRun_pho_EE_out_High;
    delete invMass;
    delete invMass_Et0;
    delete invMassUp_Et0;
    delete invMassDown_Et0;
    delete invMass_Et1;
    delete invMassUp_Et1;
    delete invMassDown_Et1;
    delete invMass_Et2;
    delete invMassUp_Et2;
    delete invMassDown_Et2;
    delete invMass_Et3;
    delete invMassUp_Et3;
    delete invMassDown_Et3;
    delete invMass_Et4;
    delete invMassUp_Et4;
    delete invMassDown_Et4;
    delete invMass_Et5;
    delete invMassUp_Et5;
    delete invMassDown_Et5;
    delete invMass_Et0_EBin;
    delete invMass_Et0_EBout;
    delete invMass_Et0_EEin;
    delete invMass_Et0_EEout;
    delete invMass_Et1_EBin;
    delete invMass_Et1_EBout;
    delete invMass_Et1_EEin;
    delete invMass_Et1_EEout;
    delete invMass_Et2_EBin;
    delete invMass_Et2_EBout;
    delete invMass_Et2_EEin;
    delete invMass_Et2_EEout;
    delete invMass_Et3_EBin;
    delete invMass_Et3_EBout;
    delete invMass_Et3_EEin;
    delete invMass_Et3_EEout;
    delete invMass_Et4_EBin;
    delete invMass_Et4_EBout;
    delete invMass_Et4_EEin;
    delete invMass_Et4_EEout;
    delete r9Dist;
    delete r9Dist_EB;
    delete r9Dist_EE;
    delete invMass_R90;
    delete invMass_R91;
    delete invMass_R92;
    delete invMass_R93;
    delete invMass_R94;
    delete invMass_R95;
    delete invMassUp_R90;
    delete invMassUp_R91;
    delete invMassUp_R92;
    delete invMassUp_R93;
    delete invMassUp_R94;
    delete invMassUp_R95;
    delete invMassDown_R90;
    delete invMassDown_R91;
    delete invMassDown_R92;
    delete invMassDown_R93;
    delete invMassDown_R94;
    delete invMassDown_R95;
    delete invMass_ele_EB_in_Low;
    delete invMass_ele_EB_out_Low;
    delete invMass_ele_EE_in_Low;
    delete invMass_ele_EE_out_Low;
    delete invMass_ele_EB_in_High;
    delete invMass_ele_EB_out_High;
    delete invMass_ele_EE_in_High;
    delete invMass_ele_EE_out_High;
    delete invMass_pho_EB_in_Low;
    delete invMass_pho_EB_out_Low;
    delete invMass_pho_EE_in_Low;
    delete invMass_pho_EE_out_Low;
    delete invMass_pho_EB_in_High;
    delete invMass_pho_EB_out_High;
    delete invMass_pho_EE_in_High;
    delete invMass_pho_EE_out_High;
    delete invMassUp_ele_EB_in_Low;
    delete invMassUp_ele_EB_out_Low;
    delete invMassUp_ele_EE_in_Low;
    delete invMassUp_ele_EE_out_Low;
    delete invMassUp_ele_EB_in_High;
    delete invMassUp_ele_EB_out_High;
    delete invMassUp_ele_EE_in_High;
    delete invMassUp_ele_EE_out_High;
    delete invMassUp_pho_EB_in_Low;
    delete invMassUp_pho_EB_out_Low;
    delete invMassUp_pho_EE_in_Low;
    delete invMassUp_pho_EE_out_Low;
    delete invMassUp_pho_EB_in_High;
    delete invMassUp_pho_EB_out_High;
    delete invMassUp_pho_EE_in_High;
    delete invMassUp_pho_EE_out_High;
    delete invMassDown_ele_EB_in_Low;
    delete invMassDown_ele_EB_out_Low;
    delete invMassDown_ele_EE_in_Low;
    delete invMassDown_ele_EE_out_Low;
    delete invMassDown_ele_EB_in_High;
    delete invMassDown_ele_EB_out_High;
    delete invMassDown_ele_EE_in_High;
    delete invMassDown_ele_EE_out_High;
    delete invMassDown_pho_EB_in_Low;
    delete invMassDown_pho_EB_out_Low;
    delete invMassDown_pho_EE_in_Low;
    delete invMassDown_pho_EE_out_Low;
    delete invMassDown_pho_EB_in_High;
    delete invMassDown_pho_EB_out_High;
    delete invMassDown_pho_EE_in_High;
    delete invMassDown_pho_EE_out_High;
    delete puWeights;
    delete leading_pt;
    delete subleading_pt;
    delete pt_55_65;
    delete pt_55_65_up;
    delete pt_55_65_down;

    return;
}

