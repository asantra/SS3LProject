#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "TF1.h"
#include "TFile.h"
#include "TH1F.h"
#include "TKey.h"
#include "TTree.h"
#include "TLorentzVector.h"


#include <ROOT/TDataFrame.hxx>

#include <ROOT/RDataFrame.hxx>
using TDataFrame = ROOT::RDataFrame;

using namespace std;

void Export(string in_file_name, string out_file_name) {



  string tree_name = "HFntupleNONE";
  //string tree_name = "DiLeptonTree_SRall";

  std::cout << "File : " << in_file_name << std::endl;
  std::cout << "tree name : " << tree_name << std::endl;

  TFile *f_in = new TFile(in_file_name.c_str());

  if (f_in->IsZombie()) {
    cout << "file failed " << endl;
    return;
  }

  TDataFrame df(tree_name.c_str(), f_in);


  auto colNames = df.GetColumnNames();

  //std::vector<std::string> allCols;

    //for (auto &&colName : colNames) {
    //  std::cout << colName << std::endl;
    //  if (colName.find(".") == std::string::npos)
    //    allCols.push_back(colName);
    //}

    auto compute_jet_pt = [](const std::vector<TLorentzVector> & jetTLVs ) {
      std::vector<float> jetpts;
      for ( auto& tlv : jetTLVs){
        jetpts.push_back(tlv.Pt());
      }
      return jetpts;
    };

    auto compute_sum_pt = [](const std::vector<TLorentzVector> & TLVs ) {
      float sumpts = 0;
      for (auto & tlv : TLVs){
        sumpts+=tlv.Pt();
      }
      return sumpts;
    };

    auto compute_sum_bjet_pt = [](const std::vector<TLorentzVector> & TLVs, const std::vector<float> & tags ) {
      float sumbjpts = 0;
      for (int i=0; i<TLVs.size(); i++){
        if ( (bool) tags.at(i)) {
        sumbjpts+=TLVs.at(i).Pt();
        }
      }
      return sumbjpts;
    };

    auto compute_deltaR = [](const std::vector<TLorentzVector> & tlvs ) {
      float min_DeltaR = 999;
      for(auto & l: tlvs){
        for(auto & r: tlvs){
          if(&l == &r){
            continue;
          }
          float tmp = l.DeltaR(r);
          if(tmp < min_DeltaR){
            min_DeltaR = tmp;
          }
        }
      }
      return min_DeltaR;
    };

    auto compute_deltaR_diff = [](const std::vector<TLorentzVector> & left, const std::vector<TLorentzVector> & right ) {
      float min_DeltaR = 999;
      for(auto & l: left){
        for(auto & r: right){
          float tmp = l.DeltaR(r);
          if(tmp < min_DeltaR){
            min_DeltaR = tmp;
          }
        }
      }
      return min_DeltaR;
    };

    auto compute_mee = [](const std::vector<TLorentzVector> & tlvs, const std::vector<float> & charges ) {
      float mass = 0;
      bool found = false;
      for(size_t i=0;i<tlvs.size();i++){
        for(size_t j=i+1;j<tlvs.size();j++){
          if(charges.at(i)*charges.at(j)>0){
            found = true;
            mass = (tlvs.at(i)+tlvs.at(j)).M();
            break;
          }
          if(found) break;
        } 
      }
      return mass;
    };

    auto compute_3L = [](const std::vector<TLorentzVector> & tlvs, const std::vector<float> & charges ) {
      bool foundSS3L = false;
      if(tlvs.size()>2){
        for(size_t i=0;i<tlvs.size();i++){
          for(size_t j=i;j<tlvs.size();j++){
            for(size_t k=i;k<tlvs.size();k++){
              if(!(i==j || i==k || j==k)){
                if(charges.at(i)==charges.at(j) && charges.at(i)==charges.at(k)){
                  foundSS3L = true;
                  break;
                }
              }
              if(foundSS3L) break;
            }
            if(foundSS3L) break;
          }
        }
      }
      return foundSS3L;
    };


    // Skimming
    std::vector<std::string> allCols = {"pass2lcut","pass3lcut", "pass2lossf", "passHiggs", "num_bjets", "nJets", "eT_miss", "JetPt", "mll3lZ", "mll3lZmin", "mTWZ", "mTWZmin",
      "Pass2ElTrigger", "Pass2MuTrigger", "PassElMuTrigger", "Pass1ElTrigger", "Pass1MuTrigger",
      "m_3lepweight","m_3lepweightUnc","EventNumber","LepPdgId","LepPt_x","LepPt_y","mlll","mll2l","isOS","isSS","TreatAsYear",
      "LepPt", "mlll", "elecSF", "muonSF", "EventWeight", "jvtSF", "pileupweight",
      "mll3lmin", "mll3lmax", "LepPhi", "LepCharge", "LepEta", "JetEta", "RunNumber", "year",
      "mll","nEl","nMu","JetPhi","numVtx","averageIntPerXing","lbn","MET_px","MET_py","LepM","isEM","isEE","isMM","mt2"};

    df.Snapshot(tree_name.c_str(), out_file_name.c_str(), allCols);

    // Adding Variables
    //std::vector<std::string> allCols = {"el_n","mu_n","Nbjet","mu_n","Njet25","Njet35","Njet40","Njet50","EtMiss","Meff","Zevent","lep_1stPt","lep_2ndPt","lep_3rdPt","elSF","muSF","MCWeight","XSecWeight_N","XSecWeight_SW","mcWeightVariations","TotalWeight"};
    //allCols.push_back("JetPt");
    //allCols.push_back("SumJetPt");
    //allCols.push_back("SumBJetPt");
    //allCols.push_back("SumLepPt");
    //allCols.push_back("DeltaRll");
    //allCols.push_back("DeltaRlj");
    //allCols.push_back("Mee");
    //allCols.push_back("SS3L");


    //df.Define("JetPt", compute_jet_pt, {"jetTLV"})
    //.Define("SumJetPt", compute_sum_pt, {"jetTLV"})
    //.Define("SumBJetPt", compute_sum_bjet_pt, {"jetTLV","jetBtag"})
    //.Define("SumLepPt", compute_sum_pt, {"lepTLV"})
    //.Define("DeltaRll", compute_deltaR, {"lepTLV"})
    //.Define("DeltaRlj", compute_deltaR_diff, {"lepTLV", "jetTLV"})
    //.Define("Mee", compute_mee, {"elTLV", "elCharges"})
    //.Define("SS3L", compute_3L, {"lepTLV", "lepCharges"})
    //.Snapshot(tree_name.c_str(), out_file_name.c_str(), allCols);


}

void ExportMyTree(string tmp_file) {
  ifstream in0;
  std::cout << "tmp_file is : " << tmp_file << std::endl;
  in0.open(tmp_file.c_str());
  string in_file, out_file;

  while (true) {
    in0 >> in_file >> out_file;
    if (!in0.good())
      break;
    Export(in_file, out_file);
  }
  in0.close();
}

int main(int argc, char *argv[]) {

  Export(argv[1], argv[2]);

  return 0;
}
