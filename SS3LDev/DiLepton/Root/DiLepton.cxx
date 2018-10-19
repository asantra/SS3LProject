#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <DiLepton/DiLepton.h>
#include "EventLoop/OutputStream.h"

#include <PathResolver/PathResolver.h>

#include "xAODEventInfo/EventInfo.h"
#include "xAODEventFormat/EventFormat.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTagging/BTagging.h"
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "xAODRootAccess/tools/TFileAccessTracer.h"

#include "SUSYTools/SUSYObjDef_xAOD.h"
#include "GoodRunsLists/GoodRunsListSelectionTool.h"
#include "PileupReweighting/PileupReweightingTool.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"
#include "METUtilities/METSystematicsTool.h"
#include "PATCore/PATCoreEnums.h"
#include "AsgTools/AnaToolHandle.h"

#include "LHAPDF/LHAPDF.h"
#include "LHAPDF/Reweighting.h"
#include "MCTruthClassifier/MCTruthClassifier.h"
#include "DiLepton/DileptonTriggerWeight.h"
//#include "JetSubStructureUtils/BosonTag.h"
//#include "Reconstruction/Jet/JetSubStructureUtils/BosonTag.h"
#include "JetReclustering/JetReclusteringTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronChargeIDSelectorTool.h"
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"
#include "ElectronEfficiencyCorrection/IAsgElectronEfficiencyCorrectionTool.h"
//#include "TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionTool.h"
//#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"
#include <iostream>
#include <fstream>

ClassImp(DiLepton)


  DiLepton :: DiLepton ()
: SusyObjTool(0),
  GRLTool(0),
  JRTool(0),
  MCClassifier(0),
  CFTool(0),
  CFToolLoose(0)
  //TriggerSFTool(0),
  //TriggerSFToolUP(0),
  //TriggerSFToolDOWN(0)
{
}

EL::StatusCode DiLepton :: setupJob (EL::Job& job){

  job.useXAOD ();
  xAOD::Init( "DiLepton" ).ignore(); 

  EL::OutputStream out ("output");
  job.outputAdd (out);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode DiLepton :: histInitialize (){

  const char* APP_NAME = "histInitialize()";

  //Generic Histos
  hCutflow     = new TH1F("hCutflow", "hCutflow", 50, -0.5, 49.5);
  hEventsTotal = new TH1F("hEventsTotal", "hEventsTotal", 10, 0, 10);
  wk()->addOutput(hCutflow);
  wk()->addOutput(hEventsTotal);

  Info( APP_NAME, "Created histogram %s", hEventsTotal->GetName() );
  return EL::StatusCode::SUCCESS;
}

bool DiLepton :: InitializeHistograms (){

  const char* APP_NAME = "InitializeHistograms()";

  if(!makeHist) return true;
  Info( APP_NAME, "Initialize histograms" );

  NSys = SysInfoList_K.size();
  NSr  = SignalRegions.size();
  Info(APP_NAME, "Size of histogram array (%i, %i) ",(int)NSr, (int)NSys );

  for(unsigned int sr(0); sr<NSr; ++sr){ 
    TString SRsuf = "_"+(TString)SignalRegions.at(sr).Name;
    for(unsigned int sys(0); sys<NSys; sys++){
      TString SYSsuf = sys ? "_"+(TString)(this->getSysNames(SysInfoList_K)).at(sys) : "";


      hEvents[sr][sys]        = new TH1F("hEvents"+SRsuf+SYSsuf,  "hEvents"+SRsuf+SYSsuf, 10, 0, 10);
      hEventWeight[sr][sys]   = new TH1F("hEventWeight"+SRsuf+SYSsuf, "hEventWeight"+SRsuf+SYSsuf, 100, 0, 1000);
      hEventWeightSF[sr][sys] = new TH1F("hEventWeightSF"+SRsuf+SYSsuf, "hEventWeightSF"+SRsuf+SYSsuf, 100, 0, 1000);

      //Baseline histos
      BaselineMet[sr][sys]    = new TH1F("BaselineMet"+SRsuf+SYSsuf,   "BaselineMet"+SRsuf+SYSsuf,   200, 0, 2000000);
      BaselineLepPt[sr][sys]  = new TH2F("BaselineLepPt"+SRsuf+SYSsuf, "BaselineLepPt"+SRsuf+SYSsuf, 200, 0, 2000000, 200, 0, 2000000);
      BaselineLepEta[sr][sys] = new TH2F("BaselineLepEta"+SRsuf+SYSsuf,"BaselineLepEta"+SRsuf+SYSsuf,100, -3.5, 3.5, 100, -3.5, 3.5);

      BaselineJet_Mj[sr][sys]     = new TH1F("BaselineJet_Mj"+SRsuf+SYSsuf,    "BaselineJet_Mj"+SRsuf+SYSsuf,   200, 0, 2000000);
      BaselineJet_Mjj[sr][sys]    = new TH1F("BaselineJet_Mjj"+SRsuf+SYSsuf,   "BaselineJet_Mjj"+SRsuf+SYSsuf,  200, 0, 2000000);
      BaselineJet_MjjdR[sr][sys]  = new TH1F("BaselineJet_MjjdR"+SRsuf+SYSsuf, "BaselineJet_MjjdR"+SRsuf+SYSsuf,200, 0, 2000000); 
      BaselineJet_MjRec[sr][sys]  = new TH1F("BaselineJet_MjRec"+SRsuf+SYSsuf, "BaselineJet_MjRec"+SRsuf+SYSsuf,200, 0, 2000000);
      BaselineJet_MjAll[sr][sys]  = new TH1F("BaselineJet_MjAll"+SRsuf+SYSsuf, "BaselineJet_MjAll"+SRsuf+SYSsuf,200, 0, 2000000);
      BaselineJet_MjRecAll[sr][sys] = new TH1F("BaselineJet_MjRecAll"+SRsuf+SYSsuf, "BaselineJet_MjRecAll"+SRsuf+SYSsuf,200, 0, 2000000);

      //Trigger histos
      TrigMet[sr][sys]  = new TH1F("TrigMet"+SRsuf+SYSsuf,  "TrigMet"+SRsuf+SYSsuf,  200, 0, 2000000);
      TrigMuPt[sr][sys] = new TH2F("TrigMuPt"+SRsuf+SYSsuf, "TrigMuPt"+SRsuf+SYSsuf, 200, 0, 2000000, 200, 0, 2000000);
      TrigElPt[sr][sys] = new TH2F("TrigElPt"+SRsuf+SYSsuf, "TrigElPt"+SRsuf+SYSsuf, 200, 0, 2000000, 200, 0, 2000000);
      TrigMuEta[sr][sys] = new TH2F("TrigMuEta"+SRsuf+SYSsuf, "TrigMuEta"+SRsuf+SYSsuf, 100, -3.5, 3.5, 100, -3.5, 3.5);
      TrigElEta[sr][sys] = new TH2F("TrigElEta"+SRsuf+SYSsuf, "TrigElEta"+SRsuf+SYSsuf, 100, -3.5, 3.5, 100, -3.5, 3.5);   
      TrigJetPt[sr][sys] = new TH2F("TrigJetPt"+SRsuf+SYSsuf, "TrigJetPt"+SRsuf+SYSsuf, 200, 0, 5000000, 200, 0, 5000000);

      //Signal histos
      NMu[sr][sys]   = new TH1F("NMu"+SRsuf+SYSsuf,  "NMu"+SRsuf+SYSsuf,   10,0,10);
      NEl[sr][sys]   = new TH1F("NEl"+SRsuf+SYSsuf,  "NEl"+SRsuf+SYSsuf,   10,0,10);
      NJet[sr][sys]  = new TH1F("NJet"+SRsuf+SYSsuf, "NJet"+SRsuf+SYSsuf,  20,0,20);
      NBJet[sr][sys] = new TH1F("NBJet"+SRsuf+SYSsuf,"NBJet"+SRsuf+SYSsuf, 20,0,20);

      Mu_1stPt[sr][sys]  = new TH1F("Mu_1stPt"+SRsuf+SYSsuf,  "Mu_1stPt"+SRsuf+SYSsuf, 200, 0, 2000000);
      Mu_2ndPt[sr][sys]  = new TH1F("Mu_2ndPt"+SRsuf+SYSsuf,  "Mu_2ndPt"+SRsuf+SYSsuf, 200, 0, 2000000);
      Mu_3rdPt[sr][sys]  = new TH1F("Mu_3rdPt"+SRsuf+SYSsuf,  "Mu_3rdPt"+SRsuf+SYSsuf, 200, 0, 2000000);
      El_1stPt[sr][sys]  = new TH1F("El_1stPt"+SRsuf+SYSsuf,  "El_1stPt"+SRsuf+SYSsuf, 200, 0, 2000000);
      El_2ndPt[sr][sys]  = new TH1F("El_2ndPt"+SRsuf+SYSsuf,  "El_2ndPt"+SRsuf+SYSsuf, 200, 0, 2000000);
      El_3rdPt[sr][sys]  = new TH1F("El_3rdPt"+SRsuf+SYSsuf,  "El_3rdPt"+SRsuf+SYSsuf, 200, 0, 2000000);
      Lep_1stPt[sr][sys]  = new TH1F("Lep_1stPt"+SRsuf+SYSsuf,  "Lep_1stPt"+SRsuf+SYSsuf, 200, 0, 2000000);
      Lep_2ndPt[sr][sys]  = new TH1F("Lep_2ndPt"+SRsuf+SYSsuf,  "Lep_2ndPt"+SRsuf+SYSsuf, 200, 0, 2000000);
      Lep_3rdPt[sr][sys]  = new TH1F("Lep_3rdPt"+SRsuf+SYSsuf,  "Lep_3rdPt"+SRsuf+SYSsuf, 200, 0, 2000000);
      Jet_1stPt[sr][sys]  = new TH1F("Jet_1stPt"+SRsuf+SYSsuf,  "Jet_1stPt"+SRsuf+SYSsuf, 200, 0, 2000000);
      Jet_2ndPt[sr][sys]  = new TH1F("Jet_2ndPt"+SRsuf+SYSsuf,  "Jet_2ndPt"+SRsuf+SYSsuf, 200, 0, 2000000);

      Jet_Mj[sr][sys]     = new TH1F("Jet_Mj"+SRsuf+SYSsuf,    "Jet_Mj"+SRsuf+SYSsuf,   200, 0, 2000000);
      Jet_Mjj[sr][sys]    = new TH1F("Jet_Mjj"+SRsuf+SYSsuf,   "Jet_Mjj"+SRsuf+SYSsuf,  200, 0, 2000000);
      Jet_MjjdR[sr][sys]  = new TH1F("Jet_MjjdR"+SRsuf+SYSsuf, "Jet_MjjdR"+SRsuf+SYSsuf,200, 0, 2000000); 
      Jet_MjRec[sr][sys]  = new TH1F("Jet_MjRec"+SRsuf+SYSsuf, "Jet_MjRec"+SRsuf+SYSsuf,200, 0, 2000000);
      Jet_MjAll[sr][sys]  = new TH1F("Jet_MjAll"+SRsuf+SYSsuf, "Jet_MjAll"+SRsuf+SYSsuf,200, 0, 2000000);
      Jet_MjRecAll[sr][sys] = new TH1F("Jet_MjRecAll"+SRsuf+SYSsuf, "Jet_MjRecAll"+SRsuf+SYSsuf,200, 0, 2000000);

      Mu_1stEta[sr][sys]  = new TH1F("Mu_1stEta"+SRsuf+SYSsuf,  "Mu_1stEta"+SRsuf+SYSsuf, 100, -3.5, 3.5);
      Mu_2ndEta[sr][sys]  = new TH1F("Mu_2ndEta"+SRsuf+SYSsuf,  "Mu_2ndEta"+SRsuf+SYSsuf, 100, -3.5, 3.5);
      El_1stEta[sr][sys]  = new TH1F("El_1stEta"+SRsuf+SYSsuf,  "El_1stEta"+SRsuf+SYSsuf, 100, -3.5, 3.5);
      El_2ndEta[sr][sys]  = new TH1F("El_2ndEta"+SRsuf+SYSsuf,  "El_2ndEta"+SRsuf+SYSsuf, 100, -3.5, 3.5);

      ETmiss[sr][sys] = new TH1F("ETmiss"+SRsuf+SYSsuf,"ETmiss"+SRsuf+SYSsuf,200, 0, 1000000);
      Meff[sr][sys]   = new TH1F("Meff"+SRsuf+SYSsuf,  "Meff"+SRsuf+SYSsuf,  200, 0, 5000000);
      Minv[sr][sys]   = new TH1F("Minv"+SRsuf+SYSsuf,  "Minv"+SRsuf+SYSsuf,  200, 0, 5000000);
      MT[sr][sys]     = new TH1F("MT"+SRsuf+SYSsuf,    "MT"+SRsuf+SYSsuf,    200, 0, 5000000);
      HT[sr][sys]     = new TH1F("HT"+SRsuf+SYSsuf,    "HT"+SRsuf+SYSsuf,    200, 0, 5000000);

      Mu_SumPt[sr][sys] = new TH1F("Mu_SumPt"+SRsuf+SYSsuf, "Mu_SumPt"+SRsuf+SYSsuf, 200, 0, 2000000);
      El_SumPt[sr][sys] = new TH1F("El_SumPt"+SRsuf+SYSsuf, "El_SumPt"+SRsuf+SYSsuf, 200, 0, 2000000);  
      Jet_SumPt[sr][sys]  = new TH1F("Jet_SumPt"+SRsuf+SYSsuf, "Jet_SumPt"+SRsuf+SYSsuf, 200, 0, 2000000);

      //Add histos to output
      wk()->addOutput(hEvents[sr][sys]);
      wk()->addOutput(hEventWeight[sr][sys]);
      wk()->addOutput(hEventWeightSF[sr][sys]);

      wk()->addOutput(BaselineMet[sr][sys]);
      wk()->addOutput(BaselineLepPt[sr][sys]);
      wk()->addOutput(BaselineLepEta[sr][sys]);

      wk()->addOutput(BaselineJet_Mj[sr][sys]);
      wk()->addOutput(BaselineJet_Mjj[sr][sys]);
      wk()->addOutput(BaselineJet_MjjdR[sr][sys]);
      wk()->addOutput(BaselineJet_MjRec[sr][sys]); 
      wk()->addOutput(BaselineJet_MjAll[sr][sys]);    
      wk()->addOutput(BaselineJet_MjRecAll[sr][sys]);

      wk()->addOutput(TrigMuPt[sr][sys]);
      wk()->addOutput(TrigElPt[sr][sys]);
      wk()->addOutput(TrigMuEta[sr][sys]);
      wk()->addOutput(TrigElEta[sr][sys]);
      wk()->addOutput(TrigJetPt[sr][sys]);
      wk()->addOutput(TrigMet[sr][sys]);

      wk()->addOutput(NMu[sr][sys]);
      wk()->addOutput(NEl[sr][sys]);
      wk()->addOutput(NJet[sr][sys]);
      wk()->addOutput(NBJet[sr][sys]);

      wk()->addOutput(Mu_1stPt[sr][sys]);
      wk()->addOutput(Mu_2ndPt[sr][sys]);
      wk()->addOutput(Mu_3rdPt[sr][sys]);
      wk()->addOutput(El_1stPt[sr][sys]);
      wk()->addOutput(El_2ndPt[sr][sys]);
      wk()->addOutput(El_3rdPt[sr][sys]);
      wk()->addOutput(Lep_1stPt[sr][sys]);
      wk()->addOutput(Lep_2ndPt[sr][sys]);
      wk()->addOutput(Lep_3rdPt[sr][sys]);
      wk()->addOutput(Jet_1stPt[sr][sys]);
      wk()->addOutput(Jet_2ndPt[sr][sys]);

      wk()->addOutput(Jet_Mj[sr][sys]);
      wk()->addOutput(Jet_Mjj[sr][sys]);
      wk()->addOutput(Jet_MjjdR[sr][sys]);
      wk()->addOutput(Jet_MjRec[sr][sys]); 
      wk()->addOutput(Jet_MjAll[sr][sys]);
      wk()->addOutput(Jet_MjRecAll[sr][sys]); 

      wk()->addOutput(Mu_1stEta[sr][sys]);
      wk()->addOutput(Mu_2ndEta[sr][sys]);
      wk()->addOutput(El_1stEta[sr][sys]);
      wk()->addOutput(El_2ndEta[sr][sys]);

      wk()->addOutput(Meff[sr][sys]);
      wk()->addOutput(ETmiss[sr][sys]);
      wk()->addOutput(Minv[sr][sys]);
      wk()->addOutput(MT[sr][sys]);
      wk()->addOutput(HT[sr][sys]);

      wk()->addOutput(Mu_SumPt[sr][sys]);
      wk()->addOutput(El_SumPt[sr][sys]);
      wk()->addOutput(Jet_SumPt[sr][sys]);

    }
  }
  Info( APP_NAME, "Initialized histograms successfully" );
  return true;
}

bool DiLepton :: InitializeTree() {

  const char* APP_NAME = "InitializeTree()";

  if(!makeTree) return true;
  Info( APP_NAME, "Initialize TTree" );  

  TFile *file = wk()->getOutputFile ("output");
  if(file) Info("Created file", file->GetName() );

  ControlTree = new TTree("ControlTree", "ControlTree");
  Info( APP_NAME, "Created TTree %s", ControlTree->GetName() );
  ControlTree->SetDirectory(file);
  ControlTree->Branch("EventNumber",   &EventNumber,    "EventNumber/L");
  ControlTree->Branch("RunNumber",     &RunNumber,      "RunNumber/L");
  ControlTree->Branch("AverageMu",     &AverageMu);
  ControlTree->Branch("LumiBlock",     &LB);
  ControlTree->Branch("MCId",          &MCId);
  ControlTree->Branch("MCWeight",      &EventWeight);
  ControlTree->Branch("NFiles",        &NFiles);
  ControlTree->Branch("FileEntry",     &m_fileentry);
  ControlTree->Branch("xAODEventSum",  &xAODEventSum);
  ControlTree->Branch("xAODWeightSum", &xAODWeightSum);
  ControlTree->Branch("PDGId1",        &PDGId1);
  ControlTree->Branch("PDGId2",        &PDGId2);
  ControlTree->Branch("GLDec1",        &GLDec1);
  ControlTree->Branch("GLDec2",        &GLDec2);

  NSys = SysInfoList_K.size();
  NSr  = SignalRegions.size();
  Info(APP_NAME, "Size of TTree array (%i, %i) ",(int)NSr, (int)NSys );
  setROOTmsg(0);

  for(unsigned int sr(0); sr<NSr; ++sr){ 
    TString SRsuf = "_"+(TString)SignalRegions.at(sr).Name;
    for(unsigned int sys(0); sys<NSys; sys++){
      TString SYSsuf = sys ? "_"+(TString)(this->getSysNames(SysInfoList_K)).at(sys) : "";


      DiLeptonTree[sr][sys] = new TTree("DiLeptonTree"+SRsuf+SYSsuf, "DiLeptonTree"+SRsuf+SYSsuf);
      std::cout << APP_NAME << "\t Created TTree " << DiLeptonTree[sr][sys]->GetName() << std::endl;
      DiLeptonTree[sr][sys]->SetDirectory(file);
      DiLeptonTree[sr][sys]->Branch("EventNumber",  &EventNumber,    "EventNumber/L");
      DiLeptonTree[sr][sys]->Branch("RunNumber",    &RunNumber,      "RunNumber/L");
      DiLeptonTree[sr][sys]->Branch("PileUpHash",   &PileUpHash,     "PileUpHash/L");         
      DiLeptonTree[sr][sys]->Branch("RandomRN",     &randomRN);
      DiLeptonTree[sr][sys]->Branch("ConfigYear",   &configYear);
      DiLeptonTree[sr][sys]->Branch("eventrunNumber",   &eventrunNumber);
      DiLeptonTree[sr][sys]->Branch("MCId",         &MCId);
      DiLeptonTree[sr][sys]->Branch("MCWeight",     &EventWeight);
      DiLeptonTree[sr][sys]->Branch("PileUpWeight", &PileUpWeight);     
      DiLeptonTree[sr][sys]->Branch("ObjectSF",     &ObjectSF);
      DiLeptonTree[sr][sys]->Branch("TriggerSF",    &triggerSF);
      DiLeptonTree[sr][sys]->Branch("ChargeFlipSF", &chargeFlipSF);
      DiLeptonTree[sr][sys]->Branch("TotalWeight",  &weight);
      DiLeptonTree[sr][sys]->Branch("mcWeightVariations",  &mcWeightVariations);
      DiLeptonTree[sr][sys]->Branch("EtMiss",       &EtMiss);
      DiLeptonTree[sr][sys]->Branch("EtMissPhi",    &EtMissPhi);
      DiLeptonTree[sr][sys]->Branch("Meff",         &meff);
      DiLeptonTree[sr][sys]->Branch("Minv",         &minv);      
      DiLeptonTree[sr][sys]->Branch("Mt",           &mt);
      DiLeptonTree[sr][sys]->Branch("Ht",           &ht);
      DiLeptonTree[sr][sys]->Branch("Mj",           &mj);
      DiLeptonTree[sr][sys]->Branch("Mjj",          &mjj);
      DiLeptonTree[sr][sys]->Branch("MjjdR",        &mjjdR);
      DiLeptonTree[sr][sys]->Branch("MjRec",        &mjRec); 
      DiLeptonTree[sr][sys]->Branch("TruthHt",      &truthHt);
      DiLeptonTree[sr][sys]->Branch("TruthMET",     &truthMET);
      DiLeptonTree[sr][sys]->Branch("Zevent",       &isZevent);
      DiLeptonTree[sr][sys]->Branch("NlepBL",       &NlepBL);
      DiLeptonTree[sr][sys]->Branch("NlepBLEta",    &NlepBLEta);
      DiLeptonTree[sr][sys]->Branch("Njet25",       &Njet25);
      DiLeptonTree[sr][sys]->Branch("Njet35",       &Njet35);
      DiLeptonTree[sr][sys]->Branch("Njet40",       &Njet40);
      DiLeptonTree[sr][sys]->Branch("Njet50",       &Njet50);
      DiLeptonTree[sr][sys]->Branch("Nbjet",        &Nbjet);
      DiLeptonTree[sr][sys]->Branch("jetBtag",      &jetBtag);
      DiLeptonTree[sr][sys]->Branch("jetTLV",       &jetTLV);
      DiLeptonTree[sr][sys]->Branch("lepTLV",       &lepTLV);
      DiLeptonTree[sr][sys]->Branch("lepTLV_BL",    &lepTLV_BL);
      DiLeptonTree[sr][sys]->Branch("lepCharges",   &lepCharges);
      DiLeptonTree[sr][sys]->Branch("lepCFmedium",  &lepCFmedium);
      DiLeptonTree[sr][sys]->Branch("lepCFloose",   &lepCFloose);
      DiLeptonTree[sr][sys]->Branch("lepTruth",     &lepTruth);
      DiLeptonTree[sr][sys]->Branch("SSChannel",    &SSChannel);
      DiLeptonTree[sr][sys]->Branch("SysNames",     &SysNames);
      DiLeptonTree[sr][sys]->Branch("SysWeights",   &SysWeights); 
      DiLeptonTree[sr][sys]->Branch("PDFWeights",   &PDFWeights);
      DiLeptonTree[sr][sys]->Branch("CFTWeights",   &CFTWeights);
      //DiLeptonTree[sr][sys]->Branch("TrigWeights",  &TrigWeights); 
      DiLeptonTree[sr][sys]->Branch("triggerInfo",  &triggerInfo);
      DiLeptonTree[sr][sys]->Branch("PDGId1",       &PDGId1);
      DiLeptonTree[sr][sys]->Branch("PDGId2",       &PDGId2);
      DiLeptonTree[sr][sys]->Branch("GLDec1",       &GLDec1);
      DiLeptonTree[sr][sys]->Branch("GLDec2",       &GLDec2);
      DiLeptonTree[sr][sys]->Branch("IntPerX",      &IntPerX);
      DiLeptonTree[sr][sys]->Branch("topoStr",      &topoStr);
      DiLeptonTree[sr][sys]->Branch("muTLV",        &muTLV);
      DiLeptonTree[sr][sys]->Branch("elTLV",        &elTLV); 
      DiLeptonTree[sr][sys]->Branch("jetTLV_R1",    &jetTLV_R1);
      DiLeptonTree[sr][sys]->Branch("jetID_R1",     &jetID_R1);
      DiLeptonTree[sr][sys]->Branch("jetWtag_R1",   &jetWtag_R1);
      DiLeptonTree[sr][sys]->Branch("jetN_R1",      &jetN_R1);
      DiLeptonTree[sr][sys]->Branch("jetD2_R1",     &jetD2_R1);
      DiLeptonTree[sr][sys]->Branch("jetC2_R1",     &jetC2_R1);
      DiLeptonTree[sr][sys]->Branch("jetD12_R1",    &jetD12_R1);
      DiLeptonTree[sr][sys]->Branch("muCharges",    &muCharges);
      DiLeptonTree[sr][sys]->Branch("elCharges",    &elCharges);
      DiLeptonTree[sr][sys]->Branch("muCharges_GL", &muCharges_GL);
      DiLeptonTree[sr][sys]->Branch("elCharges_GL", &elCharges_GL);
      DiLeptonTree[sr][sys]->Branch("muSF",         &muSF);
      DiLeptonTree[sr][sys]->Branch("elSF",         &elSF);
      DiLeptonTree[sr][sys]->Branch("jetMV2",       &jetMV2);
      DiLeptonTree[sr][sys]->Branch("jetJVT",       &jetJVT); 
      DiLeptonTree[sr][sys]->Branch("mu_n",         &mu_n);
      DiLeptonTree[sr][sys]->Branch("el_n",         &el_n);
      DiLeptonTree[sr][sys]->Branch("jet_n",        &jet_n);
      DiLeptonTree[sr][sys]->Branch("b_n",          &b_n);
      DiLeptonTree[sr][sys]->Branch("mu_1stPt",     &mu_1stPt);
      DiLeptonTree[sr][sys]->Branch("mu_2ndPt",     &mu_2ndPt);
      DiLeptonTree[sr][sys]->Branch("mu_3rdPt",     &mu_3rdPt);
      DiLeptonTree[sr][sys]->Branch("el_1stPt",     &el_1stPt);
      DiLeptonTree[sr][sys]->Branch("el_2ndPt",     &el_2ndPt);
      DiLeptonTree[sr][sys]->Branch("el_2ndPt",     &el_2ndPt);
      DiLeptonTree[sr][sys]->Branch("lep_1stPt",    &lep_1stPt);
      DiLeptonTree[sr][sys]->Branch("lep_2ndPt",    &lep_2ndPt);
      DiLeptonTree[sr][sys]->Branch("lep_3rdPt",    &lep_3rdPt);

      if( makeTree>1 ){
        DiLeptonTree[sr][sys]->Branch("IntPerX",      &IntPerX);
        DiLeptonTree[sr][sys]->Branch("topoStr",      &topoStr);
        DiLeptonTree[sr][sys]->Branch("muTLV",        &muTLV);
        DiLeptonTree[sr][sys]->Branch("elTLV",        &elTLV); 
        DiLeptonTree[sr][sys]->Branch("jetTLV_R1",    &jetTLV_R1);
        DiLeptonTree[sr][sys]->Branch("jetID_R1",     &jetID_R1);
        DiLeptonTree[sr][sys]->Branch("jetWtag_R1",   &jetWtag_R1);
        DiLeptonTree[sr][sys]->Branch("jetN_R1",      &jetN_R1);
        DiLeptonTree[sr][sys]->Branch("jetD2_R1",     &jetD2_R1);
        DiLeptonTree[sr][sys]->Branch("jetC2_R1",     &jetC2_R1);
        DiLeptonTree[sr][sys]->Branch("jetD12_R1",    &jetD12_R1);
        DiLeptonTree[sr][sys]->Branch("muCharges",    &muCharges);
        DiLeptonTree[sr][sys]->Branch("elCharges",    &elCharges);
        DiLeptonTree[sr][sys]->Branch("muCharges_GL", &muCharges_GL);
        DiLeptonTree[sr][sys]->Branch("elCharges_GL", &elCharges_GL);
        DiLeptonTree[sr][sys]->Branch("muSF",         &muSF);
        DiLeptonTree[sr][sys]->Branch("elSF",         &elSF);
        DiLeptonTree[sr][sys]->Branch("jetMV2",       &jetMV2);
        DiLeptonTree[sr][sys]->Branch("jetJVT",       &jetJVT); 
      }
    }
  }

  setROOTmsg(1);
  Info( APP_NAME, "Initialized TTree successfully" );
  std::cout<<"Create cutflow txt file"<<std::endl;
  //std::ofstream cutflow_file;
  cutflow_file_trig.open("list_events_trig.txt");
  cutflow_file_baseline.open("list_events_baseline.txt");
  cutflow_file_all.open("list_events_sig_all.txt");
  cutflow_file_SS.open("list_events_sig_SS.txt");
  cutflow_file_ee.open("list_events_sig_ee.txt");
  cutflow_file_emu.open("list_events_sig_emu.txt");
  cutflow_file_mumu.open("list_events_sig_mumu.txt");

  return true;
}

EL::StatusCode DiLepton :: fileExecute (){

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode DiLepton :: changeInput (bool firstFile){

  const char* APP_NAME = "changeInput()"; 
  m_event = wk()->xaodEvent();
  m_fileentry = 0; 

  int sumEvents(0); double sumWeights(0), sumWeightsSq(0);
  xAODEventSum = 0, xAODWeightSum = 0;
  if(firstFile) NFiles = 0;

  Info( APP_NAME, "Number of files processed = %i", (int)NFiles+1 );
  Info( APP_NAME, "Number of events in this file = %11i", (int)m_event->getEntries() );

  //Retrive total events numbers of original xAOD
  Info( APP_NAME, "Retrieve MetaData");
  const xAOD::CutBookkeeperContainer* completeCBC(0);
  if (!m_event->retrieveMetaInput(completeCBC, "CutBookkeepers").isSuccess()) {
    Error( APP_NAME , "Failed to retrieve CutBookkeepers from MetaData");
    return EL::StatusCode::SUCCESS;
  }
  const xAOD::CutBookkeeper* allEventsCBK(0);
  int maxCycle(-1); 
  for(auto cbk : *completeCBC){ 
    if(cbk->inputStream()=="StreamAOD" && cbk->name()=="AllExecutedEvents" && cbk->cycle()>maxCycle){ 
      maxCycle = cbk->cycle(); 
      allEventsCBK = cbk; 
    } 
  } 
  if( !allEventsCBK ){
    Error( APP_NAME, "Failed to retrieve CutBookkeeper for AllExecutedEvents");
    return EL::StatusCode::SUCCESS;
  }

  sumEvents    = (int)allEventsCBK->nAcceptedEvents();
  sumWeights   = (double)allEventsCBK->sumOfEventWeights();
  sumWeightsSq = (double)allEventsCBK->sumOfEventWeightsSquared();    

  Info( APP_NAME, "CutBookkeeper::Events in original xAOD File %i, sumWeights %.1f, sumWeightsSq %.1f", sumEvents, sumWeights, sumWeightsSq);
  xAODEventSum  = sumEvents;
  xAODWeightSum = sumWeights;

  hEventsTotal->Fill(0., sumEvents);
  hEventsTotal->Fill(1., sumWeights);
  hEventsTotal->Fill(2., sumWeightsSq);

  NFiles++;
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode DiLepton :: initialize (){

  if(ccCheck){
    CP::CorrectionCode::enableFailure();
    CP::SystematicCode::enableFailure();
  }
  xAOD::TFileAccessTracer::enableDataSubmission(false);

  const char* APP_NAME = "initialize()"; 
  m_event = wk()->xaodEvent();
  store = *(wk()->xaodStore());
  m_entry = 0;

  Info( APP_NAME, "Starting initialize");
  Info( APP_NAME, "Sample name       = %s",  SampleName.c_str() );
  Info( APP_NAME, "mc15[a=1|b=2|c=3] = %i",  (int)mc15 ); 
  Info( APP_NAME, "mc16[2015/2016/2017/2018] = %i",  (int)mc16 ); 
  Info( APP_NAME, "Debug messages    = %i",  (int)Debug );
  Info( APP_NAME, "Create Histos     = %i",  (int)makeHist );
  Info( APP_NAME, "Create TTree      = %i",  (int)makeTree );
  Info( APP_NAME, "Run systematics   = %i",  (int)doSys );
  Info( APP_NAME, "Set leptons       = %i",  (int)setLeptons );
  Info( APP_NAME, "N signal leptons  = %i",  (int)NSignalLeptons );
  Info( APP_NAME, "Process SR        = %s",  SigRegion.c_str() );
  Info( APP_NAME, "Sys. Variations   = %s",  SysList.c_str() );
  std::cout << std::endl;

  //Initialize SUSY Tools
  Info(APP_NAME, "Initializing Tool: \t %s", "SUSYObjDef_xAOD");
  SusyObjTool = new ST::SUSYObjDef_xAOD( "SUSYObjDef_xAOD" );
  switch( (int)Debug ){ 
    case 1:  SusyObjTool->msg().setLevel( MSG::ERROR ); break;
    case 2:  SusyObjTool->msg().setLevel( MSG::DEBUG ); break;
    case 3:  SusyObjTool->msg().setLevel( MSG::VERBOSE ); break;
    default: SusyObjTool->msg().setLevel( MSG::ERROR );   break;
  }

  //Set PRW config
  std::vector<std::string> PileUpConf, PileUpLumiCalc;
  //std::string prwPath = "$ROOTCOREBIN/data/DiLepton/PileupReweighting/";
  std::string lumiPath = PathResolverFindCalibDirectory("DiLepton/PileupReweighting/");
  std::string prwPath = PathResolverFindCalibDirectory("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/PRW_AUTOCONFIG/files/");
  /*switch( mc15 ){
    case 1:
    Info("PileupReweighting", "Using mc15%s", "a");
    PileUpConf.push_back(prwPath+"merged_prw.root");
    PileUpConf.push_back(prwPath+"mergedSignalsSS3L_prw.root");
    break;
    case 2:
    Info("PileupReweighting", "Using mc15%s", "b");
    PileUpConf.push_back(prwPath+"my.prw_410000_mc15b.root");
    break;
    case 3:
    Info("PileupReweighting", "Using mc15%s", "c");
    PileUpConf.push_back(prwPath+"merged_prw_mc15c_latest.root");
    PileUpConf.push_back(prwPath+"merged_prw_mc15c_signal_latest.root");
    break;
    default:
    Info("PileupReweighting", "Default mc15%s", "c");
    PileUpConf.push_back(prwPath+"merged_prw_mc15c_latest.root");
    PileUpConf.push_back(prwPath+"merged_prw_mc15c_signal_latest.root");
    break;
    }*/
  std::cout << "MC16: " << mc16 << std::endl;

  //switch( mc16 ){
  //  case 2016:
  //    PileUpLumiCalc.push_back(lumiPath+"PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root"); //2015 data
  //    PileUpLumiCalc.push_back(lumiPath+"PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root");  /// 2016 data
  //  case 2017:
    PileUpLumiCalc.push_back(lumiPath+"physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root"); //2017 data
  //  case 2018:
  //    PileUpLumiCalc.push_back(lumiPath+"physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-001.root"); 
  //}

  //Set shower type
  int ShowerType = !isData ? SusyObjTool->getMCShowerType(SampleName) : 0;

  //Set other SUSYTools configurations
  ST::ISUSYObjDef_xAODTool::DataSource DataSource = (isData ? ST::ISUSYObjDef_xAODTool::Data : (isAtlfast ? ST::ISUSYObjDef_xAODTool::AtlfastII : ST::ISUSYObjDef_xAODTool::FullSim));
  //std::string ConfigFile = "$ROOTCOREBIN/data/DiLepton/ST_DiLepton.conf";
  std::string ConfigFile = PathResolverFindCalibFile("DiLepton/SS3L_Default.conf");

  ANA_CHECK( SusyObjTool->setProperty("DataSource", DataSource) );
  ANA_CHECK( SusyObjTool->setProperty("ConfigFile", ConfigFile) ); 
  ANA_CHECK( SusyObjTool->setProperty("ShowerType", ShowerType) );
  ANA_CHECK( SusyObjTool->setProperty("DebugMode",  (Debug>1)) );
  //ANA_CHECK( SusyObjTool->setProperty("PRWConfigFiles",    PileUpConf) );
  ANA_CHECK( SusyObjTool->setProperty("PRWLumiCalcFiles",  PileUpLumiCalc) );
  ANA_CHECK( SusyObjTool->setBoolProperty("AutoconfigurePRWTool", true ) );

  if( SusyObjTool->initialize() != StatusCode::SUCCESS){
    Error( APP_NAME, "Cannot intialize SUSYTools" );
    return EL::StatusCode::FAILURE;
  } 
  Info( APP_NAME, "SUSYObjDef_xAOD initialized... " );
  Info( APP_NAME, "ConfigFile = %s", ConfigFile.c_str());
  Info( APP_NAME, "IsData     = %i", isData);
  Info( APP_NAME, "IsAtlfast  = %i", isAtlfast);
  Info( APP_NAME, "ShowerType = %i", ShowerType);

  //Reset systematics
  if( !this->resetSystematics() ){  
    Error(APP_NAME, "Cannot reset systematics" );
    return EL::StatusCode::FAILURE;
  }

  //Set some other flags
  usePhotons = true;
  useTaus    = false;
  useZVeto   = false;
  doSys   = (doSys && !isData);
  doPDF   = false;
  applySF = true;
  ccCheck  = false;
  truthMsg = false;
  doJetCut = true;
  doCFTag  = true;
  doDPSrw  = true;
  doPileup = PileupReweighting;
  Info( APP_NAME, "Apply SF  = %i",  applySF);
  Info( APP_NAME, "Do PileupReweighting   = %i", doPileup);
  Info( APP_NAME, "Use Photons = %i", usePhotons);
  Info( APP_NAME, "Use Taus    = %i", useTaus);
  Info( APP_NAME, "Get R1 jets = %i", getR1Jets);

  //Set lepton-pt cuts
  lep_PtCut[0] = 20000.;
  lep_PtCut[1] = 20000.;
  lep_PtCut[2] = 10000.;

  //Jet/bjet-pt cuts
  jet_PtCut[0] = 50000.;
  jet_PtCut[1] = 20000.;

  Info( APP_NAME, "Signal objects:");
  Info( APP_NAME, "Lepton[1st|2nd|3rd] ::  PtCut = %.0f, %.0f, %.0f", lep_PtCut[0], lep_PtCut[1], lep_PtCut[2]);
  Info( APP_NAME, "Jets[LF|HF]         ::  PtCut = %.0f, %.0f,", jet_PtCut[0], jet_PtCut[1] );

  //SignalRegions(Name, Nj, Nb, NbMax, pT(j), MET, Meff, pairing)
  SignalRegions.clear();
  this->addSR("SRall",    0,  0, -1,  0,       0.,       0., "2L+1"); 
  this->addSR("SR1b-e",   6,  1, -1,  25,  150000.,  600000., "2L+1");
  this->addSR("SR0b-c",   6,  0, -1,  25,  150000.,      0., "2L+1");
  this->addSR("SR1b1-f",  0,  1, -1,  25,  150000.,  750000., "2L+1");
  this->addSR("SR3b2-i",  6,  3, -1,  25,  250000., 1200000., "2L+1");  
  this->addSR("SR1b1",   6,  1, -1,  25,  150000.,  600000., "2L+1");
  this->addSR("SR1b2",   6,  1, -1,  25,  250000., 1250000., "2L+1");
  this->addSR("SR3b1",   6,  3, -1,  25,  150000.,  750000., "2L+1");
  this->addSR("SR3b2",   6,  3, -1,  25,  250000., 1200000., "2L+1");  
  this->addSR("SR0b1",   6,  0,  0,  25,  150000.,  600000., "2L+1");
  this->addSR("SR0b2",   6,  0,  0,  40,  250000.,  900000., "2L+1");
  this->addSR("SR3L0b1", 4,  0,  0,  40,  200000.,  600000., "3L"  );
  this->addSR("SR3L0b2", 4,  0,  0,  40,  200000., 1600000., "3L"  );
  this->addSR("SR3L1b1", 4,  1, -1,  40,  200000.,  600000., "3L"  );
  this->addSR("SR3L1b2", 4,  1, -1,  40,  200000., 1600000., "3L"  );

  //Select only specific SR
  this->checkNames(SignalRegions, this->tokenize(SigRegion) );
  Info( APP_NAME, "Running on %i Signal Regions", (int)SignalRegions.size() );

  //GoodRunList
  Info(APP_NAME, "Initializing Tool: \t %s", "GoodRunsListSelectionTool"); 
  GRLTool = new GoodRunsListSelectionTool("GoodRunsListSelectionTool");
  std::vector<std::string> GRLs;
  //TString GRL2015 = "$ROOTCOREBIN/data/DiLepton/GRL/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml";
  //TString GRL2016 = "$ROOTCOREBIN/data/DiLepton/GRL/data16_13TeV.periodAllYear_DetStatus-v88-pro20-21_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml";
  /*TString GRL2015 = PathResolverFindCalibFile("DiLepton/GRL/data15_13TeV.periodAllYear_DetStatus-v89-pro21-02_Unknown_PHYS_StandardGRL_All_Good_25ns.xml");
    TString GRL2016 = PathResolverFindCalibFile( "DiLepton/GRL/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml");
    TString GRL2017 = PathResolverFindCalibFile( "data17_13TeV.periodAllYear_DetStatus-v97-pro21-13_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml");*/
  TString GRL2015 = PathResolverFindCalibFile("DiLepton/GRL/physics_25ns_21.0.19_2015.xml");
  TString GRL2016 = PathResolverFindCalibFile("DiLepton/GRL/physics_25ns_21.0.19_2016.xml");
  TString GRL2017 = PathResolverFindCalibFile("DiLepton/GRL/data17_13TeV.periodAllYear_DetStatus-v97-pro21-13_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml");
  TString GRL2018 = PathResolverFindCalibFile("DiLepton/GRL/data18_13TeV.periodAllYear_DetStatus-v102-pro22-03_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml");

  GRLs.push_back( GRL2015.Data() );
  GRLs.push_back( GRL2016.Data() );
  GRLs.push_back( GRL2017.Data() );
  GRLs.push_back( GRL2018.Data() );
  ANA_CHECK( GRLTool->setProperty( "GoodRunsListVec", GRLs) );
  ANA_CHECK( GRLTool->setProperty( "PassThrough", false) ); 
  ANA_CHECK( GRLTool->initialize() );
  for(auto GRL : GRLs) Info(APP_NAME, "Initialized GoodRunList %s", GRL.c_str());

  //Set Triggers
  Info(APP_NAME, "Set trigger configuration: \t %s", TriggerName.c_str());
  if( !this->setTriggers(triggerList2015, TriggerName, 2015) ){
    Error(APP_NAME, "Cannot set triggers" );
    return EL::StatusCode::FAILURE;
  }
  if( !this->setTriggers(triggerList2016, TriggerName, 2016) ){
    Error(APP_NAME, "Cannot set triggers" ); 
    return EL::StatusCode::FAILURE;
  }
  if( !this->setTriggers(triggerList2017, TriggerName, 2017) ){
    Error(APP_NAME, "Cannot set triggers" );
    return EL::StatusCode::FAILURE;
  }
  if( !this->setTriggers(triggerList2017_PS, "DILEPTON_MET_PRESCALE", 2017) ){
    Error(APP_NAME, "Cannot set triggers" );
    return EL::StatusCode::FAILURE;
  }

  //Set PDF
  /*if( doPDF && !isData ){
    std::string setName = this->getPDFName(SampleName, 0);
    Info( APP_NAME, "Initializing PDF set %s", setName.c_str() );

    PdfSet = new LHAPDF::PDFSet(setName);
    PDFs = PdfSet->mkPDFs(); 
    }
    else{ Info( APP_NAME, "No PDF set initialized" ); }
    */

  //Initialize systematics
  SysInfoList = SusyObjTool->getSystInfoList();
  this->checkNames(SysInfoList, this->tokenize(SysList) );
  Info( APP_NAME, "Number of found systematics %i", (int)SysInfoList.size()-1 );

  SysInfoList_W = this->getSysInfoList(doSys, SysInfoList, Weight);
  Info( APP_NAME, "Systematics :: Weight-affecting %i", (int)SysInfoList_W.size()-1 );
  PrintSys(SysInfoList_W);

  SysInfoList_K = this->getSysInfoList(doSys, SysInfoList, Kinematic);
  Info( APP_NAME, "Systematics :: Kinematics-affecting %i", (int)SysInfoList_K.size()-1 );
  PrintSys(SysInfoList_K);

  //Intialize Jet reclustering tool
  Info(APP_NAME, "Initializing Tool: \t %s", "JetReclusteringTool");
  JRTool = new JetReclusteringTool("JRTool");
  JRTool->msg().setLevel( MSG::ERROR );
  ANA_CHECK( JRTool->setProperty("InputJetContainer",  "InputJets") );
  ANA_CHECK( JRTool->setProperty("OutputJetContainer", "OutputJets") );
  ANA_CHECK( JRTool->setProperty("RCJetSubjetRadius", 0.4) );
  ANA_CHECK( JRTool->setProperty("ReclusterRadius",   1.0) );
  ANA_CHECK( JRTool->setProperty("InputJetPtMin", jet_PtCut[1]/1000.) );
  ANA_CHECK( JRTool->setProperty("RCJetPtMin",    jet_PtCut[0]/1000.) );
  ANA_CHECK( JRTool->setProperty("RCJetPtFrac",   0.05) );
  ANA_CHECK( JRTool->setProperty("DoArea", false) );  
  ANA_CHECK( JRTool->initialize() );

  //Initialize MCTruth Classifier
  Info(APP_NAME, "Initializing Tool: \t %s", "MCTruthClassifier");
  MCClassifier = new MCTruthClassifier("MCClassifier"); 
  MCClassifier->msg().setLevel( MSG::ERROR );
  ANA_CHECK( MCClassifier->initialize() );

  //Initialize ChargeFlipTools
  Info(APP_NAME, "Initializing Tool[Medium,Loose]: \t %s", "ElectronChargeIDSelectorTool");
  //std::string trainFile = PathResolverFindCalibFile("ElectronPhotonSelectorTools/ChargeID/ECIDS_20180731rel21Summer2018.root");
  std::string trainFile = PathResolverFindCalibFile("ElectronPhotonSelectorTools/ChargeID/ECIDS_20161125for2017Moriond.root");
  float BDT_OP(0.);
  CFTool = new AsgElectronChargeIDSelectorTool("ChargeFlipToolMedium");
  CFTool->msg().setLevel( MSG::WARNING );
  //std::string trainFile = "$ROOTCOREBIN/data/ElectronPhotonSelectorTools/ECIDS_20161125for2017Moriond.root";
  ANA_CHECK( CFTool->setProperty("TrainingFile", trainFile) );
  ANA_CHECK( CFTool->setProperty("WorkingPoint", "MediumLHElectron"));
  ANA_CHECK( CFTool->setProperty("CutOnBDT", BDT_OP) );
  ANA_CHECK( CFTool->initialize() );

  CFToolLoose = new AsgElectronChargeIDSelectorTool("ChargeFlipToolLoose");
  CFToolLoose->msg().setLevel( MSG::WARNING );
  ANA_CHECK( CFToolLoose->setProperty("TrainingFile", trainFile) );
  ANA_CHECK( CFToolLoose->setProperty("WorkingPoint", "LooseLHElectron"));
  ANA_CHECK( CFToolLoose->setProperty("CutOnBDT", BDT_OP) );
  ANA_CHECK( CFToolLoose->initialize() );


  // Intitalize Trigger SF tool
  /*Info(APP_NAME, "Initializing Tool: \t %s", "TrigGlobalEfficiencyCorrectionTool");
    if( !this->InitializeTriggerTools() ){ 
    Error(APP_NAME, "Cannot initialize trigger SF Tools. Exiting." ); return EL::StatusCode::FAILURE; }
    TriggerSFTool = new TrigGlobalEfficiencyCorrectionTool("TrigGlobalEfficiencyCorrectionTool");
    ANA_CHECK( TriggerSFTool->setProperty("ElectronEfficiencyTools",electronEffTools) );
    ANA_CHECK( TriggerSFTool->setProperty("ElectronScaleFactorTools",electronSFTools) );
    ANA_CHECK( TriggerSFTool->setProperty("MuonTools",muonTools) );
    ANA_CHECK( TriggerSFTool->setProperty("ListOfLegsPerTool",LegsPerTool) );
    ANA_CHECK( TriggerSFTool->setProperty("TriggerCombination2015", "2e12_lhloose_L12EM10VH || mu18_mu8noL1 || e17_lhloose_mu14") );
    ANA_CHECK( TriggerSFTool->setProperty("TriggerCombination2016", "2e17_lhvloose_nod0 || e17_lhloose_nod0_mu14 || mu22_mu8noL1") );
    ANA_CHECK( TriggerSFTool->initialize() );

    Info(APP_NAME, "Initializing Tool: \t %s", "TrigGlobalEfficiencyCorrectionToolUP");
    if( !this->InitializeTriggerTools("UP") ){ 
    Error(APP_NAME, "Cannot initialize trigger SF Tools. Exiting." ); return EL::StatusCode::FAILURE; }
    TriggerSFToolUP = new TrigGlobalEfficiencyCorrectionTool("TrigGlobalEfficiencyCorrectionToolUP");
    ANA_CHECK( TriggerSFToolUP->setProperty("ElectronEfficiencyTools",electronEffTools) );
    ANA_CHECK( TriggerSFToolUP->setProperty("ElectronScaleFactorTools",electronSFTools) );
    ANA_CHECK( TriggerSFToolUP->setProperty("MuonTools",muonTools) );
    ANA_CHECK( TriggerSFToolUP->setProperty("ListOfLegsPerTool",LegsPerTool) );
    ANA_CHECK( TriggerSFToolUP->setProperty("TriggerCombination2015", "2e12_lhloose_L12EM10VH || mu18_mu8noL1 || e17_lhloose_mu14") );
    ANA_CHECK( TriggerSFToolUP->setProperty("TriggerCombination2016", "2e17_lhvloose_nod0 || e17_lhloose_nod0_mu14 || mu22_mu8noL1") );
    ANA_CHECK( TriggerSFToolUP->initialize() );

    Info(APP_NAME, "Initializing Tool: \t %s", "TrigGlobalEfficiencyCorrectionToolDOWN");
    if( !this->InitializeTriggerTools("DOWN") ){ 
    Error(APP_NAME, "Cannot initialize trigger SF Tools. Exiting." ); return EL::StatusCode::FAILURE; }
    TriggerSFToolDOWN = new TrigGlobalEfficiencyCorrectionTool("TrigGlobalEfficiencyCorrectionToolDOWN");
    ANA_CHECK( TriggerSFToolDOWN->setProperty("ElectronEfficiencyTools",electronEffTools) );
    ANA_CHECK( TriggerSFToolDOWN->setProperty("ElectronScaleFactorTools",electronSFTools) );
    ANA_CHECK( TriggerSFToolDOWN->setProperty("MuonTools",muonTools) );
    ANA_CHECK( TriggerSFToolDOWN->setProperty("ListOfLegsPerTool",LegsPerTool) );
    ANA_CHECK( TriggerSFToolDOWN->setProperty("TriggerCombination2015", "2e12_lhloose_L12EM10VH || mu18_mu8noL1 || e17_lhloose_mu14") );
    ANA_CHECK( TriggerSFToolDOWN->setProperty("TriggerCombination2016", "2e17_lhvloose_nod0 || e17_lhloose_nod0_mu14 || mu22_mu8noL1") );
    ANA_CHECK( TriggerSFToolDOWN->initialize() );
    */
  //Set Histograms

  if( !this->InitializeHistograms() ){
    Error(APP_NAME, "Cannot intialize histograms. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  //Set Tree
  if( !this->InitializeTree() ){
    Error(APP_NAME, "Cannot intialize TTree. Exiting." );
    return EL::StatusCode::FAILURE;
  } 

  Info( APP_NAME, "returning StatusCode::SUCCESS" );
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode DiLepton :: execute (){

  const char* APP_NAME = "DiLepton::execute()";

  if((m_entry % 1000)==0) Info(APP_NAME, "Already processed %i events", m_entry);
  //if(Debug){std::cout << APP_NAME << " DEBUG \t Event no. \t "<< m_entry << std::endl;}  
  m_entry++; m_fileentry++;

  const xAOD::EventInfo* eventInfo(0);
  if( ! m_event->retrieve(eventInfo, "EventInfo").isSuccess() ){
    Error(APP_NAME, "Failed to retrieve event info collection. Exiting." );
    return EL::StatusCode::FAILURE;
  }
  if(eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION )) isMC = true;
  else isMC = false;

  RunNumber   = eventInfo->runNumber();
  EventNumber = eventInfo->eventNumber();
  IntPerX = SusyObjTool->GetCorrectedAverageInteractionsPerCrossing();
  if (
  //    EventNumber!=208049 &&
  //    EventNumber!=240476 &&
  //    EventNumber!=245964 &&
  //    EventNumber!=276233 &&
  //    EventNumber!=278982 &&
  //    EventNumber!=313214 &&
  //    EventNumber!=330860 &&
  //    EventNumber!=344346 &&
      EventNumber!=388841 
  //    EventNumber!=208182 &&
  //    EventNumber!=214402 &&
  //    EventNumber!=234546 &&
  //    EventNumber!=261722 &&
  //    EventNumber!=319986
     ) {return true;
      if(Debug){std::cout << "Found EventNumber: " << EventNumber << std::endl; }
  }


  //Getting informations on lumi block and average pile-up (mainly for trigger studies) PT-04-07-'18
  AverageMu = eventInfo->averageInteractionsPerCrossing();
  LB = eventInfo->lumiBlock();

  MCId = isMC ? eventInfo->mcChannelNumber() : 0;  
  EventWeight = isMC ? (eventInfo->mcEventWeights()).at(0) : 1.;


  //if(EventNumber!=59956 && EventNumber!=59983)
  //return EL::StatusCode::SUCCESS;

  if(Debug){std::cout << APP_NAME << " DEBUG \t " 
    << Form("EventNumber %ld \t RunNumber %ld \t MCId %i \t IntPerX %i", EventNumber, RunNumber, MCId, IntPerX) << std::endl;}

  //PileUp Reweighting
  if(isMC) ANA_CHECK( SusyObjTool->ApplyPRWTool() );
  //ANA_CHECK( SusyObjTool->ApplyPRWTool() );
  PileUpWeight = isMC ? this->getPileupWeight() : 1.;
  PileUpHash   = isMC ? this->getPileupHash()   : 1.;
  //randomRN = eventInfo->isAvailable<unsigned int>("RandomRunNumber") ? eventInfo->auxdata<unsigned int>("RandomRunNumber") : -1 ; 
  randomRN = -1;
  if(isMC) randomRN = SusyObjTool->GetRandomRunNumber();
  configYear = SusyObjTool->treatAsYear();  
  eventrunNumber = SusyObjTool->GetRunNumber();  
  int dataPeriod = 0;
  if (configYear==2016){
    if (eventrunNumber>296930 && eventrunNumber<302880) dataPeriod=1;
    if (eventrunNumber>302881 && eventrunNumber<304010) dataPeriod=2;
    if (eventrunNumber>304011) dataPeriod=3;
  }

  //if(RunNumber<290000) 
  //  configYear = 2015;/// 2015 data
  //if(RunNumber>290000 && RunNumber<320000)
  //  configYear = 2016;
  //if (RunNumber>320000)
  //  configYear = 2017;/// 2016 data
    
  if(Debug){std::cout << APP_NAME <<" DEBUG \t PileupReweightingTool::Pileupweight "<<PileUpWeight<<"  RandomRunNumber "<<randomRN<<"   RunNumber   "<<eventrunNumber<<"  PileupHash "<<PileUpHash<<"  configYear "<<configYear<< std::endl;} 
  EventWeightPileUp = EventWeight*PileUpWeight;
  if(Debug){std::cout << APP_NAME <<" DEBUG \t Event weight "<<  EventWeight <<"\t  weight*PileUp "<< EventWeightPileUp << std::endl;}

  //Sherpa reweighting
  bool doSherpaRW = false;
  if( doSherpaRW ){
    float sherpaSF = SusyObjTool->getSherpaVjetsNjetsWeight();
    EventWeightPileUp *= sherpaSF;
    if(Debug){std::cout << APP_NAME <<" DEBUG \t Sherpa weight "<< sherpaSF <<"\t weight*PileUp*sherpaSF "<< EventWeightPileUp << std::endl;}
  }

  //SUSY truth numbers
  if( isMC ){
    std::vector<int> pdgIDs = this->getSusyPDG(m_event);
    PDGId1 = pdgIDs[0];
    PDGId2 = pdgIDs[1];
    GLDec1 = pdgIDs[2];
    GLDec2 = pdgIDs[3];  
  }

  //Start loop on signal regions  
  for(unsigned int sr(0); sr<NSr; ++sr){  
    SR SignalRegion = SignalRegions.at(sr);
    if(Debug) this->PrintSR(SignalRegion, "DEBUG");

    this->resetSystematics(0);
    for(unsigned int sys(0); sys<NSys; sys++){
      this->applySystematic( (CP::SystematicSet)SysInfoList_K[sys].systset, Debug);

      // Clear all variables
      weight = EventWeightPileUp;
      SysWeights.clear();
      mcWeightVariations.clear();
      PDFWeights.clear();
      CFTWeights.clear();
      //TrigWeights.clear();      
      SysNames.clear();
      store.clear();

      muTLV.clear();
      elTLV.clear();
      lepTLV.clear();
      muTLV_GL.clear();
      elTLV_GL.clear();
      lepTLV_GL.clear();
      lepTLV_BL.clear();
      jetTLV.clear();
      jetBtag.clear();
      jetTLV_R1.clear();
      jetID_R1.clear();
      jetWtag_R1.clear();
      jetN_R1.clear();
      jetD2_R1.clear();
      jetC2_R1.clear();
      jetD12_R1.clear();
      muCharges.clear();
      elCharges.clear();
      lepCharges.clear();
      jetCharges.clear();
      elQualities.clear();
      muQualities.clear();
      lepQualities.clear();
      muCharges_GL.clear();
      elCharges_GL.clear();
      lepCharges_GL.clear();
      elQualities_GL.clear();
      muQualities_GL.clear();
      lepQualities_GL.clear();
      jetQualities.clear();
      lepCFmedium.clear();
      lepCFloose.clear();
      lepTruth.clear();

      muSF.clear();
      elSF.clear();
      jetMV2.clear();
      jetJVT.clear();  
      triggerInfo.clear();    
      topoStr.Clear();

      SSChannel = 0;
      IntPerX   = 0;
      isZevent  = 0;
      trigSFerr = 0;

      mu_1stPt  = 0;
      mu_2ndPt  = 0;
      mu_3rdPt  = 0;
      mu_1stEta = 0;
      mu_2ndEta = 0;

      el_1stPt  = 0;
      el_2ndPt  = 0;
      el_3rdPt  = 0;
      el_1stEta = 0;
      el_2ndEta = 0;
      el_1stTEta = 0;
      el_2ndTEta = 0;

      lep_1stPt = 0;
      lep_2ndPt = 0;
      lep_3rdPt = 0;

      jet_1stPt = 0;
      jet_2ndPt = 0;
      jet_3rdPt = 0;

      EtMiss    = 0;
      EtMissPhi = 0;
      meff      = 0;
      minv      = 0;
      mt        = 0;
      ht        = 0;
      mj        = 0;
      mjj       = 0;
      mjjdR     = 0;
      mjRec     = 0;
      truthHt   = 0;
      truthMET  = 0;

      mu_n     = 0;
      el_n     = 0;
      lep_n    = 0;
      jet_n    = 0;
      b_n      = 0;

      // Create & store xAOD Containers
      xAOD::MuonContainer*         GoodMuons = new xAOD::MuonContainer(SG::VIEW_ELEMENTS);
      xAOD::ElectronContainer* GoodElectrons = new xAOD::ElectronContainer(SG::VIEW_ELEMENTS);
      xAOD::JetContainer*           GoodJets = new xAOD::JetContainer(SG::VIEW_ELEMENTS);     
      xAOD::JetContainer*         GoodJetsR1 = new xAOD::JetContainer(SG::VIEW_ELEMENTS);

      xAOD::MissingETContainer*       met = new xAOD::MissingETContainer;
      xAOD::MissingETAuxContainer* metAux = new xAOD::MissingETAuxContainer;
      met->setStore(metAux);

      ANA_CHECK( store.record(GoodMuons,     "SelectedGoodMuons") );
      ANA_CHECK( store.record(GoodElectrons, "SelectedGoodElectrons") );  
      ANA_CHECK( store.record(GoodJets,      "SelectedGoodJets") );
      ANA_CHECK( store.record(GoodJetsR1,    "SelectedGoodJetsR1") );

      ANA_CHECK( store.record(met,           "MET_RefFinal") );
      ANA_CHECK( store.record(metAux,        "MET_RefFinalAux.") );

      //Retrieving info of truth particles
      truthHt  = isMC ? this->getTruthVar(m_event, 0) : 0.;
      truthMET = isMC ? this->getTruthVar(m_event, 1) : 0.;

      //Fill event properties
      FillHisto(hEvents[sr][sys], 0);
      FillHisto(hEvents[sr][sys], 1,   EventWeight);
      FillHisto(hEventWeight[sr][sys], EventWeight);

      // Needed for theory uncertainties
      //if (!isData) {
      //  const xAOD::TruthEventContainer* truthevents = 0;
      //  if ( !( (evtStore()->retrieve( truthevents, "TruthEvents" )).isSuccess()) ) {    
      //        std::cout << "Can't retrieve TruthEvents " << std::endl;
      //  }
      //  if (truthevents->size()>0) {
      //    const xAOD::TruthEvent *truthevent = (*truthevents)[0];
      //    std::vector<float> variationweights = truthevent->weights();
      //    if (variationweights.size()>0) {
      //      for (int i=0;i<=variationweights.size();i++) {
      //        mcWeightVariations.push_back(variationweights[i]);
      //      }
      //    }
      //  }
      //}

      //Fill Control Tree
      bool nom = (!sys && !sr); 
      if(nom) this->FillTree(ControlTree, (bool)makeTree);

      //Starting CutFlow
      Int_t CutFlowIndex(0);
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;

      //Cut on truth variables
      bool doTruthCut = true;
      if( doTruthCut && truthVeto(MCId, truthHt, truthMET) ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;

      //GRL Cut
      bool doGRL = true;
      if( doGRL && !isMC && !GRLTool->passRunLB(RunNumber, eventInfo->lumiBlock()) ) continue; 
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;

      //Check for primary vertex  
      const xAOD::Vertex* PV = SusyObjTool->GetPrimVtx();
      int nTrk = PV ? PV->nTrackParticles() : 0;
      if(Debug){std::cout << APP_NAME << " DEBUG \t Number of PV tracks \t "<< nTrk << std::endl;} 
      if( !nTrk ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;

      setROOTmsg(0);
      //***** Retrieving baseline muons *****
      xAOD::MuonContainer* muons_baseline(0);
      xAOD::ShallowAuxContainer* muons_baselineAux(0);
      ANA_CHECK( SusyObjTool->GetMuons(muons_baseline, muons_baselineAux) );

      //***** Retrieving baseline electrons *****
      xAOD::ElectronContainer* electrons_baseline(0);
      xAOD::ShallowAuxContainer* electrons_baselineAux(0);
      ANA_CHECK( SusyObjTool->GetElectrons(electrons_baseline, electrons_baselineAux) );

      // ***** Retrieving baseline photons ****
      xAOD::PhotonContainer* photons_baseline(0);
      xAOD::ShallowAuxContainer* photons_baselineAux(0);
      if(usePhotons) ANA_CHECK( SusyObjTool->GetPhotons(photons_baseline, photons_baselineAux) );

      // ***** Retrieving baseline taus *****
      xAOD::TauJetContainer* taus_baseline(0);
      xAOD::ShallowAuxContainer* taus_baselineAux(0);
      if(useTaus) ANA_CHECK( SusyObjTool->GetTaus(taus_baseline, taus_baselineAux) );

      // ***** Retrieving baseline jets *****
      xAOD::JetContainer* jets_baseline(0);
      xAOD::ShallowAuxContainer* jets_baselineAux(0);
      ANA_CHECK( SusyObjTool->GetJets(jets_baseline, jets_baselineAux) );

      // ***** Retrieving baseline R1 jets *****
      xAOD::JetContainer* jetsR1_baseline(0);
      xAOD::ShallowAuxContainer* jetsR1_baselineAux(0);
      if(getR1Jets==1) ANA_CHECK( SusyObjTool->GetFatJets(jetsR1_baseline, jetsR1_baselineAux) );
      setROOTmsg(1);

      if(Debug){std::cout << APP_NAME << " DEBUG \t No. of Jets in container \t" << (int)(jets_baseline)->size() << std::endl;}
      //(*jet_itr)->auxdata<float>("Jvt");
      for( auto jet: *jets_baseline ) {
        jet->auxdata<float>("jvt") = jet->auxdata<float>("Jvt");
      }


      //Missing Et
      this->getMET(EtMiss, EtMissPhi, met, jets_baseline, electrons_baseline, muons_baseline, photons_baseline, taus_baseline, true, true);
      if(Debug){std::cout << APP_NAME << " DEBUG \t EtMiss " << EtMiss << "  EtMissPhi " << EtMissPhi << std::endl;}

      //Pass trigger
      bool prescale = 0;
      bool passTrigger = true;
      switch( configYear ){
        case 2015:
          if( !isTriggered(triggerList2015, triggerInfo) ) passTrigger = false; 
          if( !isTriggeredMet(triggerList2015, EtMiss, triggerInfo) ) passTrigger = false;
          break;
        case 2016:
          if( !isTriggered(triggerList2016, triggerInfo) ) passTrigger = false; 
          if( !isTriggeredMet2016(triggerList2016, EtMiss, triggerInfo, dataPeriod) ) passTrigger = false; 
          break;
        case 2017:
          if (eventrunNumber>=326834 && eventrunNumber<=328393){ //prescaled
            prescale = 1;
            if( !isTriggered(triggerList2017_PS, triggerInfo) ) passTrigger = false;
            if( !isTriggeredMet(triggerList2017_PS, EtMiss, triggerInfo) ) passTrigger = false;
            //if(Debug){std::cout << "Prescaled::RunNumber: " << eventrunNumber << std::endl;}
            //if(!(SusyObjTool->IsTrigPassed("HLT_2e24_lhvloose_nod0") ||
            //    SusyObjTool->IsTrigPassed("HLT_e17_lhloose_nod0_mu14") ||
            //    SusyObjTool->IsTrigPassed("HLT_mu22_mu8noL1") ||
            //   (EtMiss>250000 && SusyObjTool->IsTrigPassed("HLT_xe110_pufit_L1XE55")))) {
            //    if(Debug){std::cout << "Passed unprescaled" << std::endl;}
            //    passTrigger = false;
            //  }
          }
          if (eventrunNumber<326834 || eventrunNumber>328393){ //unprescaled
            if( !isTriggered(triggerList2017, triggerInfo) ) passTrigger = false;
            if( !isTriggeredMet(triggerList2017, EtMiss, triggerInfo) ) passTrigger = false;
          //  if(Debug){std::cout << "Unprescaled::RunNumber: " << eventrunNumber << std::endl;}
          //  if(SusyObjTool->IsTrigPassed("HLT_2e24_lhvloose_nod0") ||
          //      SusyObjTool->IsTrigPassed("HLT_2e17_lhvloose_nod0_L12EM15VHI") ||
          //      SusyObjTool->IsTrigPassed("HLT_e17_lhloose_nod0_mu14") ||
          //      SusyObjTool->IsTrigPassed("HLT_mu22_mu8noL1") ||
          //      (EtMiss>250000 && SusyObjTool->IsTrigPassed("HLT_xe110_pufit_L1XE55"))) {
          //      passTrigger = true;
          //      if(Debug){std::cout << "Passed unprescaled" << std::endl;}
          //    }
          }
          isTriggered(triggerList2017, triggerInfo);
          break;
          //if( !isTriggered(triggerList2017, triggerInfo) ) passTrigger = false;
          //if( !isTriggeredMet(triggerList2017, EtMiss, triggerInfo) ) passTrigger = false;
          //break;
        default: break;
      }
      if( !passTrigger ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;
      cutflow_file_trig << EventNumber << "\n";

      //LAr errors
      bool isLArError = ((eventInfo->errorState(xAOD::EventInfo::LAr)  == xAOD::EventInfo::Error) 
          || (eventInfo->errorState(xAOD::EventInfo::Tile) == xAOD::EventInfo::Error)
          || (eventInfo->errorState(xAOD::EventInfo::SCT)  == xAOD::EventInfo::Error) 
          || (eventInfo->isEventFlagBitSet(xAOD::EventInfo::Core, 18)) );
      if(!isMC && isLArError ) continue;   
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;

      //Overlap removal
      ANA_CHECK( SusyObjTool->OverlapRemoval(electrons_baseline, muons_baseline, jets_baseline) );

      //Removing bad baseline electrons 
      if( !this->isBaseline(electrons_baseline) ) Error(APP_NAME, "Cannot retrieve electron container");

      //Reset OR flag for non-BL objects 
      this->checkOR(muons_baseline);
      this->checkOR(electrons_baseline);
      this->checkOR(jets_baseline);

      //lepton/lepton OR
      /* 
         if(Debug){std::cout << APP_NAME << " DEBUG \t No. of Muons in container \t" << (int)(muons_baseline)->size() << std::endl;}
         xAOD::MuonContainer::iterator mu_itr = (muons_baseline)->begin();
         xAOD::MuonContainer::iterator mu_end = (muons_baseline)->end();
         for( ; mu_itr != mu_end; ++mu_itr ) {
      //this->isSignalMuon( **mu_itr ); 
      PrintInfo(*mu_itr, APP_NAME, Debug); 
      }

      if(Debug){std::cout << APP_NAME << " DEBUG \t No. of Electrons in container \t" << (int)(electrons_baseline)->size() << std::endl;}
      xAOD::ElectronContainer::iterator el_itr = (electrons_baseline)->begin();
      xAOD::ElectronContainer::iterator el_end = (electrons_baseline)->end();
      for( ; el_itr != el_end; ++el_itr ) {
      //this->isSignalElectron( **el_itr,20000,2 );
      PrintInfo(*el_itr, APP_NAME, Debug);  
      }
      */

      if(usePhotons){
        if(Debug){std::cout << APP_NAME << " DEBUG \t No. of Photons in container \t" << (int)(photons_baseline)->size() << std::endl;}
        xAOD::PhotonContainer::iterator ph_itr = (photons_baseline)->begin();
        xAOD::PhotonContainer::iterator ph_end = (photons_baseline)->end();
        for( ; ph_itr != ph_end; ++ph_itr ) {
          PrintInfo(*ph_itr, APP_NAME, 0);  
        } 
      }

      if(useTaus){
        if(Debug){std::cout << APP_NAME << " DEBUG \t No. of Taus in container \t" << (int)(taus_baseline)->size() << std::endl;}
        xAOD::TauJetContainer::iterator tau_itr = (taus_baseline)->begin();
        xAOD::TauJetContainer::iterator tau_end = (taus_baseline)->end();
        for( ; tau_itr != tau_end; ++tau_itr ) {
          PrintInfo(*tau_itr, APP_NAME, 0);
        }
      }

      if(Debug){std::cout << APP_NAME << " DEBUG \t No. of Jets in container \t" << (int)(jets_baseline)->size() << std::endl;}
      xAOD::JetContainer::iterator jet_itr = (jets_baseline)->begin();
      xAOD::JetContainer::iterator jet_end = (jets_baseline)->end();
      for( ; jet_itr != jet_end; ++jet_itr ) {
        this->isSignalJet( **jet_itr );
        PrintInfo(*jet_itr, APP_NAME, Debug);
      }

      //Loop on good muons
      /*for(mu_itr = (muons_baseline)->begin(); mu_itr!=(muons_baseline)->end(); ++mu_itr ) {       
        if( getQuality( **mu_itr )>0 ) {
        muTLV.push_back( (*mu_itr)->p4() );
        muCharges.push_back( (*mu_itr)->charge() );
        muQualities.push_back( getQuality( **mu_itr ) );

        }
        if( getQuality( **mu_itr )>1 ) GoodMuons->push_back(*mu_itr);     
        }    

      //Loop on good electrons
      for(el_itr = (electrons_baseline)->begin(); el_itr!=(electrons_baseline)->end(); ++el_itr  ) {      
      if( getQuality( **el_itr )>0 ) {
      elTLV.push_back( (*el_itr)->p4() );
      elCharges.push_back( (*el_itr)->charge() );
      elQualities.push_back( getQuality( **el_itr ) );
      }
      if( getQuality( **el_itr )>1 ) GoodElectrons->push_back(*el_itr);       
      } */


      //Loop on good jets
      for(jet_itr = (jets_baseline)->begin(); jet_itr!=(jets_baseline)->end(); ++jet_itr ) {
        if( getQuality( **jet_itr )>1 ) {
          GoodJets->push_back(*jet_itr); 
          jetTLV.push_back( (*jet_itr)->p4() );
          jetCharges.push_back(0);
          jetQualities.push_back( getQuality( **jet_itr) );
          jetMV2.push_back( getWeight(**jet_itr, MV2) );
          jetJVT.push_back( getWeight(**jet_itr, JVT) );
        } 
      }

      //Bad muon veto
      bool badMuons = badMuonVeto(muons_baseline);
      if( badMuons ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;

      //One jet pass OR
      bool jetPassOR = getNjets(jets_baseline, 1);
      if( doJetCut && !jetPassOR ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;

      //Bad jet veto
      bool badJets = badJetVeto(jets_baseline);
      if( badJets ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;

      //One signal jet
      bool signalJet = getNjets(jets_baseline, 2);
      if( doJetCut && !signalJet ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;

      Lep_Jet_OR(electrons_baseline,muons_baseline,jets_baseline);
      this->checkOR(muons_baseline);
      this->checkOR(electrons_baseline);
      this->checkOR(jets_baseline);

      Electron_Muon_OR(electrons_baseline,muons_baseline);
      //Reset OR flag for non-BL objects 
      this->checkOR(muons_baseline);
      this->checkOR(electrons_baseline);
      this->checkOR(jets_baseline);


      if(Debug){std::cout << APP_NAME << " DEBUG \t No. of Muons in container \t" << (int)(muons_baseline)->size() << std::endl;}
      xAOD::MuonContainer::iterator mu_itr = (muons_baseline)->begin();
      xAOD::MuonContainer::iterator mu_end = (muons_baseline)->end();
      for( ; mu_itr != mu_end; ++mu_itr ) {
        this->isSignalMuon( **mu_itr );
        PrintInfo(*mu_itr, APP_NAME, Debug);
      }

      if(Debug){std::cout << APP_NAME << " DEBUG \t No. of Electrons in container \t" << (int)(electrons_baseline)->size() << std::endl;}
      xAOD::ElectronContainer::iterator el_itr = (electrons_baseline)->begin();
      xAOD::ElectronContainer::iterator el_end = (electrons_baseline)->end();
      for( ; el_itr != el_end; ++el_itr ) {
        //this->isSignalElectron( **el_itr );
        this->isSignalElectron( **el_itr,20000,2. );
        //if(Debug){std::cout << " Is Signal Electron: " << this->isSignalElectron( **el_itr,20000,2. ) << std::endl;}
        //if(Debug){std::cout << " Get Quality: " << getQuality( **el_itr ) << std::endl;}
        PrintInfo(*el_itr, APP_NAME, Debug);
      }


      //Loop on good muons
      for(mu_itr = (muons_baseline)->begin(); mu_itr!=(muons_baseline)->end(); ++mu_itr ) {
        if( getQuality( **mu_itr )>0 ) {
          muTLV.push_back( (*mu_itr)->p4() );
          muCharges.push_back( (*mu_itr)->charge() );
          muQualities.push_back( getQuality( **mu_itr ) );

        }
        if( getQuality( **mu_itr )>1 ) {
          GoodMuons->push_back(*mu_itr);
        }
      }

      //Loop on good electrons
      for(el_itr = (electrons_baseline)->begin(); el_itr!=(electrons_baseline)->end(); ++el_itr  ) {
        if( getQuality( **el_itr )>0 ) {
          elTLV.push_back( (*el_itr)->p4() );
          elCharges.push_back( (*el_itr)->charge() );
          elQualities.push_back( getQuality( **el_itr ) );
        }
        if( getQuality( **el_itr )>1 ) {
          GoodElectrons->push_back(*el_itr);
        }
      }



      //Check for cosmic muons
      /*bool isCosmic = cosmicsVeto(muons_baseline);
        if( isCosmic ) continue;
        FillHisto(hCutflow, CutFlowIndex, nom);*/
      CutFlowIndex++;


      //Filling GoodJetsR1 either with large jet collection or reclustered jets
      switch( getR1Jets ){
        case 1:
          for(xAOD::JetContainer::iterator jet=(jetsR1_baseline)->begin(); jet!=(jetsR1_baseline)->end(); ++jet ){ if(isGoodR1Jet(*jet)) GoodJetsR1->push_back(*jet); } 
          break;
        case 2:
          this->executeJRTool(GoodJets, *GoodJetsR1);
          break;
      }
      if( !this->getJetOrigin(GoodJetsR1, m_event, isMC, truthMsg) ) Error( APP_NAME, "Cannot classify JetContainer" );   

      //W-tag for large jets
      /*static JetSubStructureUtils::BosonTag WTagger("medium", "smooth", "$ROOTCOREBIN/data/JetSubStructureUtils/config_13TeV_Wtagging_MC15_Prerecommendations_20150809.dat", false, false);
        for(auto jet : *GoodJetsR1){	
        int truthID = this->truthID(jet, isMC);
        int Wtag  = WTagger.result(*jet, "AK10LCTRIMF5R20");	
        int NSubJ = jet->numConstituents();
        float d2  = this->getSubstrVar(jet,"D2");
        float c2  = this->getSubstrVar(jet,"C2");
        float d12 = this->getSubstrVar(jet,"D12");	  

        jetTLV_R1.push_back( jet->p4() );
        jetID_R1.push_back( (float)truthID );
        jetWtag_R1.push_back( (float)Wtag ); 
        jetN_R1.push_back( (float)NSubJ );
        jetD2_R1.push_back( d2 );
        jetC2_R1.push_back( c2 );
        jetD12_R1.push_back( d12 );    
        }

        for(unsigned int i(0); i<jetTLV_R1.size(); i++){	
        if(Debug){std::cout << APP_NAME << Form(" DEBUG \t Wtagger::Jet(Pt,Eta,Phi,M) = (%.1f, %.3f, %.3f, %.3f) (NSubJets=%i, D2=%.3f, C2=%.3f, D12=%.3f) Wtag=%i [truthID=%i]", 
        (jetTLV_R1[i]).Pt(),(jetTLV_R1[i]).Eta(),(jetTLV_R1[i]).Phi(),(jetTLV_R1[i]).M(),(int)jetN_R1[i],jetD2_R1[i],jetC2_R1[i],jetD12_R1[i],(int)jetWtag_R1[i],(int)jetID_R1[i])<<std::endl;}
        }
        */

      //Merged lepton vector - baseline
      lepTLV = muTLV;
      lepCharges = muCharges;
      lepQualities = muQualities;
      lepTLV.insert(lepTLV.end(), elTLV.begin(), elTLV.end());
      lepCharges.insert(lepCharges.end(), elCharges.begin(), elCharges.end());
      lepQualities.insert(lepQualities.end(), elQualities.begin(), elQualities.end());

      //Copy of original jet TLVs
      std::vector<TLorentzVector> jetTLV_copy = jetTLV;

      //Sort vectors
      sortPt(muTLV,   muCharges,  muQualities);
      sortPt(elTLV,   elCharges,  elQualities);
      sortPt(lepTLV,  lepCharges, lepQualities);
      sortPt(jetTLV,  jetCharges, jetQualities); 

      //Sort jet weights    
      sortPt(jetTLV_copy, jetMV2, jetJVT);

      //Storde B-tag info
      jetBtag = this->getBtags(jetTLV, GoodJets);

      //Apply efficiency SF
      bool doSF = (isMC && applySF);
      ObjectSF = doSF ? this->getTotalSF(GoodMuons, GoodElectrons, GoodJets) : 1.;
      weight *= ObjectSF;
      if(Debug){std::cout << APP_NAME <<" DEBUG \t Total SF of signal objects "<< ObjectSF <<"\t Event weight after SF "<< weight << std::endl;}

      //Charge flip info
      if( doCFTag ){ 
        std::vector< std::vector<float> > chareFlipInfo = this->getChargeFlipInfo(electrons_baseline);
        std::vector<float> elCFm = chareFlipInfo[0];
        std::vector<float> elCFl = chareFlipInfo[1];

        //Setting ChargeFlip info for all Pt sorted baseline leptons
        lepCFmedium.resize(lepTLV.size());
        lepCFloose.resize(lepTLV.size());
        for(unsigned int l(0); l<lepTLV.size(); l++){
          lepCFmedium[l] = 1.;
          lepCFloose[l]  = 1.;
          int elId(0);	  
          for (auto el : *electrons_baseline){ 
            if( getQuality(*el) && match(lepTLV.at(l),el->p4()) ){ lepCFmedium[l] = elCFm.at(elId); lepCFloose[l] = elCFl.at(elId); }
            elId++;
          }
          if(Debug){std::cout << APP_NAME << Form(" DEBUG \t Lepton(Pt=%.1f,Eta=%.3f,Phi=%.3f) :: ChargeFlipTool(LooseWP=%.4f, MediumWP=%.4f)",
              (lepTLV.at(l)).Pt(),(lepTLV.at(l)).Eta(),(lepTLV.at(l)).Phi(),lepCFloose[l],lepCFmedium[l]) << std::endl;}
        }
      }

      //Fill baseline histos
      float l_1stPt(-1.), l_1stEta(-99.);
      float l_2ndPt(-1.), l_2ndEta(-99.);    
      if(lepTLV.size()>0){
        l_1stPt  = lepTLV.at(0).Pt();
        l_1stEta = lepTLV.at(0).Eta();
      }
      if(lepTLV.size()>1){
        l_2ndPt  = lepTLV.at(1).Pt();
        l_2ndEta = lepTLV.at(1).Eta();
      }
      FillHisto(BaselineMet[sr][sys], EtMiss);
      FillHisto(BaselineLepPt[sr][sys], l_1stPt, l_2ndPt);
      FillHisto(BaselineLepEta[sr][sys], l_1stEta, l_2ndEta);


      mj    = jetTLV.size()>0 ? (jetTLV.at(0)).M() : 0.;
      mjRec = jetTLV_R1.size()>0 ? (jetTLV_R1.at(0)).M() : 0.;
      mjj   = jetTLV.size()>1 ? (jetTLV.at(0) + jetTLV.at(1)).M() : 0.;
      mjjdR = this->MjjdRmin(jetTLV); 
      if(Debug){std::cout << APP_NAME << " DEBUG \t "<< Form("Mj=%.0f | Mjj=%.0f | MjjdR=%.0f | MjRec=%.0f", mj, mjj, mjjdR, mjRec) << std::endl;}

      FillHisto(BaselineJet_Mj[sr][sys],  mj);
      FillHisto(BaselineJet_Mjj[sr][sys], mjj);
      FillHisto(BaselineJet_MjjdR[sr][sys], mjjdR);
      FillHisto(BaselineJet_MjRec[sr][sys], mjRec);
      for(unsigned int i(0); i<jetTLV.size();    i++) FillHisto(BaselineJet_MjAll[sr][sys],    (jetTLV.at(i)).M());
      for(unsigned int i(0); i<jetTLV_R1.size(); i++) FillHisto(BaselineJet_MjRecAll[sr][sys], (jetTLV_R1.at(i)).M());     

      //Cut on baseline leptons
      NlepBL    = getLepNumber(muTLV, elTLV, 2.47);
      NlepBLEta = getLepNumber(muTLV, elTLV, 2.0);
      if( NlepBL < 2 ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;
      if(Debug){std::cout << APP_NAME <<" DEBUG \t Cutflow::BaselineLep EventNumber "<<EventNumber<<Form("\t NlepBL = %i (%i for red.Eta)",NlepBL,NlepBLEta) << std::endl;}
      cutflow_file_baseline << EventNumber << "\n";


      //Select signal leptons
      lepTLV_BL = lepTLV;
      unsigned int Nlep = NSignalLeptons;
      this->getSignalLeptons("Muon",     muTLV,  muCharges,  muQualities,  Nlep);
      this->getSignalLeptons("Electron", elTLV,  elCharges,  elQualities,  Nlep);
      this->getSignalLeptons("Lepton",   lepTLV, lepCharges, lepQualities, Nlep);

      //leading (subleading) objects
      mu_1stPt  = (muTLV.size()>0) ? (muTLV.at(0).Pt()) : -1.;
      mu_2ndPt  = (muTLV.size()>1) ? (muTLV.at(1).Pt()) : -1.;
      mu_3rdPt  = (muTLV.size()>2) ? (muTLV.at(2).Pt()) : -1.;
      mu_1stEta = (muTLV.size()>0) ? (muTLV.at(0).Eta()) : -99.;
      mu_2ndEta = (muTLV.size()>1) ? (muTLV.at(1).Eta()) : -99.;

      el_1stPt  = (elTLV.size()>0) ? (elTLV.at(0).Pt()) : -1.;
      el_2ndPt  = (elTLV.size()>1) ? (elTLV.at(1).Pt()) : -1.;
      el_3rdPt  = (elTLV.size()>2) ? (elTLV.at(2).Pt()) : -1.;
      el_1stEta = (elTLV.size()>0) ? (elTLV.at(0).Eta()) : -99.;    
      el_2ndEta = (elTLV.size()>1) ? (elTLV.at(1).Eta()) : -99.;

      lep_1stPt = (lepTLV.size()>0) ? (lepTLV.at(0).Pt()) : -1.;
      lep_2ndPt = (lepTLV.size()>1) ? (lepTLV.at(1).Pt()) : -1.;
      lep_3rdPt = (lepTLV.size()>2) ? (lepTLV.at(2).Pt()) : -1.;


      jet_1stPt = (jetTLV.size()>0) ? (jetTLV.at(0).Pt()) : -1.;
      jet_2ndPt = (jetTLV.size()>1) ? (jetTLV.at(1).Pt()) : -1.;
      jet_3rdPt = (jetTLV.size()>2) ? (jetTLV.at(2).Pt()) : -1.;

      //topoStr = Form("%im %ie %il %ij",(int)muTLV_GL.size(), (int)elTLV_GL.size(), (int)lepTLV_GL.size(), (int)jetTLV.size() );
      topoStr = Form("%im %ie %il %ij",(int)muTLV.size(), (int)elTLV.size(), (int)lepTLV.size(), (int)jetTLV.size() );
      if(Debug){std::cout << APP_NAME << " DEBUG \t Topology before SR cuts "<< topoStr << std::endl;}

      //Fill trigger histos
      FillHisto(TrigMet[sr][sys], EtMiss);
      FillHisto(TrigJetPt[sr][sys], jet_1stPt, jet_2ndPt);
      FillHisto(TrigElPt[sr][sys],  el_1stPt,  el_2ndPt );
      FillHisto(TrigElEta[sr][sys], el_1stEta, el_2ndEta);    
      FillHisto(TrigMuPt[sr][sys],  mu_1stPt,  mu_2ndPt );
      FillHisto(TrigMuEta[sr][sys], mu_1stEta, mu_2ndEta);

      //Cut on signal leptons
      if( !hasLeptons(lepTLV) ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;
      if(Debug){std::cout << APP_NAME << " DEBUG \t Cutflow::SignalLep EventNumber "<< EventNumber << Form("\t NlepS=%i",(int)lepTLV.size()) << std::endl;}
      cutflow_file_all << EventNumber << "\n";


      //SS selection
      //bool muSS  = SSleptons(muTLV,  muCharges);
      //bool elSS  = SSleptons(elTLV,  elCharges);
      bool lepSS = SSleptons(lepTLV, lepCharges);
      //bool muSS  = SSleptons(SignalRegion,  muTLV,  muCharges);
      //bool elSS  = SSleptons(SignalRegion,  elTLV,  elCharges);
      //bool lepSS = SSleptons(SignalRegion, lepTLV, lepCharges);

      bool SameSign = (lepSS );
      //bool SameSign = !setLeptons ? lepSS : (setLeptons==1 ? muSS : elSS);
      if(Debug){std::cout << APP_NAME << " DEBUG \t Lepton config = " << setLeptons << " \t found SS event = " << SameSign << std::endl;}

      if( !SameSign ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;
      cutflow_file_SS << EventNumber << "\n";

      //Trigger matching
      bool trigMatched = this->isTrigMatch(configYear, GoodMuons, GoodElectrons, EtMiss, triggerInfo, true, dataPeriod, prescale);
      if( !trigMatched ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;


      ////Z Veto
      //isZevent = (isResonance(muTLV, muCharges) || isResonance(elTLV, elCharges));
      //if( useZVeto && isZevent ) continue;
      //FillHisto(hCutflow, CutFlowIndex, nom);
      //CutFlowIndex++;


      ////Get SS pair
      //std::vector <TLorentzVector> SSTLV;
      //SSTLV = !setLeptons ? SSpair(lepTLV,lepCharges) : (setLeptons==1 ? SSpair(muTLV,muCharges) : SSpair(elTLV,elCharges));
      //if(Debug){std::cout << APP_NAME <<" DEBUG \t SS1(Pt,eta,phi,E): ("<<SSTLV.at(0).Pt()<<", "<<SSTLV.at(0).Eta()<<", "<<SSTLV.at(0).Phi()<<", "<<SSTLV.at(0).E() << ") \n" 
      //  << APP_NAME <<" DEBUG \t SS2(Pt,eta,phi,E): ("<<SSTLV.at(1).Pt()<<", "<<SSTLV.at(1).Eta()<<", "<<SSTLV.at(1).Phi()<<", "<<SSTLV.at(1).E() << ")" 
      //    << std::endl;}

      //SSChannel = this->getChannel(SSTLV, muTLV, elTLV, setLeptons);
      //if(SSChannel==1){
      //  FillHisto(hCutflow, CutFlowIndex, nom);
      //  cutflow_file_mumu << EventNumber << "\n";
      //}
      //CutFlowIndex++;
      //if(SSChannel==2){
      //  FillHisto(hCutflow, CutFlowIndex, nom);
      //  cutflow_file_ee << EventNumber << "\n";
      //}
      //CutFlowIndex++;
      //if(SSChannel==3){
      //  FillHisto(hCutflow, CutFlowIndex, nom);
      //  cutflow_file_emu << EventNumber << "\n";
      //}
      //CutFlowIndex++;

      //Apply trigger SF
      /*      bool globTrigSF = true;
              std::vector< float > triggerSFvec;
              if(globTrigSF){
              triggerSFvec = this->getGlobTriggerSF(configYear, GoodMuons, GoodElectrons, EtMiss, triggerInfo, doSF, Debug);
              triggerSF = triggerSFvec.at(0);
              } 
              else{
              triggerSF = this->getTriggerSF(configYear, randomRN, GoodMuons, GoodElectrons, trigSFerr, EtMiss, triggerInfo, doSF, Debug);
              triggerSFvec.push_back(triggerSF);
              triggerSFvec.push_back(triggerSF+trigSFerr);
              triggerSFvec.push_back(triggerSF-trigSFerr);
              }
              weight *= triggerSF;
              if(Debug){std::cout << APP_NAME <<" DEBUG \t Event weight after trigger SF "<< weight << std::endl;}
              */
      //Store weigths for TTree
      muSF = getSF(GoodMuons, doSF, true);
      elSF = getSF(GoodElectrons, doSF, true);

      //ChargeFlip SF
      chargeFlipSF = doSF ? SusyObjTool->GetTotalElectronSF(*GoodElectrons,0,0,0,0,"",1) : 1.;
      weight *= chargeFlipSF;
      if(Debug){std::cout << APP_NAME <<" DEBUG \t ChargeFlip SF " << chargeFlipSF << "\t Event weight after CHF SF " << weight << std::endl;}

      //Set weights
      if(doSys && doSF){
        SysWeights  = this->getSysWeights(GoodMuons, GoodElectrons, GoodJets, SysInfoList_W, SysNames, sys); 
        //PDFWeights  = this->getPDFWeights(m_event);
        CFTWeights  = this->getChargeFlipSF(GoodElectrons, sys);
        //TrigWeights = triggerSFvec;
      }

      //Store truth info
      lepTruth.resize(lepTLV.size());
      if(isMC) lepTruth = this->getTruthInfo(lepTLV, GoodMuons, GoodElectrons);

      //Apply DPS reweighting 
      float dpsWeight = (doSF && doDPSrw) ? this->getDPSweight(MCId, (int)jetTLV.size()) : 1.;
      weight *= dpsWeight;

      // Good muons
      if(Debug){std::cout << APP_NAME << " DEBUG \t Final no. of good muons \t" << (int)muTLV.size() << std::endl;}
      float mu_sumPt(0.);
      TLorentzVector V_mu;    
      for(std::vector<TLorentzVector>::iterator muItr=muTLV.begin(); muItr!=muTLV.end(); muItr++){
        if(Debug) Printf("\t \t \t \t (Pt,Eta,Phi,E) = (%f, %f, %f, %f)", (*muItr).Pt(), (*muItr).Eta(), (*muItr).Phi(), (*muItr).E() );

        mu_sumPt += (*muItr).Pt();
        V_mu += (*muItr);
        mu_n++;
      }

      // Good electrons
      if(Debug){std::cout << APP_NAME << " DEBUG \t Final no.of good electrons \t" << (int)elTLV.size() << std::endl;}
      float el_sumPt(0.);
      TLorentzVector V_el;    
      for(std::vector<TLorentzVector>::iterator elItr=elTLV.begin(); elItr!=elTLV.end(); elItr++){
        if(Debug) Printf("\t \t \t \t (Pt,Eta,Phi,E) = (%f, %f, %f, %f)",(*elItr).Pt(), (*elItr).Eta(), (*elItr).Phi(), (*elItr).E());

        el_sumPt += (*elItr).Pt();
        V_el += (*elItr);
        el_n++;
      }

      // Good electrons                                                                                        
      if(Debug){std::cout << APP_NAME << " DEBUG \t Final no.of good leptons \t" << (int)lepTLV.size() << std::endl;}
      unsigned int Nl(0); float lep_sumPt(0.);
      TLorentzVector V_lep;
      for(std::vector<TLorentzVector>::iterator lepItr=lepTLV.begin(); lepItr!=lepTLV.end(); lepItr++){
        if(Debug) Printf("\t \t \t \t (Pt,Eta,Phi,E) = (%f, %f, %f, %f) \t | truth(%i)",(*lepItr).Pt(), (*lepItr).Eta(), (*lepItr).Phi(), (*lepItr).E(), (int)lepTruth.at(Nl));

        lep_sumPt += (*lepItr).Pt();
        V_lep += (*lepItr);
        lep_n++;
        Nl++;
      }

      // Good jets
      if(Debug){std::cout << APP_NAME << " DEBUG \t Final no. of good jets  \t" << (int)jetTLV.size() << std::endl;}    
      unsigned int Nj(0); float jet_sumPt(0.);
      for(std::vector<TLorentzVector>::iterator jetItr=jetTLV.begin(); jetItr!=jetTLV.end(); ++jetItr){
        if(Debug) Printf("\t \t \t \t (Pt,Eta,Phi,E) = (%f, %f, %f, %f) \t | isB(%i)",(*jetItr).Pt(), (*jetItr).Eta(), (*jetItr).Phi(), (*jetItr).E(), (int)jetBtag.at(Nj));

        bool passPtLF = (*jetItr).Pt() >= jet_PtCut[0];
        bool passPtHF = (*jetItr).Pt() >= jet_PtCut[1];	
        if( (bool)jetBtag.at(Nj) ){
          if( passPtHF ) b_n++;
          if( passPtLF ) jet_n++;
        }
        else{ 
          if( passPtLF ) jet_n++;
        }
        jet_sumPt += (*jetItr).Pt();
        Nj++;
      }

      //Set jet numbers
      Nbjet   = b_n;
      Njet50  = jet_n;
      Njet25  = this->getCutNumber(jetTLV, 25000.);
      Njet35  = this->getCutNumber(jetTLV, 35000.);
      Njet40  = this->getCutNumber(jetTLV, 40000.);    
      if(Debug){std::cout << APP_NAME << " DEBUG \t "<< Form("Njet25=%i, Njet35=%i, Njet40=%i, Njet50=%i, Nbjet=%i", Njet25, Njet35, Njet40, Njet50, Nbjet) << std::endl;}

      //Computing kinematic quantities
      ht   = lep_sumPt + jet_sumPt;
      meff = ht + EtMiss;
      minv = V_lep.M();
      mt   = this->getMt(lepTLV, EtMiss, EtMissPhi);
      if(Debug){std::cout << APP_NAME << " DEBUG \t "<< Form("EtMiss= %.0f | Ht= %.0f | Meff= %.0f | Minv= %.0f | Mt= %.0f", EtMiss, ht, meff, minv, mt) << std::endl;}


      //Cut on number of jets(bjets)
      int  Njets = SignalRegion.JetPt==50 ? Njet50 : (SignalRegion.JetPt==40 ? Njet40 : (SignalRegion.JetPt==35 ? Njet35 : Njet25)); 
      bool passJmin =  Njets >= SignalRegion.Njet;
      bool passBmin =  Nbjet >= SignalRegion.NBjet;
      bool passBmax = (SignalRegion.NBmax < 0 || Nbjet < SignalRegion.NBmax);

      // ==========Cutflow - separating SS Channels========
      std::vector <TLorentzVector> SSTLV;
      SSTLV = !setLeptons ? SSpair(lepTLV,lepCharges) : (setLeptons==1 ? SSpair(muTLV,muCharges) : SSpair(elTLV,elCharges));
      if(Debug){std::cout << APP_NAME <<" DEBUG \t SS1(Pt,eta,phi,E): ("<<SSTLV.at(0).Pt()<<", "<<SSTLV.at(0).Eta()<<", "<<SSTLV.at(0).Phi()<<", "<<SSTLV.at(0).E() << ") \n" 
        << APP_NAME <<" DEBUG \t SS2(Pt,eta,phi,E): ("<<SSTLV.at(1).Pt()<<", "<<SSTLV.at(1).Eta()<<", "<<SSTLV.at(1).Phi()<<", "<<SSTLV.at(1).E() << ")" 
          << std::endl;}

      SSChannel = this->getChannel(SSTLV, muTLV, elTLV, setLeptons);
      if(SSChannel==1){
        FillHisto(hCutflow, CutFlowIndex, nom);
        cutflow_file_mumu << EventNumber << "\n";
      }
      CutFlowIndex++;
      if(SSChannel==1 && Nbjet>0) {FillHisto(hCutflow, CutFlowIndex, nom);} CutFlowIndex++;
      if(SSChannel==1 && Nbjet>0 && Njet50>3) {FillHisto(hCutflow, CutFlowIndex, nom);} CutFlowIndex++;
      if(SSChannel==1 && Nbjet>0 && Njet50>3 && EtMiss>125000) {FillHisto(hCutflow, CutFlowIndex, nom);} CutFlowIndex++;

      if(SSChannel==2){
        FillHisto(hCutflow, CutFlowIndex, nom);
        cutflow_file_ee << EventNumber << "\n";
      }
      CutFlowIndex++;
      if(SSChannel==2 && Nbjet>0) {FillHisto(hCutflow, CutFlowIndex, nom);} CutFlowIndex++;
      if(SSChannel==2 && Nbjet>0 && Njet50>3) {FillHisto(hCutflow, CutFlowIndex, nom);} CutFlowIndex++;
      if(SSChannel==2 && Nbjet>0 && Njet50>3 && EtMiss>125000) {FillHisto(hCutflow, CutFlowIndex, nom);} CutFlowIndex++;

      if(SSChannel==3){
        FillHisto(hCutflow, CutFlowIndex, nom);
        cutflow_file_emu << EventNumber << "\n";
      }
      CutFlowIndex++;
      if(SSChannel==3 && Nbjet>0) {FillHisto(hCutflow, CutFlowIndex, nom);} CutFlowIndex++;
      if(SSChannel==3 && Nbjet>0 && Njet50>3) {FillHisto(hCutflow, CutFlowIndex, nom);} CutFlowIndex++;
      if(SSChannel==3 && Nbjet>0 && Njet50>3 && EtMiss>125000) {FillHisto(hCutflow, CutFlowIndex, nom);} CutFlowIndex++;
      //==============================

      if( !passJmin || !passBmin || !passBmax ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;

      // MET,Meff Cut
      float MeffCut   = SignalRegion.MeffCut;
      float EtMissCut = SignalRegion.EtMissCut;

      if( EtMiss < EtMissCut ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;

      if( meff < MeffCut ) continue;
      FillHisto(hCutflow, CutFlowIndex, nom);
      CutFlowIndex++;

      topoStr = Form("%im_%ie_%il_%ij(50)_%ib(20)", mu_n, el_n, lep_n, jet_n, b_n );
      if(Debug){std::cout << APP_NAME << " DEBUG \t Event reaches SR "<< SignalRegion.Name << " | Topology after cuts "<< topoStr << std::endl;} 


      //Filling histos 
      FillHisto(hEventWeightSF[sr][sys], weight);

      FillHisto(NMu[sr][sys],   mu_n,  weight);
      FillHisto(NEl[sr][sys],   el_n,  weight);
      FillHisto(NJet[sr][sys],  jet_n, weight);
      FillHisto(NBJet[sr][sys], b_n,   weight);

      FillHisto(Mu_1stPt[sr][sys], mu_1stPt, weight);
      FillHisto(Mu_2ndPt[sr][sys], mu_2ndPt, weight);
      FillHisto(Mu_3rdPt[sr][sys], mu_3rdPt, weight);

      FillHisto(El_1stPt[sr][sys], el_1stPt, weight);
      FillHisto(El_2ndPt[sr][sys], el_2ndPt, weight);
      FillHisto(El_2ndPt[sr][sys], el_3rdPt, weight);

      FillHisto(Lep_1stPt[sr][sys], lep_1stPt, weight);
      FillHisto(Lep_2ndPt[sr][sys], lep_2ndPt, weight);
      FillHisto(Lep_3rdPt[sr][sys], lep_3rdPt, weight);

      FillHisto(Jet_1stPt[sr][sys], jet_1stPt, weight);
      FillHisto(Jet_2ndPt[sr][sys], jet_2ndPt, weight);

      FillHisto(Jet_Mj[sr][sys],   mj,    weight);
      FillHisto(Jet_Mjj[sr][sys],  mjj,   weight);
      FillHisto(Jet_MjjdR[sr][sys],mjjdR, weight); 
      FillHisto(Jet_MjRec[sr][sys],mjRec, weight);
      for(unsigned int i(0); i<jetTLV.size();    i++) FillHisto(Jet_MjAll[sr][sys],    (jetTLV.at(i)).M(),    weight);
      for(unsigned int i(0); i<jetTLV_R1.size(); i++) FillHisto(Jet_MjRecAll[sr][sys], (jetTLV_R1.at(i)).M(), weight);

      FillHisto(Mu_1stEta[sr][sys], mu_1stEta, weight);
      FillHisto(Mu_2ndEta[sr][sys], mu_2ndEta, weight);
      FillHisto(El_1stEta[sr][sys], el_1stEta, weight);
      FillHisto(El_2ndEta[sr][sys], el_2ndEta, weight);

      FillHisto(Meff[sr][sys],   meff,   weight);
      FillHisto(Minv[sr][sys],   minv,   weight);
      FillHisto(MT[sr][sys],     mt,     weight);
      FillHisto(HT[sr][sys],     ht,     weight);
      FillHisto(ETmiss[sr][sys], EtMiss, weight);

      FillHisto(Mu_SumPt[sr][sys],  mu_sumPt,  weight); 
      FillHisto(El_SumPt[sr][sys],  el_sumPt,  weight);
      FillHisto(Jet_SumPt[sr][sys], jet_sumPt, weight);

      //Fill TTree
      this->FillTree( DiLeptonTree[sr][sys], (bool)makeTree );

      if(Debug){std::cout << APP_NAME << " DEBUG \t End SYS loop" << std::endl;}
    }
    if(Debug){std::cout << APP_NAME << " DEBUG \t End SR loop" << std::endl;}  
  }
  if(Debug){std::cout << APP_NAME << " DEBUG \t End execute() event" << std::endl;}

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode DiLepton :: postExecute (){

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode DiLepton :: finalize (){

  const char* APP_NAME = "finalize()";

  Info( APP_NAME, "Printing Cutflow" );
  for(unsigned int sr(0); sr<NSr; ++sr){ 
    std::cout << "Signal Region " << SignalRegions.at(sr).Name << std::endl;
    PrintCutflow( hCutflow );
  }
  cutflow_file_trig.close();
  cutflow_file_baseline.close();
  cutflow_file_all.close();
  cutflow_file_SS.close();
  cutflow_file_mumu.close();
  cutflow_file_ee.close();
  cutflow_file_emu.close();

  Info( APP_NAME, "Deleting all tools" );
  if( GRLTool ){ delete GRLTool; GRLTool = 0; }
  if( JRTool ){ delete JRTool; JRTool = 0; }
  if( MCClassifier ){delete MCClassifier; MCClassifier = 0; }
  if( CFTool ){delete CFTool; CFTool = 0; }
  if( CFToolLoose ){delete CFToolLoose; CFToolLoose = 0; }
  //if( TriggerSFTool ){delete TriggerSFTool; TriggerSFTool = 0; }
  //if( TriggerSFToolUP ){delete TriggerSFToolUP; TriggerSFToolUP = 0; }
  //if( TriggerSFToolDOWN ){delete TriggerSFToolDOWN; TriggerSFToolDOWN = 0; }

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode DiLepton :: histFinalize (){

  return EL::StatusCode::SUCCESS;
}


void DiLepton :: PrintCutflow(TH1 *hCut){

  std::cout << " Cutflow summary: " << std::endl;
  std::cout << "============================" << std::endl;
  std::cout << " Events Total   \t"<< (int)hCut->GetBinContent(1)  << std::endl;
  std::cout << " MC Truth veto  \t"<< (int)hCut->GetBinContent(2)  << std::endl; 
  std::cout << " GoodRunList    \t"<< (int)hCut->GetBinContent(3)  << std::endl; 
  std::cout << " Primary vertex \t"<< (int)hCut->GetBinContent(4)  << std::endl;
  std::cout << " Trigger        \t"<< (int)hCut->GetBinContent(5)  << std::endl;
  std::cout << " Global flags   \t"<< (int)hCut->GetBinContent(6)  << std::endl;
  std::cout << " Bad muons veto \t"<< (int)hCut->GetBinContent(7)  << std::endl;
  std::cout << " 1 jet pass OR  \t"<< (int)hCut->GetBinContent(8)  << std::endl;
  std::cout << " Bad jet veto   \t"<< (int)hCut->GetBinContent(9) << std::endl;
  std::cout << " 1 signal jet   \t"<< (int)hCut->GetBinContent(10) << std::endl;
  std::cout << " Cosmics veto   \t"<< (int)hCut->GetBinContent(11) << std::endl;
  std::cout << " 2 baseline lep.\t"<< (int)hCut->GetBinContent(12) << std::endl;
  std::cout << " 2 signal lep.  \t"<< (int)hCut->GetBinContent(13) << std::endl;
  std::cout << " SS leptons     \t"<< (int)hCut->GetBinContent(14) << std::endl;
  std::cout << " Trigger match  \t"<< (int)hCut->GetBinContent(15) << std::endl;
  //std::cout << " Z veto[off]    \t"<< (int)hCut->GetBinContent(16) << std::endl;
  std::cout << " mumu channel   \t"<< (int)hCut->GetBinContent(16) << std::endl;
  std::cout << " mumu NBjets>0  \t"<< (int)hCut->GetBinContent(17) << std::endl;
  std::cout << " mumu NJets>3   \t"<< (int)hCut->GetBinContent(18) << std::endl;
  std::cout << " mumu MET>125   \t"<< (int)hCut->GetBinContent(19) << std::endl;
  std::cout << " ee channel     \t"<< (int)hCut->GetBinContent(20) << std::endl;
  std::cout << " ee NBjets>0    \t"<< (int)hCut->GetBinContent(21) << std::endl;
  std::cout << " ee NJets>3     \t"<< (int)hCut->GetBinContent(22) << std::endl;
  std::cout << " ee MET>125     \t"<< (int)hCut->GetBinContent(23) << std::endl;
  std::cout << " emu channel    \t"<< (int)hCut->GetBinContent(24) << std::endl;
  std::cout << " emu NBjets>0   \t"<< (int)hCut->GetBinContent(25) << std::endl;
  std::cout << " emu NJets>3    \t"<< (int)hCut->GetBinContent(26) << std::endl;
  std::cout << " emu MET>125    \t"<< (int)hCut->GetBinContent(27) << std::endl;
  //std::cout << " SR::NJet(bjet) \t"<< (int)hCut->GetBinContent(17) << std::endl;
  //std::cout << " SR::EtMiss Cut \t"<< (int)hCut->GetBinContent(18) << std::endl;
  //std::cout << " SR::Meff Cut   \t"<< (int)hCut->GetBinContent(19) << std::endl;
  std::cout << "============================" << std::endl; 
  std::cout << std::endl;
}

bool DiLepton :: getMET(float &met, float &metphi, 
    xAOD::MissingETContainer *MET, 
    const xAOD::JetContainer *jet, 
    const xAOD::ElectronContainer *el, 
    const xAOD::MuonContainer *mu, 
    const xAOD::PhotonContainer *ph, 
    const xAOD::TauJetContainer *tau, bool doTST, bool doJVT){

  const char * APP_NAME = "DiLepton::getMET()";

  int Nmu  = mu ? mu->size() : 0;
  int Nel  = el ? el->size() : 0;
  int Nph  = ph ? ph->size() : 0;  
  int Nj = jet ? jet->size() : 0;
  int Nt = tau ? tau->size() : 0;
  if(Debug){std::cout << APP_NAME << Form(" DEBUG \t BL objects for MET: %i mu, %i el, %i jets, %i ph, %i tau [doJVT=%i|doTST=%i]", 
      Nmu, Nel, Nj, Nph, Nt, doTST, doJVT) << std::endl;}
  if(Debug){
    std::cout << APP_NAME << " DEBUG \t Muons in container"<< std::endl;
    for(int imu=0;imu<Nmu;imu++){
      std::cout << APP_NAME << " DEBUG \t Pt: "<<mu->at(imu)->pt()<<"\t Eta: "<<mu->at(imu)->eta()<<"\t Phi: "<<mu->at(imu)->phi()<<std::endl;
    }
    std::cout << APP_NAME << " DEBUG \t Electrons in container"<< std::endl;
    for(int iel=0;iel<Nel;iel++){
      std::cout << APP_NAME << " DEBUG \t Pt: "<<el->at(iel)->pt()<<"\t Eta: "<<el->at(iel)->eta()<<"\t Phi: "<<el->at(iel)->phi()<<std::endl;
    }
    std::cout << APP_NAME << " DEBUG \t Photons in container"<< std::endl;
    for(int iph=0;iph<Nph;iph++){
      std::cout << APP_NAME << " DEBUG \t Pt: "<<ph->at(iph)->pt()<<"\t Eta: "<<ph->at(iph)->eta()<<"\t Phi: "<<ph->at(iph)->phi()<<std::endl;
    }
    std::cout << APP_NAME << " DEBUG \t Jets in container"<< std::endl;
    for(int ijet=0;ijet<Nj;ijet++){
      std::cout << APP_NAME << " DEBUG \t Pt: "<<jet->at(ijet)->pt()<<"\t Eta: "<<jet->at(ijet)->eta()<<"\t Phi: "<<jet->at(ijet)->phi()<<std::endl;
    }
  }

  ANA_CHECK( SusyObjTool->GetMET(*MET, jet, el, mu, ph, tau, doTST, doJVT, 0) );

  xAOD::MissingETContainer::const_iterator it = MET->find("Final");      
  if(it == MET->end()){
    Error( APP_NAME, "No RefFinal inside MET container" );
    met    = -1;
    metphi = -1;
  }
  else{
    if(Debug){std::cout << APP_NAME << Form(" DEBUG \t mpx: %f mpy: %f metphi: %f",(*it)->mpx(),(*it)->mpy(),(*it)->phi()) << std::endl;}
    met    = TMath::Sqrt((*it)->mpx()*(*it)->mpx()+(*it)->mpy()*(*it)->mpy());
    metphi = (*it)->phi();
  }
  return true;
}

void DiLepton :: sortPt(std::vector<TLorentzVector> &v, std::vector<float> &c, std::vector<float> &q){

  std::vector <TLorentzVector> v_out;
  std::vector <float> c_out;
  std::vector <float> q_out;

  std::vector<double> pt_vec;
  for(unsigned int i(0); i<v.size(); i++) pt_vec.push_back( v.at(i).Pt() );

  std::sort(pt_vec.begin(), pt_vec.end());
  std::reverse(pt_vec.begin(), pt_vec.end());

  std::vector<int> index;
  for(unsigned int i(0); i<pt_vec.size(); i++){
    double pt = pt_vec.at(i);
    for(unsigned int j(0); j<v.size(); j++){
      if( TMath::Abs((pt - v.at(j).Pt())/pt) < 0.000001 ) index.push_back(j);
      else continue;   
    }
  }  
  for(unsigned int i(0); i<index.size(); i++){
    v_out.push_back( v.at(index.at(i)) ); 
    c_out.push_back( c.at(index.at(i)) ); 
    q_out.push_back( q.at(index.at(i)) );
  } 
  v.clear(); c.clear(); q.clear();

  v = v_out;
  c = c_out;
  q = q_out;

  return;
}

float DiLepton :: getMt(std::vector<TLorentzVector> leptons, float met, float metPhi){
  if( leptons.empty() ) return 0;
  TVector3 lepVec(0.,0.,0.);
  lepVec.SetPtEtaPhi(leptons.at(0).Pt(), 0., leptons.at(0).Phi());

  TLorentzVector Vt(0.,0.,0.,0.);
  Vt.SetPxPyPzE( met*TMath::Cos(metPhi)+lepVec.Px(), 
      met*TMath::Sin(metPhi)+lepVec.Py(), 
      0., 
      met+lepVec.Pt() );

  return (float)Vt.M();
}

bool DiLepton :: cosmicsVeto(xAOD::MuonContainer* muons, bool enabled){
  if(!enabled) return false;
  bool cosmics(false);
  for( auto mu : *muons ) {
    if( mu->auxdata<char>("passOR")==1 && mu->auxdata<char>("cosmic")!=0 ) cosmics = true;
  }
  return cosmics;
}

bool DiLepton :: badMuonVeto(xAOD::MuonContainer* muons, bool enabled){
  if(!enabled) return false;
  bool badMuon(false);
  for( auto mu : *muons ) {
    if( mu->auxdata<char>("baseline")==1 && mu->auxdata<char>("bad")!=0  ) badMuon = true;
  }
  return badMuon;
}

bool DiLepton :: badJetVeto(xAOD::JetContainer* jets, bool enabled){
  if(!enabled) return false;
  bool badjet(false);
  for( auto jet : *jets ) {
    if( jet->auxdata<char>("passOR")==1 && jet->auxdata<char>("bad")!=0 ) badjet = true;
  }
  return badjet;
}

float DiLepton :: getPileupWeight(){
  float w(1.);
  if(!doPileup || isData) return w;
  const xAOD::EventInfo* evt(0);
  if( !m_event->retrieve(evt, "EventInfo").isSuccess() ){
    Error("DiLepton::getPileupWeight() \t Error retrieving EventInfo, returning ",0); return w; 
  }
  int RN = evt->runNumber();
  if( RN != 222525 && RN != 222526 && RN != 284500 ) return w;
  w *= SusyObjTool->GetPileupWeight();    
  return w;
}

ULong64_t DiLepton :: getPileupHash(){
  if(!doPileup || isData) return 1;
  const xAOD::EventInfo* evt(0);
  if( !m_event->retrieve(evt, "EventInfo").isSuccess() ){
    Error("DiLepton::getPileupHash() \t Error retrieving EventInfo, returning ",0); return 1;
  }
  int RN = evt->runNumber();
  if( RN != 222525 && RN != 222526 && RN != 284500 ) return 1;
  return SusyObjTool->GetPileupWeightHash();
}

bool DiLepton :: isResonance(std::vector<TLorentzVector> vectors, std::vector< float > charges){

  bool res(false);
  bool OS[6] = {0., 0., 0., 0., 0., 0.};
  float M[6] = {0., 0., 0., 0., 0., 0.};
  float MZlow(81200.), MZup(101200.);

  if(vectors.size() < 2) return res;
  if(vectors.size() == 2){
    M[0] = (vectors.at(0) + vectors.at(1)).M();
    if(charges.at(0) != charges.at(1)) OS[0] = true;
  }
  else if(vectors.size() == 3){
    M[0] = (vectors.at(0) + vectors.at(1)).M();
    M[1] = (vectors.at(1) + vectors.at(2)).M();
    M[2] = (vectors.at(0) + vectors.at(2)).M();
    if(charges.at(0) != charges.at(1)) OS[0] = true;
    if(charges.at(1) != charges.at(2)) OS[1] = true;
    if(charges.at(0) != charges.at(2)) OS[2] = true;
  }
  else{
    M[0] = (vectors.at(0) + vectors.at(1)).M();
    M[1] = (vectors.at(1) + vectors.at(2)).M();
    M[2] = (vectors.at(0) + vectors.at(2)).M();
    M[3] = (vectors.at(3) + vectors.at(1)).M();
    M[4] = (vectors.at(3) + vectors.at(2)).M();
    M[5] = (vectors.at(3) + vectors.at(0)).M();
    if(charges.at(0) != charges.at(1)) OS[0] = true;
    if(charges.at(1) != charges.at(2)) OS[1] = true;
    if(charges.at(0) != charges.at(2)) OS[2] = true;
    if(charges.at(3) != charges.at(1)) OS[3] = true;
    if(charges.at(3) != charges.at(2)) OS[4] = true;
    if(charges.at(3) != charges.at(0)) OS[5] = true;
  }

  if( (OS[0] && (M[0] > MZlow && M[0] < MZup)) 
      || (OS[1] && (M[1] > MZlow && M[1] < MZup)) 
      || (OS[2] && (M[2] > MZlow && M[2] < MZup)) 
      || (OS[3] && (M[3] > MZlow && M[3] < MZup))
      || (OS[4] && (M[4] > MZlow && M[4] < MZup))
      || (OS[5] && (M[5] > MZlow && M[5] < MZup))) res = true;

  if(Debug){std::cout << "DiLepton::ResVeto()" <<" DEBUG \t Found Z resonance:  "<< res << std::endl;}
  return res;
}

bool DiLepton :: addSR(TString name, int jetmin, int bmin, int bmax, float jetpt, float metmin, float meffmin, TString pairing){
  SR sr;
  sr.Name      = name;
  sr.Njet      = jetmin;
  sr.NBjet     = bmin;
  sr.NBmax     = bmax;
  sr.JetPt     = jetpt;
  sr.EtMissCut = metmin;
  sr.MeffCut   = meffmin;
  sr.Pairing   = pairing;

  SignalRegions.push_back(sr);
  this->PrintSR(sr, "addSR()");

  return true;
}

void DiLepton :: PrintSR(SR sr, const char * APP_NAME){
  std::cout << "DiLepton::PrintSR() " << APP_NAME << "\t SR :: " 
    << Form("Name %s, Njet %i, NBjet %i, NBmax %i, JetPt %.0f, EtMissCut %.0f, MeffCut %.0f, Pairing %s",
        sr.Name.Data(), sr.Njet, sr.NBjet, sr.NBmax, sr.JetPt, sr.EtMissCut, sr.MeffCut, sr.Pairing.Data() ) << std::endl;
  return;
}

bool DiLepton :: setTriggers(std::vector<std::string> &trigList, std::string trigname, int option){ 

  if(isMC) ANA_CHECK( SusyObjTool->ApplyPRWTool() );
  PileUpWeight = isMC ? this->getPileupWeight() : 1.;
  PileUpHash   = isMC ? this->getPileupHash()   : 1.;


  if(Debug){std::cout << "Trigname: " << trigname << std::endl;}

  const char * APP_NAME = "setTrigger()";
  //No trigger
  if( !trigname.length() || !option ) { 
    Info( APP_NAME, "No trigger or option selected, returning 1");
    return true;
  }
  //Set only one trigger
  if( ((TString)trigname).Contains("HLT_") ){
    trigList.push_back(trigname);
    Info( APP_NAME, "Asking only = %s", trigname.c_str());
    return true;
  }
  //Set DiLepton trigger
  if(trigname=="DILEPTON"){
    switch(option){
      case 2015:
        trigList.push_back("HLT_mu18_mu8noL1");
        trigList.push_back("HLT_2e12_lhloose_L12EM10VH");
        trigList.push_back("HLT_e17_lhloose_mu14");
        break;
      case 2016:
        trigList.push_back("HLT_mu22_mu8noL1");
        trigList.push_back("HLT_2e17_lhvloose_nod0");
        trigList.push_back("HLT_e17_lhloose_nod0_mu14");
        break;
      default: break;
    }
  }
  //Set DiLepton+MET trigger - use when L1_2EM15VHI is not unprescaled
  if(trigname=="DILEPTON_MET"){
    switch(option){
      case 2015:  
        trigList.push_back("HLT_xe70");
        trigList.push_back("HLT_mu18_mu8noL1");
        trigList.push_back("HLT_2e12_lhloose_L12EM10VH");
        trigList.push_back("HLT_e17_lhloose_mu14");
        break;
      case 2016:
        trigList.push_back("HLT_xe90_mht_L1XE50");
        trigList.push_back("HLT_xe100_mht_L1XE50");
        trigList.push_back("HLT_xe110_mht_L1XE50");
        trigList.push_back("HLT_mu22_mu8noL1");
        trigList.push_back("HLT_2e17_lhvloose_nod0");
        trigList.push_back("HLT_e17_lhloose_nod0_mu14");
        break;
      case 2017:
        trigList.push_back("HLT_2e17_lhvloose_nod0_L12EM15VHI"); // L1_2EM15VHI was accidentally prescaled in periods B5-B8 (runs 326834-328393) - only use when unprescaled
        trigList.push_back("HLT_2e24_lhvloose_nod0"); 
        trigList.push_back("HLT_e17_lhloose_nod0_mu14");  
        trigList.push_back("HLT_mu22_mu8noL1");
        trigList.push_back("HLT_xe110_pufit_L1XE55");
    }
  }

  // Use when L1_2EM15VHI is prescaled
  if(trigname=="DILEPTON_MET_PRESCALE"){
    switch(option){
      case 2017:
        trigList.push_back("HLT_2e24_lhvloose_nod0"); 
        trigList.push_back("HLT_e17_lhloose_nod0_mu14");  
        trigList.push_back("HLT_mu22_mu8noL1");
        trigList.push_back("HLT_xe110_pufit_L1XE55");

      default: break;
    }
  }
  Info( APP_NAME, "Configured trigger option %i", option);
  for(unsigned int t(0); t<trigList.size(); t++) Info( APP_NAME, "Asking trigger = %s", (trigList.at(t)).c_str() );
  return true;
}

void DiLepton :: setTrigItem(std::vector<std::string> &trigList, std::string trigItem, unsigned int it){
  if(trigList.size() < it+1) return;
  trigList[it] = trigItem;
  return;
}

bool DiLepton :: isTriggered(std::vector<std::string> trigList, std::vector<std::string> &trigInfo, bool enabled){

  if( !enabled || trigList.empty() ) return true;
  bool trigPass(false);
  std::vector<std::string>::iterator trigger;
  for(trigger=trigList.begin(); trigger!=trigList.end(); trigger++){
    if( SusyObjTool->IsTrigPassed(*trigger) ){ 
      if(Debug){std::cout << "DiLepton::Trigger()" << " DEBUG \t Event passed trigger = " << *trigger << std::endl;}
      trigInfo.push_back(*trigger);
      trigPass = true;
    }
  }
  return trigPass;
}

bool DiLepton :: isTriggeredMet(std::vector<std::string> trigList, float met, std::vector<std::string> trigInfo, float metcut){

  if( trigList.empty() ) return true;
  if( trigInfo.empty() ) return false;
  if( met > metcut ) return true;

  if( (int)trigInfo.size() == this->nMetTrigger(trigInfo) ){
    if(Debug){std::cout << "DiLepton::Trigger()" << " DEBUG \t " << Form("EtMiss < %.0f and only MET triggers are fired, return %i", metcut, 0) << std::endl;}
    return false;
  }
  return true;
}

bool DiLepton :: isTriggeredMet2016(std::vector<std::string> trigList, float met, std::vector<std::string> trigInfo, int period, float metcut){

  if( trigList.empty() ) return true;
  if( trigInfo.empty() ) return false;

  int totalMETPass = this->nMetTrigger_xe90(trigInfo) + this->nMetTrigger_xe100(trigInfo) + this->nMetTrigger_xe110(trigInfo);
  if (totalMETPass == (int)trigInfo.size() && met < metcut) return false;
  if ((int)trigInfo.size() > totalMETPass) return true;

  if(Debug){std::cout << "Still in MET Trigger" << std::endl;}

  if (period==1 && met > metcut && this->nMetTrigger_xe90(trigInfo) > 0 ){
      if(Debug){std::cout << "DiLepton::Trigger()" << " DEBUG \t " << Form("EtMiss > %.0f and MET trigger are fired, return %i", metcut, 0) << " - Period 1" << std::endl;}
      return true;
  }
  if (period==2 && met > metcut && this->nMetTrigger_xe100(trigInfo) > 0 ){
      if(Debug){std::cout << "DiLepton::Trigger()" << " DEBUG \t " << Form("EtMiss > %.0f and MET trigger are fired, return %i", metcut, 0) << " - Period 2" << std::endl;}
      return true;
  }
  if (period==3 && met > metcut && this->nMetTrigger_xe110(trigInfo) > 0 ){
      if(Debug){std::cout << "DiLepton::Trigger()" << " DEBUG \t " << Form("EtMiss > %.0f and MET trigger are fired, return %i", metcut, 0) << " - Period 3" << std::endl;}
      return true;
  }

  return false;
}

int DiLepton :: nMetTrigger(std::vector<std::string> trigInfo){

  std::string metKey("HLT_xe");
  unsigned int nTrig(0);
  for(unsigned int tr(0); tr<trigInfo.size(); tr++){ 
    if( (trigInfo.at(tr)).find(metKey) != std::string::npos ) nTrig++; 
  }
  return (int)nTrig;
}

int DiLepton :: nMetTrigger_xe90(std::vector<std::string> trigInfo){

  std::string metKey("HLT_xe90");
  unsigned int nTrig(0);
  for(unsigned int tr(0); tr<trigInfo.size(); tr++){ 
    if( (trigInfo.at(tr)).find(metKey) != std::string::npos ) nTrig++; 
  }
  return (int)nTrig;
}

int DiLepton :: nMetTrigger_xe100(std::vector<std::string> trigInfo){

  std::string metKey("HLT_xe100");
  unsigned int nTrig(0);
  for(unsigned int tr(0); tr<trigInfo.size(); tr++){ 
    if( (trigInfo.at(tr)).find(metKey) != std::string::npos ) nTrig++; 
  }
  return (int)nTrig;
}

int DiLepton :: nMetTrigger_xe110(std::vector<std::string> trigInfo){

  std::string metKey("HLT_xe110");
  unsigned int nTrig(0);
  for(unsigned int tr(0); tr<trigInfo.size(); tr++){ 
    if( (trigInfo.at(tr)).find(metKey) != std::string::npos ) nTrig++; 
  }
  return (int)nTrig;
}

bool DiLepton :: isTrigMatch(int yearOpt, xAOD::MuonContainer *muons, xAOD::ElectronContainer* electrons, float met, std::vector<std::string> trigInfo, bool doMatch, int period, bool prescale, float metcut){

  const char *APP_NAME = "DiLepton::TriggerMatch()";
  bool pass(true);
  if( !doMatch ) return pass;
  std::vector<std::string> trigList;
  std::string MuMuItem, ElElItem, ElMuItem, ElElItem2;
  float ptCutMuMu(0), ptCutElEl(0), ptCutElMu(0);

  switch( yearOpt ){
    case 2015:
      trigList = triggerList2015;
      MuMuItem  = "HLT_mu18_mu8noL1";
      ElElItem  = "HLT_2e12_lhloose_L12EM10VH";
      ElMuItem  = "HLT_e17_lhloose_mu14";
      ptCutMuMu = 20000.;
      ptCutElEl = 20000.;
      ptCutElMu = 20000.;
      if( met>metcut && this->nMetTrigger(trigInfo) ){
        if(Debug){std::cout << APP_NAME << Form(" DEBUG \t EtMiss > %.0f and event is triggered by %i EtMiss trigger, return %i", metcut, this->nMetTrigger(trigInfo), pass) << std::endl;}
        return pass;
      }
      break;
    case 2016:
      trigList = triggerList2016;
      MuMuItem  = "HLT_mu22_mu8noL1"; 
      ElElItem  = "HLT_2e17_lhvloose_nod0";
      ElMuItem  = "HLT_e17_lhloose_nod0_mu14"; 
      ptCutMuMu  = 23000.;
      ptCutElEl  = 20000.;
      ptCutElMu  = 20000.;
      if( period==1 && met>metcut && this->nMetTrigger_xe90(trigInfo) ){
        if(Debug){std::cout << APP_NAME << Form(" DEBUG \t EtMiss > %.0f and event is triggered by %i EtMiss trigger, return %i", metcut, this->nMetTrigger_xe90(trigInfo), pass) << std::endl;}
        return pass;
      }
      if( period==2 && met>metcut && this->nMetTrigger_xe100(trigInfo) ){
        if(Debug){std::cout << APP_NAME << Form(" DEBUG \t EtMiss > %.0f and event is triggered by %i EtMiss trigger, return %i", metcut, this->nMetTrigger_xe100(trigInfo), pass) << std::endl;}
        return pass;
      }
      if( period==3 && met>metcut && this->nMetTrigger_xe110(trigInfo) ){
        if(Debug){std::cout << APP_NAME << Form(" DEBUG \t EtMiss > %.0f and event is triggered by %i EtMiss trigger, return %i", metcut, this->nMetTrigger_xe110(trigInfo), pass) << std::endl;}
        return pass;
      }
      break;
    case 2017:
      if(prescale==1){
        trigList = triggerList2017_PS;
        MuMuItem  = "HLT_mu22_mu8noL1";
        ElElItem  = "HLT_2e24_lhvloose_nod0";
        ElMuItem  = "HLT_e17_lhloose_nod0_mu14";
        ptCutMuMu  = 23000.;
        ptCutElEl  = 20000.;
        ptCutElMu  = 20000.;
      }
      if(prescale==0){
        trigList = triggerList2017;
        MuMuItem  = "HLT_mu22_mu8noL1";
        ElElItem  = "HLT_2e24_lhvloose_nod0";
        ElElItem2 = "HLT_2e17_lhvloose_nod0_L12EM15VHI";
        ElMuItem  = "HLT_e17_lhloose_nod0_mu14";
        ptCutMuMu  = 23000.;
        ptCutElEl  = 20000.;
        ptCutElMu  = 20000.;
      }
      if( met>metcut && this->nMetTrigger(trigInfo) ){
        if(Debug){std::cout << APP_NAME << Form(" DEBUG \t EtMiss > %.0f and event is triggered by %i EtMiss trigger, return %i", metcut, this->nMetTrigger(trigInfo), pass) << std::endl;}
        return pass;
      }
      break;
    default: 
      break;
  }

  int matchMuMu(0), matchElEl(0), matchElMu(0);
  for(auto mu1 : *muons){
    if( mu1->pt()>ptCutElMu && SusyObjTool->IsTrigMatched(mu1, ElMuItem) ) matchElMu=1;
    for(auto mu2 : *muons){
      if( mu1!=mu2 && (mu1->pt()>ptCutMuMu || mu2->pt()>ptCutMuMu) && SusyObjTool->IsTrigMatched(mu1, mu2, MuMuItem)) matchMuMu=2;
    }
  }
  for(auto el : *electrons){
    if( el->pt()>ptCutElEl && SusyObjTool->IsTrigMatched(el, ElElItem) ) matchElEl++;
    if( el->pt()>ptCutElMu && SusyObjTool->IsTrigMatched(el, ElMuItem) ) matchElMu++;
    switch( yearOpt ){
      case 2017:
        if(prescale==0){
          if( el->pt()>ptCutElEl && SusyObjTool->IsTrigMatched(el, ElElItem2) ) matchElEl++;
        }
    }
  }
  if(Debug){std::cout << APP_NAME << Form(" DEBUG \t TrigConfig(%i) %s|%s|%s = [%i|%i|%i]",yearOpt,MuMuItem.c_str(),ElElItem.c_str(),ElMuItem.c_str(),matchMuMu,matchElEl,matchElMu) << std::endl;}

  bool MM = (matchMuMu>1  && containStr(trigInfo, MuMuItem) );
  bool EE = (matchElEl>1  && containStr(trigInfo, ElElItem) );
  bool EM = (matchElMu>1  && containStr(trigInfo, ElMuItem) );
  bool EE2(0);
  switch( yearOpt ){
    case 2017:
      if(prescale==0){
        EE2 = (matchElEl>2  && containStr(trigInfo, ElElItem2) );
      }
  }

  pass = (MM || EE || EM);  
  switch( yearOpt ){
    case 2017:
      if(prescale==0){
        pass = (MM || EE || EM || EE2);
      }
  }

  return pass;
}

double DiLepton :: dR(float eta1, float phi1, float eta2, float phi2){
  float deta = TMath::Abs(eta1 - eta2);
  float dphi = TMath::Abs(phi1 - phi2) < TMath::Pi() ? TMath::Abs(phi1 - phi2) : 2*TMath::Pi() - TMath::Abs(phi1 - phi2);
  return TMath::Sqrt(deta*deta + dphi*dphi);
}

double DiLepton :: dR(TLorentzVector v1, TLorentzVector v2){
  return dR(v1.Eta(), v1.Phi(), v2.Eta(), v2.Phi()); 
}

bool DiLepton :: match(TLorentzVector v1, TLorentzVector v2){
  if(v1.Pt()<1E-4 || v2.Pt()<1E-4) return false;
  if(dR(v1,v2)<1E-4 && TMath::Abs(v1.Pt()-v2.Pt())<1E-4) return true;
  else return false;
}

float DiLepton :: MjjdRmin(std::vector<TLorentzVector> jets){

  if(jets.size()==0) return 0;
  if(jets.size()==1) return (jets.at(0)).M();
  if(jets.size()==2) return (jets.at(0) + jets.at(1)).M();

  int it1(0), it2(0);
  float dR(99.);  
  for(unsigned int I(0); I<jets.size()-1; I++){
    for(unsigned int i(I+1); i<jets.size(); i++){
      float dr = (jets.at(I)).DeltaR(jets.at(i));
      if( dr<dR ){ dR = dr; it1 = I; it2 = i; }
    }
  }
  if(Debug){std::cout << "DiLepton::MjjdRmin()" <<" DEBUG \t Min dR between jets "<< it1 << " and " << it2 << " dR: " << dR << std::endl;}
  return (jets.at(it1) + jets.at(it2)).M();
}

void DiLepton :: PrintInfo(const xAOD::Muon* mu, const char* APP_NAME, bool msg){
  if(!msg) return;
  if(!mu){Info(APP_NAME, "WARNING: No PrintInfo() for NullPtr"); return;}
  try{std::cout << APP_NAME << " DEBUG " 
    << Form("\t Muon Pt|Eta|Phi|C \t %.2f | %.3f | %.3f | %.0f", mu->pt(), mu->eta(), mu->phi(), mu->charge()) 
      << Form("\t isBL|OR|isSig|OK %i | %i | %i | %i",(int)mu->auxdata<char>("baseline"),(int)mu->auxdata<char>("passOR"),(int)mu->auxdata<char>("signal"),(int)mu->auxdata<char>("SIG")) 
      << Form("\t isCosmic|isBad %i | %i",(int)mu->auxdata<char>("cosmic"),(int)mu->auxdata<char>("bad")) << std::endl;
  }
  catch(SG::ExcBadAuxVar&){ Info(APP_NAME, "WARNING: Incomplete decoration of muon");}
  return;
}

void DiLepton :: PrintInfo(const xAOD::Electron* el, const char* APP_NAME, bool msg){
  if(!msg) return;
  if(!el){Info(APP_NAME, "WARNING: No PrintInfo() for NullPtr"); return;}  
  try{std::cout << APP_NAME << " DEBUG " 
    << Form("\t Electron Pt|Eta|Phi|C \t %.2f | %.3f | %.3f | %.0f", el->pt(), el->eta(), el->phi(), el->charge()) 
      << Form("\t isBL|OR|isSig|OK %i | %i | %i | %i",(int)el->auxdata<char>("baseline"),(int)el->auxdata<char>("passOR"),(int)el->auxdata<char>("signal"),(int)el->auxdata<char>("SIG")) 
      << std::endl;
  }
  catch(SG::ExcBadAuxVar&){ Info(APP_NAME, "WARNING: Incomplete decoration of electron");}
  return;
}

void DiLepton :: PrintInfo(const xAOD::Jet* jet, const char* APP_NAME, bool msg){
  if(!msg) return;
  if(!jet){Info(APP_NAME, "WARNING: No PrintInfo() for NullPtr"); return;} 
  try{std::cout << APP_NAME << " DEBUG " 
    << Form("\t Jet Pt|Eta|Phi \t %.2f | %.3f | %.3f", jet->pt(), jet->eta(), jet->phi()) 
      << Form("\t isBL|OR|isSig|OK %i | %i | %i | %i",(int)jet->auxdata<char>("baseline"),(int)jet->auxdata<char>("passOR"),(int)jet->auxdata<char>("signal"),(int)jet->auxdata<char>("SIG")) 
      << Form("\t isB|isBad|Jvt %i | %i | %.2f",(int)jet->auxdata<char>("bjet"),(int)jet->auxdata<char>("bad"),(float)jet->auxdata<float>("Jvt")) << std::endl;
  }
  catch(SG::ExcBadAuxVar&){ Info(APP_NAME, "WARNING: Incomplete decoration of jet");}
  return;
}

void DiLepton :: PrintInfo(const xAOD::TauJet* tau, const char* APP_NAME, bool msg){
  if(!msg) return;
  if(!tau){Info(APP_NAME, "WARNING: No PrintInfo() for NullPtr"); return;}
  try{std::cout << APP_NAME << " DEBUG " 
    << Form("\t Tau Pt|Eta|Phi \t %.2f | %.3f | %.3f", tau->pt(), tau->eta(), tau->phi()) << std::endl;
  }
  catch(SG::ExcBadAuxVar&){ Info(APP_NAME, "WARNING: Incomplete decoration of tau");}
  return;
}

void DiLepton :: PrintInfo(const xAOD::Photon* ph, const char* APP_NAME, bool msg){
  if(!msg) return;
  if(!ph){Info(APP_NAME, "WARNING: No PrintInfo() for NullPtr"); return;}
  try{std::cout << APP_NAME << " DEBUG " 
    << Form("\t Photon Pt|Eta|Phi \t %.2f | %.3f | %.3f", ph->pt(), ph->eta(), ph->phi()) << std::endl;
  }
  catch(SG::ExcBadAuxVar&){ Info(APP_NAME, "WARNING: Incomplete decoration of photon");}
  return;
}

int DiLepton :: getPairingOption(SR sr){
  std::map<TString, int> options;
  options["2L+1"]  = 0;
  options["2L"]    = 1;
  options["3L"]    = 2;
  options["2L_EX"] = 3;

  return (int)options[sr.Pairing];
}

bool DiLepton::SSleptons(std::vector< TLorentzVector > lepTLV, std::vector<float> lepCharges){
//bool DiLepton::SSleptons(SR sr, std::vector< TLorentzVector > lepTLV, std::vector<float> lepCharges){

  //Case 0: Keep if L1L2 are SS OR if L3 present
  //Case 1: No requirement on 3rd lepton, L1L2 SS
  //Case 2: At least 3 leptons 
  //Case 3: Exactly 2 leptons  

  //int option = this->getPairingOption(sr);
  //bool passLepton(false), passPt(false), passCharges(false);
  bool passPt(false), passCharges(false);
  unsigned int LepSize = lepTLV.size();

  //if(Debug){std::cout << "DiLepton::SSleptons()" << " DEBUG \t Using pairing option = " << option << std::endl;}
  if(Debug){std::cout << "DiLepton::SSleptons() - in loop"<< std::endl;}
  if(Debug){std::cout << "LepSize: " << LepSize << std::endl;}

  if( LepSize<2 ) {
    if(Debug){std::cout << "DiLepton::SSlepton() - didn't find 2leptons" << std::endl;}
    return false;
  }
  if(LepSize==2){
    passPt = lepTLV.at(0).Pt()>lep_PtCut[0] && lepTLV.at(1).Pt()>lep_PtCut[1];
    passCharges = lepCharges[0]*lepCharges[1]>0;
    if(Debug){std::cout << "LepCharge[0]: " << lepCharges[0] << ", LepCharge[1]: " << lepCharges[1] << std::endl;}
  }
  if(LepSize>2){
    passPt = lepTLV.at(0).Pt()>lep_PtCut[0] && lepTLV.at(1).Pt()>lep_PtCut[1] && lepTLV.at(2).Pt()>lep_PtCut[2];
    passCharges = true;
  }
  //switch(option){
  //  case 0:
  //    passLepton = LepSize>=2;
  //    passPt = lepTLV.at(0).Pt()>lep_PtCut[0] && lepTLV.at(1).Pt()>lep_PtCut[1];  
  //    if(LepSize>=3) passPt = passPt && lepTLV.at(2).Pt()>lep_PtCut[2];
  //    passCharges = (lepCharges[0]*lepCharges[1]>0 || LepSize>=3);   
  //    break;
  //  case 1:
  //    passLepton = LepSize>=2;
  //    passPt = lepTLV.at(0).Pt()>lep_PtCut[0] && lepTLV.at(1).Pt()>lep_PtCut[1];  
  //    passCharges = lepCharges[0]*lepCharges[1]>0; 
  //    break;
  //  case 2:
  //    passLepton = LepSize>=3;    
  //    passPt = lepTLV.at(0).Pt()>lep_PtCut[0] && lepTLV.at(1).Pt()>lep_PtCut[1] && lepTLV.at(2).Pt()>lep_PtCut[2];	     
  //    passCharges = (lepCharges[0]*lepCharges[1]>0 || LepSize>=3);
  //    break;
  //  case 3:
  //    passLepton = LepSize==2;    
  //    passPt = lepTLV.at(0).Pt()>lep_PtCut[0] && lepTLV.at(1).Pt()>lep_PtCut[1];	    
  //    passCharges = lepCharges[0]*lepCharges[1]>0; 
  //    break;
  //  default:
  //    Error("DiLepton::SSleptons() \t Pairing option unknown returning",0);
  //}
  //bool selected = (passLepton && passPt && passCharges);  
  bool selected = (passPt && passCharges);  

  return selected;
}

void DiLepton :: Lep_Jet_OR(xAOD::IParticleContainer *electrons_baseline,xAOD::IParticleContainer *muons_baseline,xAOD::JetContainer *jets_baseline){

  TLorentzVector electron_1;
  TLorentzVector muon_1;
  TLorentzVector jet_1;
  float dR=9999.;

  for (auto jet : *jets_baseline){
    if((*jet).auxdata<char>("passOR") == 0) continue;
    if(badJvt(*jet)) continue;
    jet_1 = (*jet).p4();
    for (auto mu : *muons_baseline){
      if((*mu).auxdata<char>("passOR") == 0) continue;
      muon_1 = (*mu).p4();
      double DY = fabs(jet_1.Rapidity() - muon_1.Rapidity());
      double Dphi =fabs(jet_1.Phi() - muon_1.Phi());
      if ( Dphi > M_PI ) Dphi = 2*M_PI - Dphi;
      dR = sqrt(DY*DY + Dphi*Dphi);
      if(dR<std::min(0.4, 0.1 + 9600./muon_1.Pt())){
        (*mu).auxdata<char>("passOR") = 0;
        std::cout << "JL: Muon is being removed" << std::endl;
      }
    }
    for (auto el : *electrons_baseline){
      if((*el).auxdata<char>("passOR") == 0) continue;
      electron_1 = (*el).p4();
      double DY = fabs(jet_1.Rapidity() - electron_1.Rapidity());
      double Dphi =fabs(jet_1.Phi() - electron_1.Phi());
      if ( Dphi > M_PI ) Dphi = 2*M_PI - Dphi;
      dR = sqrt(DY*DY + Dphi*Dphi);
      if(dR<std::min(0.4, 0.1 + 9600./electron_1.Pt())){
        (*el).auxdata<char>("passOR") = 0;
        std::cout << "JL: Electron is removed" << std::endl;
      }
    }
  }

}


void DiLepton :: Electron_Muon_OR(xAOD::IParticleContainer *electrons_baseline,xAOD::IParticleContainer *muons_baseline){

  TLorentzVector electron_1;
  TLorentzVector muon_1;
  TLorentzVector electron_2;
  float dR=9999.;

  for (auto el : *electrons_baseline){
    if((*el).auxdata<char>("passOR") == 0) continue;
    electron_1 = (*el).p4();
    for (auto mu : *muons_baseline){
      if((*mu).auxdata<char>("passOR") == 0) continue;
      muon_1 = (*mu).p4();
      double DY = fabs(electron_1.Rapidity() - muon_1.Rapidity());
      double Dphi =fabs(electron_1.Phi() - muon_1.Phi());
      if ( Dphi > M_PI ) Dphi = 2*M_PI - Dphi;
      dR = sqrt(DY*DY + Dphi*Dphi);
      if(dR<0.01){
        (*el).auxdata<char>("passOR") = 0;
      }
    }
    for (auto el2 : *electrons_baseline){
      if((*el2).auxdata<char>("passOR") == 0) continue;
      electron_2 = (*el2).p4();
      if(el == el2) continue;
      double DY = fabs(electron_1.Rapidity() - electron_2.Rapidity());
      double Dphi =fabs(electron_1.Phi() - electron_2.Phi());
      if ( Dphi > M_PI ) Dphi = 2*M_PI - Dphi;
      dR = sqrt(DY*DY + Dphi*Dphi);
      if(dR<0.05){
        if(electron_1.Pt() < electron_2.Pt()) {
          (*el).auxdata<char>("passOR") = 0;
        }
        if(electron_1.Pt() > electron_2.Pt()) {
          (*el2).auxdata<char>("passOR") = 0;
        }
      }
    }
  }


}

bool DiLepton :: inCrackRegion(xAOD::Electron *el){
  float crack[2] = {1.37, 1.52};
  if( TMath::Abs(el->caloCluster()->eta()) > crack[0] && TMath::Abs(el->caloCluster()->eta()) < crack[1] ) return true;
  return false;
}

bool DiLepton :: isSignalMuon(xAOD::Muon &mu, float ptcut, float etamax){

  static SG::AuxElement::Decorator<char> dec_Sig("SIG");
  bool pass(false);

  if(mu.auxdata<char>("passOR")==1 && mu.auxdata<char>("signal")==1 &&  
      mu.pt() > ptcut && TMath::Abs(mu.eta()) < etamax ) pass = true;
  dec_Sig(mu) = pass;
  return pass;
}

bool DiLepton :: isSignalElectron(xAOD::Electron &el, float ptcut, float etamax){

  static SG::AuxElement::Decorator<char> dec_Sig("SIG");
  bool pass(false);
  //if(Debug){std::cout << "sSignalElectron():: passOR: " << (int)el.auxdata<char>("passOR") << std::endl;}
  //if(Debug){std::cout << "sSignalElectron():: signal: " << (int)el.auxdata<char>("signal") << std::endl;}
  //if(Debug){std::cout << "sSignalElectron():: Pt: " << el.pt() << " - Ptcut: " << ptcut << std::endl;}
  //if(Debug){std::cout << "sSignalElectron():: Eta: " << TMath::Abs(el.trackParticle()->eta()) << " - Etacut: " << etamax << std::endl;}

  if(el.auxdata<char>("passOR")==1 && el.auxdata<char>("signal")==1 && 
      TMath::Abs(el.trackParticle()->eta()) < etamax ) pass = true;
      //el.pt() > ptcut && TMath::Abs(el.trackParticle()->eta()) < etamax ) pass = true;
  dec_Sig(el) = pass;

  return pass;
}

bool DiLepton :: isSignalJet(const xAOD::Jet& jet, float ptcut, float EtaMax, float EtaMaxB){

  static SG::AuxElement::Decorator<char> dec_Sig("SIG");
  bool isSignal(false);

  bool isB = SusyObjTool->IsBJet( jet );
  float etacut = isB ? EtaMaxB : EtaMax;
  if( jet.auxdata<char>("passOR")==1 && jet.auxdata<char>("signal")==1
      && TMath::Abs(jet.eta()) < etacut && jet.pt() > ptcut && !badJvt(jet) ) isSignal = true;

  dec_Sig(jet) = isSignal;
  return isSignal;
}

bool DiLepton :: isGoodR1Jet(const xAOD::Jet *jet, float ptCut, float etaCut, float jvtCut){

  const char *APP_NAME = "DiLepton::getR1Jet()"; 
  bool isGood(false);
  if(!jet) return isGood;
  float jvt = jet->isAvailable<float>("Jvt") ? jet->auxdata<float>("Jvt") : 0.;

  if(jet->pt() > ptCut && TMath::Abs(jet->eta()) < etaCut && jvt > jvtCut){
    if(Debug){std::cout<<APP_NAME<<" DEBUG \t selected R1 jet " << Form("Pt=%.1f, Eta=%.3f, Phi=%.3f, M=%.1f",jet->pt(),jet->eta(),jet->phi(),(jet->p4()).M())<<std::endl;}
    isGood = true;
  }
  return isGood;
}


bool DiLepton :: badJvt(const xAOD::Jet& jet, float ptCut, float etaCut, float jvtCut){
  bool isBadJvt(false);
  float jvt = jet.isAvailable<float>("Jvt") ? jet.auxdata<float>("Jvt") : 0.;

  if(jet.pt() < ptCut && TMath::Abs(jet.eta()) < etaCut && jvt < jvtCut){ 
    if(Debug){std::cout << "DiLepton::badJvt()" << " DEBUG Removed jet " << Form("Pt=%.1f, Eta=%.3f, Jvt=%.3f", jet.pt(), jet.eta(), jvt) << std::endl;}
    isBadJvt = true;
  }
  return isBadJvt;
}

void DiLepton :: checkOR(xAOD::IParticleContainer *particles){
  for(auto p : *particles)
    if(p->auxdata<char>("baseline")==0 || p->auxdata<char>("passOR")==0) p->auxdata<char>("passOR") = 0; 
}


bool DiLepton :: isBaseline(xAOD::ElectronContainer* electrons, bool setCuts, double d0Cut){

  const char *APP_NAME = "DiLepton::isBaseline()";
  if( !setCuts ) return true;

  const xAOD::EventInfo* evt(0);
  if( !m_event->retrieve(evt, "EventInfo").isSuccess() ){ Error(APP_NAME, "Error retrieving EventInfo, returning"); return false; }
  for(auto el : *electrons ){
    bool isBaseline = el->auxdata<char>("baseline");
    if(!isBaseline) continue;
    const xAOD::TrackParticle* tp =  el->trackParticle();
    double el_d0sig = xAOD::TrackingHelpers::d0significance(tp, evt->beamPosSigmaX(), evt->beamPosSigmaY(), evt->beamPosSigmaXY() );
    if( inCrackRegion(el) ){
      isBaseline = false;
      if(Debug){std::cout<<APP_NAME<<" DEBUG \t Removed crack el with eta(cluster) = " << el->caloCluster()->eta() <<std::endl;}    
    }
    if(el_d0sig!=0 && TMath::Abs(el_d0sig)>d0Cut){ 
      isBaseline = false;
      if(Debug){std::cout<<APP_NAME<<" DEBUG \t Removed displaced el (eta,phi)="<<Form("(%.2f,%.2f)",el->eta(),el->phi())<<"\t with sig(d0)="<< el_d0sig <<std::endl;}
    }  
    el->auxdata<char>("baseline") = isBaseline; 
  }
  return true;
}

int DiLepton :: getQuality(const xAOD::Muon &mu){
  int type(0);
  if( mu.auxdata<char>("passOR")==1 ) type++;
  if( type>0 && mu.auxdata<char>("SIG")==1 ) type++;
  return type;
}

int DiLepton :: getQuality(const xAOD::Electron &el){
  int type(0);
  if( el.auxdata<char>("passOR")==1 ) type++;
  //if(Debug){std::cout << "passOR: " << type << std::endl;}
  if( type>0 && el.auxdata<char>("SIG")==1 ) type++;
  //if(Debug){std::cout << "getQuality: " << type << std::endl;}
  return type;
}

int DiLepton :: getQuality(const xAOD::Jet &jet){
  int type(0);
  if( jet.auxdata<char>("passOR")==1 ) type++;
  if( type>0 && jet.auxdata<char>("SIG")==1 ) type++;
  return type;
}

int DiLepton :: getNjets(const xAOD::JetContainer *Jets, int seli){
  int N(0); 
  switch( seli ){
    case 1:
      for(auto jet : *Jets){ if( jet->auxdata<char>("passOR")==1 ) N++; }
      break;
    case 2:
      for(auto jet : *Jets){ if( jet->auxdata<char>("signal")==1 ) N++; }
      break;
    case 3:
      for(auto jet : *Jets){ if( jet->auxdata<char>("SIG")==1 ) N++; }
      break;
  }
  return N;
}

std::vector<float> DiLepton :: getBtags(std::vector<TLorentzVector> jetV, const xAOD::JetContainer *jets){  
  std::vector<float> bVec(0);
  if(!jets) return bVec;

  for(std::vector<TLorentzVector>::iterator jIt=jetV.begin(); jIt!=jetV.end(); jIt++){
    for(auto jet : *jets){
      int isB = jet->auxdata<char>("bjet");
      if( this->match(*jIt, jet->p4()) ){ bVec.push_back( isB ); break; } 
    }
  }
  return bVec;
}

std::vector<float> DiLepton :: getTruthInfo(std::vector<TLorentzVector> lep, const xAOD::MuonContainer *muons, const xAOD::ElectronContainer *electrons){

  const char * APP_NAME = "DiLepton::getTruthInfo()";
  std::vector<float> truthInfo(0);
  for(std::vector<TLorentzVector>::iterator it=lep.begin(); it!=lep.end(); it++){
    for(auto mu : *muons){
      int type  = mu->auxdata<int>("truthType"), origin = mu->auxdata<int>("truthOrigin");
      int truth = (type==6);
      if( this->match(*it, mu->p4()) ){ 
        if(Debug){std::cout << APP_NAME <<" DEBUG \t "
          << Form("Mu(pt,eta) (%.2f,%.3f): truthType|Origin = (%i|%i)", mu->pt(), mu->eta(), type, origin) << std::endl;}
        truthInfo.push_back( truth ); break; }
    }    
    for(auto el : *electrons){
      int type  = el->auxdata<int>("truthType"), origin = el->auxdata<int>("truthOrigin");
      int truth = (type==2 || (origin==5 && fabs(el->auxdata<int>("firstEgMotherPdgId"))==11 && (el->charge() * el->auxdata<int>("firstEgMotherPdgId"))<0));
      if( this->match(*it, el->p4()) ){ 
        if(Debug){std::cout << APP_NAME <<" DEBUG \t "
          << Form("El(pt,eta) (%.2f,%.3f): truthType|Origin = (%i|%i)", el->pt(), el->eta(), type, origin) << std::endl;}
        truthInfo.push_back( truth ); break; }
    }
  }
  return truthInfo;
}

void DiLepton :: getSignalLeptons(const char *type, std::vector< TLorentzVector > &leptons, std::vector<float> &charges, std::vector<float> &qual, unsigned int Nlep){

  std::vector <TLorentzVector> l_out;
  std::vector <float> c_out;
  std::vector <float> q_out;

  int qCut(2);
  for(unsigned int i(0); i<leptons.size(); i++){    

    if(Nlep==0) qCut = 1;
    if(Nlep==1 && l_out.size()>0) qCut = 1;
    if(Nlep==2 && l_out.size()>1) qCut = 1;

    if( (int)qual.at(i) >= qCut ){
      l_out.push_back( leptons.at(i) );
      c_out.push_back( charges.at(i) );
      q_out.push_back( qual.at(i) );
      if(Debug){std::cout << "DiLepton::getSignal()" << " DEBUG \t Select "<< type <<" "<< i+1 << " with quality " << (int)qual.at(i) <<std::endl;}
    }
  }
  leptons.clear();
  charges.clear();
  qual.clear();

  leptons = l_out;
  charges = c_out;
  qual    = q_out;  
  return;
}

bool DiLepton :: hasLeptons(std::vector<TLorentzVector> leptons){
  if(leptons.size()<2) return false;
  if(leptons.at(0).Pt()<lep_PtCut[0] || leptons.at(1).Pt()<lep_PtCut[1] ) return false;
  return true;
}

//std::vector<TLorentzVector> DiLepton :: SSpair(std::vector< TLorentzVector > leptons, std::vector<float> charges){
//
//  std::vector<TLorentzVector> SSpair;
//  unsigned int N = leptons.size(); 
//  if( N==2 ){ 
//    if(charges[0]*charges[1]>0){
//      SSpair.push_back(lepTLV.at(0));
//      SSpair.push_back(lepTLV.at(1));
//      return SSpair;
//    }
//  }
//  if( N==3 ){
//    if(charges[0]*charges[1]>0){
//      SSpair.push_back(lepTLV.at(0));
//      SSpair.push_back(lepTLV.at(1));
//      return SSpair;
//    }
//    if(charges[0]*charges[2]>0){
//      SSpair.push_back(lepTLV.at(0));
//      SSpair.push_back(lepTLV.at(2));
//      return SSpair;
//    }
//    if(charges[1]*charges[2]>0){
//      SSpair.push_back(lepTLV.at(1));
//      SSpair.push_back(lepTLV.at(2));
//      return SSpair;
//    }
//  }    
//  if( N>3 ){
//    if(charges[0]*charges[1]>0){
//      SSpair.push_back(lepTLV.at(0));
//      SSpair.push_back(lepTLV.at(1));
//      return SSpair;
//    }
//    if(charges[0]*charges[2]>0){
//      SSpair.push_back(lepTLV.at(0));
//      SSpair.push_back(lepTLV.at(2));
//      return SSpair;
//    }
//    if(charges[0]*charges[3]>0){
//      SSpair.push_back(lepTLV.at(0));
//      SSpair.push_back(lepTLV.at(3));
//      return SSpair;
//    }
//    if(charges[1]*charges[2]>0){
//      SSpair.push_back(lepTLV.at(1));
//      SSpair.push_back(lepTLV.at(2));
//      return SSpair;
//    }
//    if(charges[1]*charges[3]>0){
//      SSpair.push_back(lepTLV.at(1));
//      SSpair.push_back(lepTLV.at(3));
//      return SSpair;
//    }
//    if(charges[2]*charges[3]>0){
//      SSpair.push_back(lepTLV.at(2));
//      SSpair.push_back(lepTLV.at(3));
//      return SSpair;
//    }
//  }
//  return SSpair;
//}

std::vector<TLorentzVector> DiLepton :: SSpair(std::vector< TLorentzVector > leptons, std::vector<float> charges){

  std::vector<TLorentzVector> SSpair;
  unsigned int N = leptons.size(); 
  if( N==2 ){ 
    if(charges[0]*charges[1]>0){
      SSpair.push_back(lepTLV.at(0));
      SSpair.push_back(lepTLV.at(1));
      return SSpair;
    }
  }
  if( N==3 ){
    if(charges[0]*charges[1]>0){
      SSpair.push_back(lepTLV.at(0));
      SSpair.push_back(lepTLV.at(1));
      return SSpair;
    }
    if(charges[0]*charges[2]>0){
      SSpair.push_back(lepTLV.at(0));
      SSpair.push_back(lepTLV.at(2));
      return SSpair;
    }
    if(charges[1]*charges[2]>0){
      SSpair.push_back(lepTLV.at(1));
      SSpair.push_back(lepTLV.at(2));
      return SSpair;
    }
  }    
  if( N>3 ){
    if(charges[0]*charges[1]>0){
      SSpair.push_back(lepTLV.at(0));
      SSpair.push_back(lepTLV.at(1));
      return SSpair;
    }
    if(charges[0]*charges[2]>0){
      SSpair.push_back(lepTLV.at(0));
      SSpair.push_back(lepTLV.at(2));
      return SSpair;
    }
    if(charges[0]*charges[3]>0){
      SSpair.push_back(lepTLV.at(0));
      SSpair.push_back(lepTLV.at(3));
      return SSpair;
    }
    if(charges[1]*charges[2]>0){
      SSpair.push_back(lepTLV.at(1));
      SSpair.push_back(lepTLV.at(2));
      return SSpair;
    }
    if(charges[1]*charges[3]>0){
      SSpair.push_back(lepTLV.at(1));
      SSpair.push_back(lepTLV.at(3));
      return SSpair;
    }
    if(charges[2]*charges[3]>0){
      SSpair.push_back(lepTLV.at(2));
      SSpair.push_back(lepTLV.at(3));
      return SSpair;
    }
  }
  return SSpair;
}

double DiLepton :: getWeight(const xAOD::Jet &jet, DiLepton::JetWeight type, bool msg){

  const char *APP_NAME = "DiLepton::getWeight()";
  bool info = Debug ? msg : false;

  TString name("");
  double weight(0.); 
  switch(type){
    case MV2:
      name = "MV2";
      (jet.btagging())->MVx_discriminant("MV2c10", weight);
      break; 
    case JVT:
      name = "JVT";
      if(jet.isAvailable<float>("Jvt")) weight = jet.auxdata<float>("Jvt");
      break;  
    default:
      Error("DiLepton::getWeight() \t Weight type unknown: Returning ",0);
  }
  if(info){std::cout << APP_NAME <<" DEBUG \t "<< Form("Jet(pt,eta) (%.2f,%.3f)",jet.pt(),jet.eta()) << Form("\t weight %s = %.3f",name.Data(),weight) << std::endl;}
  return weight;
}

void DiLepton :: FillTree(TTree *t, bool fill){
  if(!fill || !t) return;
  if(Debug){std::cout << "DiLepton::FillTree()" << " DEBUG \t filling TTree " << t->GetName() << std::endl;}
  t->Fill();
  return;
}

void DiLepton :: FillHisto(TH1* h, float x, float w, bool fill){ 
  if(!fill) return;
  if(h) h->Fill(x, w); 
  return; 
}

void DiLepton :: FillHisto(TH2* h, float x, float y, float w, bool fill){ 
  if(!fill) return;
  if(h) h->Fill(x, y, w); 
  return; 
}


int DiLepton :: getChannel(std::vector<TLorentzVector> ss, std::vector<TLorentzVector> muons, std::vector<TLorentzVector> electrons, int lepConfig){

  if( lepConfig>0 ){
    if(Debug){std::cout << "DiLepton::Channel()" << " DEBUG \t setLeptons = " << lepConfig << " returning this"  << std::endl;}
    return lepConfig;
  }
  int m(0), e(0); 
  for(std::vector<TLorentzVector>::iterator mu=muons.begin(); mu!=muons.end(); mu++){
    if( match( ss.at(0), *mu ) ) m++; 
    if( match( ss.at(1), *mu ) ) m++;
  }
  for(std::vector<TLorentzVector>::iterator el=electrons.begin(); el!=electrons.end(); el++){
    if( match( ss.at(0), *el ) ) e++; 
    if( match( ss.at(1), *el ) ) e++;
  }
  int ch = (m==1 && e==1) ? 3 : ( m>1 ? 1 : (e>1 ? 2 : 0) );
  if(Debug){std::cout << "DiLepton::Channel()" << " DEBUG \t Lepton channel "<< (TString)Form("%im%ie", m, e) <<" -> returning "<< ch << std::endl;}
  return ch;
}

int DiLepton :: getCutNumber(std::vector<TLorentzVector> particles, float ptcut){
  int n(0);
  std::vector<TLorentzVector>::iterator p = particles.begin();
  std::vector<TLorentzVector>::iterator E = particles.end();
  for( ; p != E; ++p ){ if( (*p).Pt() > ptcut ) n++; }
  return n;
}

int DiLepton :: getLepNumber(std::vector<TLorentzVector> mu, std::vector<TLorentzVector> el, float etacut){
  int n(0);
  for(std::vector<TLorentzVector>::iterator m=mu.begin(); m!=mu.end(); m++)  n++; 
  for(std::vector<TLorentzVector>::iterator e=el.begin(); e!=el.end(); e++){ if(TMath::Abs((*e).Eta()) < etacut) n++; }
  return n;
}

std::vector<float> DiLepton :: getSF(const xAOD::MuonContainer *muons, bool doSF, bool info){  

  bool RecoSF(1), IsoSF(1);
  std::vector<float> SF;
  if(!doSF) return SF;
  for (auto mu : *muons){ 
    float sfTot = SusyObjTool->GetSignalMuonSF(*mu, RecoSF, IsoSF);
    float sfIso = SusyObjTool->GetSignalMuonSF(*mu, 0,      IsoSF);
    float sfRec = SusyObjTool->GetSignalMuonSF(*mu, RecoSF, 0);

    if(Debug && info) Info("DiLepton::getSF()", "Muon(pt=%.1f) SF %f \t RecoSF %f \t IsoSF %f", mu->pt(), sfTot, sfRec, sfIso);
    SF.push_back( sfTot );
  }
  return SF;
}

std::vector<float> DiLepton :: getSF(const xAOD::ElectronContainer *electrons, bool doSF, bool info){

  bool RecoSF(1), IsoSF(1), IdSF(1), TriggerSF(0);
  std::vector<float> SF;
  if(!doSF) return SF;
  for (auto el : *electrons){ 
    float sfTot = SusyObjTool->GetSignalElecSF(*el, RecoSF, IdSF, TriggerSF, IsoSF, "");
    float sfIso = SusyObjTool->GetSignalElecSF(*el,      0,    0,         0, IsoSF, "");
    float sfPID = SusyObjTool->GetSignalElecSF(*el,      0, IdSF,         0,     0, "");
    float sfRec = SusyObjTool->GetSignalElecSF(*el, RecoSF,    0,         0,     0, "");

    if(Debug && info) Info("DiLepton::getSF()", "Electron(pt=%.1f) SF %f \t RecoSF %f \t PidSF %f \t IsoSF %f" , el->pt(), sfTot, sfRec, sfPID, sfIso);
    SF.push_back( sfTot );
  }
  return SF;
}

double DiLepton :: getTotalSF(const xAOD::MuonContainer *mu, const xAOD::ElectronContainer* el, const xAOD::JetContainer *je){

  double sf(1.);
  bool RecoSF(1), IsoSF(1), IdSF(1), TriggerSF(0), BtagSF(1), JvtSF(1);
  std::string TrigExpr(""); 
  if(mu) sf *= SusyObjTool->GetTotalMuonSF(*mu, RecoSF, IsoSF, TrigExpr);
  if(el) sf *= SusyObjTool->GetTotalElectronSF(*el, RecoSF, IdSF, TriggerSF, IsoSF);
  if(je) sf *= SusyObjTool->GetTotalJetSF(je, BtagSF, JvtSF);
  return sf;
}

bool DiLepton :: applySystematic(CP::SystematicSet sys, bool info){

  const char * APP_NAME = "Systematics()";
  if( SusyObjTool->applySystematicVariation(sys) != CP::SystematicCode::Ok ){
    Error(APP_NAME, "Cannot configure SUSYTools for systematic var. %s", sys.name().c_str() );
    return false;
  }
  else{
    if(info) Info(APP_NAME, "Variation \"%s\" configured...", sys.name().c_str() ); 
  }
  return true;
}

bool DiLepton :: resetSystematics(bool info){

  const char * APP_NAME = "Systematics()";
  if( SusyObjTool->resetSystematics() != CP::SystematicCode::Ok ){
    Error(APP_NAME, "Cannot reset SUSYTools systematics" );
    return false;
  }
  else{
    if(info) Info(APP_NAME, "Systematics reset successfully");
  }
  return true;
}

void DiLepton :: PrintSys(std::vector<ST::SystInfo> syslist){

  const char * APP_NAME = "Systematics()";
  for(const auto& sysInfo : syslist){
    const CP::SystematicSet& sys = sysInfo.systset;
    Info(APP_NAME, "Variation \"%s\" configured...", sys.name().c_str() ); 
  }
  return;
}

std::vector<ST::SystInfo> DiLepton :: getSysInfoList(bool fill, std::vector<ST::SystInfo> syslist, DiLepton::SysType type, bool useTau, bool usePhoton){

  const char * APP_NAME = "Systematics()";
  std::vector<ST::SystInfo> output;
  output.push_back( emptySys() ); 
  if(!fill) return output; 

  for(const auto& sysInfo : syslist){
    if(!useTau && ST::testAffectsObject(xAOD::Type::Tau, sysInfo.affectsType) ) continue;
    if(!usePhoton && (ST::testAffectsObject(xAOD::Type::Photon, sysInfo.affectsType) && !ST::testAffectsObject(xAOD::Type::Electron, sysInfo.affectsType)) ) continue;    

    switch(type){
      case Weight:
        if(sysInfo.affectsWeights) output.push_back(sysInfo);
        break;
      case Kinematic:
        if(sysInfo.affectsKinematics) output.push_back(sysInfo);
        break;
      default:
        Error(APP_NAME, "Unknown type of systematics error");
    }
  }
  return output;
}

void DiLepton :: checkNames(std::vector<ST::SystInfo> &syslist, std::vector<std::string> names){

  const char * APP_NAME = "Systematics()";
  if( names.empty() ) return;
  if( names.size()==1 && !names.front().length() ) return;

  Info(APP_NAME, "Checking systematics by names"); 
  std::vector<ST::SystInfo> output;
  output.push_back( emptySys() );

  for(const auto& sysInfo : syslist){
    const CP::SystematicSet& sys = sysInfo.systset;
    for(unsigned int i(0); i<names.size(); i++){
      if( names.at(i)==sys.name() ){ 
        output.push_back(sysInfo); 
        continue;
      }
    }
  }
  if( output.empty() ){ Error(APP_NAME, "No systematics found... returning full list"); return; }

  syslist.clear();
  syslist = output;
  return;
}

void DiLepton :: checkNames(std::vector< SR > &srs, std::vector<std::string> names){

  const char * APP_NAME = "SignalRegion()";
  if( names.empty() ) return;
  if( names.size()==1 && !names.front().length() ) return;

  Info(APP_NAME, "Checking Signal Regions by names"); 
  std::vector < SR > output;  
  for(const auto& sr : srs){
    for(unsigned int i(0); i<names.size(); i++){
      TString srname = names.at(i);
      if( srname == sr.Name ){
        output.push_back( sr );
        continue;
      }
    }
  }
  if( output.empty() ){ Error(APP_NAME, "No systematics found... returning full list"); return; }

  srs.clear();
  srs = output;
  return;
}

ST::SystInfo DiLepton :: emptySys(){

  ST::SystInfo sys;
  sys.affectsKinematics = false;
  sys.affectsWeights = false;
  sys.affectsType = ST::Unknown;
  return sys;
}

CP::SystematicSet DiLepton :: getSys(std::string name, std::vector<ST::SystInfo> syslist){

  CP::SystematicSet sys;
  for(const auto& sysInfo : syslist){
    sys = sysInfo.systset;    
    if(sys.name() == name) return sys; 
  }
  Error("getSys()", "Systematic %s not found, returning", name.c_str());
  return sys;
}

std::vector<std::string> DiLepton :: getSysNames(std::vector<ST::SystInfo> syslist){

  std::vector<std::string> SysNames;
  for(const auto& sysInfo : syslist){
    const CP::SystematicSet& sys = sysInfo.systset;
    SysNames.push_back( sys.name() );
  }
  return SysNames;
}

std::vector<float> DiLepton :: getSysWeights(const xAOD::MuonContainer *Muons, 
    const xAOD::ElectronContainer* Electrons, 
    const xAOD::JetContainer *Jets, 
    std::vector<ST::SystInfo> syslist, 
    std::vector<std::string> &names, unsigned int nsys){

  const char * APP_NAME = "DiLepton::Systematics()";
  std::vector<float> output;
  names.clear();
  bool RecoSF(1), IsoSF(1), IdSF(1), TriggerSF(0),BtagSF(1), JvtSF(1);
  std::string TrigExpr = "";

  this->resetSystematics(Debug);
  float muSF  = SusyObjTool->GetTotalMuonSF(*Muons, RecoSF, IsoSF, TrigExpr);
  float elSF  = SusyObjTool->GetTotalElectronSF(*Electrons, RecoSF, IdSF, TriggerSF, IsoSF);
  float jetSF = SusyObjTool->GetTotalJetSF(Jets, BtagSF, JvtSF);
  float prwSF  = this->getPileupWeight();

  output.push_back( muSF );
  output.push_back( elSF );
  output.push_back( jetSF );
  output.push_back( prwSF );

  names.push_back("MU_SF_NOM");
  names.push_back("EL_SF_NOM");
  names.push_back("JET_SF_NOM");
  names.push_back("PILEUP_NOM");  
  if(Debug){std::cout << APP_NAME << " DEBUG \t Nominal values "<< Form("(muSF,elSF,jetSF,prwSF) = (%.4f, %.4f, %.4f, %.4f)",muSF,elSF,jetSF,prwSF)<<std::endl;}

  for(const auto& sysInfo : syslist){
    const CP::SystematicSet& sys = sysInfo.systset;
    double sf(1.);
    this->applySystematic(sys, 0);
    bool sysMu   = ST::testAffectsObject(xAOD::Type::Muon,     sysInfo.affectsType);
    bool sysEl   = ST::testAffectsObject(xAOD::Type::Electron, sysInfo.affectsType); 
    bool sysJet = (ST::testAffectsObject(xAOD::Type::Jet, sysInfo.affectsType) || ST::testAffectsObject(xAOD::Type::BTag, sysInfo.affectsType));
    bool sysPRW = (ST::testAffectsObject(sysInfo.affectsType) == "EventWeight");

    if(!sysMu && !sysEl && !sysJet && !sysPRW) continue;

    if(sysMu)   sf = SusyObjTool->GetTotalMuonSF(*Muons, RecoSF, IsoSF, TrigExpr);
    if(sysEl)   sf = SusyObjTool->GetTotalElectronSF(*Electrons, RecoSF, IdSF, TriggerSF, IsoSF);
    if(sysJet)  sf = SusyObjTool->GetTotalJetSF(Jets, BtagSF, JvtSF);
    if(sysPRW)  sf = this->getPileupWeight(); 

    if(Debug){std::cout << APP_NAME << " DEBUG \t "<< Form("SF %.4f \t [MU|EL|JET|PRW] = [%i|%i|%i|%i]",sf,sysMu,sysEl,sysJet,sysPRW)<< "\t Syst: "<<sys.name()<<std::endl;} 

    names.push_back( sys.name() );
    output.push_back(sf);
  }
  if(nsys) this->applySystematic( (CP::SystematicSet)SysInfoList_K[nsys].systset, Debug); 
  else this->resetSystematics(Debug);

  return output;
}

std::vector<float> DiLepton :: getChargeFlipSF(const xAOD::ElectronContainer* electrons, unsigned int nsys){

  const char * APP_NAME = "DiLepton::chargeFlipSF()";
  std::vector<float> SF(0);
  CP::SystematicSet CFup, CFdown;
  CFup   = this->getSys("EL_EFF_ChargeIDSel_TOTAL_1NPCOR_PLUS_UNCOR__1up",   SysInfoList);
  CFdown = this->getSys("EL_EFF_ChargeIDSel_TOTAL_1NPCOR_PLUS_UNCOR__1down", SysInfoList);

  this->resetSystematics(Debug);
  SF.push_back( (float)SusyObjTool->GetTotalElectronSF(*electrons,0,0,0,0,"",1) );
  this->applySystematic(CFup, 0);
  SF.push_back( (float)SusyObjTool->GetTotalElectronSF(*electrons,0,0,0,0,"",1) );
  this->applySystematic(CFdown, 0);
  SF.push_back( (float)SusyObjTool->GetTotalElectronSF(*electrons,0,0,0,0,"",1) );

  if(Debug){std::cout << APP_NAME << " DEBUG \t "<< Form("CF SF: %.4f, up=%.4f, down=%.4f", SF[0], SF[1], SF[2]) << std::endl;}

  if(nsys) this->applySystematic( (CP::SystematicSet)SysInfoList_K[nsys].systset, Debug);
  else this->resetSystematics(Debug);

  return SF;
}

bool DiLepton :: containStr(std::vector<std::string> v, std::string name){
  bool cont(false);
  for(std::vector<std::string>::iterator it=v.begin(); it!=v.end(); it++){ if(*it == name) cont = true; }
  return cont;
}

std::vector<std::string> DiLepton :: tokenize(std::string s){

  std::vector<std::string> v;
  if( !(bool)s.length() ) return v;

  TObjArray * strings = ((TString)s).Tokenize(",");
  for(int i(0); i<strings->GetEntries(); i++){ 
    TString part = ((TObjString*)strings->At(i))->GetString();
    part.ReplaceAll(" ","");
    v.push_back( (std::string)part );    
  }
  return v;
}

bool DiLepton :: truthVeto(int MCId, float trHt, float trMET){
  if(!MCId) return false;

  bool combHt(true), combMET(false);
  if(combMET){
    if(MCId==407012 && trMET<200000.) return true;
    if(MCId==410000 && trMET>200000.) return true;
  }
  if(combHt){
    if(MCId==410000 && trHt>600000.) return true;
    if(MCId==407009 && (trHt<600000.  || trHt>1000000.) ) return true;
    if(MCId==407010 && (trHt<1000000. || trHt>1500000.) ) return true;
    if(MCId==407011 && trHt<1500000. ) return true;
  }
  return false;
}

float DiLepton :: getTruthVar(xAOD::TEvent *event, int type){

  const char * APP_NAME = "DiLepton::getTruthInfo()";
  float out(0); TString var("");
  const xAOD::EventInfo* evt(0);
  if( !event->retrieve(evt, "EventInfo").isSuccess() ){ Error(APP_NAME, "Error retrieving EventInfo"); return out; }
  try{
    switch(type){
      case 0: 
        out = evt->isAvailable<float>("GenFiltHT") ? evt->auxdata<float>("GenFiltHT") : 0.; 
        var = "HT"; 
        break;
      case 1: 
        out = evt->isAvailable<float>("GenFiltMET") ? evt->auxdata<float>("GenFiltMET") : 0.; 
        var = "MET"; 
        break;
      default: break;
    }
  }
  catch(SG::ExcBadAuxVar&){ Info(APP_NAME, "WARNING: Truth metadata is not available"); }

  if(Debug){std::cout << APP_NAME << " DEBUG \t TruthVar " << var << " :: " << out << std::endl;}
  return out;
}

std::vector<float> DiLepton :: getPDFTruthInfo(xAOD::TEvent *event){

  const char * APP_NAME = "DiLepton::getTruthInfo()";
  std::vector<float> pdfinfo;
  if(isData) return pdfinfo;  

  const xAOD::TruthEventContainer* truthEv(0);
  if( !event->retrieve( truthEv, "TruthEvents" ).isSuccess() ){
    Error( APP_NAME, "Failed to retrieve truth-info" );
    return pdfinfo;
  }
  if(Debug){std::cout << APP_NAME <<" DEBUG \t Found "<< (int)truthEv->size() << " events in xAOD::TruthContainer" << std::endl;}

  xAOD::TruthEventContainer::const_iterator itr = truthEv->begin();
  xAOD::TruthEventContainer::const_iterator end = truthEv->end();
  int id1(0), id2(0), pdf1(0), pdf2(0);
  float x1(0), x2(0), q(0);
  for( ; itr != end; ++itr ){
    try{
      (*itr)->pdfInfoParameter(id1, xAOD::TruthEvent::PDGID1);
      (*itr)->pdfInfoParameter(id2, xAOD::TruthEvent::PDGID2);
      (*itr)->pdfInfoParameter(x1,  xAOD::TruthEvent::X1);
      (*itr)->pdfInfoParameter(x2,  xAOD::TruthEvent::X2);
      (*itr)->pdfInfoParameter(q,   xAOD::TruthEvent::Q);
      (*itr)->pdfInfoParameter(pdf1,xAOD::TruthEvent::PDFID1);
      (*itr)->pdfInfoParameter(pdf2,xAOD::TruthEvent::PDFID2);
    }
    catch(SG::ExcBadAuxVar&){ Info(APP_NAME, "WARNING: Incomplete decoration of xAOD::TruthEvent"); }
  }
  if(Debug){std::cout << APP_NAME <<" DEBUG \t "<< Form("PDF Info: Id1|Id2: %i|%i, X1|X2: %f|%f, Q: %f, Pdf1|Pdf2 %i|%i",id1,id2,x1,x2,q,pdf1,pdf2) << std::endl;}
  if(!id1 || !id2) return pdfinfo;

  pdfinfo.push_back(id1);
  pdfinfo.push_back(id2);
  pdfinfo.push_back(x1); 
  pdfinfo.push_back(x2); 
  pdfinfo.push_back(q);
  pdfinfo.push_back(pdf1);
  pdfinfo.push_back(pdf2);  
  return pdfinfo;
}

/*std::vector<float> DiLepton :: getPDFWeights(xAOD::TEvent *event){

  const char * APP_NAME = "DiLepton::getPDFweight()";
  std::vector<float> weights(0);
  if(isData || !doPDF) return weights; 

  std::vector<double> pdfweights(0);
  float pdfSF(1.), pdfSFup(1.), pdfSFdown(1.);
  std::vector<float> PDFInfo = this->getPDFTruthInfo(event);
  if( PDFInfo.size()>4 ){
  for(size_t i=0; i<=PdfSet->size()-1; i++) pdfweights.push_back(LHAPDF::weightxxQ2(PDFInfo[0],PDFInfo[1],PDFInfo[2],PDFInfo[3],PDFInfo[4],PDFs[0],PDFs[i] ) );
  }
  else{ Info(APP_NAME, "WARNING: PDF Info couldn't be retrieved"); }

  if(!pdfweights.empty()){
  LHAPDF::PDFUncertainty PDFSF = PdfSet->uncertainty(pdfweights, 68);
  pdfSF     = PDFSF.central;
  pdfSFup   = (pdfSF + PDFSF.errplus)/pdfSF;
  pdfSFdown = (pdfSF - PDFSF.errminus)/pdfSF;
  }
  if(Debug){std::cout << APP_NAME <<" DEBUG \t "<< Form("PDF Weight: %f, errUp %f, errDown %f", pdfSF, pdfSFup, pdfSFdown) << std::endl;}

  weights.push_back(pdfSF);
  weights.push_back(pdfSFup);
  weights.push_back(pdfSFdown);
  return weights;
  }
  */
std::string DiLepton :: getPDFName(std::string sample, bool set){

  TString name = sample;
  std::string setName("NNPDF23_lo_as_0130_qed");
  if(!set) return setName;

  if(name.Contains("CT10"))  setName = "CT10";
  if(name.Contains("NNPDF")) setName = "NNPDF23_lo_as_0130_qed";    
  if(name.Contains("AZNLO")) setName = "CT10";
  if(name.Contains("A14"))   setName = "NNPDF23_lo_as_0130_qed";
  return setName; 
}

int DiLepton :: truthID(const xAOD::Jet *jet, bool isMC){
  int id(0);
  if(!isMC || !jet) return id; 
  if(jet->auxdata<char>("isW"))     id = 1;
  if(jet->auxdata<char>("isZ"))     id = id+1E1;
  if(jet->auxdata<char>("isLQ"))    id = id+1E2;
  if(jet->auxdata<char>("isTop"))   id = id+1E3;
  if(jet->auxdata<char>("isLepPh")) id = id+1E4;
  if(jet->auxdata<char>("isMeson")) id = id+1E5;
  return id;
}

bool DiLepton :: getJetOrigin(const xAOD::JetContainer *Jets, xAOD::TEvent *event, bool isMC, bool msg){

  bool info = Debug ? msg : false;
  const char * APP_NAME = "DiLepton::getJetOrigin()";  
  if(!isMC || !Jets) return true;

  for (auto jet : *Jets){ 
    if( !this->getMotherParticle(jet, event, info) ){ Error( APP_NAME, "Failed to get mother particle of jet"); return false; }

    if(info){std::cout << APP_NAME << " DEBUG \t " << Form("Jet (Pt=%.1f|Eta=%.3f|Phi=%.3f|M=%.1f) \t",jet->pt(),jet->eta(),jet->phi(),(jet->p4()).M()) 
      << Form("( W=%i | Z=%i | LQ=%i | Top=%i | LepPh=%i | Meson=%i ) ID=%i",
          (int)jet->auxdata<char>("isW"), (int)jet->auxdata<char>("isZ"), (int)jet->auxdata<char>("isLQ"),
          (int)jet->auxdata<char>("isTop"), (int)jet->auxdata<char>("isLepPh"), 
          (int)jet->auxdata<char>("isMeson"), (int)truthID(jet, isMC) ) << std::endl;}
  }
  return true;
}

bool DiLepton :: getMotherParticle(const xAOD::Jet *jet, xAOD::TEvent *event, bool msg){

  bool info = Debug ? msg : false;
  const char * APP_NAME = "DiLepton::getMotherParticle()";

  static SG::AuxElement::Decorator<char> dec_W("isW");
  static SG::AuxElement::Decorator<char> dec_Z("isZ");
  static SG::AuxElement::Decorator<char> dec_LQ("isLQ");
  static SG::AuxElement::Decorator<char> dec_Top("isTop");
  static SG::AuxElement::Decorator<char> dec_LepPh("isLepPh");
  static SG::AuxElement::Decorator<char> dec_Meson("isMeson");

  bool isW(0), isZ(0), isLQ(0), isTop(0), isLepPh(0), isMeson(0);
  float ptmin(10000.), dRmax(0.5);

  const xAOD::TruthParticleContainer* truthP(0);
  if( !event->retrieve( truthP, "TruthParticles" ).isSuccess() ){ Error( APP_NAME, "Failed to retrieve truth-info" ); return false; }
  for (auto tp : *truthP) { 
    if(tp->pt() < ptmin) continue;
    float dr = this->dR(jet->p4(),tp->p4());
    if(dr > dRmax) continue;

    if(tp->isW())     isW = true;
    if(tp->isZ())     isZ = true;
    if(tp->isQuark()) isLQ = true;
    if(tp->isTop())   isTop = true;
    if(tp->isMeson()) isMeson = true;
    if(tp->isLepton() || tp->isPhoton()) isLepPh = true;

    if(info){std::cout << APP_NAME << " DEBUG \t " 
      << Form("Jet (Pt=%.1f|Eta=%.3f|Phi=%.3f|M=%.1f) \t",jet->pt(),jet->eta(),jet->phi(),(jet->p4()).M())
        << Form("Found truthParticle (Pt=%.1f|Eta=%.3f|Phi=%.3f) dR=%.3f \t (PDG=%i|W=%i|Z=%i|LQ=%i|Top=%i|LepPh=%i|Meson=%i)",
            tp->pt(), tp->eta(), tp->phi(), dr, tp->pdgId(), isW, isZ, isLQ, isTop, isLepPh, isMeson) << std::endl;}

    this->quarkdR(tp, info);

    if(!this->getParents(tp, isW, isZ, isLQ, isTop, isLepPh, isMeson, info)){ std::cout << APP_NAME << " DEBUG \t " << "Cannot retrieve parent particles " << std::endl; }
  }

  dec_W( *jet ) = isW;
  dec_Z( *jet ) = isZ;
  dec_LQ( *jet )  = isLQ;
  dec_Top( *jet ) = isTop;
  dec_LepPh( *jet ) = isLepPh;
  dec_Meson( *jet ) = isMeson;

  return true;
}

bool DiLepton :: getParents(const xAOD::TruthParticle *tp, bool &isW, bool &isZ, bool &isLQ, bool &isTop, bool &isLepPh, bool &isMeson, bool msg){

  bool info = Debug ? msg : false;
  const char * APP_NAME = "DiLepton::getParents()";

  for(size_t i(0); i<tp->nParents(); i++){ 
    const xAOD::TruthParticle *par = tp->parent(i); 

    if(!isW) this->quarkdR(par, info);

    if(par->isW())     isW = true;
    if(par->isZ())     isZ = true;
    if(par->isQuark()) isLQ = true;
    if(par->isTop())   isTop = true;
    if(par->isMeson()) isMeson = true;
    if(par->isLepton() || par->isPhoton()) isLepPh = true;

    if(info){std::cout << APP_NAME << " DEBUG \t " << Form("\t Parent no %i (PDG=%i|W=%i|Z=%i|LQ=%i|Top=%i|LepPh=%i|Meson=%i)",(int)i,par->pdgId(),isW,isZ,isLQ,isTop,isLepPh,isMeson) << std::endl;}
  }
  return true;
}

float DiLepton :: quarkdR(const xAOD::TruthParticle *tp, bool msg){

  bool info = Debug ? msg : false;
  const char * APP_NAME = "DiLepton::quarkdR()";

  float dr(0.);
  if(!tp) return dr;
  if(!tp->isW()) return dr;

  std::vector<TLorentzVector> quarks; std::vector<float> charge,pdgIDs;
  for(size_t i(0); i<tp->nChildren(); i++){
    const xAOD::TruthParticle *ch = tp->child(i);  
    if(!ch->isQuark()) continue;  
    if(ch->pt()<1E3)   continue;
    quarks.push_back(ch->p4());
    charge.push_back(ch->charge());
    pdgIDs.push_back(ch->pdgId());
  }
  if(quarks.size()<2) return dr;

  sortPt(quarks,charge,pdgIDs);
  dr = this->dR(quarks[0], quarks[1]);

  if(info){std::cout << APP_NAME << " DEBUG \t " 
    << Form("W(pt=%.1f) :: Q1[%i](pt=%.1f|eta=%.3f|phi=%.3f)  Q2[%i](pt=%.1f|eta=%.3f|phi=%.3f) dR=%.3f",tp->pt(),
        (int)pdgIDs[0],quarks[0].Pt(),quarks[0].Eta(),quarks[0].Phi(),(int)pdgIDs[1],quarks[1].Pt(),quarks[1].Eta(),quarks[1].Phi(),dr) << std::endl;}

  return dr;
}

/*float DiLepton :: getTriggerSF(int yearOpt, int randomRN, const xAOD::MuonContainer *Muons, const xAOD::ElectronContainer* Electrons, float &sferr,
  float met, std::vector<std::string> triggerInfo, bool doSF, bool info, float metcut){

  const char* APP_NAME = "DiLepton::getTriggerSF()";
  double sf(1.), unc(0);
  if( !doSF ) return sf; 
  if( Muons->empty() && Electrons->empty() ) return sf;

  std::vector<std::string> trigList;
  switch( yearOpt ){
  case 2015: trigList = triggerList2015; break;
  case 2016: trigList = triggerList2016; break;
  default: break;
  }
  if( trigList.empty() ) return sf; 
  if( met>metcut && this->nMetTrigger(triggerInfo) ){
  if(Debug){std::cout << APP_NAME << Form(" DEBUG \t Event is triggered by %i EtMiss trigger, return %.1f", this->nMetTrigger(triggerInfo) ,sf) << std::endl;}
  return sf;
  }

  DileptonTriggerWeight triggerSFTool;
  triggerSFTool.Reset();
  for(auto mu : *Muons)      triggerSFTool.AddMuon(mu->pt(), mu->eta(), mu->phi(), randomRN);      
  for(auto el : *Electrons ) triggerSFTool.AddElectron(el->pt(), el->eta(), randomRN, !isAtlfast);   

  sf *= triggerSFTool.GetScaleFactor(unc);
  sferr = (float)unc;
  if(info){std::cout << APP_NAME << Form(" DEBUG \t Trigger SF for %im%ie = %f (err=%.3f)",(int)Muons->size(), (int)Electrons->size(), sf, sferr) << std::endl;}  

  return (float)sf;
  }
  */
/*bool DiLepton :: InitializeTriggerTools(std::string var){

  const char* APP_NAME = "InitializeTriggerTools()";
  LegsPerTool.clear();
  Info( APP_NAME, "Starting to initialize single lepton efficiency tools for GlobalEfficiencyCorrectionTool[%s]", var.c_str() );

  electronEffTools.clear();
  electronEffTools.clear();
  electronSFTools.clear();
  electronSFTools.clear();
  muonTools.clear();
  muonTools.clear();

//Trigger efficiency tool for 1st electron
AsgElectronEfficiencyCorrectionTool* elEffTool1 = new AsgElectronEfficiencyCorrectionTool("ElTrigEff-1"+var);
elEffTool1->msg().setLevel( MSG::FATAL );
elEffTool1->setProperty("MapFilePath","/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/map0.txt").ignore();
elEffTool1->setProperty("TriggerKey","Eff_MULTI_L_2015_e17_lhloose_2016_e17_lhloose_nod0").ignore();
elEffTool1->setProperty("IdKey","Medium").ignore();
elEffTool1->setProperty("IsoKey","FixedCutTight").ignore(); 
elEffTool1->setProperty("CorrelationModel","TOTAL").ignore();
if(!isData){  
if(isAtlfast) elEffTool1->setProperty("ForceDataType", (int) PATCore::ParticleDataType::Fast).ignore();
else elEffTool1->setProperty("ForceDataType", (int) PATCore::ParticleDataType::Full).ignore();
}
if(elEffTool1->initialize() != StatusCode::SUCCESS){ Error(APP_NAME, "Unable to initialize ElectronTriggerEfficiencyTool-1"); return false;}
else{ Info(APP_NAME, "Initialized ElectronTriggerEfficiencyTool-1");}
LegsPerTool["ElTrigEff-1"+var] = "e17_lhloose,e17_lhloose_nod0";

//Trigger SF tool for 1st electron
AsgElectronEfficiencyCorrectionTool* elSFTool1 = new AsgElectronEfficiencyCorrectionTool("ElTrigSF-1"+var);
elSFTool1->msg().setLevel( MSG::FATAL );
elSFTool1->setProperty("MapFilePath","/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/map0.txt").ignore();
elSFTool1->setProperty("TriggerKey","MULTI_L_2015_e17_lhloose_2016_e17_lhloose_nod0").ignore();
elSFTool1->setProperty("IdKey","Medium").ignore();
elSFTool1->setProperty("IsoKey","FixedCutTight").ignore();
elSFTool1->setProperty("CorrelationModel","TOTAL").ignore();
if(!isData){  
if(isAtlfast) elSFTool1->setProperty("ForceDataType", (int) PATCore::ParticleDataType::Fast).ignore();
else elSFTool1->setProperty("ForceDataType", (int) PATCore::ParticleDataType::Full).ignore();
}
if(elSFTool1->initialize() != StatusCode::SUCCESS){ Error(APP_NAME, "Unable to initialize ElectronTriggerSFTool-1"); return false;}
else{ Info(APP_NAME, "Initialized ElectronTriggerSFTool-1");}
LegsPerTool["ElTrigSF-1"+var] = LegsPerTool["ElTrigEff-1"+var];

//Trigger efficiency tool for 2nd electron
AsgElectronEfficiencyCorrectionTool* elEffTool2 = new AsgElectronEfficiencyCorrectionTool("ElTrigEff-2"+var);
elEffTool2->msg().setLevel( MSG::FATAL );
elEffTool2->setProperty("MapFilePath","/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/map0.txt").ignore();
elEffTool2->setProperty("TriggerKey","Eff_DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0").ignore();
elEffTool2->setProperty("IdKey","Medium").ignore();
elEffTool2->setProperty("IsoKey","FixedCutTight").ignore();
elEffTool2->setProperty("CorrelationModel","TOTAL").ignore();
if(!isData){  
if(isAtlfast) elEffTool2->setProperty("ForceDataType", (int) PATCore::ParticleDataType::Fast).ignore();
else elEffTool2->setProperty("ForceDataType", (int) PATCore::ParticleDataType::Full).ignore(); 
}
if(elEffTool2->initialize() != StatusCode::SUCCESS){ Error(APP_NAME, "Unable to initialize ElectronTriggerEfficiencyTool-2"); return false;}
else{ Info(APP_NAME, "Initialized ElectronTriggerEfficiencyTool-2");}
LegsPerTool["ElTrigEff-2"+var] = "e12_lhloose_L1EM10VH,e17_lhvloose_nod0";

//Trigger SF tool for 2nd electron
AsgElectronEfficiencyCorrectionTool* elSFTool2 = new AsgElectronEfficiencyCorrectionTool("ElTrigSF-2"+var);
elSFTool2->msg().setLevel( MSG::FATAL );
elSFTool2->setProperty("MapFilePath","/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/map0.txt").ignore();
elSFTool2->setProperty("TriggerKey","DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0").ignore();
elSFTool2->setProperty("IdKey","Medium").ignore();
elSFTool2->setProperty("IsoKey","FixedCutTight").ignore();
elSFTool2->setProperty("CorrelationModel","TOTAL").ignore();
if(!isData){  
if(isAtlfast) elSFTool2->setProperty("ForceDataType", (int) PATCore::ParticleDataType::Fast).ignore();
else elSFTool2->setProperty("ForceDataType", (int) PATCore::ParticleDataType::Full).ignore();
}
if(elSFTool2->initialize() != StatusCode::SUCCESS){ Error(APP_NAME, "Unable to initialize ElectronTriggerSFTool-2"); return false;}
else{ Info(APP_NAME, "Initialized ElectronTriggerSFTool-2");}
LegsPerTool["ElTrigSF-2"+var] = LegsPerTool["ElTrigEff-2"+var];


// Trigger SF tool for 1st muon
CP::MuonTriggerScaleFactors* toolMuons1 =  new CP::MuonTriggerScaleFactors("MuonTrigEff-2015"+var);
toolMuons1->msg().setLevel( MSG::FATAL );
toolMuons1->setProperty("CalibrationRelease", "170209_Moriond").ignore();
toolMuons1->setProperty("MuonQuality", "Tight").ignore();
toolMuons1->setProperty("Isolation", "GradientLoose").ignore();
toolMuons1->setProperty("Year", "2015").ignore();
if(toolMuons1->initialize() != StatusCode::SUCCESS){ Error(APP_NAME, "Unable to initialize MuonTriggerScaleFactorsTool"); return false;}
else{ Info(APP_NAME, "Initialized MuonTriggerScaleFactorsTool for 2015");}

// Trigger SF tool for 2nd muon
CP::MuonTriggerScaleFactors* toolMuons2 =  new CP::MuonTriggerScaleFactors("MuonTrigEff-2016"+var);
toolMuons2->msg().setLevel( MSG::FATAL );
toolMuons2->setProperty("CalibrationRelease", "170209_Moriond").ignore();
toolMuons2->setProperty("MuonQuality", "Tight").ignore();
toolMuons2->setProperty("Isolation", "GradientLoose").ignore();
toolMuons2->setProperty("Year", "2016").ignore();
if(toolMuons2->initialize() != StatusCode::SUCCESS){ Error(APP_NAME, "Unable to initialize MuonTriggerScaleFactorsTool"); return false;}
else{ Info(APP_NAME, "Initialized MuonTriggerScaleFactorsTool for 2016");}


//Systematics
CP::SystematicSet sysEffEl, sysSFEl, sysSFMu;
if(var=="UP"){   
  sysEffEl = this->getSys("EL_EFF_TriggerEff_TOTAL_1NPCOR_PLUS_UNCOR__1up", SysInfoList);
  sysSFEl  = this->getSys("EL_EFF_Trigger_TOTAL_1NPCOR_PLUS_UNCOR__1up",    SysInfoList);    
  sysSFMu  = this->getSys("MUON_EFF_TrigStatUncertainty__1up",              SysInfoList);
}
if(var=="DOWN"){
  sysEffEl = this->getSys("EL_EFF_TriggerEff_TOTAL_1NPCOR_PLUS_UNCOR__1down", SysInfoList);
  sysSFEl  = this->getSys("EL_EFF_Trigger_TOTAL_1NPCOR_PLUS_UNCOR__1down",    SysInfoList);
  sysSFMu  = this->getSys("MUON_EFF_TrigStatUncertainty__1down",              SysInfoList);
}

if( !var.empty() ){
  if(elEffTool1->applySystematicVariation(sysEffEl) != CP::SystematicCode::Ok || 
      elEffTool2->applySystematicVariation(sysEffEl) != CP::SystematicCode::Ok ){ 
    Error(APP_NAME, "Unable to apply systematic variation %s",sysEffEl.name().c_str()); return false; }
  else 
    Info(APP_NAME, "Applied systematic variation %s",sysEffEl.name().c_str()); 

  if(elSFTool1->applySystematicVariation(sysSFEl) != CP::SystematicCode::Ok ||
      elSFTool2->applySystematicVariation(sysSFEl) != CP::SystematicCode::Ok ){ 
    Error(APP_NAME, "Unable to apply systematic variation %s",sysSFEl.name().c_str()); return false; }
  else
    Info(APP_NAME, "Applied systematic variation %s",sysSFEl.name().c_str()); 

  if(toolMuons1->applySystematicVariation(sysSFMu) != CP::SystematicCode::Ok ||
      toolMuons2->applySystematicVariation(sysSFMu) != CP::SystematicCode::Ok ){ 
    Error(APP_NAME, "Unable to apply systematic variation %s",sysSFMu.name().c_str()); return false; }
  else 
    Info(APP_NAME, "Applied systematic variation %s",sysSFMu.name().c_str()); 
}

electronEffTools.push_back(elEffTool1);
electronEffTools.push_back(elEffTool2);
electronSFTools.push_back(elSFTool1);
electronSFTools.push_back(elSFTool2);
muonTools.push_back(toolMuons1);
muonTools.push_back(toolMuons2);

Info( APP_NAME, "Initialized single lepton efficiency tools for GlobalEfficiencyCorrectionTool[%s]", var.c_str() );
return true;
}

std::vector<float> DiLepton :: getGlobTriggerSF(int yearOpt, const xAOD::MuonContainer *Muons, const xAOD::ElectronContainer* Electrons, 
    float met, std::vector<std::string> triggerInfo, bool doSF, bool info, float metcut){

  const char* APP_NAME = "DiLepton::getTriggerSF()";

  std::vector<float> sf(3);
  for(unsigned int i(0); i<sf.size(); i++) sf[i] = 1.;

  if( !doSF ) return sf; 
  if( Muons->empty() && Electrons->empty() ) return sf;

  std::vector<std::string> trigList;
  switch( yearOpt ){
    case 2015: trigList = triggerList2015; break;
    case 2016: trigList = triggerList2016; break;
    default: break;
  }
  if( trigList.empty() ) return sf; 
  if( met>metcut && this->nMetTrigger(triggerInfo) ){
    if(Debug){std::cout << APP_NAME << Form(" DEBUG \t Event is triggered by %i EtMiss trigger, return 1", this->nMetTrigger(triggerInfo)) << std::endl;}
    return sf;
  }

  double nom(1.), up(1.), down(1.);
  std::vector<const xAOD::Muon*> trigMu(0);
  std::vector<const xAOD::Electron*> trigEl(0);

  for(auto mu : *Muons) trigMu.push_back(mu);
  for(auto el : *Electrons) trigEl.push_back(el);

  bool cc1 = (TriggerSFTool->getEfficiencyScaleFactor(trigEl, trigMu, nom) == CP::CorrectionCode::Ok);
  bool cc2 = (TriggerSFToolUP->getEfficiencyScaleFactor(trigEl, trigMu, up) == CP::CorrectionCode::Ok);
  bool cc3 = (TriggerSFToolDOWN->getEfficiencyScaleFactor(trigEl, trigMu, down) == CP::CorrectionCode::Ok);  
  if( !cc1 || !cc2 || !cc3 ){ 
    Warning(APP_NAME, "SF evaluation failed [%i|%i|%i], returning %.1f",cc1,cc2,cc3,sf[0]); 
    return sf; 
  }

  if(info){std::cout << APP_NAME << Form(" DEBUG \t Trigger SF for %im%ie = %.6f (up=%.6f, down=%.6f)",
      (int)trigMu.size(),(int)trigEl.size(),nom,up,down) << std::endl;} 
  sf[0] = (float)nom;
  sf[1] = (float)up;
  sf[2] = (float)down;

  return sf;
}
*/
std::vector<int> DiLepton :: getSusyPDG(xAOD::TEvent *event){

  const char* APP_NAME = "DiLepton::getSusyPDG()";
  std::vector<int> pdgs(4);

  const xAOD::EventInfo* evt(0);
  if( !event->retrieve(evt, "EventInfo").isSuccess() ){
    Error(APP_NAME, "Error retrieving EventInfo"); return pdgs;
  }
  if( 370000 > evt->mcChannelNumber() || evt->mcChannelNumber() > 450000 ) return pdgs;

  int pdgid1(0), pdgid2(0), gluDecay1(0), gluDecay2(0);
  const xAOD::TruthParticleContainer* truthP(0);
  if( !event->retrieve( truthP, "TruthParticles" ).isSuccess() ){
    Error( APP_NAME, "Failed to retrieve truth-info" ); return pdgs;
  }
  if( !SusyObjTool->FindSusyHP(truthP, pdgid1, pdgid2) ) return pdgs;

  for (const auto& tp : *truthP) {
    if (tp->pdgId() == 1000021){
      int _ch[2]={0}; int ich(0);
      bool foundSUSY(false);
      if (tp->nChildren() == 3) {
        for(unsigned int ch(0); ch<tp->nChildren(); ch++){
          for (const auto& tpC : *truthP) {
            if (tpC->barcode() == tp->child(ch)->barcode() ) {
              if((abs(tpC->pdgId()) > 1000020 && abs(tpC->pdgId()) < 1000040)){
                foundSUSY = true;
              }else{
                _ch[ich]=tpC->pdgId(); 
                ich++;
              }
            }
          }
        }
      }
      if(foundSUSY && _ch[0]==-1*_ch[1]){
        if(gluDecay1==0) gluDecay1 = abs(_ch[0]);
        else gluDecay2 = abs(_ch[0]);
      }
    }
  }
  if(Debug){std::cout << APP_NAME << " DEBUG \t "<< Form("pdgId1=%i | pdgId2=%i | GLDec1=%i | GLDec2=%i",pdgid1,pdgid2,gluDecay1,gluDecay2) << std::endl;}
  pdgs[0] = pdgid1;
  pdgs[1] = pdgid2;
  pdgs[2] = gluDecay1;
  pdgs[3] = gluDecay2;

  return pdgs;
}

void DiLepton :: executeJRTool(const xAOD::JetContainer *Jets, xAOD::JetContainer &RecJets){

  const char* APP_NAME = "DiLepton::executeJRTool()";
  xAOD::TStore* str = xAOD::TActiveStore::store();

  xAOD::JetContainer* jets = new xAOD::JetContainer();
  xAOD::AuxContainerBase* jetsAux = new xAOD::AuxContainerBase();
  jets->setStore(jetsAux);

  for ( xAOD::JetContainer::const_iterator jetItr = Jets->begin(); jetItr != Jets->end(); ++jetItr ){
    xAOD::Jet* jet = new xAOD::Jet();
    jet->makePrivateStore(*jetItr);
    jets->push_back(jet) ;
  }
  if( !(str->record(jets,    "InputJets"   ).isSuccess()) ){ Error(APP_NAME, "Failed to record jet collection to store" ); return; };
  if( !(str->record(jetsAux, "InputJetsAux").isSuccess()) ){ Error(APP_NAME, "Failed to record jet collection to store" ); return; };

  JRTool->execute();
  xAOD::JetContainer* JetsR1(0);
  if( !(str->retrieve(JetsR1, "OutputJets").isSuccess()) ){ Error(APP_NAME, "Failed to retrieve event info collection. Exiting." ); return; }

  if(Debug){ std::cout << APP_NAME << " DEBUG \t "<< Form("Retrieved fat jet collection with size %i", (int)JetsR1->size() ) << std::endl;}
  for( auto jet : *JetsR1){
    RecJets.push_back(jet);
    if(Debug){std::cout << APP_NAME << " DEBUG \t "<< Form("JetR1 Pt|Eta|Phi \t %.2f | %.3f | %.3f", jet->pt(), jet->eta(), jet->phi()) << std::endl;} 
  }
  return;
}


double DiLepton :: getSubstrVar(const xAOD::Jet *jet, TString name, bool msg){
  bool info = Debug ? msg : false;
  double var(0.);
  if(!jet) return var;
  if(jet->numConstituents() < 2) return var;

  if( name=="D2" ){
    if(jet->isAvailable<float>("D2")) var = jet->auxdata<float>("D2");
    else{
      if(info){std::cout << "DiLepton::getSubstrVar()" << " DEBUG \t "<< "D2 not available, using ECF" << std::endl;} 
      if(jet->isAvailable<float>("ECF1") && jet->isAvailable<float>("ECF2") && jet->isAvailable<float>("ECF3")) 
        var = jet->auxdata<float>("ECF3") * TMath::Power((jet->auxdata<float>("ECF1"))/(jet->auxdata<float>("ECF2")),3);
      else{ if(info){std::cout << "DiLepton::getSubstrVar()" << " DEBUG \t "<< "ECF not available, returning" << std::endl;} }
    }
  }
  if( name=="C2" ){
    if(jet->isAvailable<float>("C2")) var = jet->auxdata<float>("C2");
    else{ if(info){std::cout << "DiLepton::getSubstrVar()" << " DEBUG \t "<< "C2 not available, returning" << std::endl;} }
  }
  if( name=="D12" ){
    if(jet->isAvailable<float>("Split12")) var = jet->auxdata<float>("Split12");
    else{ if(info){std::cout << "DiLepton::getSubstrVar()" << " DEBUG \t "<< "Split12 not available, returning" << std::endl;} }
  }
  return var;
}

std::vector< std::vector<float> > DiLepton::getChargeFlipInfo(const xAOD::ElectronContainer *electrons){

  std::vector< std::vector<float> > CFInfo(0);
  if(!electrons || !CFTool || !CFToolLoose) return CFInfo;

  std::vector<float> bdtM(0),bdtL(0);
  for (auto el : *electrons){
    float wM = getQuality(*el) ? CFTool->calculate(el) : -99.;
    float wL = getQuality(*el) ? CFToolLoose->calculate(el) : -99.;
    bdtM.push_back(wM);
    bdtL.push_back(wL);
  }
  CFInfo.push_back(bdtM);
  CFInfo.push_back(bdtL);

  return CFInfo;
}

float DiLepton :: getDPSweight(int MCId, int njets){

  float w(1.);
  if(MCId != 407309) return w;

  std::map<int,float> DPSW;
  DPSW[0]  = 1.187;  
  DPSW[1]  = 1.129;
  DPSW[2]  = 1.440;
  DPSW[3]  = 1.982;
  DPSW[4]  = 2.718;
  DPSW[5]  = 5.105;
  DPSW[6]  = 7.947;
  DPSW[7]  = 8.320;
  DPSW[8]  = 11.595;
  DPSW[9]  = 16.159;
  DPSW[10] = 22.521;
  DPSW[11] = 31.386;
  DPSW[12] = 43.742;
  DPSW[13] = 60.961;
  DPSW[14] = 84.960;

  if( DPSW.find(njets)==DPSW.end() ) w = DPSW[DPSW.size()-1];
  else w = DPSW[njets];

  if(Debug){std::cout << "getDPSweight()" << " DEBUG \t "<< Form("Applying DPS SF=%.3f", w) << std::endl;}
  return w;
}

void DiLepton :: setROOTmsg(int level){
  switch(level){
    case 0: gErrorIgnoreLevel = kFatal; break;
    case 1: gErrorIgnoreLevel = kUnset; break;
    default: break;  
  }
  return;
}

