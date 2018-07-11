#ifndef MyAnalysis_MyxAODAnalysis_H
#define MyAnalysis_MyxAODAnalysis_H

#include <EventLoop/Algorithm.h>

/// Infrastructure include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

/// AnaToolHandle 
#include "AsgTools/AnaToolHandle.h"

/// SusyTools
#include "SUSYTools/ISUSYObjDef_xAODTool.h"

/// OR Tool Box
#include "AssociationUtils/ToolBox.h"


/// GRL
#include "AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h"
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"

#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h" 
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h" 
#include "ElectronEfficiencyCorrection/ElectronChargeEfficiencyCorrectionTool.h"
#include "AsgAnalysisInterfaces/IEfficiencyScaleFactorTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronChargeIDSelectorTool.h"


#include "IsolationSelection/IIsolationSelectionTool.h"
#include "IsolationSelection/IsolationSelectionTool.h"
#include "IsolationCorrections/IIsolationCorrectionTool.h"


/// added for trigger scale factors
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"

#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"

#include <TH1.h>
#include <TTree.h>

#include <map>


class JetCleaningTool;


class MyxAODAnalysis : public EL::Algorithm
{

#ifndef __CINT__
  
 
  asg::AnaToolHandle<ST::ISUSYObjDef_xAODTool> objTool{"ST::SUSYObjDef_xAOD/SS3Lep_SusyTools"}; //!
  asg::AnaToolHandle<IGoodRunsListSelectionTool> m_grl{"GoodRunsListSelectionTool/SS3Lep_grl"}; //!
  ///asg::AnaToolHandle<IPileupReweightingTool> m_pileupReweightingTool{"PileupReweightingTool/SS3Lep_PRW"}; //!
  
  ORUtils::ToolBox my_orToolbox; //!

  asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_elecSelLikelihood_LhLoose{"AsgElectronLikelihoodTool/SS3Lep_EleSelLikelihood_LooseAndBLayerLLH"}; //!
  asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_elecSelLikelihood_LhMed{"AsgElectronLikelihoodTool/SS3Lep_EleSelLikelihood_MediumLLH"}; //!
  asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_elecSelLikelihood_LhTight{"AsgElectronLikelihoodTool/SS3Lep_EleSelLikelihood_TightLH"}; //!

  asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronSF_reco{"AsgElectronEfficiencyCorrectionTool/SS3Lep_electronSF_reco"}; //!
  asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronSF_LhLoose{"AsgElectronEfficiencyCorrectionTool/SS3Lep_electronSF_LhLoose"}; //!
  asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronSF_LhMed{"AsgElectronEfficiencyCorrectionTool/SS3Lep_electronSF_LhMed"}; //!
  asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronSF_LhTight{"AsgElectronEfficiencyCorrectionTool/SS3Lep_electronSF_LhTight"}; //!
    
  asg::AnaToolHandle<CP::ElectronChargeEfficiencyCorrectionTool> m_electronSF_CFID{"CP::ElectronChargeEfficiencyCorrectionTool/SS3Lep_electronSF_CFID"}; //!
  asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronSF_CFT{"AsgElectronEfficiencyCorrectionTool/SS3Lep_electronSF_CFT"}; //!
  asg::AnaToolHandle<AsgElectronChargeIDSelectorTool> CFTToolmedium{"AsgElectronChargeIDSelectorTool/SS3Lep_CFTToolmedium"}; //!
  asg::AnaToolHandle<AsgElectronChargeIDSelectorTool> CFTToolLoose{"AsgElectronChargeIDSelectorTool/SS3Lep_CFTToolLoose"}; //!

  
  asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_electronSF_isoLhMed{"AsgElectronEfficiencyCorrectionTool/SS3Lep_electronSF_isoLhMed"}; //!
 
  asg::AnaToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool{"CP::MuonSelectionTool/SS3Lep_muonSelectionTool"}; //!
  asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEffSFs{"CP::MuonEfficiencyScaleFactors/SS3Lep_muonMediumSFTool"}; //!
  asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonISOSFs{"CP::MuonEfficiencyScaleFactors/SS3Lep_muonISOSFTool"}; //!

  
  asg::AnaToolHandle<CP::IIsolationSelectionTool> isoLooseTO{"CP::IsolationSelectionTool/SS3Lep_isoLooseTO"}; //!
  asg::AnaToolHandle<CP::IIsolationSelectionTool> isoLoose{"CP::IsolationSelectionTool/SS3Lep_isoLoose"}; //!
  asg::AnaToolHandle<CP::IIsolationSelectionTool> isoTight{"CP::IsolationSelectionTool/SS3Lep_isoTight"}; //!
  asg::AnaToolHandle<CP::IIsolationSelectionTool> isoGrad{"CP::IsolationSelectionTool/SS3Lep_isoGrad"}; //!
  asg::AnaToolHandle<CP::IIsolationSelectionTool> isoGradLoose{"CP::IsolationSelectionTool/SS3Lep_isoGradLoose"}; //!
  asg::AnaToolHandle<CP::IsolationSelectionTool> isoGradCustom{"CP::IsolationSelectionTool/SS3Lep_isoGradCustom"}; //!
  asg::AnaToolHandle<CP::IsolationSelectionTool> isoGradCustomTight{"CP::IsolationSelectionTool/SS3Lep_isoGradCustomTight"}; //!

  asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> muonTool{"CP::MuonTriggerScaleFactors/MuonTrigEff/SS3Lep_muonTool"}; //!
  std::vector<asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>> factoryEle; //!
  asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool> myTriggerSFTool{"TrigGlobalEfficiencyCorrectionTool/SS3Lep_TrigGlobal"}; //!

  
#endif // not __CINT__
 private: 
  /**   std::vector<float> EtaMuons; //! */
  /**   std::vector<float> pTMuons; //! */
  /**   std::vector<float> MassMuons; //! This is here for sanity checks */
  /**   std::vector<float> PhiMuons; //! */
  
  bool isMyOR = false;
  double pT_treshold;
  
  bool DEBUG = false;

  
  /// trigger
  std::map<const std::string, bool> trig_map;
  
  
  /// general 
  unsigned long long tEventNumber; //!
  Int_t tChannelNumber; //!
  Double_t tAvgMu; //!
  Double_t EventWeight; //!
  Double_t tPRWWeight; //!
  Double_t tTriggerDileptonSF; //!
  Int_t bcid; //!
  Int_t LB; //!
  Int_t tpassGRL; //!
  Int_t RunNb; //!
  unsigned int tPRWrandomRunNumber; //!
  Int_t tDetError; //!
  Int_t  Nvtx; //!
  /// Float_t primvertex_x; //!
  /// Float_t primvertex_y; //!
  Float_t primvertex_z; //!
  
  
  /// Muon Block
  Int_t   NMu=0; //!
  std::vector<Double_t> Mu_eta; //!
  std::vector<Double_t> Mu_phi; //!
  std::vector<Double_t> Mu_pT; //!
  std::vector<Int_t>    Mu_charge; //!
  std::vector<Double_t> Mu_sigd0; //!
  std::vector<Double_t> Mu_d0pvtx; //!
  std::vector<Double_t> Mu_d0pvtxerr; //!
  std::vector<Double_t> Mu_z0pvtx; //!
  std::vector<bool>     Mu_isBad; //!
  std::vector<bool>     Mu_isCosmic; //!
  std::vector<bool>     Mu_passOR; //!
  std::vector<bool>     Mu_isTight; //!
  std::vector<bool>     Mu_isSig; //!
  std::vector<float>    Mu_PromptLepTaggerBDTweight; //!
  std::vector<Int_t>    Mu_type; //!
  std::vector<Int_t>    Mu_origin; //!
  /// muon isolation 
  std::vector<Double_t> Mu_ptcone20; //!
  std::vector<Double_t> Mu_ptcone20_TightTTVA_pt500; //!
  std::vector<Double_t> Mu_ptcone20_TightTTVA_pt1000; //!
  std::vector<Double_t> Mu_ptcone30; //!
  std::vector<Double_t> Mu_ptvarcone30_TightTTVA_pt500; //!
  std::vector<Double_t> Mu_ptvarcone30_TightTTVA_pt1000; //!
  std::vector<Double_t> Mu_ptcone40; //!
  std::vector<Double_t> Mu_ptvarcone20; //!
  std::vector<Double_t> Mu_ptvarcone30; //!
  std::vector<Double_t> Mu_ptvarcone40; //!
  std::vector<Double_t> Mu_topoetcone20; //!
  std::vector<Double_t> Mu_topoetcone30; //!
  std::vector<Double_t> Mu_topoetcone40; //!
  std::vector<Double_t> Mu_neflowisol20; //!
  std::vector<bool>     Mu_passIsoLooseTO; //!
  std::vector<bool>     Mu_passIsoLoose; //!
  std::vector<bool>     Mu_passIsoTight; //!
  std::vector<bool>     Mu_passIsoGrad; //!
  std::vector<bool>     Mu_passIsoGradCustom; //!
  std::vector<bool>     Mu_passIsoGradCustomTight; //!
  std::vector<bool>     Mu_passIsoGradLoose; //!
  /// muon reco & ID scale factors
  std::vector<Double_t> Mu_SFw; //!
  std::vector<float>    Mu_IsoSFw; //!
  std::vector<float>    Mu_StatUncReco; //!
  std::vector<float>    Mu_SystUncReco; //!
  std::vector<float>    Mu_StatUncReco_LOWPT; //!
  std::vector<float>    Mu_SystUncReco_LOWPT; //!
  std::vector<float>    Mu_StatUncISO; //!
  std::vector<float>    Mu_SystUncISO; //!
  /// muon trigger matching
  std::vector<bool>     Mu_TrigMatch_mu26_ivarmedium; //!
  std::vector<bool>     Mu_TrigMatch_mu20_iloose_L1MU15; //!
  std::vector<bool>     Mu_TrigMatch_mu40; //!
  std::vector<bool>     Mu_TrigMatch_mu50; //!
  std::vector<bool>     Mu_TrigMatch_mu14; //!
  std::vector<bool>     Mu_TrigMatch_mu18; //!
  std::vector<bool>     Mu_TrigMatch_mu8noL1; //!
  std::vector<bool>     Mu_TrigMatch_mu18_mu8noL1; //!
  std::vector<bool>     Mu_TrigMatch_e17_lhloose_mu14; //!
  std::vector<bool>     Mu_TrigMatch_mu20_mu8noL1; //!
  std::vector<bool>     Mu_TrigMatch_mu22_mu8noL1; //!
  std::vector<bool>     Mu_TrigMatch_e17_lhloose_nod0_mu14; //!
  std::vector<bool>     Mu_TrigMatch_mu24_iloose; //!
  std::vector<bool>     Mu_TrigMatch_mu24_ivarloose; //!
  std::vector<bool>     Mu_TrigMatch_mu24_iloose_L1MU15; //!
  std::vector<bool>     Mu_TrigMatch_mu24_ivarloose_L1MU15; //!
  std::vector<std::vector<bool> >     Mu_TrigMatchPair_mu18_mu8noL1; //!
  std::vector<std::vector<bool> >     Mu_TrigMatchPair_mu20_mu8noL1; //!
  std::vector<std::vector<bool> >     Mu_TrigMatchPair_mu22_mu8noL1; //!
  

  /// Electron Block
  Int_t   NEl=0; //!
  std::vector<Double_t> El_eta; //!
  std::vector<Double_t> El_etaclus; //!
  std::vector<Double_t> El_phi; //!
  std::vector<Double_t> El_pT; //!
  std::vector<Double_t> El_E; //!
  std::vector<Int_t>    El_charge; //!
  std::vector<Double_t> El_sigd0; //!
  std::vector<Double_t> El_d0pvtx; //!
  std::vector<Double_t> El_d0pvtxerr; //!
  std::vector<Double_t> El_z0pvtx; //!
  std::vector<bool>     El_isLooseLH_baseline; //!
  std::vector<bool>     El_isLooseLH_fromTool; //!
  std::vector<bool>     El_isMediumLH; //!
  std::vector<bool>     El_isTightLH; //!
  std::vector<bool>     El_isSig; //!
  std::vector<Int_t>    El_nBLayerHits; //!
  std::vector<Int_t>    El_expectBLayerHit; //!
  std::vector<bool>     El_passOR; //!
  /// charge flip killer tool
  ///  std::vector<bool>     El_passChargeFlipTagger; //!
  std::vector<float>    El_passChargeFlipTaggerBDTmedium; //!
  std::vector<float>    El_passChargeFlipTaggerBDTloose; //!
  /// prompt lepton tagger
  std::vector<float>    El_PromptLepTaggerBDTweight; //!
  /// truth branches
  std::vector<Int_t>    El_truthType; //!
  std::vector<Int_t>    El_truthOrigin; //!
  std::vector<Int_t>    El_truthPdgId; //!
  std::vector<Int_t>    El_bkgTruthType; //! 
  std::vector<Int_t>    El_bkgTruthOrigin; //!
  std::vector<Int_t>    El_bkgMotherPdgId; //!
  std::vector<Int_t>    El_firstEgMotherTruthType; //!
  std::vector<Int_t>    El_firstEgMotherTruthOrigin; //!
  std::vector<Int_t>    El_firstEgMotherPdgId; //!
  std::vector<Int_t>    El_lastEgMotherTruthType; //!
  std::vector<Int_t>    El_lastEgMotherTruthOrigin; //!
  std::vector<Int_t>    El_lastEgMotherPdgId; //!
  std::vector<Int_t>    El_chFlip; //!

  /// electron isolation
  std::vector<Double_t> El_ptcone20; //!
  std::vector<Double_t> El_ptcone20_TightTTVA_pt500; //!
  std::vector<Double_t> El_ptcone20_TightTTVA_pt1000; //!
  std::vector<Double_t> El_ptcone30; //!
  std::vector<Double_t> El_ptcone40; //!
  std::vector<Double_t> El_ptvarcone20; //!
  std::vector<Double_t> El_ptvarcone20_TightTTVA_pt1000; //!
  std::vector<Double_t> El_ptvarcone30; //!
  std::vector<Double_t> El_ptvarcone30_TightTTVA_pt1000; //!
  std::vector<Double_t> El_ptvarcone40; //!
  std::vector<Double_t> El_topoetcone20; //!
  std::vector<Double_t> El_topoetcone30; //!
  std::vector<Double_t> El_topoetcone40; //!
  std::vector<Double_t> El_neflowisol20; //!
  std::vector<bool>     El_passIsoLooseTO; //!
  std::vector<bool>     El_passIsoLoose; //!
  std::vector<bool>     El_passIsoTight; //!
  std::vector<bool>     El_passIsoGrad; //!
  std::vector<bool>     El_passIsoGradCustom; //!
  std::vector<bool>     El_passIsoGradCustomTight; //!
  std::vector<bool>     El_passIsoGradLoose; //!
  /// electrons reco & ID & Iso & trigger scale factors
  std::vector<Double_t> El_SFwUncReco; //!
  std::vector<Double_t> El_SFwTightLH; //!
  std::vector<Double_t> El_SFwMediumLH; //!
  std::vector<Double_t> El_SFwUncMediumLH; //!
  std::vector<Double_t> El_SFwLooseLH; //!
  std::vector<Double_t> El_SFweightCFT; //!
  std::vector<Double_t> El_SFUncweightCFT; //!
  std::vector<Double_t> El_SFweightCFID; //!
  std::vector<Double_t> El_SFStatweightCFID; //!
  std::vector<Double_t> El_SFSystweightCFID; //!
  std::vector<float>    El_IsoSFwMediumLH; //!
  std::vector<float>    El_IsoSFwUncMediumLH; //!
  /// electron trigger matching
  std::vector<bool>     El_TrigMatch_e12_lhloose_L1EM10VH; //!
  std::vector<bool>     El_TrigMatch_e17_lhloose; //!
  std::vector<bool>     El_TrigMatch_e24_lhmedium_L1EM20VH; //!
  std::vector<bool>     El_TrigMatch_e24_lhmedium_iloose_L1EM20VH; //!
  std::vector<bool>     El_TrigMatch_e24_lhmedium_nod0_ivarloose; //!
  std::vector<bool>     El_TrigMatch_e24_lhtight_nod0_ivarloose; //!
  std::vector<bool>     El_TrigMatch_e26_lhtight_nod0_ivarloose; //!
  std::vector<bool>     El_TrigMatch_e60_lhmedium; //!
  std::vector<bool>     El_TrigMatch_e60_lhmedium_nod0; //!
  std::vector<bool>     El_TrigMatch_2e12_lhloose_L12EM10VH; //!
  std::vector<bool>     El_TrigMatch_2e15_lhloose_L12EM10VH; //!
  std::vector<bool>     El_TrigMatch_2e15_lhvloose_L12EM13VH; //!
  std::vector<bool>     El_TrigMatch_2e15_lhvloose_nod0_L12EM13VH; //!
  std::vector<bool>     El_TrigMatch_2e17_lhvloose_nod0; //!
  std::vector<bool>     El_TrigMatch_2e17_lhvloose_nod0_L12EM15VHI; //!
  std::vector<bool>     El_TrigMatch_2e24_lhvloose_nod0; //!
  std::vector<bool>     El_TrigMatch_e17_lhloose_mu14; //!
  std::vector<bool>     El_TrigMatch_e17_lhloose_nod0_mu14; //!
  

  /// JET Block
  Int_t   NJet=0; //!
  std::vector<Double_t> Jet_eta; //!
  std::vector<Double_t> Jet_phi; //!
  std::vector<Double_t> Jet_pT; //!
  std::vector<Double_t> Jet_E; //!
  std::vector<Int_t> Jet_nTrk; //!
  std::vector<Double_t> Jet_quality; //!
  std::vector<Float_t>  Jet_EMFrac; //!
  std::vector<Float_t>  Jet_HECFrac; //!
  std::vector<Double_t> Jet_JVT; //!
  std::vector<Double_t> Jet_JVTsf; //!
  float totalJVTsf; //!
  std::vector<Double_t> Jet_MV2c10; //!
  std::vector<Double_t> Jet_SFw; //!
  std::vector<bool>  Jet_passOR; //!
  /// truth branches
  std::vector<Int_t> Jet_ConeTruthLabel; //!
  std::vector<Int_t> Jet_PartonTruthLabel; //!
  std::vector<Int_t> Jet_HadronConeExclTruthLabel; //!
  std::vector<Double_t> Jet_deltaR; //!
  
  
  /// Missing Energy
  Float_t Etmiss_TST_Etx=0.; //!
  Float_t Etmiss_TST_Ety=0.; //!
  Float_t Etmiss_TST_Et=0.; //!
  
  
  
  /// Truth business 
  
  /// truth missing transverse energy
  Float_t Etmiss_Truth_Etx=0.; //!
  Float_t Etmiss_Truth_Ety=0.; //!
  Float_t Etmiss_Truth_Et=0.; //!
  
  /// truthJets from "AntiKt4TruthJets" collection 
  Int_t  NTruthAntiktJet=0; //!
  std::vector<Double_t> TruthAntiktJet_eta; //!
  std::vector<Double_t> TruthAntiktJet_phi; //!
  std::vector<Double_t> TruthAntiktJet_pT; //!
  std::vector<Double_t> TruthAntiktJet_E; //!
  std::vector<Int_t>    TruthAntiktJet_ConeTruthLabel; //!
  std::vector<Int_t>    TruthAntiktJet_HadronConeExclTruthLabelID; //!
  std::vector<Int_t>    TruthAntiktJet_PartonTruthLabel; //!
  
  /// need these branches for ttV HF / tt Hf studies (2b / 3b regions)
  std::vector<Int_t>    TruthAntiktJetJet_ClassHF; //!
  Int_t extraB=0; //!
  Int_t extraC=0; //!
    
  /// jets from "TruthParticles" collection
  Int_t   NTruthJet=0; //!
  std::vector<Double_t> TruthJet_eta; //!
  std::vector<Double_t> TruthJet_phi; //!
  std::vector<Double_t> TruthJet_pT; //!
  std::vector<Double_t> TruthJet_E; //!
  std::vector<Double_t> TruthJet_id; //!
  std::vector<Double_t> TruthJet_origin; //!
  
  /// only real lepton (from t/W/Z/SUSY/etc) from "TruthParticles" collection
  Int_t  NTruthRealL=0; //!
  std::vector<Double_t> TruthRealL_eta; //!
  std::vector<Double_t> TruthRealL_phi; //!
  std::vector<Double_t> TruthRealL_pT; //!
  std::vector<Int_t>    TruthRealL_id; //!
  std::vector<Int_t>    TruthRealL_origin; //!

  /// susy particles
  Int_t pdgid1; //!
  Int_t pdgid2; //!
  Int_t gluDecay1; //!
  Int_t gluDecay2; //!

  
  /// other
  Float_t GenFiltHT ; //!
  Float_t GenFiltMET ; //!

  /// PDF INFO!!!
  float TruthX1=0.; //!
  float TruthX2=0.; //!
  /// float TruthXF1=0.; //!
  /// float TruthXF2=0.; //!
  float TruthQ=0.; //!
  int TruthPDGID1=0.; //!
  int TruthPDGID2=0.; //!
  /// int TruthPDFID1=0.; //!
  /// int TruthPDFID2=0.; //!

  Float_t SherpaNjetWeight; //!

  


  /// put your configuration variables here as public variables.
  /// that way they can be set directly from CINT and python.
 public:
  int isData,isData15,isData16,isData17;
  int isAtlfast;
  int ismc16a,ismc16d;
  int useLeptonTrigger;
  int doSyst;
  std::string inputDS;
  //  std::vector<CP::SystematicSet> sysList; //!
  
  /// variables that don't get filled at submission time should be
  /// protected from being send from the submission node to the worker
  /// node (done by the //!)
 public:
  TTree *MyTree; //!
  
  float  m_finalSumOfWeights; //!
  float  m_initialSumOfWeights; //!
  float  m_finalNevents; //!
  float  m_initialNevents; //!
  
  TH1D* hEvents; //!
  TH1D* hWeights; //!
  TH1D* hWeightsSquared; //!

  const int nTrig = 67;
  TString nameTrig[67]={ 
	"HLT_e24_lhmedium_nod0_ivarloose",
	"HLT_e24_lhtight_nod0_ivarloose", 
	"HLT_e24_lhmedium_nod0_L1EM20VH",
	"HLT_e26_lhtight_iloose", 	 
	"HLT_e26_lhtight_ivarloose", 	  	  	  	 
	"HLT_e26_lhtight_nod0_iloose", 	  	  	  	 
	"HLT_e26_lhtight_nod0_ivarloose", 
	"HLT_e60_lhmedium", 	  	  	
	"HLT_e60_lhmedium_nod0", 	  	
	"HLT_e120_lhloose_nod0", 	  	
	"HLT_e140_lhloose_nod0", 	  	
	"HLT_2e17_lhvloose_nod0",
    "HLT_2e17_lhvloose_nod0_L12EM15VHI",	
	"HLT_2e15_lhvloose_nod0_L12EM13VH",
	"HLT_2e24_lhvloose_nod0",
	"HLT_e24_lhmedium_e9_lhmedium", 
	"HLT_e24_lhmedium_L1EM20VH", 	 
	"HLT_e24_lhmedium_iloose_L1EM20VH", 	 
	"HLT_e12_lhvloose_L12EM10VH",
	"HLT_e17_lhloose_2e9_lhloose", 	  	  	  	 
	"HLT_mu24_ivarmedium",
	"HLT_mu24_imedium", 	  	
	"HLT_mu24_ivarloose", 	  	
	"HLT_mu24_iloose", 	  	  	  	 
	"HLT_mu26_ivarmedium",
	"HLT_mu20_ivarmedium_L1MU15",
	"HLT_mu20_imedium_L1MU15", 	  	  	  	 
	"HLT_mu20_ivarloose_L1MU15", 	  	  	  	 
	"HLT_mu20_iloose_L1MU15", 	  	  	  	 
	"HLT_mu20_L1MU15",
	"HLT_mu20_mu8noL1", 	  	  	  	 
	"HLT_mu22_mu8noL1", 	  
	"HLT_mu20_2mu4noL1", 	  
	"HLT_mu22_2mu4noL1", 	  
	"HLT_mu40", 	  	  
	"HLT_mu50", 	  	  
	"HLT_2mu10", 	  	  
	"HLT_2mu10_nomucomb",	  	  	  	 
	"HLT_2mu14", 	  	  
	"HLT_2mu14_nomucomb", 	  	  	  	 
	"HLT_3mu6",
    "HLT_3mu4",	
	"HLT_3mu6_msonly", 	  	
	"HLT_xe100_L1XE50", 	  	  	  	 
	"HLT_xe80_mht_L1XE50", 	  
	"HLT_xe90_mht_L1XE50",
	"HLT_xe90_pufit_L1XE50",
	"HLT_xe100_mht_L1XE50",
	"HLT_xe100_tc_em_L1XE50",
	"HLT_xe100_pufit_L1XE55",
	"HLT_xe100_pufit_L1XE50",
	"HLT_xe110_mht_L1XE50",
    "HLT_xe110_pufit_L1XE55",
	"HLT_xe110_pufit_L1XE50",
	"HLT_xe80_tc_lcw_L1XE50", 	  
	"HLT_xe90_tc_lcw_L1XE50", 	  
	"HLT_xe80_tc_lcw_wEFMu_L1XE50", 	  	  	  	  
	"HLT_e7_lhmedium_mu24", 	
	"HLT_e7_lhmedium_nod0_mu24", 	
	"HLT_e17_lhloose_nod0_mu14", 
	"HLT_e26_lhmedium_nod0_L1EM22VHI_mu8noL1", 
	"HLT_e26_lhmedium_nod0_mu8noL1",
	"HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1",
	"HLT_2e12_lhloose_L12EM10VH",
	"HLT_e17_lhloose_mu14",
	"HLT_mu18_mu8noL1",
	"HLT_xe70"
  };
  xAOD::TEvent *m_event;  //!
  xAOD::TStore store;  //!


  /// this is a standard constructor
  MyxAODAnalysis ();

  /// these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();


  /// this is needed to distribute the algorithm to the workers
  ClassDef(MyxAODAnalysis, 1);
};

#endif
