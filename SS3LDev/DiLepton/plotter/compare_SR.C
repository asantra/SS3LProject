#include <iostream>
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"

using namespace std;

void compare_SR()
{
  TString path = "/afs/cern.ch/user/p/ptornamb/SS3L/DiLepton/util"; //name of the directory of the root file
  TString filename = "hist-mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r6630_r6264.root"; //name of the root file
  TString output_dir = "/afs/cern.ch/user/p/ptornamb/SS3L/DiLepton/util/compare_histo"; //here you can set the output directory for the plots
  TString histname = "";
  
  bool saving = true; //TRUE: saves all histos in the output directory, put FALSE to turn off
  
  TFile f1(path+"/"+filename);                                        
  
  vector<TString> SR, ANA;

  //Definition of the analysis to compare

  ANA.push_back("Moriond");
  ANA.push_back("Inclusive");
  ANA.push_back("Exclusive LEP");
  ANA.push_back("Exclusive Analysis");
  
  TString SR_in;

  //Gets the name of the histo to compare
  cout<<"Choose the histo to compare"<<endl;
  cin>>histname;

  //Gets the SR to compare for each analysis

  for (unsigned int n_ana=0; n_ana<ANA.size(); n_ana++){
    cout<<"Choose the SR for the "<<ANA[n_ana]<<" (all/0b/1b/3b/3Lhigh/3Llow)"<<endl;
    cin>>SR_in;
    SR.push_back("SR"+SR_in);
  }

  TH1F *h1 = (TH1F*)f1.Get(histname+"_"+SR[0]+"_A");
  TH1F *h2 = (TH1F*)f1.Get(histname+"_"+SR[1]+"_B");
  TH1F *h3 = (TH1F*)f1.Get(histname+"_"+SR[2]+"_C");
  TH1F *h4 = (TH1F*)f1.Get(histname+"_"+SR[3]+"_D");
  
  if (!h1 || !h2 || !h3 || !h4) {
    cout<<"ERROR! Histo not found!"<<endl;
    return;
  }
  else cout<<"Histos found!"<<endl;
  
  TCanvas *c = new TCanvas("c","c");
  
  h1->SetLineColor(kBlack);
  h1->SetLineWidth(1); 
  h1->Draw("hist");
  
  h2->SetLineColor(kRed);
  h2->SetLineWidth(1);
  h2->Draw("same hist");
  
  h3->SetLineColor(kGreen);
  h3->SetLineWidth(1);
  h3->Draw("same hist");
  
  h4->SetLineColor(kOrange);
  h4->SetLineWidth(1);
  h4->Draw("same hist");
  
  TLegend *leg = new TLegend(0.72, 0.48, 0.82, 0.73);
  leg->SetBorderSize(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.03);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetLineColor(0);
  leg->AddEntry(h1, ANA[0], "l");
  leg->AddEntry(h2, ANA[1], "l");
  leg->AddEntry(h3, ANA[2], "l");
  leg->AddEntry(h4, ANA[3], "l");
  
  leg->Draw("same");
  
  if(saving) c->SaveAs(output_dir+histname+"_ALL_ANA.png");
  
  delete c;
  delete h1,h2,h3,h4;
  delete leg;
}
