#ifndef DiLeptonTree_SRall_h
#define DiLeptonTree_SRall_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include "TLorentzVector.h"

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <vector>
#include <TString.h>
#include <fstream>

// Fixed size dimensions of array or collections stored in the TTree if any.

class DiLeptonTree_SRall : public TSelector {
 public :
  TTree          *fChain;   //!pointer to the analyzed TTree or TChain

  bool debug;
  bool showPDF;
  bool GlFilter;

  float lumi;
  float NTotal;
  float xSec;

  int   RAW;
  float sumW;
  float sumW_pdfUp;
  float sumW_pdfDown;

  ofstream outfile;

  float  meffcut;
  float  metcut;
  int    njets;
  int    nbjets;
  int    nbmax;
  int    nlep;

  double getNorm(double v);


  // Declaration of leaf types
  Long64_t        EventNumber;
  Long64_t        RunNumber;
  Float_t         MCWeight;
  Float_t         PileUpWeight;
  Float_t         ObjectSF;
  Float_t         TotalWeight;
  Float_t         EtMiss;
  Float_t         Meff;
  Float_t         Minv;
  Float_t         Mt;
  Float_t         Ht;
  Float_t         Mjj;
  Float_t         TruthHt;
  Bool_t          Zevent;
  Int_t           NlepBL;
  Int_t           Njet25;
  Int_t           Njet40;
  Int_t           Njet50;
  Int_t           Nbjet;
  Int_t           GLDec1;
  Int_t           GLDec2;
  vector<TLorentzVector> *lepTLV;
  Int_t           SSChannel;
  vector<string>  *SysNames;
  vector<float>   *SysWeights;
  vector<float>   *PDFWeights;
  Int_t           IntPerX;
  TString         *topoStr;
  vector<TLorentzVector> *muTLV;
  vector<TLorentzVector> *elTLV;
  vector<TLorentzVector> *jetTLV;
  vector<float>   *muCharges;
  vector<float>   *elCharges;
  vector<float>   *lepCharges;
  vector<float>   *muSF;
  vector<float>   *elSF;
  vector<float>   *jetMV2;
  vector<float>   *jetJVT;
  vector<string>  *triggerInfo;

  // List of branches
  TBranch        *b_EventNumber;   //!
  TBranch        *b_RunNumber;   //!
  TBranch        *b_MCWeight;   //!
  TBranch        *b_PileUpWeight;   //!
  TBranch        *b_ObjectSF;   //!
  TBranch        *b_TotalWeight;   //!
  TBranch        *b_EtMiss;   //!
  TBranch        *b_Meff;   //!
  TBranch        *b_Minv;   //!
  TBranch        *b_Mt;   //!
  TBranch        *b_Ht;   //!
  TBranch        *b_Mjj;   //!
  TBranch        *b_TruthHt;   //!
  TBranch        *b_Zevent;   //!
  TBranch        *b_NlepBL;   //!
  TBranch        *b_Njet25;   //!
  TBranch        *b_Njet40;   //!
  TBranch        *b_Njet50;   //!
  TBranch        *b_Nbjet;   //!
  TBranch        *b_GLDec1; //!
  TBranch        *b_GLDec2; //!
  TBranch        *b_lepTLV;   //!
  TBranch        *b_SSChannel;   //!
  TBranch        *b_SysNames;   //!
  TBranch        *b_SysWeights;   //!
  TBranch        *b_PDFWeights;   //!
  TBranch        *b_IntPerX;   //!
  TBranch        *b_topoStr;   //!
  TBranch        *b_muTLV;   //!
  TBranch        *b_elTLV;   //!
  TBranch        *b_jetTLV;   //!
  TBranch        *b_muCharges;   //!
  TBranch        *b_elCharges;   //!
  TBranch        *b_lepCharges;   //!
  TBranch        *b_muSF;   //!
  TBranch        *b_elSF;   //!
  TBranch        *b_jetMV2;   //!
  TBranch        *b_jetJVT;   //!
  TBranch        *b_triggerInfo;   //!

 DiLeptonTree_SRall(TTree * /*tree*/ =0) : fChain(0) { }
  virtual ~DiLeptonTree_SRall() { }
  virtual Int_t   Version() const { return 2; }
  virtual void    Begin(TTree *tree);
  virtual void    SlaveBegin(TTree *tree);
  virtual void    Init(TTree *tree);
  virtual Bool_t  Notify();
  virtual Bool_t  Process(Long64_t entry);
  virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
  virtual void    SetOption(const char *option) { fOption = option; }
  virtual void    SetObject(TObject *obj) { fObject = obj; }
  virtual void    SetInputList(TList *input) { fInput = input; }
  virtual TList  *GetOutputList() const { return fOutput; }
  virtual void    SlaveTerminate();
  virtual void    Terminate();

  ClassDef(DiLeptonTree_SRall,0);
};

#endif

#ifdef DiLeptonTree_SRall_cxx
void DiLeptonTree_SRall::Init(TTree *tree)
{
  bool fullTree = false;
  GlFilter = true;

  // Set object pointer
  lepTLV = 0;
  SysNames = 0;
  SysWeights = 0;
  PDFWeights = 0;
  topoStr = 0;
  muTLV = 0;
  elTLV = 0;
  jetTLV = 0;
  muCharges = 0;
  elCharges = 0;
  lepCharges = 0;
  muSF = 0;
  elSF = 0;
  jetMV2 = 0;
  jetJVT = 0;
  triggerInfo = 0;
  // Set branch addresses and branch pointers
  if (!tree) return;
  fChain = tree;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
  fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
  fChain->SetBranchAddress("MCWeight", &MCWeight, &b_MCWeight);
  fChain->SetBranchAddress("PileUpWeight", &PileUpWeight, &b_PileUpWeight);
  fChain->SetBranchAddress("ObjectSF", &ObjectSF, &b_ObjectSF);
  fChain->SetBranchAddress("TotalWeight", &TotalWeight, &b_TotalWeight);
  fChain->SetBranchAddress("EtMiss", &EtMiss, &b_EtMiss);
  fChain->SetBranchAddress("Meff", &Meff, &b_Meff);
  fChain->SetBranchAddress("Minv", &Minv, &b_Minv);
  fChain->SetBranchAddress("Mt", &Mt, &b_Mt);
  fChain->SetBranchAddress("Ht", &Ht, &b_Ht);
  fChain->SetBranchAddress("Mjj", &Mjj, &b_Mjj);
  fChain->SetBranchAddress("TruthHt", &TruthHt, &b_TruthHt);
  fChain->SetBranchAddress("Zevent", &Zevent, &b_Zevent);
  fChain->SetBranchAddress("NlepBL", &NlepBL, &b_NlepBL);
  fChain->SetBranchAddress("Njet25", &Njet25, &b_Njet25);
  fChain->SetBranchAddress("Njet40", &Njet40, &b_Njet40);
  fChain->SetBranchAddress("Njet50", &Njet50, &b_Njet50);
  fChain->SetBranchAddress("Nbjet", &Nbjet, &b_Nbjet);
  fChain->SetBranchAddress("lepTLV", &lepTLV, &b_lepTLV);
  fChain->SetBranchAddress("SSChannel", &SSChannel, &b_SSChannel);
  fChain->SetBranchAddress("SysNames", &SysNames, &b_SysNames);
  fChain->SetBranchAddress("SysWeights", &SysWeights, &b_SysWeights);
  fChain->SetBranchAddress("PDFWeights", &PDFWeights, &b_PDFWeights);

  if(GlFilter){
    fChain->SetBranchAddress("GLDec1", &GLDec1, &b_GLDec1);
    fChain->SetBranchAddress("GLDec2", &GLDec2, &b_GLDec2);
  }

  if(fullTree){
    fChain->SetBranchAddress("IntPerX", &IntPerX, &b_IntPerX);
    fChain->SetBranchAddress("topoStr", &topoStr, &b_topoStr);
    fChain->SetBranchAddress("muTLV", &muTLV, &b_muTLV);
    fChain->SetBranchAddress("elTLV", &elTLV, &b_elTLV);
    fChain->SetBranchAddress("jetTLV", &jetTLV, &b_jetTLV);
    fChain->SetBranchAddress("muCharges", &muCharges, &b_muCharges);
    fChain->SetBranchAddress("elCharges", &elCharges, &b_elCharges);
    fChain->SetBranchAddress("lepCharges", &lepCharges, &b_lepCharges);
    fChain->SetBranchAddress("muSF", &muSF, &b_muSF);
    fChain->SetBranchAddress("elSF", &elSF, &b_elSF);
    fChain->SetBranchAddress("jetMV2", &jetMV2, &b_jetMV2);
    fChain->SetBranchAddress("jetJVT", &jetJVT, &b_jetJVT);
    fChain->SetBranchAddress("triggerInfo", &triggerInfo, &b_triggerInfo);
  }
  
}

Bool_t DiLeptonTree_SRall::Notify()
{
  return kTRUE;
}

#endif // #ifdef DiLeptonTree_SRall_cxx
