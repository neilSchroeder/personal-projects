/*
 *
 *
 *
 *
 * This script creates a root file called r9CorrInvMass_fileName
scale_absEta_0_1-R9_0.400_0.600	=	1.002981 +/- 3e-05 L(1.002952 - 1.003011)
 *   which contains histograms of 2D distributions (invariant mass by r9)
 *   in various eta regions
 *
 *
 *
 *
 *
 */

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
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
#include <TChain.h>

#include <assert.h>
#include <string>
#include <fstream>
#include <iostream>
#include <thread>

#include "/usr/include/boost/program_options.hpp"
#include "/afs/cern.ch/user/n/nschroed/tools-dev/interface/EnergyScaleCorrection_class.hh"

//#define DEBUG

using namespace std;

const string info = "[INFO] ";
const string error = "[ERROR] ";

void producer_scaleSmearHists(string, string, string, std::vector<string>);

bool _flag_tightID = false;
bool _flag_mediumID = false;
bool _flag_transformR9 = false;
bool _flag_makeTree = false;
bool _flag_gain12 = false;

std::string output = "";
std::string temp_output = "";

float R9CUT = 0.96;
float ptCutLead = 32;
float ptCutSub = 20;

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
        ("r9", opts::value<float>(&R9CUT)->default_value(0.96), "R9 boundary, default is 0.96")
        ("ptLead", opts::value<float>(&ptCutLead)->default_value(32), "Pt cut on leading electron")
        ("ptSub", opts::value<float>(&ptCutSub)->default_value(20), "Pt cut on subleading electron")
        ("makeTree,t", opts::bool_switch(&_flag_makeTree), "Make a root tree which contains all the necessary information to make the histograms in this file")
        ("mediumID", opts::bool_switch(&_flag_mediumID), "Apply the medium ID working point")
        ("tightID", opts::bool_switch(&_flag_tightID), "Apply the tight ID working point")
        ("transformR9", opts::bool_switch(&_flag_transformR9), "Apply R9 corrections")
        ("onlyGain12", opts::bool_switch(&_flag_gain12), "Assume all entries are gain 12")
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

    temp_output = "dat/producer_scaleSmearHists_"+output;
    std::cout << "[INFO] output file: " << temp_output << std::endl;
    std::cout << "[INFO] the R9 boundary is: " << R9CUT << std::endl;
    std::cout << "[INFO] the Et cuts are: " << ptCutLead << " " << ptCutSub << std::endl;

    system( std::string("./scripts/check_output.sh "+temp_output+".dat").c_str());

    gRandom->SetSeed(89389);
    producer_scaleSmearHists(fileTypes[0], inputFiles[0], ssFiles[0], puFiles);
    producer_scaleSmearHists(fileTypes[1], inputFiles[1], ssFiles[1], puFiles);

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


    //////////////////////////////////////////////////////////////////////////////
    // temp and tmep are the bin edges for the run ranges in 2016 and 2017/2018 //
    //////////////////////////////////////////////////////////////////////////////

    std::vector<float> runs_2018 = {-1};
    //std::ifstream in("/afs/cern.ch/work/n/nschroed/ss_pyfit/CMSSW_10_2_14/src/cms-ecal-scales-and-smearings/datFiles/run_divide_ul2018.dat");
    std::ifstream run_file("/afs/cern.ch/work/n/nschroed/ss_pyfit/CMSSW_10_2_14/src/cms-ecal-scales-and-smearings/datFiles/run_divide_ul2017.dat");
    
    float run_min, run_max;
    while(run_file >> run_min >> run_max){
        if( runs_2018.size() > 0){
            if( run_min - 0.5 != runs_2018[runs_2018.size()-1]) runs_2018.push_back(run_min-0.5);
            runs_2018.push_back(run_max+0.5);
        }
        else {
            runs_2018.push_back(run_min-0.5);
            runs_2018.push_back(run_max+0.5);
        }
    }
    run_file.close();
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
    TH1D * puWeights_data;
    TH1D * puWeights_mc;
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
        puWeights = new TH1D("puWeights", "", 100, 0, 100);
        for(int i = 1; i <= 100; i++){
            puWeights->SetBinContent(i, 1.);
        }
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
                    puWeights_data = (TH1D*)weightFile->Get(key->GetName());
                    puWeights = (TH1D*) puWeights_data->Clone("puWeights");
                }
                TFile * weightFile2 = TFile::Open(pileupFiles[1].c_str(), "READ");
                TIter next2(weightFile2->GetListOfKeys());
                while( (key = (TKey*)next2()) ){
                    puWeights_mc = (TH1D*)weightFile2->Get(key->GetName());
                }
                if(puWeights_data->Integral() != 1) puWeights_data->Scale(1/puWeights_data->Integral());
                if(puWeights_mc->Integral() != 1) puWeights_mc->Scale(1/puWeights_mc->Integral());
                for(int i = 1; i <= puWeights_data->GetNbinsX(); i++){
                    if( puWeights_mc->GetBinContent(i) != 0) puWeights->SetBinContent(i, puWeights_data->GetBinContent(i)/puWeights_mc->GetBinContent(i));
                    else puWeights->SetBinContent(i, puWeights_data->GetBinContent(i));
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
    
    bool useScalesSmears = scaleSmearFile.compare("x") != 0;

    EnergyScaleCorrection_class scalerSmearer;
    if( useScalesSmears ){
//        std::cout << "[INFO] Creating EnergyScaleCorrection_class scaleSmearer from: " << scaleSmearFile << std::endl;
        scalerSmearer = EnergyScaleCorrection_class(scaleSmearFile.c_str(), 0);
        std::cout << std::endl;
    }
    else {
        std::cout << "[INFO] no EnergyScaleCorrection_class scaleSmearer will be created" << std::endl;
    }

    ////////////////////////////////////////////////////////
    // declare the histograms we use for the cross checks //
    ////////////////////////////////////////////////////////

    float pt_bins [13] = {20, 40, 60, 80, 100, 140, 180, 220, 260, 300, 400, 500, 1000};
    float fine_et_bins [14] = {20, 35, 37, 39, 40, 41, 42, 43, 44, 45, 46, 47, 49, 52};
    float new_et_bins [8] = {30, 40, 50, 60, 75, 90, 110, 9999};
    float stat_et_bins [8] = {20, 39, 50, 65, 80, 100, 1000};
    float mee_bins [401];
    for(int jjj = 0; jjj < 401; jjj++){ 
        mee_bins[jjj] = 80 + (float)jjj/20.;
    }
    TH3F * fine_grain_linearity = new TH3F("fine_grain_linearity", "", 12, pt_bins, 12, pt_bins, 400, mee_bins);
    TH2F * mee_vs_pt = new TH2F("mee_vs_pt", "", 12, pt_bins, 400, mee_bins);
    TH2F * meeUp_vs_pt = new TH2F("meeUp_vs_pt", "", 12, pt_bins, 400, mee_bins);
    TH2F * meeDown_vs_pt = new TH2F("meeDown_vs_pt", "", 12, pt_bins, 400, mee_bins);
    TH2F * mee_et = new TH2F("mee_et", "", 6, stat_et_bins, 400, mee_bins);
    TH2F * mee_et_eb = new TH2F("mee_et_eb", "", 6, stat_et_bins, 400, mee_bins);
    TH2F * meeUp_et_eb = new TH2F("meeUp_et_eb", "", 6, stat_et_bins, 400, mee_bins);
    TH2F * meeDown_et_eb = new TH2F("meeDown_et_eb", "", 6, stat_et_bins, 400, mee_bins);
    TH2F * mee_et_ee = new TH2F("mee_et_ee", "", 6, stat_et_bins, 400, mee_bins);
    TH2F * meeUp_et_ee = new TH2F("meeUp_et_ee", "", 6, stat_et_bins, 400, mee_bins);
    TH2F * meeDown_et_ee = new TH2F("meeDown_et_ee", "", 6, stat_et_bins, 400, mee_bins);
    TH2F * mee_et_stats = new TH2F("mee_et_stats", "", 6, stat_et_bins, 6, stat_et_bins);
    TH2F * mee_fine_bins = new TH2F("mee_fine_bins", "", 13, fine_et_bins, 400, mee_bins);

    std::cout << "[INFO] initializing histograms" << std::endl;
    TH2F * invMassVsNPU = new TH2F("invMassVsNPU", "", 35, npuBins, 400, 80, 100);
    TH2F * mee_npu_rawSC_low = new TH2F("mee_npu_rawSC_low", "", 35, npuBins, 400, 80, 100);
    TH2F * mee_npu_rawSC_high = new TH2F("mee_npu_rawSC_high", "", 35, npuBins, 400, 80, 100);
    TH2F * mee_npu_ele_low = new TH2F("mee_npu_ele_low", "", 35, npuBins, 400, 80, 100);
    TH2F * mee_npu_ele_high = new TH2F("mee_npu_ele_high", "", 35, npuBins, 400, 80, 100);
    TH2F * et_stats0 = new TH2F("et_stats0", "", 1000, 20, 1020, 1000, 20, 1020);
    TH2F * et_stats1 = new TH2F("et_stats1", "", 1000, 20, 1020, 1000, 20, 1020);
    TH2F * et_stats2 = new TH2F("et_stats2", "", 1000, 20, 1020, 1000, 20, 1020);
    TH2F * et_stats3 = new TH2F("et_stats3", "", 1000, 20, 1020, 1000, 20, 1020);
    TH2F * et_stats4 = new TH2F("et_stats4", "", 1000, 20, 1020, 1000, 20, 1020);
    

    TH1F * invMass = new TH1F("invMass", "", 400, 80, 100);
    TH1F * r9Dist = new TH1F("r9dist", "", 50000, 0, 10);
    TH1F * r9Dist_EB = new TH1F("r9Dist_EB", "", 50000, 0, 10);
    TH1F * r9Dist_EE = new TH1F("r9Dist_EE", "", 50000, 0, 10);
    
    TH1F * invMass_EbEb_ptz200 = new TH1F("invMass_EbEb_ptz200", "", 1200,80,100);
    TH1F * invMassUp_EbEb_ptz200 = new TH1F("invMassUp_EbEb_ptz200", "", 1200,80,100);
    TH1F * invMassDown_EbEb_ptz200 = new TH1F("invMassDown_EbEb_ptz200", "", 1200,80,100);
    TH1F * invMass_EeEe_ptz200 = new TH1F("invMass_EeEe_ptz200", "", 1200,80,100);
    TH1F * invMassUp_EeEe_ptz200 = new TH1F("invMassUp_EeEe_ptz200", "", 1200,80,100);
    TH1F * invMassDown_EeEe_ptz200 = new TH1F("invMassDown_EeEe_ptz200", "", 1200,80,100);

    TH1F * invMass_ele_EB_in_Low = new TH1F("invMass_ele_EB_in_Low", "", 1200, 80, 100);
    TH1F * invMass_ele_EB_out_Low = new TH1F("invMass_ele_EB_out_Low", "", 1200, 80, 100);
    TH1F * invMass_ele_EE_in_Low = new TH1F("invMass_ele_EE_in_Low", "", 1200, 80, 100);
    TH1F * invMass_ele_EE_out_Low = new TH1F("invMass_ele_EE_out_Low", "", 1200, 80, 100);
    TH1F * invMass_ele_EB_in_High = new TH1F("invMass_ele_EB_in_High", "", 1200, 80, 100);
    TH1F * invMass_ele_EB_out_High = new TH1F("invMass_ele_EB_out_High", "", 1200, 80, 100);
    TH1F * invMass_ele_EE_in_High = new TH1F("invMass_ele_EE_in_High", "", 1200, 80, 100);
    TH1F * invMass_ele_EE_out_High = new TH1F("invMass_ele_EE_out_High", "", 1200, 80, 100);
    TH1F * invMass_ele_NotEbEb_high = new TH1F("invMass_ele_NotEbEb_high", "", 1200, 80, 100);
    TH1F * invMassUp_ele_NotEbEb_high = new TH1F("invMassUp_ele_NotEbEb_high", "", 1200, 80, 100);
    TH1F * invMassDown_ele_NotEbEb_high = new TH1F("invMassDown_ele_NotEbEb_high", "", 1200, 80, 100);
    TH1F * invMass_ele_NotEbEb = new TH1F("invMass_ele_NotEbEb", "", 1200, 80, 100);
    TH1F * invMassUp_ele_NotEbEb = new TH1F("invMassUp_ele_NotEbEb", "", 1200, 80, 100);
    TH1F * invMassDown_ele_NotEbEb = new TH1F("invMassDown_ele_NotEbEb", "", 1200, 80, 100);
    TH1F * invMass_ele_EbEb_high = new TH1F("invMass_ele_EbEb_high", "", 1200, 80, 100);
    TH1F * invMassUp_ele_EbEb_high = new TH1F("invMassUp_ele_EbEb_high", "", 1200, 80, 100);
    TH1F * invMassDown_ele_EbEb_high = new TH1F("invMassDown_ele_EbEb_high", "", 1200, 80, 100);
    TH1F * invMassUp_ele_EB_in_Low = new TH1F("invMassUp_ele_EB_in_Low", "", 1200, 80, 100);
    TH1F * invMassUp_ele_EB_out_Low = new TH1F("invMassUp_ele_EB_out_Low", "", 1200, 80, 100);
    TH1F * invMassUp_ele_EE_in_Low = new TH1F("invMassUp_ele_EE_in_Low", "", 1200, 80, 100);
    TH1F * invMassUp_ele_EE_out_Low = new TH1F("invMassUp_ele_EE_out_Low", "", 1200, 80, 100);
    TH1F * invMassUp_ele_EB_in_High = new TH1F("invMassUp_ele_EB_in_High", "", 1200, 80, 100);
    TH1F * invMassUp_ele_EB_out_High = new TH1F("invMassUp_ele_EB_out_High", "", 1200, 80, 100);
    TH1F * invMassUp_ele_EE_in_High = new TH1F("invMassUp_ele_EE_in_High", "", 1200, 80, 100);
    TH1F * invMassUp_ele_EE_out_High = new TH1F("invMassUp_ele_EE_out_High", "", 1200, 80, 100);
    TH1F * invMassDown_ele_EB_in_Low = new TH1F("invMassDown_ele_EB_in_Low", "", 1200, 80, 100);
    TH1F * invMassDown_ele_EB_out_Low = new TH1F("invMassDown_ele_EB_out_Low", "", 1200, 80, 100);
    TH1F * invMassDown_ele_EE_in_Low = new TH1F("invMassDown_ele_EE_in_Low", "", 1200, 80, 100);
    TH1F * invMassDown_ele_EE_out_Low = new TH1F("invMassDown_ele_EE_out_Low", "", 1200, 80, 100);
    TH1F * invMassDown_ele_EB_in_High = new TH1F("invMassDown_ele_EB_in_High", "", 1200, 80, 100);
    TH1F * invMassDown_ele_EB_out_High = new TH1F("invMassDown_ele_EB_out_High", "", 1200, 80, 100);
    TH1F * invMassDown_ele_EE_in_High = new TH1F("invMassDown_ele_EE_in_High", "", 1200, 80, 100);
    TH1F * invMassDown_ele_EE_out_High = new TH1F("invMassDown_ele_EE_out_High", "", 1200, 80, 100);

    TH1F * invMass_EBEB = new TH1F("invMass_EBEB", "", 1200, 80, 100);
    TH1F * invMassUp_EBEB = new TH1F("invMassUp_EBEB", "", 1200, 80, 100);
    TH1F * invMassDown_EBEB = new TH1F("invMassDown_EBEB", "", 1200, 80, 100);
    TH1F * invMass_EBEB_low = new TH1F("invMass_EBEB_low", "", 1200, 80, 100);
    TH1F * invMass_EBEB_high = new TH1F("invMass_EBEB_high", "", 1200, 80, 100);
    TH1F * invMass_EEEE = new TH1F("invMass_EEEE", "", 1200, 80, 100);
    TH1F * invMassUp_EEEE = new TH1F("invMassUp_EEEE", "", 1200, 80, 100);
    TH1F * invMassDown_EEEE = new TH1F("invMassDown_EEEE", "", 1200, 80, 100);
    TH1F * invMass_EEEE_low = new TH1F("invMass_EEEE_low", "", 1200, 80, 100);
    TH1F * invMass_EEEE_high = new TH1F("invMass_EEEE_high", "", 1200, 80, 100);

    TH1F * invMass_R9_96_99 = new TH1F("invMass_R9_96_99", "", 400, 80, 100);
    TH1F * invMass_R9_99_100 = new TH1F("invMass_R9_99_100", "", 400, 80, 100);
    TH1F * invMass_R9_99_1000 = new TH1F("invMass_R9_99_1000", "", 400, 80, 100);

    TH1F * leading_pt = new TH1F("leading_pt", "", 1000, 0, 100);
    TH1F * subleading_pt = new TH1F("subleading_pt", "", 1000, 0, 100);
    TH1F * npu_distribution = new TH1F("npu_dist", "", 100, 0, 100);

    TH1F * pt_40_55 = new TH1F("pt_40_55", "", 1200, 80, 100);
    TH1F * pt_40_55_up = new TH1F("pt_40_55_up", "", 1200, 80, 100);
    TH1F * pt_40_55_down = new TH1F("pt_40_55_down", "", 1200, 80, 100);
    TH1F * pt_55_65 = new TH1F("pt_55_65", "", 1200, 80, 100);
    TH1F * pt_55_65_up = new TH1F("pt_55_65_up", "", 1200, 80, 100);
    TH1F * pt_55_65_down = new TH1F("pt_55_65_down", "", 1200, 80, 100);
    TH1F * ptDiag_55_65 = new TH1F("ptDiag_55_65", "", 1200, 80, 100);
    TH1F * ptDiag_55_65_up = new TH1F("ptDiag_55_65_up", "", 1200, 80, 100);
    TH1F * ptDiag_55_65_down = new TH1F("ptDiag_55_65_down", "", 1200, 80, 100);
    TH1F * pt_65_90 = new TH1F("pt_65_90", "", 1200, 80, 100);
    TH1F * pt_65_90_up = new TH1F("pt_65_90_up", "", 1200, 80, 100);
    TH1F * pt_65_90_down = new TH1F("pt_65_90_down", "", 1200, 80, 100);
    TH2F * etaDist_pt = new TH2F("etaDist_pt", "", 100, 0, 2.5,100,0,2.5);
    TH1F * pt_90_inf = new TH1F("pt_90_inf", "", 1200, 80, 100);
    TH1F * pt_90_inf_up = new TH1F("pt_90_inf_up", "", 1200, 80, 100);
    TH1F * pt_90_inf_down = new TH1F("pt_90_inf_down", "", 1200, 80, 100);
    TH2F * r9Dist_pt = new TH2F("r9Dist_pt", "", 1000, 0, 10, 1000, 0, 10);

    TH1F * pt_55_65_0_40 = new TH1F("pt_55_65_0_40", "", 1200, 80, 100);
    TH1F * pt_55_65_40_55 = new TH1F("pt_55_65_40_55", "", 1200, 80, 100);
    TH1F * pt_55_65_55_65 = new TH1F("pt_55_65_55_65", "", 1200, 80, 100);
    TH1F * pt_55_65_65_100 = new TH1F("pt_55_65_65_100", "", 1200, 80, 100);
    TH1F * pt_55_65_100_1000 = new TH1F("pt_55_65_100_1000", "", 1200, 80, 100);

    TH1F * invMass_gain6_EB = new TH1F("invMass_gain6_EB", "", 1200, 80, 100);
    TH1F * invMassUp_gain6_EB = new TH1F("invMassUp_gain6_EB", "", 1200, 80, 100);
    TH1F * invMassDown_gain6_EB = new TH1F("invMassDown_gain6_EB", "", 1200, 80, 100);
    TH1F * invMass_gain6_EE = new TH1F("invMass_gain6_EE", "", 1200, 80, 100);
    TH1F * invMassUp_gain6_EE = new TH1F("invMassUp_gain6_EE", "", 1200, 80, 100);
    TH1F * invMassDown_gain6_EE = new TH1F("invMassDown_gain6_EE", "", 1200, 80, 100);
    TH1F * invMass_gain1_EB = new TH1F("invMass_gain1_EB", "", 1200, 80, 100);
    TH1F * invMassUp_gain1_EB = new TH1F("invMassUp_gain1_EB", "", 1200, 80, 100);
    TH1F * invMassDown_gain1_EB = new TH1F("invMassDown_gain1_EB", "", 1200, 80, 100);
    TH1F * invMass_gain1_EE = new TH1F("invMass_gain1_EE", "", 1200, 80, 100);
    TH1F * invMassUp_gain1_EE = new TH1F("invMassUp_gain1_EE", "", 1200, 80, 100);
    TH1F * invMassDown_gain1_EE = new TH1F("invMassDown_gain1_EE", "", 1200, 80, 100);

    TH1F * et_EBin_low = new TH1F("et_EBin_low", "", 400, 0, 100);
    TH1F * et_EBin_high = new TH1F("et_EBin_high", "", 400, 0, 100);
    TH1F * et_EEin_low = new TH1F("et_EEin_low", "", 400, 0, 100);
    TH1F * et_EEin_high = new TH1F("et_EEin_high", "", 400, 0, 100);
    TH1F * et_EBout_low = new TH1F("et_EBout_low", "", 400, 0, 100);
    TH1F * et_EBout_high = new TH1F("et_EBout_high", "", 400, 0, 100);
    TH1F * et_EEout_low = new TH1F("et_EEout_low", "", 400, 0, 100);
    TH1F * et_EEout_high = new TH1F("et_EEout_high", "", 400, 0, 100);

    TH2F * invMass_fineEta = new TH2F("invMass_fineEta", "", 13, 0, 2.6, 400, 80, 100);
    TH2F * invMass_fineEtaLow = new TH2F("invMass_fineEtaLow", "", 13, 0, 2.6, 400, 80, 100);
    TH2F * invMass_fineEtaHigh = new TH2F("invMass_fineEtaHigh", "", 13, 0, 2.6, 400, 80, 100);
    float eta_bins [6] = {0, 1, 1.4442, 1.566, 2, 2.5};
    float r9_bins_eb[10] = {0, 0.4, 0.6, 0.7, 0.8, 0.9, 0.92, 0.94, 0.96, 1.5};
    float r9_bins_ee_in[17] = {0, 0.5, 0.6, 0.65, 0.7, 0.74, 0.77, 0.82, 0.84, 0.86, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98, 1.5};
    float r9_bins_ee_out[13] = {0, 0.5, 0.6, 0.7, 0.8, 0.833, 0.867, 0.9, 0.92, 0.94, 0.96, 0.98, 1.5};
    TH3F * invMass_coarseEta = new TH3F("invMass_coarseEta", "", 5, eta_bins, 5, eta_bins, 400, mee_bins);
    TH3F * invMass_coarseEtaLow = new TH3F("invMass_coarseEtaLow", "", 5, eta_bins, 5, eta_bins, 400, mee_bins);
    TH3F * invMass_coarseEtaHigh = new TH3F("invMass_coarseEtaHigh", "", 5, eta_bins, 5, eta_bins, 400, mee_bins);
    TH3F * invMass_55_65_coarseEta = new TH3F("invMass_55_65_coarseEta", "", 5, eta_bins, 5, eta_bins, 400, mee_bins);
    TH3F * invMass_55_65_coarseEtaLow = new TH3F("invMass_55_65_coarseEtaLow", "", 5, eta_bins, 5, eta_bins, 400, mee_bins);
    TH3F * invMass_55_65_coarseEtaHigh = new TH3F("invMass_55_65_coarseEtaHigh", "", 5, eta_bins, 5, eta_bins, 400, mee_bins);
    TH2F * invMass_eta0_r9dist = new TH2F("invMass_eta0_r9dist", "", 9, r9_bins_eb, 9, r9_bins_eb);
    TH2F * invMass_eta1_r9dist = new TH2F("invMass_eta1_r9dist", "", 9, r9_bins_eb, 9, r9_bins_eb);
    TH2F * invMass_eta2_r9dist = new TH2F("invMass_eta2_r9dist", "", 9, r9_bins_eb, 9, r9_bins_eb);
    TH2F * invMass_eta3_r9dist = new TH2F("invMass_eta3_r9dist", "", 16, r9_bins_ee_in, 16, r9_bins_ee_in);
    TH2F * invMass_eta4_r9dist = new TH2F("invMass_eta4_r9dist", "", 12, r9_bins_ee_out, 12, r9_bins_ee_out);

    TH1F * invMass_barrelOffDiag = new TH1F("invMass_barrelOffDiag", "", 1200, 80, 100);
    TH1F * invMass_endcapOffDiag = new TH1F("invMass_endcapOffDiag", "", 1200, 80, 100);

    TH2F * invMass_run2018_EBin = new TH2F("invMass_run2018_EBin", "", runs_2018.size()-1, runs_2018.data(), 400, mee_bins);
    TH2F * invMass_run2018_EBout = new TH2F("invMass_run2018_EBout", "", runs_2018.size()-1, runs_2018.data(), 400, mee_bins);
    TH2F * invMass_run2018_EEin = new TH2F("invMass_run2018_EEin", "", runs_2018.size()-1, runs_2018.data(), 400, mee_bins);
    TH2F * invMass_run2018_EEout = new TH2F("invMass_run2018_EEout", "", runs_2018.size()-1, runs_2018.data(), 400, mee_bins);

    //Sumw2 the shit out of everything

    mee_vs_pt->Sumw2();
    meeUp_vs_pt->Sumw2();
    meeDown_vs_pt->Sumw2();
    invMass_run2018_EBin->Sumw2();
    invMass_run2018_EBout->Sumw2();
    invMass_run2018_EEin->Sumw2();
    invMass_run2018_EEout->Sumw2();
    invMass_coarseEta->Sumw2();
    invMass_coarseEtaLow->Sumw2();
    invMass_coarseEtaHigh->Sumw2();
    invMass_55_65_coarseEta->Sumw2();
    invMass_55_65_coarseEtaLow->Sumw2();
    invMass_55_65_coarseEtaHigh->Sumw2();
    mee_et->Sumw2();
    mee_et_eb->Sumw2();
    meeUp_et_eb->Sumw2();
    meeDown_et_eb->Sumw2();
    mee_et_ee->Sumw2();
    meeUp_et_ee->Sumw2();
    meeDown_et_ee->Sumw2();
    mee_et_stats->Sumw2();
    invMassVsNPU->Sumw2();
    mee_npu_rawSC_low->Sumw2();
    mee_npu_rawSC_high->Sumw2();
    mee_npu_ele_low->Sumw2();
    mee_npu_ele_high->Sumw2();
    invMass->Sumw2();
    r9Dist->Sumw2();
    r9Dist_EB->Sumw2();
    r9Dist_EE->Sumw2();
    invMass_EbEb_ptz200->Sumw2();
    invMassUp_EbEb_ptz200->Sumw2();
    invMassDown_EbEb_ptz200->Sumw2();
    invMass_EeEe_ptz200->Sumw2();
    invMassUp_EeEe_ptz200->Sumw2();
    invMassDown_EeEe_ptz200->Sumw2();
    invMass_ele_NotEbEb_high->Sumw2();
    invMassUp_ele_NotEbEb_high->Sumw2();
    invMassDown_ele_NotEbEb_high->Sumw2();
    invMass_ele_NotEbEb->Sumw2();
    invMassUp_ele_NotEbEb->Sumw2();
    invMassDown_ele_NotEbEb->Sumw2();
    invMass_ele_EbEb_high->Sumw2();
    invMassUp_ele_EbEb_high->Sumw2();
    invMassDown_ele_EbEb_high->Sumw2();
    invMass_ele_EB_in_Low->Sumw2();
    invMass_ele_EB_out_Low->Sumw2();
    invMass_ele_EE_in_Low->Sumw2();
    invMass_ele_EE_out_Low->Sumw2();
    invMass_ele_EB_in_High->Sumw2();
    invMass_ele_EB_out_High->Sumw2();
    invMass_ele_EE_in_High->Sumw2();
    invMass_ele_EE_out_High->Sumw2();
    invMassUp_ele_EB_in_Low->Sumw2();
    invMassUp_ele_EB_out_Low->Sumw2();
    invMassUp_ele_EE_in_Low->Sumw2();
    invMassUp_ele_EE_out_Low->Sumw2();
    invMassUp_ele_EB_in_High->Sumw2();
    invMassUp_ele_EB_out_High->Sumw2();
    invMassUp_ele_EE_in_High->Sumw2();
    invMassUp_ele_EE_out_High->Sumw2();
    invMassDown_ele_EB_in_Low->Sumw2();
    invMassDown_ele_EB_out_Low->Sumw2();
    invMassDown_ele_EE_in_Low->Sumw2();
    invMassDown_ele_EE_out_Low->Sumw2();
    invMassDown_ele_EB_in_High->Sumw2();
    invMassDown_ele_EB_out_High->Sumw2();
    invMassDown_ele_EE_in_High     ->Sumw2();
    invMassDown_ele_EE_out_High    ->Sumw2();
    invMass_EBEB->Sumw2();
    invMassUp_EBEB->Sumw2();
    invMassDown_EBEB->Sumw2();
    invMass_EBEB_low->Sumw2();
    invMass_EBEB_high->Sumw2();
    invMass_EEEE->Sumw2();
    invMassDown_EEEE->Sumw2();
    invMassUp_EEEE->Sumw2();
    invMass_EEEE_low->Sumw2();
    invMass_EEEE_high->Sumw2();
    leading_pt->Sumw2();
    subleading_pt->Sumw2();
    pt_40_55->Sumw2();
    pt_40_55_up->Sumw2();
    pt_40_55_down->Sumw2();
    pt_55_65->Sumw2();
    pt_55_65_up->Sumw2();
    pt_55_65_down->Sumw2();
    pt_55_65_0_40->Sumw2();
    pt_55_65_40_55->Sumw2();
    pt_55_65_55_65->Sumw2();
    pt_55_65_65_100->Sumw2();
    pt_55_65_100_1000->Sumw2();
    ptDiag_55_65->Sumw2();
    ptDiag_55_65_up->Sumw2();
    ptDiag_55_65_down->Sumw2();
    pt_65_90->Sumw2();
    pt_65_90_up->Sumw2();
    pt_65_90_down->Sumw2();
    pt_90_inf->Sumw2();
    pt_90_inf_up->Sumw2();
    pt_90_inf_down->Sumw2();
    etaDist_pt->Sumw2();
    r9Dist_pt->Sumw2();
    invMass_gain6_EB->Sumw2();
    invMass_gain6_EE->Sumw2();
    invMass_gain1_EB->Sumw2();
    invMass_gain1_EE->Sumw2();
    invMass_fineEta->Sumw2();
    invMass_fineEtaLow->Sumw2();
    invMass_fineEtaHigh->Sumw2();
    et_EBin_low->Sumw2();
    et_EBin_high->Sumw2();
    et_EEin_low->Sumw2();
    et_EEin_high->Sumw2();
    et_EBout_low->Sumw2();
    et_EBout_high->Sumw2();
    et_EEout_low->Sumw2();
    et_EEout_high->Sumw2();
    invMass_R9_96_99->Sumw2();
    invMass_R9_99_100->Sumw2();
    invMass_R9_99_1000->Sumw2();
    invMass_barrelOffDiag->Sumw2();
    invMass_endcapOffDiag->Sumw2();
    et_stats0->Sumw2();
    et_stats1->Sumw2();
    et_stats2->Sumw2();
    et_stats3->Sumw2();
    et_stats4->Sumw2();
    mee_fine_bins->Sumw2();

    float R9Ele [3];
    float eta [3];
    float phi [3];
    float energy_ele [3];
    float energy_pho [3];
    float energyRaw [3];
    float energy5x5 [3];
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
    

    //loop over ntuples
    if(!in.is_open()){
        cout << "[ERROR] file: " << fileName << " did not open" << endl;
        return;
    }

    std::cout << "[INFO] begin processing files ... " << std::endl;
    std::vector<std::string> files;
    while( in >> thisFile ){ files.push_back(thisFile);}
    TChain thisChain("selected");
    for(int i = 0; i < files.size(); i++) thisChain.Add(files[i].c_str());

    thisChain.SetBranchAddress("R9Ele", R9Ele);
    thisChain.SetBranchAddress("etaSCEle", etaSCEle);
    thisChain.SetBranchAddress("invMass_ECAL_ele", invMass_ele);
    thisChain.SetBranchAddress("etaEle", eta);
    thisChain.SetBranchAddress("phiEle", phi);
    thisChain.SetBranchAddress("energy_ECAL_ele", energy_ele);
    thisChain.SetBranchAddress("mcGenWeight", &mcWeight);
    thisChain.SetBranchAddress("nPU", &NPU);
    thisChain.SetBranchAddress("rho", &rho);
    thisChain.SetBranchAddress("runNumber", &runNum);
    thisChain.SetBranchAddress("eleID", eleID);
    thisChain.SetBranchAddress("gainSeedSC", gain);
    thisChain.SetBranchAddress("energy_5x5SC", energy5x5);
    thisChain.SetBranchAddress("rawEnergySCEle", energyRaw);

    for(Long64_t i = 0; i < thisChain.GetEntries(); i++){
        thisChain.GetEvent(i);
        bool tracker1 = fabs(eta[0]) < 2.5;
        bool tracker2 = fabs(eta[1]) < 2.5;
        bool exclude1 = fabs(eta[0]) < 1.4442 || fabs(eta[0]) > 1.566;
        bool exclude2 = fabs(eta[1]) < 1.4442 || fabs(eta[1]) > 1.566;
        if( tracker1 && tracker2 && exclude1 && exclude2 && !std::isnan(invMass_ele[0])){

            TLorentzVector  e_lead;
            TLorentzVector  e_sub;
            TLorentzVector z;
            e_lead.SetPtEtaPhiE( energy_ele[0]/TMath::CosH(eta[0]), eta[0], phi[0], energy_ele[0]);
            e_sub.SetPtEtaPhiE( energy_ele[1]/TMath::CosH(eta[1]), eta[1], phi[1], energy_ele[1]);

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
            // new format: scaleSmearer.ScaleCorrection(runNumber, isEBEle, R9Ele, etaSCEle, EtEle, gainSeed, std::bitset<scAll> uncBitMask)

            float correction = 1.;
            float correction_lead = 1.;
            float correction_sub = 1.;
            float correction_up = 1.;
            float correction_down = 1.;


            unsigned int gain1 = 12;
            unsigned int gain2 = 12;
            if( type.compare("data") == 0 && useScalesSmears){
                if(gain[0] == 1) gain1 = 6;
                if(gain[0] > 1) gain1 = 1;
                if(gain[1] == 1) gain2 = 6;
                if( gain[1] > 1) gain2 = 1;
                gain1 = 12;
                gain2 = 12;
                correction_lead = scalerSmearer.ScaleCorrection(runNum , bool(eta[0] < 1.479), thisR91, eta[0], e_lead.Et(), gain1); //gain is gain
                correction_sub = scalerSmearer.ScaleCorrection(runNum , bool(eta[1] < 1.479), thisR92, eta[1], e_sub.Et(), gain2); //gain is gain
                std::bitset<6> b1(0);
                auto correction_lead_unc = scalerSmearer.ScaleCorrectionUncertainty(runNum , bool(eta[0] < 1.479), thisR91, eta[0], e_lead.Et(), gain1, b1); //gain is gain
                auto correction_sub_unc = scalerSmearer.ScaleCorrectionUncertainty(runNum , bool(eta[1] < 1.479), thisR92, eta[1], e_sub.Et(), gain2, b1); //gain is gain
                correction = sqrt(correction_lead*correction_sub);
                correction_up = sqrt((correction_lead + correction_lead_unc)*(correction_sub + correction_sub_unc));
                correction_down = sqrt((correction_lead - correction_lead_unc)*(correction_sub - correction_sub_unc));
            }

            //////////////////////////
            // smearing corrections //
            //////////////////////////
            // new format: scaleSmearer.getSmearingSigma(runNum, isEBEle, R9Ele, etaSCEle, EtEle, gainSeed, paramSmear_t par, nSigma)

            if( type.compare("mc") == 0 && useScalesSmears){
                if(gain[0] == 1) gain1 = 6;
                if(gain[1] > 1) gain1 = 1;
                if(gain[1] == 1) gain2 = 6;
                if( gain[1] > 1) gain2 = 1;
                double sigma1 = scalerSmearer.getSmearingSigma( runNum, bool(eta[0] < 1.479), thisR91, eta[0], e_lead.Et(), gain1, 0., 0.);
                double sigma2 = scalerSmearer.getSmearingSigma( runNum, bool(eta[1] < 1.479), thisR92, eta[1], e_sub.Et(), gain2, 0., 0.);
                double sigma1_up = scalerSmearer.getSmearingSigma( runNum, bool(eta[0] < 1.479), thisR91, eta[0], e_lead.Et(), gain1, 1., 0.);
                double sigma2_up = scalerSmearer.getSmearingSigma( runNum, bool(eta[1] < 1.479), thisR92, eta[1], e_sub.Et(), gain2, 1., 0.);
                double sigma1_down = scalerSmearer.getSmearingSigma( runNum, bool(eta[0] < 1.479), thisR91, eta[0], e_lead.Et(), gain1, -1., 0.);
                double sigma2_down = scalerSmearer.getSmearingSigma( runNum, bool(eta[1] < 1.479), thisR92, eta[1], e_sub.Et(), gain2, -1., 0.);
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

            //cout << "correction: " << correction << "\ncorrection_lead: " << correction_lead << "\ncorrection_sub: " << correction_sub << "\ncorrection_up: " << correction_up << "\ncorrection_down: " << correction_down << endl << endl;


            //some workarounds for "corner" cases
            //   data doesn't have pileup so just make the NPU = 10 and read 1 from the hist
            //   mc isn't generated for specific runs so just assign it to the first bin
            double thisNPU;
            if(type.compare("data") == 0) thisNPU = 10;
            else thisNPU = NPU;

#ifdef DEBUG_full 
            cout << "invMass_ECAL_ele: " << invMass_ele[0] << endl;
            cout << "etaEle: " << eta[0] << " " << eta[1] << endl;
            cout << "R9Ele: " << R9Ele[0] << " " << R9Ele[1] << endl;
            cout << "mcGenWeight: " << mcWeight << endl;
            cout << "npu: " << NPU << endl;
            cout << "rho: " << rho << endl;
#endif


            ////////////////////////
            // recreate 4-vectors //
            ////////////////////////

            e_lead.SetPtEtaPhiE( energy_ele[0]*correction_lead/TMath::CosH(eta[0]), eta[0], phi[0], energy_ele[0]*correction_lead);
            e_sub.SetPtEtaPhiE( energy_ele[1]*correction_sub/TMath::CosH(eta[1]), eta[1], phi[1], energy_ele[1]*correction_sub);
            z = e_lead+e_sub;

            /////////////////////////////
            // create cuts using bools //
            /////////////////////////////

            bool leadPt = e_lead.Pt() > ptCutLead;
            bool subPt = e_sub.Pt() > ptCutSub;

            bool EB_IN = fabs(eta[0]) < 1 && fabs(eta[1]) < 1;
            bool EB_IN1 = fabs(eta[0]) < 1; bool EB_IN2 = fabs(eta[1]) < 1;
            bool EB_OUT = fabs(eta[0]) > 1 && fabs(eta[0]) < 1.4442 && fabs(eta[1]) > 1 && fabs(eta[1]) < 1.4442;
            bool EB_OUT1 = fabs(eta[0]) > 1 && fabs(eta[0]) < 1.4442; bool EB_OUT2 = fabs(eta[1]) > 1 && fabs(eta[1]) < 1.4442;
            bool EB1 = fabs(eta[0]) < 1.4442; bool EB2 = fabs(eta[1]) < 1.4442;
            bool EE1 = fabs(eta[0]) > 1.566; bool EE2 = fabs(eta[1]) > 1.566;
            bool EE_IN = fabs(eta[0]) > 1.566 && fabs(eta[0]) < 2.0 && fabs(eta[1]) > 1.566 && fabs(eta[1]) < 2.0;
            bool EE_IN1 = fabs(eta[0]) > 1.566 && fabs(eta[0]) < 2.0; bool EE_IN2 = fabs(eta[1]) && fabs(eta[1]) < 2.0;
            bool EE_OUT = fabs(eta[0]) > 2.0 && fabs(eta[1]) > 2.0;
            bool EE_OUT1 = fabs(eta[0]) > 2.0; bool EE_OUT2 = fabs(eta[1]) > 2.0;

            bool lowR9 = thisR91 < R9CUT && thisR92 < R9CUT;
            bool lowR91 = thisR91 < R9CUT; bool lowR92 = thisR92 < R9CUT;
            bool highR9 = thisR91 > R9CUT && thisR92 > R9CUT;
            bool highR91 = thisR91 > R9CUT; bool highR92 = thisR92 > R9CUT;

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
            double fineEtaBins [14] = {0, 0.2, 0.4, 0.6, 0.8, 1., 1.2, 1.4, 1.6, 1.8, 2., 2.2, 2.4, 2.6};

            if(fabs(eta[0]) < 1 and fabs(eta[1]) < 1 and invMass_ele[0]*correction >= 80 and invMass_ele[0]*correction <= 100){
                if( e_lead.Pt() > 32 and e_sub.Pt() > 20){
                    for( int i = 0; i < 13; i++){
                        if( e_lead.Pt() > fine_et_bins[i] and e_lead.Pt() < fine_et_bins[i+1] and e_sub.Pt() > fine_et_bins[i] and e_sub.Pt() < fine_et_bins[i+1]){
                            mee_fine_bins->Fill(e_lead.Pt(), invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                    }
                }
            }

            if(fabs(eta[0]) < 1 and fabs(eta[1]) < 1) et_stats0->Fill(e_lead.Pt(), e_sub.Pt());
            if(fabs(eta[0]) > 1 and fabs(eta[0]) < 1.2 and fabs(eta[1]) > 1 and fabs(eta[1]) < 1.2) et_stats1->Fill(e_lead.Pt(), e_sub.Pt());
            if(fabs(eta[0]) > 1.2 and fabs(eta[0]) < 1.4442 and fabs(eta[1]) > 1.2 and fabs(eta[1]) < 1.4442) et_stats2->Fill(e_lead.Pt(), e_sub.Pt());
            if(fabs(eta[0]) > 1.566 and fabs(eta[0]) < 2 and fabs(eta[1]) > 1.566 and fabs(eta[1]) < 2) et_stats3->Fill(e_lead.Pt(), e_sub.Pt());
            if(fabs(eta[0]) > 2 and fabs(eta[0]) < 2.5 and fabs(eta[1]) > 2 and fabs(eta[1]) < 2.5) et_stats4->Fill(e_lead.Pt(), e_sub.Pt());
            if( leadPt && subPt && mvaID /* && gainCut */ ){


                ///////////////////////////////////////////////////////
                // set a mass window of 80 to 100 for invariant mass //
                ///////////////////////////////////////////////////////

                if( invMass_ele[0]*correction >= 80 && invMass_ele[0]*correction <= 100){
                    if(EB_IN) invMass_run2018_EBin->Fill(runNum, invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(EB_OUT) invMass_run2018_EBout->Fill(runNum, invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(EE_IN) invMass_run2018_EEin->Fill(runNum, invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(EE_OUT) invMass_run2018_EEout->Fill(runNum, invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    TLorentzVector z = e_lead+e_sub;
                    mee_vs_pt->Fill(z.Pt(), invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    meeUp_vs_pt->Fill(z.Pt(), invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    meeDown_vs_pt->Fill(z.Pt(), invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    fine_grain_linearity->Fill(e_lead.Pt(), e_sub.Pt(), invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));

                    mee_et_stats->Fill(e_lead.Pt(), e_sub.Pt());
                    mee_et->Fill(e_lead.Pt(), invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(EB1 and EB2){
                        mee_et_eb->Fill(e_lead.Pt(), invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        meeUp_et_eb->Fill(e_lead.Pt(), invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        meeDown_et_eb->Fill(e_lead.Pt(), invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        if( gain[0] == 1 and gain[1] == 1){
                            invMass_gain6_EB->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            invMassUp_gain6_EB->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            invMassDown_gain6_EB->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( gain[0] > 1 and gain[1] > 1){
                            invMass_gain1_EB->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            invMassUp_gain1_EB->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            invMassDown_gain1_EB->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                    }
                    if(EE1 and EE2){
                        mee_et_ee->Fill(e_lead.Pt(), invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        meeUp_et_ee->Fill(e_lead.Pt(), invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        meeDown_et_ee->Fill(e_lead.Pt(), invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        if( gain[0] == 1 and gain[1] == 1){
                            invMass_gain6_EE->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            invMassUp_gain6_EE->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            invMassDown_gain6_EE->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        if( gain[0] > 1 and gain[1] > 1){
                            invMass_gain1_EE->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            invMassUp_gain1_EE->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            invMassDown_gain1_EE->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                    }

                    invMass_coarseEta->Fill(fabs(eta[0]), fabs(eta[1]), invMass_ele[0]*correction,  TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(lowR9) invMass_coarseEtaLow->Fill(fabs(eta[0]), fabs(eta[1]), invMass_ele[0]*correction,  TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(highR9) invMass_coarseEtaHigh->Fill(fabs(eta[0]), fabs(eta[1]), invMass_ele[0]*correction,  TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));

                    if(0 < fabs(eta[0]) && fabs(eta[0]) < 1 && 0 < fabs(eta[1]) && fabs(eta[1]) < 1){
                        if(thisR91 > thisR92) invMass_eta0_r9dist->Fill(thisR92, thisR91);
                        else invMass_eta0_r9dist->Fill(thisR91, thisR92);
                    }
                    if(1 < fabs(eta[0]) && fabs(eta[0]) < 1.2 && 1 < fabs(eta[1]) && fabs(eta[1]) < 1.2){
                        if(thisR91 > thisR92) invMass_eta1_r9dist->Fill(thisR92, thisR91);
                        else invMass_eta1_r9dist->Fill(thisR91, thisR92);
                    }
                    if(1.2 < fabs(eta[0]) && fabs(eta[0]) < 1.4442 && 1.2 < fabs(eta[1]) && fabs(eta[1]) < 1.4442){
                        if(thisR91 > thisR92) invMass_eta2_r9dist->Fill(thisR92, thisR91);
                        else invMass_eta2_r9dist->Fill(thisR91, thisR92);
                    }
                    if(1.566 < fabs(eta[0]) && fabs(eta[0]) < 2 && 1.566 <  fabs(eta[1]) && fabs(eta[1]) < 2){
                        if(thisR91 > thisR92) invMass_eta3_r9dist->Fill(thisR92, thisR91);
                        else invMass_eta3_r9dist->Fill(thisR91, thisR92);
                    }
                    if(2 < fabs(eta[0]) && fabs(eta[0]) < 2.5 && 2 < fabs(eta[1]) && fabs(eta[1]) < 2.5){
                        if(thisR91 > thisR92) invMass_eta4_r9dist->Fill(thisR92, thisR91);
                        else invMass_eta4_r9dist->Fill(thisR91, thisR92);
                    }

                    leading_pt->Fill(e_lead.Pt(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    subleading_pt->Fill(e_sub.Pt(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    float lead_pt = energy_ele[0]*correction_lead/TMath::CosH(eta[0]);
                    if( lead_pt > 40 && lead_pt < 55 ){
                        pt_40_55->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        pt_40_55_up->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        pt_40_55_down->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( lead_pt > 55 && lead_pt < 65 ){
                        pt_55_65->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        pt_55_65_up->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        pt_55_65_down->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            float sub_pt = e_sub.Pt();
                        if( EB1 && EB2 ){
                            ptDiag_55_65->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            ptDiag_55_65_up->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            ptDiag_55_65_down->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            if(sub_pt < 40) pt_55_65_0_40->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            if(sub_pt > 40 && sub_pt < 55) pt_55_65_40_55->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            if(sub_pt > 55 && sub_pt < 65) pt_55_65_55_65->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            if(sub_pt > 65 && sub_pt < 100) pt_55_65_65_100->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            if(sub_pt > 100) pt_55_65_100_1000->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        etaDist_pt->Fill(fabs(eta[0]), fabs(eta[1]));
                        r9Dist_pt->Fill(thisR91, thisR92);
                    invMass_55_65_coarseEta->Fill(fabs(eta[0]), fabs(eta[1]), invMass_ele[0]*correction,  TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(lowR9) invMass_55_65_coarseEtaLow->Fill(fabs(eta[0]), fabs(eta[1]), invMass_ele[0]*correction,  TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(highR9) invMass_55_65_coarseEtaHigh->Fill(fabs(eta[0]), fabs(eta[1]), invMass_ele[0]*correction,  TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( lead_pt > 65 && lead_pt < 90 ){
                        pt_65_90->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        pt_65_90_up->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        pt_65_90_down->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( lead_pt > 90 ){
                        pt_90_inf->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        pt_90_inf_up->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        pt_90_inf_down->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }

                    if( (EB_IN1 && EB_OUT2) || (EB_OUT1 && EB_IN2) ) invMass_barrelOffDiag->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if( (EE_IN1 && EE_OUT2) || (EE_OUT1 && EE_IN2) ) invMass_endcapOffDiag->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));

                    if( EB1 && EB2){
                        invMass_EBEB->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_EBEB->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_EBEB->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        if(lowR9) invMass_EBEB_low->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        if(highR9) invMass_EBEB_high->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        if(z.Pt() >= 200){
                            invMass_EbEb_ptz200->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            invMassUp_EbEb_ptz200->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            invMassDown_EbEb_ptz200->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                    }
                    if( EE1 && EE2){
                        invMass_EEEE->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_EEEE->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_EEEE->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        if(lowR9) invMass_EEEE_low->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        if(highR9) invMass_EEEE_high->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        if(z.Pt() >= 200){
                            invMass_EeEe_ptz200->Fill(invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            invMassUp_EeEe_ptz200->Fill(invMass_ele[0]*correction_up, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                            invMassDown_EeEe_ptz200->Fill(invMass_ele[0]*correction_down, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                    }

                    //determine if the category is diagonal based on fineEtaBins
                    int eta0 = -999;
                    int eta1 = -999;
                    for( int bin = 0; bin < 13; bin++){
                        if(fabs(eta[0]) > fineEtaBins[bin] && fabs(eta[0]) < fineEtaBins[bin+1]) eta0 = bin*TMath::Sign(1,eta[0]);
                        if(fabs(eta[1]) > fineEtaBins[bin] && fabs(eta[1]) < fineEtaBins[bin+1]) eta1 = bin*TMath::Sign(1,eta[1]);
                    }

                    invMassVsNPU->Fill(thisNPU, invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));

                    if(eta0 != -1 && eta0 == eta1 ){
                        invMass_fineEta->Fill(fabs(eta[0]), invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        if( lowR9 ) invMass_fineEtaLow->Fill(fabs(eta[0]), invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        if( highR9 ) invMass_fineEtaHigh->Fill(fabs(eta[0]), invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }

                    if(lowR91 && EB_IN1) et_EBin_low->Fill(e_lead.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(lowR92 && EB_IN2) et_EBin_low->Fill(e_sub.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(highR91 && EB_IN1) et_EBin_high->Fill(e_lead.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(highR92 && EB_IN2) et_EBin_high->Fill(e_sub.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(lowR91 && EE_IN1) et_EEin_low->Fill(e_lead.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(lowR92 && EE_IN2) et_EEin_low->Fill(e_sub.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(highR91 && EE_IN1) et_EEin_high->Fill(e_lead.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(highR92 && EE_IN2) et_EEin_high->Fill(e_sub.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(lowR91 && EB_OUT1) et_EBout_low->Fill(e_lead.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(lowR92 && EB_OUT2) et_EBout_low->Fill(e_sub.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(highR91 && EB_OUT1) et_EBout_high->Fill(e_lead.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(highR92 && EB_OUT2) et_EBout_high->Fill(e_sub.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(lowR91 && EE_OUT1) et_EEout_low->Fill(e_lead.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(lowR92 && EE_OUT2) et_EEout_low->Fill(e_sub.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(highR91 && EE_OUT1) et_EEout_high->Fill(e_lead.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if(highR92 && EE_OUT2) et_EEout_high->Fill(e_sub.Et(), TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));

                    if(lowR9){
                        mee_npu_rawSC_low->Fill(thisNPU, invMass_raw[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        mee_npu_ele_low->Fill(thisNPU, invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if(highR9){
                        mee_npu_rawSC_high->Fill(thisNPU, invMass_raw[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        mee_npu_ele_high->Fill(thisNPU, invMass_ele[0]*correction, TMath::Sign(1,mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }


                    r9Dist->Fill(thisR91);
                    r9Dist->Fill(thisR92);
                    if(EB1) r9Dist_EB->Fill(thisR91);
                    if(EB2) r9Dist_EB->Fill(thisR92);
                    if(EE1) r9Dist_EE->Fill(thisR91);
                    if(EE2) r9Dist_EE->Fill(thisR92);
                   /* 
                    r9Dist->Fill(thisR91*energyRaw[0]/energy5x5[0]);
                    r9Dist->Fill(thisR92*energyRaw[1]/energy5x5[1]);
                    if(EB1) r9Dist_EB->Fill(thisR91*energyRaw[0]/energy5x5[0]);
                    if(EB2) r9Dist_EB->Fill(thisR92*energyRaw[1]/energy5x5[1]);
                    if(EE1) r9Dist_EE->Fill(thisR91*energyRaw[0]/energy5x5[0]);
                    if(EE2) r9Dist_EE->Fill(thisR92*energyRaw[1]/energy5x5[1]);
                    */

                    invMass->Fill( invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    if( ((EB1 && EE2) || (EE1 && EB2))){
                        if(highR9){
                        invMass_ele_NotEbEb_high->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_NotEbEb_high->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_NotEbEb_high->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        }
                        invMass_ele_NotEbEb->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_NotEbEb->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_NotEbEb->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( highR9 && (EB1 && EB2)){
                        invMass_ele_EbEb_high->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EbEb_high->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EbEb_high->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }

                    if( EB_IN && lowR9){
                        invMass_ele_EB_in_Low->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EB_in_Low->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EB_in_Low->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( EB_OUT && lowR9){
                        invMass_ele_EB_out_Low->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EB_out_Low->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EB_out_Low->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    } 
                    if( EE_IN && lowR9){
                        invMass_ele_EE_in_Low->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EE_in_Low->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EE_in_Low->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( EE_OUT && lowR9){
                        invMass_ele_EE_out_Low->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EE_out_Low->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EE_out_Low->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    } 
                    if( EB_IN && highR9){
                        invMass_ele_EB_in_High->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EB_in_High->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EB_in_High->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( EB_OUT && highR9){
                        invMass_ele_EB_out_High->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EB_out_High->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EB_out_High->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    } 
                    if( EE_IN && highR9){
                        invMass_ele_EE_in_High->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EE_in_High->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EE_in_High->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    }
                    if( EE_OUT && highR9){
                        invMass_ele_EE_out_High->Fill(invMass_ele[0]*correction, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassUp_ele_EE_out_High->Fill(invMass_ele[0]*correction_up, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                        invMassDown_ele_EE_out_High->Fill(invMass_ele[0]*correction_down, TMath::Sign(1, mcWeight)*puWeights->GetBinContent(puWeights->FindBin(thisNPU)));
                    } 
                }
            }
        }
    }//end for entries in the tree
    delete transformR9_EB;
    delete transformR9_EE;

    in.close();

    string prefix = "/afs/cern.ch/work/n/nschroed/ECALELF_DATA/rootFiles/scaleSmearHists_";
    string fileOut = "";

    fileOut = prefix+output+"_"+type+".root";
    cout << info << "root file written to: " << fileOut << endl;
    system(std::string("echo "+fileOut+" >> "+temp_output+".dat").c_str());
    TFile * rootOut = new TFile(fileOut.c_str(), "RECREATE");

    mee_vs_pt->Write();
    meeUp_vs_pt->Write();
    meeDown_vs_pt->Write();
    fine_grain_linearity->Write();
    invMassVsNPU->Write();
    mee_npu_rawSC_low->Write();
    mee_npu_rawSC_high->Write();
    mee_npu_ele_low->Write();
    mee_npu_ele_high->Write();
    invMass->Write();
    r9Dist->Write();
    r9Dist_EB->Write();
    r9Dist_EE->Write();
    invMass_gain6_EB->Write();
    invMassUp_gain6_EB->Write();
    invMassDown_gain6_EB->Write();
    invMass_gain6_EE->Write();
    invMassUp_gain6_EE->Write();
    invMassDown_gain6_EE->Write();
    invMass_gain1_EB->Write();
    invMassUp_gain1_EB->Write();
    invMassDown_gain1_EB->Write();
    invMass_gain1_EE->Write();
    invMassUp_gain1_EE->Write();
    invMassDown_gain1_EE->Write();
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
    invMass_ele_NotEbEb->Write();
    invMassUp_ele_NotEbEb->Write();
    invMassDown_ele_NotEbEb->Write();
    invMass_ele_EbEb_high->Write();
    invMassUp_ele_EbEb_high->Write();
    invMassDown_ele_EbEb_high->Write();
    invMassUp_ele_EB_in_Low->Write();
    invMassUp_ele_EB_out_Low->Write();
    invMassUp_ele_EE_in_Low->Write();
    invMassUp_ele_EE_out_Low->Write();
    invMassUp_ele_EB_in_High->Write();
    invMassUp_ele_EB_out_High->Write();
    invMassUp_ele_EE_in_High->Write();
    invMassUp_ele_EE_out_High->Write();
    invMassDown_ele_EB_in_Low->Write();
    invMassDown_ele_EB_out_Low->Write();
    invMassDown_ele_EE_in_Low->Write();
    invMassDown_ele_EE_out_Low->Write();
    invMassDown_ele_EB_in_High->Write();
    invMassDown_ele_EB_out_High->Write();
    invMassDown_ele_EE_in_High->Write();
    invMassDown_ele_EE_out_High->Write();
    invMass_EBEB->Write();
    invMassUp_EBEB->Write();
    invMassDown_EBEB->Write();
    invMass_EBEB_low->Write();
    invMass_EBEB_high->Write();
    invMass_EEEE->Write();
    invMassUp_EEEE->Write();
    invMassDown_EEEE->Write();
    invMass_EEEE_low->Write();
    invMass_EEEE_high->Write();
    leading_pt->Write();
    subleading_pt->Write();
    pt_40_55->Write();
    pt_40_55_up->Write();
    pt_40_55_down->Write();
    pt_55_65->Write();
    pt_55_65_up->Write();
    pt_55_65_down->Write();
    ptDiag_55_65->Write();
    ptDiag_55_65_up->Write();
    ptDiag_55_65_down->Write();
    pt_65_90->Write();
    pt_65_90_up->Write();
    pt_65_90_down->Write();
    pt_90_inf->Write();
    pt_90_inf_up->Write();
    pt_90_inf_down->Write();
    pt_55_65_0_40->Write();
    pt_55_65_40_55->Write();
    pt_55_65_55_65->Write();
    pt_55_65_65_100->Write();
    pt_55_65_100_1000->Write();
    etaDist_pt->Write();
    r9Dist_pt->Write();
    et_EBin_low->Write();
    et_EBin_high->Write();
    et_EEin_low->Write();
    et_EEin_high->Write();
    et_EBout_low->Write();
    et_EBout_high->Write();
    et_EEout_low->Write();
    et_EEout_high->Write();
    invMass_fineEta->Write();
    invMass_fineEtaLow->Write();
    invMass_fineEtaHigh->Write();
    invMass_R9_96_99->Write();
    invMass_R9_99_100->Write();
    invMass_R9_99_1000->Write();
    mee_et->Write();
    mee_et_eb->Write();
    meeUp_et_eb->Write();
    meeDown_et_eb->Write();
    mee_et_ee->Write();
    meeUp_et_ee->Write();
    meeDown_et_ee->Write();
    mee_et_stats->Write();
    invMass_barrelOffDiag->Write();
    invMass_endcapOffDiag->Write();
    invMass_coarseEta->Write();
    invMass_coarseEtaLow->Write();
    invMass_coarseEtaHigh->Write();
    invMass_55_65_coarseEta->Write();
    invMass_55_65_coarseEtaLow->Write();
    invMass_55_65_coarseEtaHigh->Write();
    invMass_eta0_r9dist->Write();
    invMass_eta1_r9dist->Write();
    invMass_eta2_r9dist->Write();
    invMass_eta3_r9dist->Write();
    invMass_eta4_r9dist->Write();
    et_stats0->Write();
    et_stats1->Write();
    et_stats2->Write();
    et_stats3->Write();
    et_stats4->Write();
    mee_fine_bins->Write();
    invMass_run2018_EBin->Write();
    invMass_run2018_EBout->Write();
    invMass_run2018_EEin->Write();
    invMass_run2018_EEout->Write();
    invMass_EbEb_ptz200->Write();
    invMassUp_EbEb_ptz200->Write();
    invMassDown_EbEb_ptz200->Write();
    invMass_EeEe_ptz200->Write();
    invMassUp_EeEe_ptz200->Write();
    invMassDown_EeEe_ptz200->Write();

    rootOut->Close();

    delete invMass_EbEb_ptz200;
    delete invMassUp_EbEb_ptz200;
    delete invMassDown_EbEb_ptz200;
    delete invMass_EeEe_ptz200;
    delete invMassUp_EeEe_ptz200;
    delete invMassDown_EeEe_ptz200;
    delete mee_vs_pt;
    delete meeUp_vs_pt;
    delete meeDown_vs_pt;
    delete fine_grain_linearity;
    delete mee_fine_bins;
    delete invMassVsNPU;
    delete et_stats0;
    delete et_stats1;
    delete et_stats2;
    delete et_stats3;
    delete et_stats4;
    delete mee_npu_rawSC_low;
    delete mee_npu_rawSC_high;
    delete mee_npu_ele_low;
    delete mee_npu_ele_high;
    delete invMass;
    delete r9Dist;
    delete r9Dist_EB;
    delete r9Dist_EE;
    delete invMass_ele_EB_in_Low;
    delete invMass_ele_EB_out_Low;
    delete invMass_ele_EE_in_Low;
    delete invMass_ele_EE_out_Low;
    delete invMass_ele_EB_in_High;
    delete invMass_ele_EB_out_High;
    delete invMass_ele_EE_in_High;
    delete invMass_ele_EE_out_High;
    delete invMassUp_ele_EB_in_Low;
    delete invMassUp_ele_EB_out_Low;
    delete invMassUp_ele_EE_in_Low;
    delete invMassUp_ele_EE_out_Low;
    delete invMassUp_ele_EB_in_High;
    delete invMassUp_ele_EB_out_High;
    delete invMassUp_ele_EE_in_High;
    delete invMassUp_ele_EE_out_High;
    delete invMassDown_ele_EB_in_Low;
    delete invMassDown_ele_EB_out_Low;
    delete invMassDown_ele_EE_in_Low;
    delete invMassDown_ele_EE_out_Low;
    delete invMassDown_ele_EB_in_High;
    delete invMassDown_ele_EB_out_High;
    delete invMassDown_ele_EE_in_High;
    delete invMassDown_ele_EE_out_High;
    delete invMass_EBEB;
    delete invMassUp_EBEB;
    delete invMassDown_EBEB;
    delete invMass_EBEB_low;
    delete invMass_EBEB_high;
    delete invMass_EEEE;
    delete invMassUp_EEEE;
    delete invMassDown_EEEE;
    delete invMass_EEEE_low;
    delete invMass_EEEE_high;
    delete puWeights;
    delete leading_pt;
    delete subleading_pt;
    delete pt_40_55;
    delete pt_40_55_up;
    delete pt_40_55_down;
    delete pt_55_65;
    delete pt_55_65_up;
    delete pt_55_65_down;
    delete pt_55_65_0_40;
    delete pt_55_65_40_55;
    delete pt_55_65_55_65;
    delete pt_55_65_65_100;
    delete pt_55_65_100_1000;
    delete ptDiag_55_65;
    delete ptDiag_55_65_up;
    delete ptDiag_55_65_down;
    delete pt_65_90;
    delete pt_65_90_up;
    delete pt_65_90_down;
    delete pt_90_inf;
    delete pt_90_inf_up;
    delete pt_90_inf_down;
    delete etaDist_pt;
    delete r9Dist_pt;
    delete invMass_fineEta;
    delete invMass_fineEtaLow;
    delete invMass_fineEtaHigh;
    delete et_EBin_low;
    delete et_EBin_high;
    delete et_EEin_low;
    delete et_EEin_high;
    delete et_EBout_low;
    delete et_EBout_high;
    delete et_EEout_low;
    delete et_EEout_high;
    delete invMass_ele_NotEbEb_high;
    delete invMassUp_ele_NotEbEb_high;
    delete invMassDown_ele_NotEbEb_high;
    delete invMass_ele_NotEbEb;
    delete invMassUp_ele_NotEbEb;
    delete invMassDown_ele_NotEbEb;
    delete invMass_ele_EbEb_high;
    delete invMassUp_ele_EbEb_high;
    delete invMassDown_ele_EbEb_high;
    delete invMass_R9_96_99;
    delete invMass_R9_99_100;
    delete invMass_R9_99_1000;
    delete mee_et;
    delete mee_et_eb;
    delete meeUp_et_eb;
    delete meeDown_et_eb;
    delete mee_et_ee;
    delete meeUp_et_ee;
    delete meeDown_et_ee;
    delete mee_et_stats;
    delete invMass_barrelOffDiag;
    delete invMass_endcapOffDiag;
    delete invMass_coarseEta;
    delete invMass_coarseEtaLow;
    delete invMass_coarseEtaHigh;
    delete invMass_55_65_coarseEta;
    delete invMass_55_65_coarseEtaLow;
    delete invMass_55_65_coarseEtaHigh;
    delete invMass_eta0_r9dist;
    delete invMass_eta1_r9dist;
    delete invMass_eta2_r9dist;
    delete invMass_eta3_r9dist;
    delete invMass_eta4_r9dist;
    delete invMass_gain6_EB;
    delete invMassUp_gain6_EB;
    delete invMassDown_gain6_EB;
    delete invMass_gain6_EE;
    delete invMassUp_gain6_EE;
    delete invMassDown_gain6_EE;
    delete invMass_gain1_EB;
    delete invMassUp_gain1_EB;
    delete invMassDown_gain1_EB;
    delete invMass_gain1_EE;
    delete invMassUp_gain1_EE;
    delete invMassDown_gain1_EE;
    delete invMass_run2018_EBin;
    delete invMass_run2018_EBout;
    delete invMass_run2018_EEin;
    delete invMass_run2018_EEout;

    return;
}

