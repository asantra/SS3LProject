#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "TSystem.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraphAsymmErrors.h"
#include "TString.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TLatex.h"
#include "TFile.h"
#include "TLegend.h"
#include "TStyle.h"
#include "THStack.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TROOT.h"
#include <iomanip>
#include "RooStats/NumberCountingUtils.h"
#include "RooGlobalFunc.h"
#include "RooStats/RooStatsUtils.h"

using namespace std;

class AnaTree
{
 public:
  AnaTree(std::string name="myAnaTree"){
    std::cout<< "\n :: Starting a new AnaTree :: "<<endl;
    TreeName = "DiLeptonTree_SRall_B";
    filename = "";
    path = "";
    xsec_filename = "susy_crosssections_13TeV.txt";
    SR = "SR1b";
    Var_cut = 0;
    MET_cut = 0;
    pt_change = 0;
    DEBUG=0;
  };
  ~AnaTree(){};

 public:

  void getSystVariation(TH1D* h1, TH1D *h2, int n_syst_var);
  void getTotalSystErr();
  void assignSystErr(TH1D *h1);
  void getSystList(TString file);
  void SetTreeName(TString _TreeName);
  void GetTLVleptons(vector<TLorentzVector> list_lep);
  void SetDebugMode();
  void GetJets(vector<TLorentzVector> list_jet);
  void GetPtDistribution(TString type,TH1D *myHist,int selection);
  void SetPtCut(double _pt_cut);
  void GetHisto(TString VarName, TH1D *myHist);
  void GetHistoRaw(TString VarName, TH1D *myHist);  
  void SetFileName(TString _name);
  void SetPath(TString _path);
  void SetHistPath(TString _histpath);
  float WeightedEvts();
  double GetXsec();
  void RescaleLumi(TH1D *hist, float lumi,int type);
  void GetFilename(TString file);
  void ClearFileList();
  void SelectJets(vector<TLorentzVector> jets, int pt_cut);
  int SR_jetscuts(vector<TLorentzVector> jets, int num_bjets);
  int EventSelector(float Meff, float MET, int njets, int num_bjets);
  void SetSR(TString _SR);
  double BinomialP(double mainObs, double backgroundObs, double relativeBkgUncert);
  double BinomialZ(double mainObs, double backgroundObs, double relativeBkgUncert);
  void SetKinCuts(double _Varcut, double _METcut);
  void setGridValues(TH2D *grid, int x_bin, vector<vector<double> > values, vector<vector<double> > err_values);
  void DrawGrid(TH2D *grid, TCanvas *canv_grid, vector<TString> label_x, vector<TString> label_y, double cont_min, double cont_max,TString title);
  void ReturnGoodJets(vector<TLorentzVector> *new_vect);

  TString path;
  TString histpath;
  TString TreeName;
  TString filename;
  TString SR;
  vector<TString> filelist;
  vector<TLorentzVector> good_jets;
  vector<double> pt_jets;
  vector<double> pt_leptons;
  vector<double> eta_leptons;
  vector<int> sample_type;
  vector<TString> syst_list;
  vector<vector<double> > syst_variation;
  vector<double> total_syst;

  double Var_cut;
  double MET_cut;
  bool pt_change;
  double pt_cut;
  bool DEBUG;

 private:

  TString xsec_filename;

};
