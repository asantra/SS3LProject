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

class DiLeptonPlotter
{
 public:
  DiLeptonPlotter(std::string name="DiLeptonPlotter"){
    CNAME = name;
    std::cout<< "\n -- Starting a new DiLeptonPlotter :: " << CNAME << std::endl;
    Debug = false;
    Print = false;
    data  = false;
    lumi  = 1.;
    scale = 1.;
    SR      = "SRall";
    pOpt    = "A";
    inpath  = "";
    outpath = "";
    BkgFiles.clear();
    BkgNames.clear();
    SigFiles.clear();
    SigNames.clear();
  };
  ~DiLeptonPlotter(){};


 public:
  void doDebug(bool debug);
  void doPrint(bool print);  
  void setSR(TString SRname);
  void setBkgFile(TString name, TString label);
  void setSigFile(TString name, TString label);
  void setPairing(TString opt);
  void setInDir(TString indir);
  void setOutDir(TString outdir);
  void setLumiScale(double LumiScale, double HistLumi=1.0);
  void useData(bool usedata);
  void setColor(TH1F* h, int i, TString option);
  bool check(TFile *f);

  void plot(TString name, float xMin, float xMax, float yMin, int rebin=1, bool log=true);

 private:
  std::string CNAME;
  bool  Debug;
  bool  Print;
  bool  data;
  float scale;
  float lumi;

  TString SR;
  TString pOpt;
  TString inpath;
  TString outpath;

  TFile *fDATA;
  TH1F  *hDATA;

  std::vector<TFile*>  BkgFiles;
  std::vector<TFile*>  SigFiles;
  std::vector<TString> BkgNames;
  std::vector<TString> SigNames;
 


};
