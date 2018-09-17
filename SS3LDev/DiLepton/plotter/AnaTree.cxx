#include "AnaTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TString.h"
#include "TDirectory.h"
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

void AnaTree::assignSystErr(TH1D *h1){
  if(total_syst.size() != h1->GetNbinsX()){
    cout<<"Systematic vector and histograms have differente size. Systematics not applied!"<<endl;
    return;
  }
  double stat=0;
  double error=0;
  for(unsigned int i_bin=0; i_bin<total_syst.size();i_bin++){
    stat = h1->GetBinError(i_bin);
    error = TMath::Sqrt(stat*stat + total_syst[i_bin]*total_syst[i_bin]);
    h1->SetBinError(i_bin,error);
    if(DEBUG) cout<<"DEBUG \t Statistic error: "<<stat<<" \t Systematic error: "<<total_syst[i_bin]<<endl;
    if(DEBUG) cout<<"DEBUG \t Error set to: "<<error<<endl;
  }
}

void AnaTree::getTotalSystErr(){
  total_syst.clear();
  double square;
  if(DEBUG) cout<<"DEBUG \t syst_variation.size(): "<<syst_variation.size()<<endl;
  if(DEBUG) cout<<"DEBUG \t syst_variation[0].size(): "<<syst_variation[0].size()<<endl;
  for(unsigned int i_bin=0; i_bin<syst_variation[0].size(); i_bin++){ 
    if(DEBUG) cout<<"DEBUG \t i_bin: "<<i_bin<<endl;    
    square=0;
    for(unsigned int i_var=0; i_var<syst_variation.size(); i_var++){
      if(DEBUG) cout<<"DEBUG \t i_var: "<<i_var<<endl;
      square = square + syst_variation[i_var][i_bin]*syst_variation[i_var][i_bin];
      if(DEBUG) cout<<"DEBUG \t square: "<<square<<endl;
    }
    total_syst.push_back(TMath::Sqrt(square));
    if(DEBUG) cout<<"DEBUG \t Total syst. error: "<<total_syst[i_bin]<<endl;
  }
}


void AnaTree::getSystVariation(TH1D *h1, TH1D *h2, int n_syst_var){
  double difference;
  for(unsigned int i_bin=0; i_bin<h1->GetNbinsX(); i_bin++){ //LOOP ON THE BINS OF THE HISTOS OF THE VARIATION
    difference = h1->GetBinContent(i_bin) - h2->GetBinContent(i_bin);
    if(DEBUG) cout<<"DEBUG \t Variation: "<<difference<<endl;
    syst_variation[n_syst_var].push_back(difference);
  }
  return;
}


void AnaTree::getSystList(TString file){

  ifstream myFile(path+file);

  if(myFile) cout<<"File "<<file<<" opened!"<<endl;
  else{
    if(DEBUG) cout<<file<<" not found!"<<endl;
    return;
  }

  TString syst_name = "";

  while(!myFile.eof()){
    myFile>>syst_name;
    syst_list.push_back(syst_name);
  }

  return;
}

void AnaTree::SetDebugMode(){
  DEBUG=1;
}

void AnaTree::SetTreeName(TString _TreeName){
  TreeName=_TreeName;
  if(DEBUG) cout<<"Tree name set to "<<TreeName<<endl;
}

void AnaTree::DrawGrid(TH2D *grid, TCanvas *canv_grid, vector<TString> label_x, vector<TString> label_y, double cont_min, double cont_max, TString title){
  cout<<"A"<<endl;
  canv_grid->cd();
  //  gROOT->LoadMacro("/home/fc12/plotter/atlasstyle-00-03-04/AtlasStyle.C");
  gStyle->SetOptStat(000000);
  gStyle->SetOptTitle(0);
  gStyle->SetHatchesLineWidth(2);
  cout<<"B"<<endl;  
  grid->SetContour(10000,0);
  grid->GetZaxis()->SetRangeUser(cont_min,cont_max);
  canv_grid->SetGrid();
  grid->Draw("COLZ TEXTE");
  gStyle->SetPaintTextFormat("4.2f");
  cout<<"C"<<endl;
  grid->SetTitle("Significance grid ; MET (GeV) ; Meff (GeV)");
  //grid->GetXaxis()->SetTitle("MET (GeV)");
  //grid->GetYaxis()->SetTitle("Meff (GeV)");
  //  grid->SetTitle("Significance grid");
  cout<<"D"<<endl;
  for(unsigned int i=0; i<label_x.size(); i++){
    grid->GetXaxis()->SetBinLabel(i+1,label_x[i]);
  }
  for(unsigned int i=0; i<label_y.size(); i++){
    grid->GetYaxis()->SetBinLabel(i+1,label_y[i]);
  }
  cout<<"E"<<endl;
  TLatex n;
  n.SetNDC();
  n.SetTextColor(kBlack);
  n.SetTextFont(72);
  n.SetTextSize(0.05);
  n.DrawLatex(0.16,0.93,title);
  n.SetTextSize(0.04);
  n.SetTextFont(42);
  cout<<"F"<<endl;
  return;
}

void AnaTree::setGridValues(TH2D *grid, int x_bin, vector<vector<double> > values, vector<vector<double> > err_values){
  for (unsigned int i = 0; i<values[0].size(); i++){
    grid->SetBinContent(x_bin+1,i+1,values[x_bin][i]);
    //grid->SetBinError(x_bin+1,i+1,err_values[x_bin][i]);
    //    cout<<"Bin i: "<<grid->GetBinContent(x_bin+1,i+1)<<endl;
  }
  grid->SetEntries(1);
  return;
}


double AnaTree::BinomialP(double mainObs, double backgroundObs, double relativeBkgUncert){

  double tau = 1./backgroundObs/(relativeBkgUncert*relativeBkgUncert);
  double auxiliaryInf = backgroundObs*tau;

  double P_Bi = TMath::BetaIncomplete(1./(1.+tau),mainObs,auxiliaryInf+1);

  //  cout<<"P value: "<<P_Bi<<endl;

  return P_Bi;
}

double AnaTree::BinomialZ(double mainObs, double backgroundObs, double relativeBkgUncert){

  double tau = 1./backgroundObs/(relativeBkgUncert*relativeBkgUncert);
  double auxiliaryInf = backgroundObs*tau;

  double P_Bi = TMath::BetaIncomplete(1./(1.+tau),mainObs,auxiliaryInf+1);
  // cout<<"P_value: "<<P_Bi<<endl;
  double Z = RooStats::PValueToSignificance(P_Bi);
  //cout<<"Significance: "<<Z<<endl;
  return Z;
}

void AnaTree::SetKinCuts(double _Varcut, double _METcut){

  if(_Varcut < 0){

    if(SR=="SRall") Var_cut = 0;
    if(SR=="SR1b") Var_cut = 600000;
    if(SR=="SR3b") Var_cut = 700000;
    if(SR=="SR0b3j") Var_cut =600000;
    if(SR=="SR0b5j") Var_cut =700000;

  }
  else Var_cut = _Varcut;

  if(_METcut < 0){

    if(SR=="SRall") MET_cut = 0;
    if(SR=="SR1b") MET_cut =150000;
    if(SR=="SR3b") MET_cut =125000;
    if(SR=="SR0b3j") MET_cut =200000;
    if(SR=="SR0b5j") MET_cut =125000;

  }
  else MET_cut = _METcut;

  cout<<"MET cut: "<<MET_cut<<endl;
  cout<<"Meff cut: "<<Var_cut<<endl;
  
  return;
}

void AnaTree::SetSR(TString _SR){
  SR = _SR;
  cout<<"SR setted to "<<SR<<endl;
  return;
}

void AnaTree::SetPtCut(double _pt_cut){
  pt_cut=_pt_cut;
  pt_change = 1;
  cout<<"Jet Pt cut set to "<<pt_cut<<endl;
}

void AnaTree::SelectJets(vector<TLorentzVector> jets, int cut_pt){
  good_jets.clear();
  for(unsigned int i=0;i<jets.size();i++){
    if(jets.at(i).Pt() > cut_pt){
      good_jets.push_back(jets.at(i));
    }
  }
  return;
}

void AnaTree::ReturnGoodJets(vector<TLorentzVector> *new_vect){
  cout<<"good_jets.size: "<<good_jets.size()<<endl;
  for(unsigned int i=0;i<good_jets.size();i++){
    new_vect->push_back(good_jets[i]);
  }
  return;
}

int AnaTree::SR_jetscuts(vector<TLorentzVector> jets, int num_bjets){

  if(SR=="SRall") return 1;
  if(SR=="SR1b"){
    if(pt_change) AnaTree::SelectJets(jets,pt_cut);
    else AnaTree::SelectJets(jets,50000);
    if(num_bjets<1) return -1;
    if(good_jets.size() < 4) return -1;
    return 1;
  }
  if(SR=="SR3b"){
    if(pt_change) AnaTree::SelectJets(jets,pt_cut);
    else AnaTree::SelectJets(jets,50000);
    if(num_bjets<3) return -1;
    if(good_jets.size() < 6) return -1;
    return 1;
  }
  if(SR=="SR0b3j"){
    if(pt_change) AnaTree::SelectJets(jets,pt_cut);
    else AnaTree::SelectJets(jets,50000);
    if(num_bjets>0) return -1;
    if(good_jets.size() < 3) return -1;
    return 1;
  }
  if(SR=="SR0b5j"){
    if(pt_change) AnaTree::SelectJets(jets,pt_cut);
    else AnaTree::SelectJets(jets,50000);
    if(num_bjets>0) return -1;
    //cout<<"Num jets: "<<good_jets.size()<<endl;
    if(good_jets.size() < 5) return -1;
    return 1;
  }
  cout<<"SR selected not existing!"<<endl;
  return 0;
}

int AnaTree::EventSelector(float Meff, float MET, int njets, int num_bjets){

  if(SR=="SRall") return 1;

  if(SR=="SR1b"){
    if(num_bjets<1 || njets<4) return -1;
    if(Meff<500000) return -1;
    if(MET<125000) return -1;
    return 1;
  }
  if(SR=="SR3b"){
    if(num_bjets<3) return -1;
    if(Meff<600000) return -1;
    if(MET<100000) return -1;
    return 1;
  }
  if(SR=="SR0b3j"){
    if(num_bjets>0 || njets<3) return -1;
    if(Meff<500000) return -1;
    if(MET<150000) return -1;
    return 1;
  }
  if(SR=="SR0b5j"){
    if(num_bjets>0 || njets<5) return -1;
    if(Meff<600000) return -1;
    if(MET<100000) return -1;
    return 1;
  }
  cout<<"SR selected not existing!"<<endl;
  return 0;
}
 
void AnaTree::GetHistoRaw(TString VarName,TH1D *myHist) {

  TFile *myFile = TFile::Open(path+filename);
  TTreeReader myReader(TreeName, myFile);
  
  TTreeReaderValue<Float_t> myPx(myReader, VarName);
  TTreeReaderValue<Float_t> weight(myReader, "TotalWeight");
  TTreeReaderValue<Float_t> meff(myReader, "Meff");
  TTreeReaderValue<Float_t> MET(myReader, "EtMiss");
  TTreeReaderValue<vector<TLorentzVector>> jets(myReader, "jetTLV");
  //TTreeReaderValue<int> chan_evt(myReader,"SSChannel");
  TTreeReaderValue<int> njets(myReader,"Njet50"); 
  TTreeReaderValue<int> num_bjets(myReader,"Nbjet");
  TTreeReaderValue<int> ev_num(myReader,"EventNumber");
  TTreeReaderValue<vector<float>> lep_ch(myReader,"lepCharges");
  /*
  while (myReader.Next()) {
    vector<float> charge = *lep_ch;  
    if(AnaTree::EventSelector(*myPx,*MET,*jets,*num_bjets)<0) continue;
    myHist->Fill(*myPx);
  }
  */
  myFile->Close();

  return;

}

void AnaTree::GetHisto(TString VarName,TH1D *myHist) {

  // Open the file containing the tree.
  TFile *myFile = TFile::Open(path+filename);
 
  TTreeReader myReader(TreeName, myFile);

  // The branch "px" contains floats; access them as myPx.
  TTreeReaderValue<Float_t> myPx(myReader, VarName);
  TTreeReaderValue<Float_t> tot_weight(myReader, "TotalWeight");
  TTreeReaderValue<Float_t> meff(myReader, "Meff");
  TTreeReaderValue<Float_t> MET(myReader, "EtMiss");
  TTreeReaderValue<int> num_bjets(myReader,"Nbjet");
  //TTreeReaderValue<int> ev_number(myReader,"EventNumber");
  TTreeReaderValue<int> njets(myReader,"Njet50");
  int counter=0;

  cout<<"Entries: "<<myReader.GetEntries(1)<<endl;
  if(DEBUG) cout<<"DEBUG \t counter: "<<counter<<endl;
  while (myReader.Next()) { 
    //cout<<"myReader: "<<myReader.Next()<<endl;
    counter++;
    if(DEBUG) cout<<"DEBUG \t counter: "<<counter<<endl;
    
    cout << "Loaded entry " << myReader.GetCurrentEntry() << '\n';
  
    continue;
    if(DEBUG){
      //cout<<"Ev. number: "<<*ev_number<<endl;
      cout<<"Meff: "<<*meff<<endl;
      cout<<"MET: "<<*MET<<endl;
      cout<<"Numbjets: "<<*num_bjets<<endl;
      cout<<"Numjets: "<<*njets<<endl;
    }
    if(DEBUG) cout<<"-----------"<<endl;
    if(AnaTree::EventSelector(*meff,*MET,*njets,*num_bjets)<0){
      myHist->Fill(*myPx,*tot_weight);
      /* if(DEBUG){
	 cout<<"Ev. number: "<<*ev_number<<endl;
	 cout<<"Meff: "<<*meff<<endl;
	 cout<<"MET: "<<*MET<<endl;
	 cout<<"Numbjets: "<<*num_bjets<<endl;
	 cout<<"Numjets: "<<*njets<<endl;
	 }
      */
      if(DEBUG){      
	cout<<"Taken!"<<endl;
      }
    }
  }

  myFile->Close();

  return;

}

void AnaTree::GetPtDistribution(TString type,TH1D *myHist, int selection) {

  // Open the file containing the tree.                                                                      
  TFile *myFile = TFile::Open(path+filename);

  TTreeReader myReader(TreeName, myFile);
  // The branch "px" contains floats; access them as myPx.                                                                                  
  TTreeReaderValue<Float_t> tot_weight(myReader, "TotalWeight");
  TTreeReaderValue<Float_t> meff(myReader, "Meff");
  TTreeReaderValue<Float_t> MET(myReader, "EtMiss");
  TTreeReaderValue<vector<TLorentzVector>> jets(myReader, type);
  TTreeReaderValue<int> num_bjets(myReader,"Nbjet");

  while (myReader.Next()) {

    if(SR=="SR1b" && *num_bjets <1) continue;
    if(SR=="SR3b" && *num_bjets <3) continue;
    if(SR=="SR0b3j" && *num_bjets>0) continue;
    if(SR=="SR0b5j" && *num_bjets>0) continue;

    if(*meff<Var_cut || *MET<MET_cut) continue;
   
    AnaTree::GetJets(*jets);
   
    if(SR=="SR1b" && pt_jets.size()<4) continue;
    if(SR=="SR0b3j" && pt_jets.size()<3) continue;
    if(SR=="SR0b5j" && pt_jets.size()<5) continue;

    if(selection==0){
      for(unsigned int j=0;j<pt_jets.size();j++){
	myHist->Fill(pt_jets[j],*tot_weight);
      }
    }
    else {
      myHist->Fill(pt_jets[selection-1],*tot_weight);
    }
  }
  myFile->Close();
  return;
}

void AnaTree::GetJets(vector<TLorentzVector> list_jet){
  pt_jets.clear();
  for(unsigned int j=0;j<list_jet.size();j++){
    pt_jets.push_back(list_jet.at(j).Pt());
  }

}

void AnaTree::GetTLVleptons(vector<TLorentzVector> list_lep){
  pt_leptons.clear();
  eta_leptons.clear();
  for(unsigned int j=0;j<list_lep.size();j++){
    pt_leptons.push_back(list_lep.at(j).Pt());
    eta_leptons.push_back(list_lep.at(j).Eta());
  }

}


float AnaTree::WeightedEvts(){

  if(DEBUG)  cout<<"---------------Getting weighted evts..."<<endl;

  /*TFile *myFile = TFile::Open(path+filename);
  cout<<"File opened.."<<endl;
  TTree *t = (TTree*)myFile->Get("ControlTree");
  cout<<"Tree"<<endl;
  TBranch *b = new TBranch();
  cout<<"Branch"<<endl;                                                                                                             

  float wevts;
  t->SetBranchAddress("xAODWeightSum",&wevts);

  vector<float> weights;
  cout<<"------------"<<endl;
  for(unsigned int i=1;i< 100;i++){
    t->GetEntry(i);
    cout<<wevts<<endl;
    if(wevts<1){
      t->GetEntry(i-1);
      cout<<wevts<<endl;
      weights.push_back(wevts);
    }
  }
  float final_w = 0;
  cout<<"Values: "<<endl;
  for(unsigned int n=0;n<weights.size();n++){
    cout<<weights[n]<<endl;
    final_w = final_w + weights[n];
  }
  cout<<"Final weight: "<<final_w<<endl;
  return final_w;
  */

  /*TFile *f = new TFile(histpath+filename); 
  
  TH1F *h1 = (TH1F*)f->Get("hEventsTotal");   

  float counter;

  counter = h1->GetBinContent(2);

  f->Close();
  */

  float counter;

  TFile *myFile = TFile::Open(path+filename);
  TTreeReader myReader("ControlTree", myFile);
  TTreeReaderValue<Int_t> FileEntry(myReader,"FileEntry");
  TTreeReaderValue<Float_t> xAODWeightSum(myReader,"xAODWeightSum");  
  
  while (myReader.Next()) {
    if(*FileEntry==1){
      counter = counter+*xAODWeightSum;
    }
  }

  if(DEBUG) cout<<"Weighted evts: "<<counter<<endl;
  myFile->Close();
  return counter;
}

void AnaTree::SetFileName(TString _name){
  
  filename = _name;
  if(DEBUG) cout<<"Filename setted: "<<filename<<endl;
  return;
}

void AnaTree::SetPath(TString _path){

  path = _path;
  cout<<"Path set to: "<<path<<endl;
  return;
}

void AnaTree::SetHistPath(TString _histpath){

  histpath = _histpath;
  cout<<"Path set to: "<<histpath<<endl;
  return;
}

void AnaTree::ClearFileList(){
  filelist.clear();
  sample_type.clear();
  cout<<"File list canceled!"<<endl;
  return;
}


void AnaTree::GetFilename(TString file){

  ifstream myFile(path+file);
  //assert (!myFile.fail());

  if(myFile) cout<<"File "<<file<<" opened!"<<endl;
  else return;

  TString mc_name = "";
  int type;

  while(!myFile.eof()){
    myFile>>mc_name>>type;
    //cout<<mc_name<<endl;
    filelist.push_back(mc_name);
    sample_type.push_back(type);
  }

  //myFile.close();

  return;

}


double AnaTree::GetXsec(){  
  if(DEBUG) cout<<"Getting xsec.."<<endl;
  TString SampleName;
  SampleName = filename;
  SampleName.Replace(0,3,"");
  SampleName.Replace(filename.Sizeof()-9,filename.Sizeof(),"");
  
  //cout<<filename<<endl;
  //cout<<SampleName<<endl;

  ifstream myFile(xsec_filename);
  //myFile.open(xsec_filename,ios::in);
  //assert (!myFile.fail());


  //if(myFile) cout<<"File "<<xsec_filename<<" opened!"<<endl;
  int mc_id = -1;
  TString mc_name = "";
  double xsec = -1;
  double K_factor = -1;
  double efficiency = -1;
  double boh = -1;


  //myFile>>mc_id>>mc_name>>xsec>>K_factor>>efficiency>>boh;

  int n = 0;
  if(myFile.eof()) cout<<"Problem!"<<endl;
  while(!myFile.eof()){
    myFile>>mc_id>>mc_name>>xsec>>K_factor>>efficiency>>boh;
    n++;
    
    if(mc_name == SampleName) break;
    if(myFile.eof() && DEBUG) cout<<SampleName<<" not found in xsec file!"<<endl;
  }
  xsec = xsec*K_factor*efficiency;
  //cout<<"xsec: "<<xsec<<endl;
  //myFile.close();
  return xsec;
}

void AnaTree::RescaleLumi(TH1D *hist, float lumi,int type){

  float scale;

  TFile *myFile = TFile::Open(path+filename);

  //cout<<"Num events: "<<hist->Integral()<<endl;
  //cout<<"Weighted evts: "<<AnaTree::WeightedEvts()<<endl;
  //cout<<"Xsec: "<<AnaTree::GetXsec()<<endl;
  //TTreeReader myReader("ControlTree",myFile);
  //TTreeReaderValue<Float_t> weight(myReader, "MCWeight");


  //float wevts;
  //wevts = AnaTree::WeightedEvts();

  /*cout<<"Getting weighted evts..."<<endl;
  TTree *t = (TTree*)myFile->Get("ControlTree");
  TBranch *b = new TBranch();
  int i = t->GetEntries();
  float wevts;
  t->SetBranchAddress("xAODWeightSum",&wevts);
  t->GetEntry(i-1);
  //while (myReader.Next()) {
  //wevts=wevts+*weight;
  // }*/
  
  float wevts;
  float xsec;
  wevts = AnaTree::WeightedEvts();
  xsec = AnaTree::GetXsec();
  scale = (lumi*xsec)/(wevts);
  
  //cout<<"Scale: "<<scale<<endl;
  //cout<<"Before: "<<hist->Integral()<<endl;
  if(DEBUG) {
    cout<<"Number of Wighted evts: "<<wevts<<endl;
    cout<<"Xsec: "<<xsec<<endl;
    cout<<"Scale: "<<scale<<endl;
  }

  hist->Scale(scale);
  //cout<<"Rescaled: "<<hist->Integral()<<endl;
  myFile->Close();
  return;
}
