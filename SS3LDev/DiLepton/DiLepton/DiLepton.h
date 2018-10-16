#ifndef DiLepton_DiLepton_H
#define DiLepton_DiLepton_H

//EDM includes
#include <EventLoop/Algorithm.h>
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/TActiveStore.h"
#include "SUSYTools/ISUSYObjDef_xAODTool.h"
#include "MCTruthClassifier/MCTruthClassifier.h"
#include "AsgTools/ToolHandle.h"

//xAOD Object includes
#ifndef __MAKECINT__
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#endif

//other includes
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <utility>
#include <memory>
#include <TMath.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TTree.h>
#include <TFile.h>
#include <TSystem.h>
#include <TLorentzVector.h>

namespace ST{ 
  class SUSYObjDef_xAOD;
}
using namespace ST;

namespace CP{
  class MuonTriggerScaleFactors;
  class IMuonTriggerScaleFactors;
}
using namespace CP;

class JetCleaningTool;
class GoodRunsListSelectionTool;
class MCTruthClassifier;
class JetReclusteringTool;
class AsgElectronChargeIDSelectorTool;

class AsgElectronEfficiencyCorrectionTool;
class IAsgElectronEfficiencyCorrectionTool;
class TrigGlobalEfficiencyCorrectionTool;
class ITrigGlobalEfficiencyCorrectionTool;

namespace LHAPDF{
  class PDFSet;
  class PDF;
}

class DiLepton : public EL::Algorithm
{
 
#ifndef __CINT__
  SUSYObjDef_xAOD *SusyObjTool;       //!
  GoodRunsListSelectionTool *GRLTool; //!
  JetReclusteringTool *JRTool;        //!
  MCTruthClassifier *MCClassifier;    //!
  AsgElectronChargeIDSelectorTool* CFTool;      //!
  AsgElectronChargeIDSelectorTool* CFToolLoose; //!
  TrigGlobalEfficiencyCorrectionTool* TriggerSFTool;     //!
  TrigGlobalEfficiencyCorrectionTool* TriggerSFToolUP;   //!
  TrigGlobalEfficiencyCorrectionTool* TriggerSFToolDOWN; //! 

  ToolHandleArray<IMuonTriggerScaleFactors> muonTools;  //! 
  ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronSFTools;  //!
  ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronEffTools; //!  

#endif 


 private:
  std::ofstream cutflow_file_trig; //!
  std::ofstream cutflow_file_baseline; //!
  std::ofstream cutflow_file_all; //!
  std::ofstream cutflow_file_SS; //!
  std::ofstream cutflow_file_emu; //!
  std::ofstream cutflow_file_ee; //!
  std::ofstream cutflow_file_mumu; //!

  Long_t    EventNumber; //!
  Long_t    RunNumber;   //!
  ULong64_t PileUpHash;  //!     

  float AverageMu; //!
  int   LB; //!
  int   MCId;        //!
  int   IntPerX;     //!
  int   NFiles;      //!
  int   randomRN;    //!
  int   configYear;  //!
  int   actualRN;  //!
  int   runNumber;  //!
  int   getRandomRN;  //!
  int   getRN;  //!
  int   eventrunNumber;  //!

  int PDGId1; //!
  int PDGId2; //!
  int GLDec1; //!
  int GLDec2; //! 

  unsigned int  NSys;  //!
  unsigned int  NSr;   //!

  int   xAODEventSum;  //!
  float xAODWeightSum; //!

  float  EventWeight;   //!
  float  mcweightvariations;   //!
  float  PileUpWeight;  //!
  float  EventWeightPileUp; //!
  float  ObjectSF;     //!
  float  chargeFlipSF; //!
  float  triggerSF;    //!
  float  trigSFerr;    //!
  float  weight;       //!

  std::vector<float> mcWeightVariations;  //!
  std::vector<float> SysWeights;  //!
  std::vector<float> PDFWeights;  //!
  std::vector<float> CFTWeights;  //!
  std::vector<float> TrigWeights; //!  
  std::vector<std::string> SysNames; //!  

  float  EtMiss;    //!
  float  EtMissPhi; //!
  float  meff;      //!
  float  minv;      //!
  float  mt;        //!
  float  ht;        //!
  float  mj;        //!
  float  mjj;       //!
  float  mjjdR;     //!
  float  mjRec;     //!
  float  truthHt;   //!
  float  truthMET;  //!

  float  bmu_1stPt; //!
  float  bmu_2ndPt; //!
  float  bmu_3rdPt; //!
  float  bmu_1stEta; //!
  float  bmu_2ndEta; //!

  float  bel_1stPt; //!
  float  bel_2ndPt; //!
  float  bel_3rdPt; //!
  float  bel_1stEta; //!
  float  bel_2ndEta; //!

  float  blep_1stPt; //!
  float  blep_2ndPt; //!
  float  blep_3rdPt; //!    

  float  mu_1stPt; //!
  float  mu_2ndPt; //!
  float  mu_3rdPt; //!
  float  mu_1stEta; //!
  float  mu_2ndEta; //!

  float  el_1stPt; //!
  float  el_2ndPt; //!
  float  el_3rdPt; //!
  float  el_1stEta; //!
  float  el_2ndEta; //!
  float  el_1stTEta; //!
  float  el_2ndTEta; //!

  float  lep_1stPt; //!
  float  lep_2ndPt; //!
  float  lep_3rdPt; //!    

  float  jet_1stPt; //!
  float  jet_2ndPt; //!
  float  jet_3rdPt; //!

  int     isZevent; //!
  TString topoStr;  //!

  int mu_n;  //!
  int el_n;  //!
  int lep_n; //!
  int jet_n; //!
  int b_n;   //!

  int NlepBL;    //!
  int NlepBLEta; //!
  int Njet25;    //!
  int Njet35;    //!
  int Njet40;    //!
  int Njet50;    //!
  int Nbjet;     //!
  int SSChannel; //!

  static const int NSR  = 20;  //!
  static const int NSYS = 60;  //!
 
  struct SR {
    TString  Name;
    int      Njet; 
    int      NBjet;
    int      NBmax;
    float    JetPt;
    float    EtMissCut;    
    float    MeffCut;   
    TString  Pairing;
  }; //!

  std::vector< SR > SignalRegions; //!
  
  void setROOTmsg(int level); //!
  void PrintSR(SR sr, const char* APP_NAME); //!
  bool addSR(TString name, int jetmin, int bmin, int bmax, float jetpt, float metmin, float meffmin, TString pairing); //!
  int getPairingOption(SR sr); //!

  bool containStr(std::vector<std::string> v, std::string name); //!
  std::vector<std::string> tokenize(std::string); //

  enum LeptonType {Loose, Medium, Tight}; //!
  
  bool isMC; //!
  void PrintCutflow(TH1* hCut); //!

  bool SSleptons(std::vector<TLorentzVector> lepTLV, std::vector<float> lepCharges); //!
  //bool SSleptons(DiLepton::SR sr, std::vector<TLorentzVector> lepTLV, std::vector<float> lepCharges); //!
  bool hasLeptons(std::vector<TLorentzVector> leptons); //!
  bool inCrackRegion(xAOD::Electron *el); //! 

  float   getPileupWeight();  //!
  ULong64_t getPileupHash();  //!
  
  bool InitializeTree();         //!
  bool InitializeHistograms();   //!
  bool InitializeTriggerTools(std::string var=""); //!

  std::map<std::string,std::string> LegsPerTool; //!

  float getMt(std::vector< TLorentzVector > leptons, float met, float met_phi); //!

  std::vector< std::string > triggerInfo; //!
  std::vector< std::string > triggerList2015; //!
  std::vector< std::string > triggerList2016; //!
  std::vector< std::string > triggerList2017; //!

  bool setTriggers(std::vector<std::string> &trigList, std::string trigname="", int option=2016); //!
  bool isTriggered(std::vector<std::string> trigList, std::vector<std::string> &trigInfo, bool enabled=1); //!
  bool isTriggeredMet(std::vector<std::string> trigList, float met, std::vector<std::string> trigInfo, float metcut=250000.);  //!
  bool isTriggeredMet2016(std::vector<std::string> trigList, float met, std::vector<std::string> trigInfo, int period, float metcut=250000.);  //!
  void setTrigItem(std::vector<std::string> &trigList, std::string trigItem, unsigned int it); //!  
  int  nMetTrigger(std::vector<std::string> trigInfo); //!
  int  nMetTrigger_xe90(std::vector<std::string> trigInfo); //!
  int  nMetTrigger_xe100(std::vector<std::string> trigInfo); //!
  int  nMetTrigger_xe110(std::vector<std::string> trigInfo); //!

  double dR(float eta1, float eta2, float phi1, float phi2); //!
  double dR(TLorentzVector v1, TLorentzVector v2);  //!
  bool match(TLorentzVector v1, TLorentzVector v2); //!  
  float MjjdRmin(std::vector<TLorentzVector> jets); //!

  enum JetWeight {MV2, JVT}; //!
  bool isResonance(std::vector<TLorentzVector> vectors, std::vector<float> charges); //!

  std::vector<float> muCharges_GL; //!
  std::vector<float> elCharges_GL; //!
  std::vector<float> lepCharges_GL; //!  
  std::vector<float> muQualities_GL; //!
  std::vector<float> elQualities_GL; //!
  std::vector<float> lepQualities_GL; //!   
  std::vector<float> muCharges; //!
  std::vector<float> elCharges; //!
  std::vector<float> lepCharges; //!  
  std::vector<float> jetCharges; //!
  std::vector<float> muQualities; //!
  std::vector<float> elQualities; //!
  std::vector<float> lepQualities; //!   
  std::vector<float> jetQualities; //!
  std::vector<TLorentzVector> muTLV_GL;  //!
  std::vector<TLorentzVector> elTLV_GL;  //!
  std::vector<TLorentzVector> lepTLV_GL;    //!
  std::vector<TLorentzVector> muTLV;  //!
  std::vector<TLorentzVector> elTLV;  //!
  std::vector<TLorentzVector> lepTLV;    //!
  std::vector<TLorentzVector> lepTLV_BL; //!
  std::vector<TLorentzVector> jetTLV;    //!
  std::vector<TLorentzVector> jetTLV_R1; //!
  std::vector<float> jetBtag;    //!
  std::vector<float> jetWtag_R1; //! 
  std::vector<float> jetID_R1; //!
  std::vector<float> jetN_R1;  //!
  std::vector<float> jetD2_R1; //!
  std::vector<float> jetC2_R1; //!
  std::vector<float> jetD12_R1;//!
  std::vector<float> muSF; //!
  std::vector<float> elSF; //!
  std::vector<float> jetMV2; //!
  std::vector<float> jetJVT; //!
  std::vector<float> lepCFloose;  //!    
  std::vector<float> lepCFmedium; //!
  std::vector<float> lepTruth;    //! 

  void FillTree(TTree *t, bool fill); //!
  void FillHisto(TH1 *h, float x, float w=1., bool fill=1); //!
  void FillHisto(TH2 *h, float x, float y, float w=1., bool fill=1); //!
  
  void sortPt(std::vector<TLorentzVector>& v, std::vector<float>& c, std::vector<float>& q); //!
  std::vector< TLorentzVector > mergeVector(std::vector<TLorentzVector> v1, std::vector<TLorentzVector> v2); //!
  std::vector< TLorentzVector > SSpair(std::vector<TLorentzVector> leptons, std::vector<float> charges); //!
  void getSignalLeptons(const char* type, std::vector< TLorentzVector > &leptons, std::vector<float> &charges, std::vector<float> &qual, unsigned int Nlep); //!
  int getChannel(std::vector<TLorentzVector> ss, std::vector<TLorentzVector> muons, std::vector<TLorentzVector> electrons, int lepConfig); //!
  int getCutNumber(std::vector<TLorentzVector> particles, float ptcut); //!
  int getLepNumber(std::vector<TLorentzVector> mu, std::vector<TLorentzVector> el, float elEtaCut=99.); //!
  
#ifndef __MAKECINT__
  int getQuality(const xAOD::Muon& mu);     //!
  int getQuality(const xAOD::Electron& el); //!
  int getQuality(const xAOD::Jet& jet);     //!

  std::vector<float> getBtags(std::vector<TLorentzVector> jetV, const xAOD::JetContainer *jets); //!
  
  std::vector<float> getPDFWeights(xAOD::TEvent *event);   //!
  std::vector<float> getPDFTruthInfo(xAOD::TEvent *event); //!
  std::string getPDFName(std::string sample, bool set=1);  //!

  float getDPSweight(int MCId, int njets); //!

  int  truthID(const xAOD::Jet *jet, bool isMC);     //!
  bool truthVeto(int MCId, float trHt, float trMET); //!
  
  int getNjets(const xAOD::JetContainer *Jets, int seli); //!
  void executeJRTool(const xAOD::JetContainer *Jets, xAOD::JetContainer &RecJets); //!
  float getTruthVar(xAOD::TEvent *event, int type);  //!
  float quarkdR(const xAOD::TruthParticle *tp, bool msg); //!
  bool getParents(const xAOD::TruthParticle *tp, bool &isW, bool &isZ, bool &isLQ, bool &isTop, bool &isLepPh, bool &isMeson, bool msg);  //!
  bool getMotherParticle(const xAOD::Jet *jet, xAOD::TEvent *event, bool msg);  //!
  bool getJetOrigin(const xAOD::JetContainer *Jets, xAOD::TEvent *event, bool isMC, bool msg=0);  //!
  std::vector<int> getSusyPDG(xAOD::TEvent *event);  //!
  std::vector<float> getTruthInfo(std::vector<TLorentzVector> lep, const xAOD::MuonContainer *muons, const xAOD::ElectronContainer *electrons); //!

  double getWeight(const xAOD::Jet& jet, DiLepton::JetWeight type, bool msg=0); //!
  double getSubstrVar(const xAOD::Jet *jet, TString name, bool msg=0);  //!
  
  bool isBaseline(xAOD::ElectronContainer *electrons, bool setCuts=true, double d0Cut=5.); //!
  bool cosmicsVeto(xAOD::MuonContainer* muons, bool enabled=1); //!
  bool badJetVeto(xAOD::JetContainer*   jets,  bool enabled=1); //!
  bool badMuonVeto(xAOD::MuonContainer* muons, bool enabled=1); //!
  bool badJvt(const xAOD::Jet& jet, float ptCut=60000., float etaCut=2.4, float jvtCut=0.59); //!
  void checkOR(xAOD::IParticleContainer *p); //!


  bool isTrigMatch(int yearOpt, xAOD::MuonContainer *muons, xAOD::ElectronContainer* electrons, float met, std::vector<std::string> trigInfo, bool doMatch=true, int period=0, float metcut=250000.); //
 
  bool isSignalMuon(xAOD::Muon &mu, float ptcut=10000., float etamax=2.5); //!
  bool isSignalElectron(xAOD::Electron &el, float ptcut=10000., float etamax=2.0); //!
  bool isSignalJet(const xAOD::Jet& jet, float ptcut=20000., float EtaMax=2.8, float EtaMaxB=2.5); //!
  bool isGoodR1Jet(const xAOD::Jet *jet, float ptCut=50000., float etaCut=2.8, float jvtCut=-99.); //  

  void Lep_Jet_OR(xAOD::IParticleContainer *electrons_baseline,xAOD::IParticleContainer *muons_baseline,xAOD::JetContainer *jets_baseline); //!
  void Electron_Muon_OR(xAOD::IParticleContainer *electrons_baseline,xAOD::IParticleContainer *muons_baseline); //!

  std::vector<float> getSF(const xAOD::MuonContainer *muons, bool doSF, bool info=0);  //!
  std::vector<float> getSF(const xAOD::ElectronContainer *electrons, bool doSF, bool info=0); //!

  std::vector< std::vector<float> > getChargeFlipInfo(const xAOD::ElectronContainer *electrons);   //!
  std::vector<float> getChargeFlipSF(const xAOD::ElectronContainer* electrons, unsigned int nsys); //!

  double getTotalSF(const xAOD::MuonContainer *mu, const xAOD::ElectronContainer* el, const xAOD::JetContainer *je); //!
  float getTriggerSF(int yearOpt, int randomRN, 
		     const xAOD::MuonContainer *Muons, 
		     const xAOD::ElectronContainer* Electrons, 
		     float &sferr, float met, std::vector<std::string> triggerInfo, bool doSF, bool info=0, float metcut=250000.);//!
  std::vector<float> getGlobTriggerSF(int yearOpt, 
				      const xAOD::MuonContainer *Muons, 
				      const xAOD::ElectronContainer* Electrons, 
				      float met, std::vector<std::string> triggerInfo, bool doSF, bool info=0, float metcut=250000.);//!

  bool getMET(float &met, float &metphi, 
	      xAOD::MissingETContainer *MET, 
	      const xAOD::JetContainer * jet, 
	      const xAOD::ElectronContainer * el, 
	      const xAOD::MuonContainer * mu, 
	      const xAOD::PhotonContainer * ph, 
	      const xAOD::TauJetContainer * tau, bool doTST=true, bool doJVT=true); //!

  bool resetSystematics(bool info=1); //!
  bool applySystematic(CP::SystematicSet sys, bool info=1);  //!
  void PrintSys(std::vector<ST::SystInfo> syslist); //!
  void checkNames(std::vector<ST::SystInfo> &syslist, std::vector<std::string> names); //!
  void checkNames(std::vector< SR > &signalregions, std::vector<std::string> names); //! 

  enum SysType {Weight, Kinematic}; //!
  ST::SystInfo emptySys(); //!
  CP::SystematicSet getSys(std::string name, std::vector<ST::SystInfo> syslist); //!
  
  std::vector<ST::SystInfo> SysInfoList;   //!
  std::vector<ST::SystInfo> SysInfoList_W; //!
  std::vector<ST::SystInfo> SysInfoList_K; //!
  std::vector<ST::SystInfo> getSysInfoList(bool fill, std::vector<ST::SystInfo> syslist, DiLepton::SysType type, bool useTau=0, bool usePhoton=0); //!
  
  std::vector<std::string> getSysNames(std::vector<ST::SystInfo> syslist); //!
  std::vector<float> getSysWeights(const xAOD::MuonContainer *Muons,
				   const xAOD::ElectronContainer *Electrons, 
				   const xAOD::JetContainer *Jets, 
				   std::vector<ST::SystInfo> syslist, 
				   std::vector<std::string> &names, unsigned int nsys); //!


  LHAPDF::PDFSet* PdfSet;           //!
  std::vector<LHAPDF::PDF*> PDFs;   //!
  
#endif


 public:

  int Debug;
  int isData;
  int isAtlfast;
  int PileupReweighting;
  int mc15;
  int doSys;
  int makeHist;
  int makeTree;
  int getR1Jets;
  int setLeptons;
  int NSignalLeptons;

  std::string SysList;
  std::string SampleName;
  std::string TriggerName;
  std::string SigRegion;
 
 public:

  int     m_entry; //!
  int     m_fileentry; //!

  float   lep_PtCut[3]; //!
  float   jet_PtCut[2]; //!
  bool    applySF;  //!
  bool    doPDF;    //!
  bool    doPileup; //!
  bool    truthMsg;  //!
  bool    doJetCut;  //!
  bool    ccCheck;   //! 
  bool    doCFTag;   //!
  bool    doDPSrw;   //!

  bool    usePhotons;   //!
  bool    useTaus;      //!
  bool    useZVeto;     //!

  //SR Cuts
  int    NJet_Min; //!
  int    NBJet_Min; //!
  int    NBJet_Max; //!
  float  METCut;  //!
  float  METMax;  //!
  float  MeffCut; //!
  float  MtCut;   //!
  float  JetPtCut;//!

  TTree *ControlTree; //!
  TTree *DiLeptonTree[NSR][NSYS]; //!

  //Histograms added to output
  TH1    *hCutflow     = 0; //!
  TH1    *hEventsTotal = 0; //!

  TH1    *hEvents[NSR][NSYS] = {{0}};        //!
  TH1    *hEventWeight[NSR][NSYS]   = {{0}}; //!
  TH1    *hEventWeightSF[NSR][NSYS] = {{0}}; //!
  
  TH1    *BaselineMet[NSR][NSYS]    = {{0}}; //!
  TH2    *BaselineLepPt[NSR][NSYS]  = {{0}}; //!
  TH2    *BaselineLepEta[NSR][NSYS] = {{0}}; //!

  TH1    *BaselineJet_Mj[NSR][NSYS]  = {{0}};   //!
  TH1    *BaselineJet_Mjj[NSR][NSYS] = {{0}};   //!
  TH1    *BaselineJet_MjjdR[NSR][NSYS] = {{0}}; //! 
  TH1    *BaselineJet_MjRec[NSR][NSYS] = {{0}}; //!
  TH1    *BaselineJet_MjAll[NSR][NSYS] = {{0}}; //! 
  TH1    *BaselineJet_MjRecAll[NSR][NSYS] = {{0}}; //! 

  TH1    *TrigMet[NSR][NSYS]  = {{0}};  //!
  TH2    *TrigMuPt[NSR][NSYS] = {{0}};  //!
  TH2    *TrigMuEta[NSR][NSYS] = {{0}}; //!
  TH2    *TrigElPt[NSR][NSYS]  = {{0}}; //!
  TH2    *TrigElEta[NSR][NSYS] = {{0}}; //!
  TH2    *TrigJetPt[NSR][NSYS] = {{0}}; //! 

  TH1    *NMu[NSR][NSYS]   = {{0}}; //!
  TH1    *NEl[NSR][NSYS]   = {{0}}; //!
  TH1    *NJet[NSR][NSYS]  = {{0}}; //!
  TH1    *NBJet[NSR][NSYS] = {{0}}; //!

  TH1    *bMu_1stPt[NSR][NSYS] = {{0}}; //!
  TH1    *bMu_2ndPt[NSR][NSYS] = {{0}}; //!
  TH1    *bMu_3rdPt[NSR][NSYS] = {{0}}; //!
  TH1    *bEl_1stPt[NSR][NSYS] = {{0}}; //!
  TH1    *bEl_2ndPt[NSR][NSYS] = {{0}}; //!
  TH1    *bEl_3rdPt[NSR][NSYS] = {{0}}; //!
  TH1    *bLep_1stPt[NSR][NSYS] = {{0}}; //!
  TH1    *bLep_2ndPt[NSR][NSYS] = {{0}}; //!
  TH1    *bLep_3rdPt[NSR][NSYS] = {{0}}; //!
  TH1    *Mu_1stPt[NSR][NSYS] = {{0}}; //!
  TH1    *Mu_2ndPt[NSR][NSYS] = {{0}}; //!
  TH1    *Mu_3rdPt[NSR][NSYS] = {{0}}; //!
  TH1    *El_1stPt[NSR][NSYS] = {{0}}; //!
  TH1    *El_2ndPt[NSR][NSYS] = {{0}}; //!
  TH1    *El_3rdPt[NSR][NSYS] = {{0}}; //!
  TH1    *Lep_1stPt[NSR][NSYS] = {{0}}; //!
  TH1    *Lep_2ndPt[NSR][NSYS] = {{0}}; //!
  TH1    *Lep_3rdPt[NSR][NSYS] = {{0}}; //!
  TH1    *Jet_1stPt[NSR][NSYS] = {{0}}; //!
  TH1    *Jet_2ndPt[NSR][NSYS] = {{0}}; //!

  TH1    *Jet_Mj[NSR][NSYS]  = {{0}};      //!
  TH1    *Jet_Mjj[NSR][NSYS] = {{0}};      //!
  TH1    *Jet_MjjdR[NSR][NSYS] = {{0}};    //! 
  TH1    *Jet_MjRec[NSR][NSYS] = {{0}};    //!
  TH1    *Jet_MjAll[NSR][NSYS] = {{0}};    //!
  TH1    *Jet_MjRecAll[NSR][NSYS] = {{0}}; //! 

  TH1    *Mu_1stEta[NSR][NSYS] = {{0}}; //!
  TH1    *Mu_2ndEta[NSR][NSYS] = {{0}}; //!
  TH1    *El_1stEta[NSR][NSYS] = {{0}}; //!
  TH1    *El_2ndEta[NSR][NSYS] = {{0}}; //!

  TH1    *ETmiss[NSR][NSYS] = {{0}}; //!
  TH1    *Meff[NSR][NSYS]   = {{0}}; //!
  TH1    *Minv[NSR][NSYS]   = {{0}}; //!
  TH1    *MT[NSR][NSYS] = {{0}};     //!
  TH1    *HT[NSR][NSYS] = {{0}};     //!

  TH1    *Mu_SumPt[NSR][NSYS] = {{0}};  //! 
  TH1    *El_SumPt[NSR][NSYS] = {{0}};  //! 
  TH1    *Jet_SumPt[NSR][NSYS] = {{0}}; //!

  xAOD::TEvent *m_event; //!
  xAOD::TStore store;    //!


#ifndef __MAKECINT__
  void PrintInfo(const xAOD::Electron* el,const char* APP_NAME, bool msg); //!
  void PrintInfo(const xAOD::Muon* mu,    const char* APP_NAME, bool msg); //!
  void PrintInfo(const xAOD::Jet* jet,    const char* APP_NAME, bool msg); //!
  void PrintInfo(const xAOD::TauJet* tau, const char* APP_NAME, bool msg); //!
  void PrintInfo(const xAOD::Photon* ph,  const char* APP_NAME, bool msg); //!
  void PrintInfo(const xAOD::TruthParticle *tp, const char* APP_NAME, bool msg); //!
#endif

  // standard constructor
  DiLepton ();


  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  // this is needed to distribute the algorithm to the workers
  ClassDef(DiLepton, 1);
};

#endif
