#include <map>
#include "TTree.h"
#include "TFile.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "TString.h"
#include "TLorentzVector.h"
#include "TROOT.h"
#include "TKey.h"
#include "TSystem.h"
#include "TSystemDirectory.h"
#include "/home/fc12/HistMakerSS3L/CalcGenericMT2/src/MT2_ROOT.h"
#include "TMath.h"

#ifdef __MAKECINT__
#pragma link C++ class vector<TLorentzVector>;
#endif

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */

// map of input branch names
std::map<TString,TString> guessBranchMapping(TTree* tree){
  
  std::map<TString,TString> bnames;
  
  if(tree->GetBranch("Meff")){
      
    cout<<"Detected tree origin: Freiburg"<<endl;
    bnames["meff"]="Meff";
    bnames["minv"]="Minv";
    bnames["ht"]="Ht";
    bnames["mt"]="Mt";
    bnames["met"]="EtMiss";
    bnames["nBJets20"]="Nbjet";
    bnames["nJets25"]="Njet25";
    bnames["nJets40"]="Njet40";
    bnames["nJets50"]="Njet50";
    bnames["SSChannel"]="SSChannel";
    bnames["puweight"]="PileUpWeight";
    bnames["totweight"]="TotalWeight";
    bnames["mcweight"]="MCWeight";
    bnames["sysSF"]="SysWeights";
    bnames["trigSF"]="TrigWeights";
    bnames["sysNames"]="SysNames";
    bnames["lepTLV"]="lepTLV";
    bnames["jetTLV"]="jetTLV";
    bnames["lepCharges"]="lepCharges";
    bnames["NlepBL"]="NlepBL";
    bnames["pdfw"]="PDFWeights";
    bnames["runn"]="RunNumber";
    bnames["evn"]="EventNumber";
    bnames["Mjj"]="Mjj";
    bnames["Zevent"]="Zevent";
    bnames["MCId"]="MCId";
    bnames["PDGId1"]="PDGId1";
    bnames["PDGId2"]="PDGId2";
    bnames["GLDec1"]="GLDec1";
    bnames["GLDec2"]="GLDec2";
    bnames["phi_met"]="EtMissPhi";

    //bnames["MT2"]="MT2";
    cout<<"First step done.."<<endl;
  } else {
    
    cout<<"ERROR: cannot detect input tree origin. Code will crash. Have fun."<<endl;
    
  }
  
  
  return bnames;
}

// set branch to local address, creating it if not existing

void setupBranch(TTree* tree, TString name, Long64_t* address){
  TBranch* _b=tree->GetBranch(name);
  if(_b==0) _b= tree->Branch(name,address,name+"/L");
  else tree->SetBranchAddress(name,address);
}

void setupBranch(TTree* tree, TString name, char* address){
  TBranch* _b=tree->GetBranch(name);
  if(_b==0) _b= tree->Branch(name,address,name+"/O");
  else tree->SetBranchAddress(name,address);
}


void setupBranch(TTree* tree, TString name, int* address){
  TBranch* _b=tree->GetBranch(name);
  if(_b==0) _b= tree->Branch(name,address,name+"/I");
  else tree->SetBranchAddress(name,address);
}

void setupBranch(TTree* tree, TString name, float* address){
  TBranch* _b=tree->GetBranch(name);
  if(_b==0) _b= tree->Branch(name,address,name+"/F");
  else tree->SetBranchAddress(name,address);
}

bool isAtlfast(Long64_t RunNumber){

  if(RunNumber == 304014) return true; //3top
  if(RunNumber == 341177) return true; //ttH dilep
  if(RunNumber == 331270 || RunNumber == 331271) return true; //ttH semilep and hadr
  if(RunNumber>=371200 && RunNumber<=371317) return true; //2STEP 
  if(RunNumber>=372300 && RunNumber<=372368) return true; //BBonestep
  if(RunNumber>=372444 && RunNumber<=372524) return true; //SLEPTONS
  if(RunNumber>=373462 && RunNumber<=373478) return true; //Extension SLN
  if(RunNumber>=403380 && RunNumber<=403481) return true; //RPV
  if(RunNumber>=370189 && RunNumber<=370238) return true; //Gtt 4-5 body
  if(RunNumber>=998000) return true; //RPV copies
  return false;
}

bool isSignal(int RunNumber){

  if(RunNumber>=370100 && RunNumber<=370187) return true;
  if(RunNumber>=371200 && RunNumber<=371308) return true;
  if(RunNumber>=372300 && RunNumber<=372368) return true;
  if(RunNumber>=372450 && RunNumber<=372524) return true;
  if(RunNumber>=403380 && RunNumber<=403481) return true;
  return false;
}

unsigned int getPosition(std::string sysname, std::vector<std::string> *v){

  std::map<std::string, int> m;

  for(unsigned int i(0); i<v->size(); i++) m[v->at(i)] = i;
  if(!m[sysname]) cout << "Cannot find name "<< sysname << " in SysNames"<< endl; 
  return m[sysname]; 
}

float getSSCharge(std::vector<TLorentzVector> *leptons,std::vector<float> *charges){

  int c(0);
  int nLep = leptons->size();
  switch(nLep){
  case 2:
    if(charges->at(0) + charges->at(1) > 0) c = 1;
    else c = -1;
    break;
  case 3:
    if( charges->at(1) == charges->at(2) ){
      if(charges->at(1) + charges->at(2) > 0) c = 1;
      else c = -1;
    }
    if( charges->at(0) == charges->at(2) ){
      if(charges->at(0) + charges->at(2) > 0) c = 1;
      else c = -1;
    }
    if( charges->at(0) == charges->at(1) ){ 
      if(charges->at(0) + charges->at(1) > 0) c = 1;
      else c = -1;
    }
    break;
  case 4:
    if( charges->at(2) == charges->at(3) ){
      if(charges->at(2) + charges->at(3) > 0) c = 1;
      else c = -1;
    }
    if( charges->at(1) == charges->at(3) ){
      if(charges->at(1) + charges->at(3) > 0) c = 1;
      else c = -1;
    }
    if( charges->at(1) == charges->at(2) ){
      if(charges->at(1) + charges->at(2) > 0) c = 1;
      else c = -1;
    }
    if( charges->at(0) == charges->at(3) ){
      if(charges->at(0) + charges->at(3) > 0) c = 1;
      else c = -1;
    }
    if( charges->at(0) == charges->at(2) ){
      if(charges->at(0) + charges->at(2) > 0) c = 1;
      else c = -1;
    }
    if( charges->at(0) == charges->at(1) ){
      if(charges->at(0) + charges->at(1) > 0) c = 1;
      else c = -1;
    }
    break;
  default: 
    break;
  }
  return (float)c;
}

float GetSFOSmass(std::vector<TLorentzVector> *leptons,std::vector<float> *charges){

  float c(0);
  int nLep = leptons->size();
  switch(nLep){
  case 2:
    c = 0;
    break;
  case 3:
    if( charges->at(0) != charges->at(1) && (int)leptons->at(0).M()==(int)leptons->at(1).M() ){
      c=(leptons->at(0)+leptons->at(1)).M();
    }
    if( charges->at(0) != charges->at(2) && (int)leptons->at(0).M()==(int)leptons->at(2).M() ){
      if(TMath::Abs((leptons->at(0)+leptons->at(2)).M()-90000) < TMath::Abs(c-90000)) c=(leptons->at(0)+leptons->at(2)).M();
    }
    if( charges->at(2) != charges->at(1) && (int)leptons->at(2).M()==(int)leptons->at(1).M() ){
      if(TMath::Abs((leptons->at(2)+leptons->at(1)).M()-90000) < TMath::Abs(c-90000)) c=(leptons->at(2)+leptons->at(1)).M();
    }
    break;
  case 4:
    if( charges->at(0) != charges->at(1) && (int)leptons->at(0).M()==(int)leptons->at(1).M() ){
      if(TMath::Abs((leptons->at(0)+leptons->at(1)).M()-90000) < TMath::Abs(c-90000)) c=(leptons->at(0)+leptons->at(1)).M();
    }
    if( charges->at(0) != charges->at(2) && (int)leptons->at(0).M()==(int)leptons->at(2).M() ){
      if(TMath::Abs((leptons->at(0)+leptons->at(2)).M()-90000) < TMath::Abs(c-90000)) c=(leptons->at(0)+leptons->at(2)).M();
    }
    if( charges->at(0) != charges->at(3) && (int)leptons->at(0).M()==(int)leptons->at(3).M() ){
      if(TMath::Abs((leptons->at(0)+leptons->at(3)).M()-90000) < TMath::Abs(c-90000)) c=(leptons->at(0)+leptons->at(3)).M();
    }
    if( charges->at(1) != charges->at(2) && (int)leptons->at(1).M()==(int)leptons->at(2).M() ){
      if(TMath::Abs((leptons->at(1)+leptons->at(2)).M()-90000) < TMath::Abs(c-90000)) c=(leptons->at(1)+leptons->at(2)).M();
    }
    if( charges->at(1) != charges->at(3) && (int)leptons->at(1).M()==(int)leptons->at(3).M() ){
      if(TMath::Abs((leptons->at(3)+leptons->at(1)).M()-90000) < TMath::Abs(c-90000)) c=(leptons->at(3)+leptons->at(1)).M();
	}
    if( charges->at(2) != charges->at(3) && (int)leptons->at(2).M()==(int)leptons->at(3).M() ){
      if(TMath::Abs((leptons->at(3)+leptons->at(2)).M()-90000) < TMath::Abs(c-90000)) c=(leptons->at(2)+leptons->at(3)).M();
    }
    break;
  default:
    break;
  }
  return c;
}

void copyTree(TTree* inTree, TTree* outTree, float xsec, float runc, Long64_t MC_ID, float nMCevents, float lumi,std::vector<Long64_t>& writtenEvents){

  //std::cout<<"copying tree. already written "<<writtenEvents.size()<<endl;

  // input and output trees, tag/folder name, xsection, #events in original MC sample, target lumi

  int nBJets20=0,nJets25=0,nJets40=0,nJets50,channel=0,nSigLep=0,nSigLepEta=0,NlepBL=0,Veto_ee=0;
  char SRveto=0;

  char Zevent=0;
  Long64_t evn64=0,runn64=0;
  Int_t evn=0,runn=0,MCId=0;
  Int_t PDGId1=0,PDGId2=0,GLDec1=0,GLDec2=0,SSChannel=0;
  float met=0,meff=0,mt=0,MT2=0;
  
  float  ht=0,minv=0,mcweight=0,puweight=0,totweight=0,Mjj=0, MinvLep=0, Pt_l=0, Pt_subl=0, Pt_3=0, Mass_3=0, Pt_j2=0,m3l=0,mSFOS=0,m_ee=0,phi_met=0;
  float SSCharge=0;

  std::vector<float> *SysSF=0;
  std::vector<float> *TrigSF=0;
  std::vector<float> *PdfSF=0;
  std::vector<float> *pdfw=0;
  std::vector<string> *SysNames=0;
  std::vector<TLorentzVector> *lepTLV=0, *jetTLV=0;
  std::vector<float> *lepCharges=0;

  std::map<TString,TString> bnames=guessBranchMapping(inTree);
  
  int nentries=inTree->GetEntries();

  // std::cout<<"inTree has "<<nentries<<std::endl;

  //cout<<"got xsec nentries nMCevents "<<xsec<< " "<<nentries<<" "<<nMCevents<<endl;

  float lumiScaling=xsec*lumi*1/nMCevents;

  // data
  if(xsec==0) lumiScaling=1;

  //cout<<"final scaling "<<lumiScaling<<endl;

  float lumiScalingUP=lumiScaling;
  float lumiScalingDOWN=lumiScaling;

  // avoid reading what we do not need
  inTree->SetBranchStatus("*",0);

  // turn on only the branches that will be read
  for(auto branch: bnames)
    inTree->SetBranchStatus(branch.second,1);
  
  
  TClass* expectedClass = 0;
  EDataType expectedType = kOther_t;
  inTree->GetBranch(bnames["evn"])->GetExpectedType(expectedClass,expectedType);
  

  // configure input branches
  inTree->SetBranchAddress(bnames["evn"],&evn64);
  inTree->SetBranchAddress(bnames["MCId"],&MCId);
  inTree->SetBranchAddress(bnames["meff"],&meff);
  inTree->SetBranchAddress(bnames["met"],&met);
  inTree->SetBranchAddress(bnames["mt"],&mt);
  inTree->SetBranchAddress(bnames["ht"],&ht);
  inTree->SetBranchAddress(bnames["minv"],&minv);
  inTree->SetBranchAddress(bnames["nJets25"],&nJets25);
  inTree->SetBranchAddress(bnames["nJets40"],&nJets40);
  inTree->SetBranchAddress(bnames["nJets50"],&nJets50);
  inTree->SetBranchAddress(bnames["nBJets20"],&nBJets20);
  //inTree->SetBranchAddress(bnames["chanDiLep"],&channel);
  inTree->SetBranchAddress(bnames["sysSF"],&SysSF);
  inTree->SetBranchAddress(bnames["trigSF"],&TrigSF);
  inTree->SetBranchAddress(bnames["sysNames"],&SysNames);
  inTree->SetBranchAddress(bnames["lepTLV"],&lepTLV);
  inTree->SetBranchAddress(bnames["jetTLV"],&jetTLV);
  inTree->SetBranchAddress(bnames["lepCharges"],&lepCharges);
  inTree->SetBranchAddress(bnames["mcweight"],&mcweight);
  inTree->SetBranchAddress(bnames["totweight"],&totweight);
  inTree->SetBranchAddress(bnames["puweight"],&puweight);
  inTree->SetBranchAddress(bnames["pdfw"],&pdfw);
  inTree->SetBranchAddress(bnames["Zevent"],&Zevent);
  inTree->SetBranchAddress(bnames["Mjj"],&Mjj);
  inTree->SetBranchAddress(bnames["NlepBL"], &NlepBL);
  inTree->SetBranchAddress(bnames["MCId"], &MCId);
  inTree->SetBranchAddress(bnames["PDGId1"],&PDGId1);
  inTree->SetBranchAddress(bnames["PDGId2"],&PDGId2);
  inTree->SetBranchAddress(bnames["GLDec1"],&GLDec1);
  inTree->SetBranchAddress(bnames["GLDec2"],&GLDec2);
  inTree->SetBranchAddress(bnames["SSChannel"],&SSChannel);
  inTree->SetBranchAddress(bnames["phi_met"],&phi_met);

  float wmu_nom,
    wmu_stat_up,
    wmu_stat_down,
    wmu_sys_up,
    wmu_sys_down, 
    wmu_stat_lowpt_up,
    wmu_stat_lowpt_down,
    wmu_sys_lowpt_up,
    wmu_sys_lowpt_down,
    wmu_trig_stat_up,
    wmu_trig_stat_down,
    wmu_trig_sys_up,
    wmu_trig_sys_down, 
    wmu_iso_stat_up,
    wmu_iso_stat_down,
    wmu_iso_sys_up,
    wmu_iso_sys_down, 

    wel_nom,
    wel_id_up,
    wel_id_down,
    wel_reco_up,
    wel_reco_down,
    wel_trig_up,
    wel_trig_down,
    wel_trigEff_up,
    wel_trigEff_down,
    wel_iso_up,
    wel_iso_down,

    wjet_nom,
    wjet_b_up,
    wjet_b_down,
    wjet_c_up,
    wjet_c_down,
    wjet_light_up,
    wjet_light_down,
    wjet_extra1_up,
    wjet_extra1_down,
    wjet_extra2_up,
    wjet_extra2_down,

    wjet_jvt_up,
    wjet_jvt_down,

    wpu_nom_sig,
    wpu_up_sig,
    wpu_down_sig,

    wpu_nom_bkg,
    wpu_up_bkg,
    wpu_down_bkg,

    wtrig_nom,
    wtrig_up,
    wtrig_down,

    wpdf_up,
    wpdf_down;

  // configure output branches
  setupBranch(outTree,"MCId",&MCId);
  setupBranch(outTree,"runn",&runn64);
  setupBranch(outTree,"evn",&evn64);
  setupBranch(outTree,"nBJets20",&nBJets20);
  setupBranch(outTree,"nJets25",&nJets25);
  setupBranch(outTree,"nJets40",&nJets40);
  setupBranch(outTree,"nJets50",&nJets50);
  setupBranch(outTree,"Pt_l",&Pt_l);
  setupBranch(outTree,"Pt_subl",&Pt_subl);
  setupBranch(outTree,"Pt_3",&Pt_3);
  setupBranch(outTree,"Mass_3",&Mass_3);
  setupBranch(outTree,"NlepBL",&NlepBL);
  setupBranch(outTree,"Mjj",&Mjj);
  setupBranch(outTree,"MinvLep",&MinvLep);
  setupBranch(outTree,"SSCharge",&SSCharge);
  setupBranch(outTree,"Zevent",&Zevent);
  setupBranch(outTree,"SRveto",&SRveto);
  setupBranch(outTree,"nSigLep",&nSigLep);
  setupBranch(outTree,"nSigLepEta",&nSigLepEta);
  //setupBranch(outTree,"chanDiLep",&channel);
  setupBranch(outTree,"mt",&mt);
  setupBranch(outTree,"ht",&ht);
  setupBranch(outTree,"minv",&minv);
  setupBranch(outTree,"meff",&meff);
  setupBranch(outTree,"met",&met);
  setupBranch(outTree,"mcweight",&mcweight);
  setupBranch(outTree,"puweight",&puweight);
  setupBranch(outTree,"totweight",&totweight);
  setupBranch(outTree,"lumiScaling",&lumiScaling);
  setupBranch(outTree,"Pt_j2",&Pt_j2);
  setupBranch(outTree,"m3l",&m3l);
  setupBranch(outTree,"Veto_ee",&Veto_ee);
  setupBranch(outTree,"mSFOS",&mSFOS);
  setupBranch(outTree,"m_ee",&m_ee);
  setupBranch(outTree,"SSChannel",&SSChannel);
  setupBranch(outTree,"MT2",&MT2);

  setupBranch(outTree,"wmu_nom",&wmu_nom);	  
  setupBranch(outTree,"wmu_stat_up",&wmu_stat_up);
  setupBranch(outTree,"wmu_stat_down",&wmu_stat_down ); 
  setupBranch(outTree,"wmu_sys_up",&wmu_sys_up);		  
  setupBranch(outTree,"wmu_sys_down",&wmu_sys_down);
  setupBranch(outTree,"wmu_stat_lowpt_up",&wmu_stat_lowpt_up);
  setupBranch(outTree,"wmu_stat_lowpt_down",&wmu_stat_lowpt_down );
  setupBranch(outTree,"wmu_sys_lowpt_up",&wmu_sys_lowpt_up);
  setupBranch(outTree,"wmu_sys_lowpt_down",&wmu_sys_lowpt_down);
  setupBranch(outTree,"wmu_trig_stat_up",&wmu_trig_stat_up);	  
  setupBranch(outTree,"wmu_trig_stat_down",&wmu_trig_stat_down);	  
  setupBranch(outTree,"wmu_trig_sys_up",&wmu_trig_sys_up);	  
  setupBranch(outTree,"wmu_trig_sys_down",&wmu_trig_sys_down);	  
  setupBranch(outTree,"wmu_iso_stat_up",&wmu_iso_stat_up);	  
  setupBranch(outTree,"wmu_iso_stat_down",&wmu_iso_stat_down);	  
  setupBranch(outTree,"wmu_iso_sys_up",&wmu_iso_sys_up);	  
  setupBranch(outTree,"wmu_iso_sys_down",&wmu_iso_sys_down);	  
  
  setupBranch(outTree,"wel_nom",&wel_nom);		  
  setupBranch(outTree,"wel_id_up",&wel_id_up);		  
  setupBranch(outTree,"wel_id_down",&wel_id_down);
  setupBranch(outTree,"wel_iso_up",&wel_iso_up);
  setupBranch(outTree,"wel_iso_down",&wel_iso_down);	  
  setupBranch(outTree,"wel_reco_up",&wel_reco_up);	  
  setupBranch(outTree,"wel_reco_down",&wel_reco_down); 
  setupBranch(outTree,"wel_trig_up",&wel_trig_up);	  
  setupBranch(outTree,"wel_trig_down",&wel_trig_down);  
  setupBranch(outTree,"wel_trigEff_up",&wel_trigEff_up);	  
  setupBranch(outTree,"wel_trigEff_down",&wel_trigEff_down);  


  setupBranch(outTree,"wjet_nom",&wjet_nom);		  
  setupBranch(outTree,"wjet_b_up",&wjet_b_up);		  
  setupBranch(outTree,"wjet_b_down",&wjet_b_down);
  setupBranch(outTree,"wjet_c_up",&wjet_c_up);		  
  setupBranch(outTree,"wjet_c_down",&wjet_c_down);
  setupBranch(outTree,"wjet_light_up",&wjet_light_up ); 
  setupBranch(outTree,"wjet_light_down",&wjet_light_down);
  setupBranch(outTree,"wjet_extra1_up",&wjet_extra1_up ); 
  setupBranch(outTree,"wjet_extra1_down",&wjet_extra1_down);
  setupBranch(outTree,"wjet_extra2_up",&wjet_extra2_up ); 
  setupBranch(outTree,"wjet_extra2_down",&wjet_extra2_down);

  setupBranch(outTree,"wjet_jvt_up",&wjet_jvt_up);
  setupBranch(outTree,"wjet_jvt_down",&wjet_jvt_down);

  setupBranch(outTree,"wpu_nom_sig",&wpu_nom_sig);		  
  setupBranch(outTree,"wpu_up_sig",&wpu_up_sig);		  
  setupBranch(outTree,"wpu_down_sig",&wpu_down_sig);
  setupBranch(outTree,"wpu_nom_bkg",&wpu_nom_bkg);
  setupBranch(outTree,"wpu_up_bkg",&wpu_up_bkg);
  setupBranch(outTree,"wpu_down_bkg",&wpu_down_bkg);

  setupBranch(outTree,"wtrig_nom",&wtrig_nom);
  setupBranch(outTree,"wtrig_up",&wtrig_up);
  setupBranch(outTree,"wtrig_down",&wtrig_down);

  setupBranch(outTree,"wpdf_up",&wpdf_up);		  
  setupBranch(outTree,"wpdf_down",&wpdf_down);
  
  // signals
  if(runc!=0){
    lumiScalingUP=lumiScaling*(1+runc);
    lumiScalingDOWN=lumiScaling*(1-runc);
    std::cout<< "got runc !=0 "<<runc<<" "<<lumiScaling<<" "<<lumiScalingUP<<" "<<lumiScalingDOWN<<std::endl;
  }

    setupBranch(outTree,"lumiScaling_UP",&lumiScalingUP);
    setupBranch(outTree,"lumiScaling_DOWN",&lumiScalingDOWN);

    // now all branches are set. will get once and for all the systematic mapping

    //inTree->GetEntry(0);

    //std::cout<<"Testing weights:"<<" " <<SysNames->size()<<std::endl;
    //for(auto n: *SysNames)
    //  cout<<n<<endl;
    
    int nocut=0,bveto=0;

    for (Long64_t i=0;i<nentries; i++) {
      
      inTree->GetEntry(i);
      nocut = nocut+1;
    
      if(MCId>=372444 && MCId<=372511 ){
	if(GLDec1==5 || GLDec2==5){
	continue;
	}
      }
      bveto = bveto+1;
      //cout<<"Good event.."<<endl;	
    
      writtenEvents.push_back(evn64);
    
      nSigLep=lepTLV->size();
      nSigLepEta=lepTLV->size();
      //Validation Regions
      
      Pt_l = lepTLV->at(0).Pt();
      Pt_subl = lepTLV->at(1).Pt();
      if(lepTLV->size()>2){
	Pt_3 = lepTLV->at(2).Pt();
	Mass_3 = lepTLV->at(2).M();
      }
      else{
	Pt_3=0;
	Mass_3=0;
      }
      
      MinvLep = (lepTLV->at(0)+lepTLV->at(1)).M();
      
      //MT2
      TLorentzVector metTLV = TLorentzVector( met*TMath::Cos(phi_met) , met*TMath::Sin(phi_met) , 0 , met);
      ComputeMT2 mycalc = ComputeMT2(lepTLV->at(0),lepTLV->at(1),metTLV,lepTLV->at(0).M(),lepTLV->at(1).M());
      MT2 = mycalc.Compute();
      
      //Eta cut
      if(lepTLV->at(0).M()<50){
        if(lepTLV->at(0).Eta() > 1.37 || lepTLV->at(0).Eta() < -1.37) nSigLepEta=nSigLepEta-1;
      }
      if(lepTLV->at(1).M()<50){
        if(lepTLV->at(1).Eta() > 1.37 || lepTLV->at(1).Eta() < -1.37) nSigLepEta=nSigLepEta-1;
      }

      SSCharge = getSSCharge(lepTLV, lepCharges);

      //new variables for VR
      if(jetTLV->size()>1) Pt_j2=jetTLV->at(1).Pt();
      else Pt_j2=0;
      if(lepTLV->size()>2) m3l=(lepTLV->at(0)+lepTLV->at(1)+lepTLV->at(2)).M();
      else m3l=0;
      if(lepTLV->size()>2){
	mSFOS=GetSFOSmass(lepTLV, lepCharges);
      }
	else
	  mSFOS=0;
      
      Veto_ee=0;
      m_ee=0;
      if(lepTLV->size()==2 && (int)lepTLV->at(0).M() < 50 && (int)lepTLV->at(1).M() < 50){
	Veto_ee=1;
	m_ee=(lepTLV->at(0)+lepTLV->at(1)).M();
      }
      
      //SR veto                                                                                           
      if(nBJets20>0 && nJets50>3 && meff>=550000 && met>=150000) SRveto=1; //!SR1b                                       
      if(nBJets20>2 && meff>=650000 && met>=125000) SRveto=1; //!SR3b                                                                       
      if(nBJets20>1 && meff>=650000 && met>=125000) SRveto=1; //SR2br                                                                      
      if(nBJets20==0 && nJets50>2 && meff>=550000 && met>=200000) SRveto=1; //!SR0b3j                                                
      if(nBJets20==0 && nJets50>4 && meff>=650000 && met>=125000) SRveto=1; //!SR0b5j 

    // MC
    if(xsec!=0){

      wmu_nom=SysSF->at(0);
      wmu_stat_up=SysSF->at(27);
      wmu_stat_down=SysSF->at(26);
      wmu_stat_lowpt_up=SysSF->at(29);
      wmu_stat_lowpt_down=SysSF->at(28);
      wmu_sys_up=SysSF->at(31);
      wmu_sys_down=SysSF->at(30);
      wmu_sys_lowpt_up=SysSF->at(33);
      wmu_sys_lowpt_down=SysSF->at(32);
      wmu_trig_stat_up=SysSF->at(35);
      wmu_trig_stat_down=SysSF->at(34);
      wmu_trig_sys_up=SysSF->at(37);
      wmu_trig_sys_down=SysSF->at(36);
      wmu_iso_stat_up=SysSF->at(39);
      wmu_iso_stat_down=SysSF->at(38);
      wmu_iso_sys_up=SysSF->at(41);
      wmu_iso_sys_down=SysSF->at(40);

      wel_nom=SysSF->at(1);                                                                                                             
      wel_id_up=SysSF->at(5);                                                                                                             
      wel_id_down=SysSF->at(4);                                                                                                           
      wel_iso_up = SysSF->at(7);                                                                                                          
      wel_iso_down = SysSF->at(6);                                                                                                          
      wel_reco_up=SysSF->at(9);                                                                                                          
      wel_reco_down=SysSF->at(8);                                                                                                         
      wel_trigEff_up=SysSF->at(11);                                                                                                          
      wel_trigEff_down=SysSF->at(10);                 
      wel_trig_up=SysSF->at(13);
      wel_trig_down=SysSF->at(12);

      wjet_nom=SysSF->at(2);
      wjet_b_up=SysSF->at(15);
      wjet_b_down=SysSF->at(14);
      wjet_c_up=SysSF->at(17);
      wjet_c_down=SysSF->at(16);
      wjet_light_up=SysSF->at(19);
      wjet_light_down=SysSF->at(18);
      wjet_extra1_up=SysSF->at(21);
      wjet_extra1_down=SysSF->at(20);
      wjet_extra2_up=SysSF->at(23);
      wjet_extra2_down=SysSF->at(22);
      wjet_jvt_up=SysSF->at(25);
      wjet_jvt_down=SysSF->at(24);
      
      if(!isSignal(MCId)){
	wpu_nom_sig=1; 
	wpu_up_sig=SysSF->at(43);
	wpu_down_sig=SysSF->at(42);
      }
      else{
	wpu_nom_sig=SysSF->at(3);
	wpu_up_sig=SysSF->at(43);
	wpu_down_sig=SysSF->at(42);

      }

      if(isSignal(MCId)){
	wpu_nom_bkg=1;
	wpu_up_bkg=SysSF->at(41);
	wpu_down_bkg=SysSF->at(40);

      }
      else{
	wpu_nom_bkg=SysSF->at(3);
	wpu_up_bkg=SysSF->at(41);
	wpu_down_bkg=SysSF->at(40);
      }


      wtrig_nom=TrigSF->at(0);
      wtrig_up=TrigSF->at(2);
      wtrig_down=TrigSF->at(1);

      //wpdf_up=pdfw->at(1);
      //wpdf_down=pdfw->at(2);
      wpdf_up=1;
      wpdf_down=1;
    }

    outTree->Fill();

    }
    cout<<"No cut: "<<nocut<<"\t Bveto: "<<bveto<<endl;

  outTree->Write("",TObject::kOverwrite);
  cout<<"Done.."<<endl;
}


// open a file, get the tree and copy it to the output. attach proper lumi rescaling

void processFile(TString filein, TString fileout, TString treein, TString treeout, float xsec,float runc, Long64_t MC_ID, float lumi,  std::vector<Long64_t>& writtenEvents){


  TFile* inFile=TFile::Open(filein, "READ");

  // need to remove "_nom" for input nominal sample
  TTree* inTree=(TTree*)inFile->Get(treein.ReplaceAll("_nom",""));

  if(!inTree){

    std::cout<<"Skipping corrupted file "<<std::endl;
    inFile->Close();
    return;

  }

  TFile* outFile=TFile::Open(fileout,"UPDATE");
  TTree* outTree=(TTree*)gROOT->FindObject(treeout);

  if(!outTree){
    //    cout<<"output tree not found, creating"<<endl;
    outTree=new TTree(treeout,treeout);
  }

  else cout<<RED<<"adding entries to existing tree "<<treeout<<RESET<<endl;

  TTree* control=(TTree*)inFile->Get("ControlTree");
 

  // get sum of weights. simple method using TTree::Draw and TArrayD crashes for large number of events
  Int_t NFiles=0, fileentry=0,raw=0,nofilt=0;
  float xAODWeightSum=0;
  float totw=0;
  float MCWeight=0;
  //int GLDec1=0, GLDec2=0;
  int MCId=0;

  // get filter eff for MC (data has xsec=0)

  if(xsec){
    control->SetBranchAddress("FileEntry",&fileentry);
    control->SetBranchAddress("xAODWeightSum",&xAODWeightSum);
    control->SetBranchAddress("NFiles",&NFiles);
    control->SetBranchAddress("MCId",&MCId);    
    control->SetBranchAddress("MCWeight",&MCWeight);

    for(int i=0;i< control->GetEntries();++i){
      control->GetEntry(i);
      if(fileentry==1) totw += xAODWeightSum;  
    }
    if((MCId>=372444 && MCId<=372524) || (MCId>=373462 && MCId<=373478)) totw = totw*0.64; //b-filtering for Slepton grid
  }
  else totw=1;

  cout<<"Xsec: "<<xsec<<endl;
  copyTree(inTree,outTree,xsec,runc,MC_ID,totw,lumi,writtenEvents);

  outFile->Write("",TObject::kOverwrite);

  outFile->Close();
  inFile->Close();

}

// functions to retrieve cross section from common SUSYTools files

// this is just a grep. returns the line(s) to be parsed by the next function
TString lookForSample(TString runnumber){

  // uses perl regexp. matches trailing whitespace (space OR tab). requires begin of line.
 
  gSystem->Exec("grep -r -P \"^"+runnumber+"\\s\" /home/fc12/HistFitter2017/Run/prepare/xsections/mc15_13TeV/* > /tmp/fc12/trash_5.out");
  std::string line;

  std::ifstream result("/tmp/fc12/trash_5.out");
  std::getline(result, line);
  if(result.eof()){
    
    cout<<RED<<"ERROR cannot find cross section for sample "<<runnumber<<" will skip it "<<RESET<<endl;
    return "";

  }

  TString linestr(line);

  gSystem->Exec("rm /tmp/fc12/trash_5.out");

  // sanity check: there must be only one possibility

  std::getline(result, line);

  if(!result.eof()){
    cout<< "ERROR retrieving cross section: too many possibilities found. Aborting"<<endl;
    exit(1);

  }

  return linestr.ReplaceAll("\t"," ");

}

// parse the result from grep, check that there is only one possibility

void getNormFactor(TString runnumber, float& norm, float& runc, Long64_t &MC_ID){

  TString grepped=lookForSample(runnumber);

  if(grepped=="") return;

  TObjArray* tokens= grepped.Tokenize(" ");
  
  Float_t xsec= ((TObjString*)(tokens->At(2)))->String().Atof();
  Float_t kfac= ((TObjString*)(tokens->At(3)))->String().Atof();
  Float_t feff= ((TObjString*)(tokens->At(4)))->String().Atof();

  MC_ID =  ((TObjString*)(tokens->At(0)))->String().Atof();
  runc = ((TObjString*)(tokens->At(5)))->String().Atof();
  
  norm = xsec*kfac*feff;

}


// reads all files in a folder. adds the contents to outtreename in outfname

void readFolder(TString dirname, TString outfname, TString intreename, TString outtreename, std::vector<string>& sys, float targetlumi){

  TSystemDirectory dir(dirname, dirname);
  TList *files = dir.GetListOfFiles();
  if (files) {
  

    bool isData=outtreename.Contains("data");
    bool isSignal=outtreename.BeginsWith("signal");

    // outer loop is on systematics

    for(auto sysname: sys){	

      std::vector<Long64_t> writtenEvents;

      TString _intreename=intreename+"_"+sysname;

      TSystemFile *file;
      TString fname;
      TIter next(files);    
      // reinit the file iterator

      while ((file=(TSystemFile*)next())) {

	fname = file->GetName();
	if(fname=="." || fname=="..") continue;

	TObjArray* tokens=fname.Tokenize(".");

	if(tokens->GetEntries()>2 || isData){

	  cout<<"processing file "<<fname<< " for systematic "<<sysname<<endl;
	  TString runnumber="";
	  float norm=-1,runc=-1;
	  Long64_t MC_ID=-1;
	  if(! isData){
	    runnumber=((TObjString*)(tokens->At(2)))->String();
	    getNormFactor(runnumber,norm,runc,MC_ID);
	    //cout<<" got cross normalization "<<norm<<endl;
	  } else {
	    // signal "data" to downstream functions using norm=0
	    norm=0; runc=0; MC_ID=0;
	  }

	  bool Atlfast;
	  Long64_t mcid = atol(runnumber);
	  Atlfast = isAtlfast(mcid);

	  // local (per file, per syst) output tree name
	  TString _outtreename="signal"+runnumber;

	  if(!isSignal){
	    // error on bg MC treated differently
	    runc=0;
	    _outtreename=outtreename;
	  } else {
	    //std::cout<<"outtree name is "<<outtreename<<std::endl;
	    // signal tree names are different, i.e. they are like signalXXXX_nom
	    TObjArray* tokens=outtreename.Tokenize("_");
	    if(tokens->GetEntries()==2){
	      _outtreename="signal"+runnumber+"_"+((TObjString*)(tokens->At(1)))->String();
	    }
	  }
	  
	  //cout<<"run "<<runnumber<<" got norm factors "<<norm<<" "<<runc<<endl;
	  
	  // sanity check
	  if( norm!=-1 && runc!=-1 ){
	    _outtreename=_outtreename+"_"+TString(sysname);
	    processFile(dirname+"/"+fname, outfname,_intreename,_outtreename,norm,runc,MC_ID,targetlumi,writtenEvents);
	  
	  } // end sanity check

	  bool doAFII=true;

	  //AFII for Fullsim samples
	  if(!Atlfast && sysname=="nom" && doAFII){
	    // create dummy AFII syst for fullsim samples, copying the nominal one
	    cout<<"The sample is fullsim, ading AFII trees..."<<endl;
	    TString sysname2("JET_RelativeNonClosure_AFII__1up");
	    TString sysname3("JET_RelativeNonClosure_AFII__1down");
	    TString _outtreename2="signal"+runnumber;
	    TString _outtreename3="signal"+runnumber;
	   
	    if(!isSignal){
	      // error on bg MC treated differently
	      runc=0;
	      _outtreename2=outtreename;
	      _outtreename3=outtreename;
	    } else {
	     
	      if(tokens->GetEntries()==2){
		_outtreename2="signal"+runnumber+"_"+((TObjString*)(tokens->At(1)))->String();
		_outtreename3="signal"+runnumber+"_"+((TObjString*)(tokens->At(1)))->String();
	      }
	    }

	    _outtreename2=_outtreename2+"_"+TString(sysname2);
	    processFile(dirname+"/"+fname, outfname,_intreename,_outtreename2,norm,runc,MC_ID,targetlumi,writtenEvents);
	    _outtreename3=_outtreename3+"_"+TString(sysname3);
	    processFile(dirname+"/"+fname, outfname,_intreename,_outtreename3,norm,runc,MC_ID,targetlumi,writtenEvents);
	  }

	}

	// for MC, clear event record once per file/sample
	if(!isData)
	  writtenEvents.clear();

      } // loop on files in folder
    } // loop on systematics
  }
  
}

// main script
#include <limits>    


void lumiRescale(int inLumi, int outLumi){
  
  TString inbg=TString("background.")+Form("%d", inLumi)+TString(".root");
  TString outbg=TString("background.")+Form("%d", outLumi)+TString(".root");

  TString insig=TString("signal.")+Form("%d", inLumi)+TString(".root");
  TString outsig=TString("signal.")+Form("%d", outLumi)+TString(".root");

  TFile* finbg=TFile::Open(inbg,"READ");

  TIter next(finbg->GetListOfKeys());
  TKey *key;

  while ((key = (TKey*)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TTree")) continue;

    TTree* inTree=(TTree*)key->ReadObj();

    cout<<"Read tree "<<inTree->GetName()<<endl;

    TFile* foutbg=TFile::Open(outbg,"UPDATE");
    foutbg->cd();

    TTree* outTree=new TTree(inTree->GetName()+TString("copy"),inTree->GetTitle());

    outTree->Write(inTree->GetName(),TObject::kOverwrite);

    cout<<"Wrote tree "<<inTree->GetName()<<endl;

    foutbg->Write("",TObject::kOverwrite);
    foutbg->Close();
    
  }

  TFile* finsig=TFile::Open(insig,"READ");

  TIter nextsig(finbg->GetListOfKeys());
  TKey *keysig;

  while ((keysig = (TKey*)nextsig())) {
    TClass *cl = gROOT->GetClass(keysig->GetClassName());
    if (!cl->InheritsFrom("TTree")) continue;

    TTree* inTree=(TTree*)keysig->ReadObj();

    cout<<"Read tree "<<inTree->GetName()<<endl;

    TFile* foutsig=TFile::Open(outsig,"UPDATE");
    foutsig->cd();

    TTree* outTree=new TTree(inTree->GetName()+TString("copy"),inTree->GetTitle());

    //copyTree(inTree,outTree,float(outLumi)/float(inLumi));    

    outTree->Write(inTree->GetName(),TObject::kOverwrite);

    cout<<"Wrote tree "<<inTree->GetName()<<endl;

    foutsig->Write("",TObject::kOverwrite);
    foutsig->Close();
    
  }
  
}


void doAll(int targetlumi){

  gROOT->ProcessLine("#include <vector>");

  gErrorIgnoreLevel=kError;

  const char* names[] = {
    "nom", // the nominal sample
    "EG_RESOLUTION_ALL__1down",
    "EG_RESOLUTION_ALL__1up",
    "EG_SCALE_ALL__1down",
    "EG_SCALE_ALL__1up",
    "JET_EtaIntercalibration_NonClosure__1up",
    "JET_EtaIntercalibration_NonClosure__1down",
    "JET_GroupedNP_1__1up",
    "JET_GroupedNP_1__1down",
    "JET_GroupedNP_2__1up",
    "JET_GroupedNP_2__1down",
    "JET_GroupedNP_3__1up",
    "JET_GroupedNP_3__1down",
    "JET_JER_SINGLE_NP__1up",
    "JET_RelativeNonClosure_AFII__1up",
    "JET_RelativeNonClosure_AFII__1down",
    "MET_SoftTrk_ResoPara",
    "MET_SoftTrk_ResoPerp",
    "MET_SoftTrk_ScaleDown",
    "MET_SoftTrk_ScaleUp",
    "MUONS_ID__1down",
    "MUONS_ID__1up",
    "MUONS_MS__1down",
    "MUONS_MS__1up",
    "MUONS_SCALE__1down",
    "MUONS_SCALE__1up"
  };
  
  std::vector<std::string> sysnames(names, names + sizeof(names)/sizeof(names[0]));
  // data has only nominal
  std::vector<std::string> sysnamesData; sysnamesData.push_back("nom");
  
  TString basepathBkg("/storage/users/fc12/DiLepton-10-11-2016/BKG/Merged/");
  TString basepathSig("/storage/users/fc12/DiLepton-10-11-2016/");
  cout<<"PathBkg: "<<basepathBkg<<endl;
  cout<<"PathSig: "<<basepathSig<<endl;
  
  // Data  
  //readFolder(basepath+"DATA",TString("data.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","data",sysnamesData,1);

  // Backgrounds
  
  readFolder(basepathBkg+"Diboson",TString("background.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","Diboson",sysnames,targetlumi);
  readFolder(basepathBkg+"Rare",TString("background.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","Rare",sysnames,targetlumi); 
  readFolder(basepathBkg+"ttW",TString("background.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","ttW",sysnames,targetlumi);
  readFolder(basepathBkg+"ttZ",TString("background.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","ttZ",sysnames,targetlumi);
  //readFolder(basepathBkg+"ZZ",TString("background.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","ZZ",sysnames,targetlumi);
  //readFolder(basepathBkg+"WZ",TString("background.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","WZ",sysnames,targetlumi);
  //readFolder(basepathBkg+"WW",TString("background.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","WW",sysnames,targetlumi);
  
  readFolder(basepathSig+"2STEP/Merged",TString("signal.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","signal",sysnames,targetlumi);
  readFolder(basepathSig+"BBONE/Merged",TString("signal.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","signal",sysnames,targetlumi);
  readFolder(basepathSig+"GSL/Merged",TString("signal.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","signal",sysnames,targetlumi);
  readFolder(basepathSig+"GTT/Merged",TString("signal.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","signal",sysnames,targetlumi);
  //readFolder(basepathSig+"New",TString("signal.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","signal",sysnames,targetlumi);
  //readFolder(basepathSig+"RPV_GG",TString("signal.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","signal",sysnames,targetlumi);
  //readFolder(basepathSig+"RPV_DD_321",TString("signal.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","signal",sysnames,targetlumi);
  //readFolder(basepathSig+"RPV_DD_331",TString("signal.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","signal",sysnames,targetlumi); 
  
  //readFolder(basepath+"TEST",TString("test.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","Test",sysnames,targetlumi);

  // signal
  //readFolder(basepath+"TEST",TString("signal.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","signal",sysnames,targetlumi);  

  //readFolder(basepath+"SIGNAL2",TString("signal.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","signal",sysnames,targetlumi);  
  //readFolder(basepath+"SIGNAL3",TString("signal.")+Form("%d", targetlumi)+TString(".root"),"DiLeptonTree_SRall","signal",sysnames,targetlumi);  
  
}
