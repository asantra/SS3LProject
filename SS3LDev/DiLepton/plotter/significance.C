#include <iostream>
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TSystem.h"
#include "TMath.h"
#include <fstream>
#include <iomanip>
#include <vector>

using namespace std;


void WriteTable(vector<vector<double> > &signal, vector<vector<double> > &background, vector<vector<double> > &significance, vector<vector<double> > &sig_err, vector<vector<double> > &bkg_err,vector<double> &mass, vector<TString> &ana_name);
void CalculateSignificance(vector<vector<double> > &signal, vector<vector<double> > &background, vector<vector<double> > &significance);
void GetBackground(vector<vector<double> > &bkg, vector<vector<double> > &bkg_err,vector<TString> &ana_suffix,TString &bkg_filename, TString histname, TString SR_in, TString file_path);
void GetSignal(vector<vector<double> > &signal_vect,vector<vector<double> > &signal_err, vector<TString> &ana_suffix, vector<TString> &signal_filename, TString histname, TString SR_in, TString file_path);
void ComparisonPlot(vector<vector<double> > &significance, vector<double> &signal_mass, vector<TString> &ana_name, TString SR_in);

void significance(){

  TString path = "./samples"; //name of the directory of the root file       
  vector<TString> s_b_filename; //vector S+B root file
  TString bkg_filename = "MC_total_background.root"; //only bkg root file

  TString Hist_name = "Meff";
  TString SR_name = "SR1b";

  //Name of the analysis in the histos and corresppondent suffixes
  vector<TString> ana_name;  
  vector<TString> ana_suffix;

  //Masses of the signal samples
  vector<double> signal_mass;

  ana_name.push_back("Inclusive 2L");
  ana_name.push_back("Inclusive 2L+1");
  ana_name.push_back("Inclusive 3L");
  ana_name.push_back("Exclusive 2L");

  ana_suffix.push_back("A");
  ana_suffix.push_back("B");
  ana_suffix.push_back("C");
  ana_suffix.push_back("D");

 //Adding the S+B root files 
  s_b_filename.push_back("Gtt_G1000_T5000_L100.root");
  s_b_filename.push_back("Gtt_G1300_T5000_L100.root");
  //sigplusbkg_filename.push_back("Gtt_G1600_T5000_L100_plus_bkg.root");
  //sigplusbkg_filename.push_back("Gtt_G1900_T5000_L100_plus_bkg.root");
  //sigplusbkg_filename.push_back("Gtt_G2500_T5000_L100_plus_bkg.root");

  signal_mass.push_back(1000);
  signal_mass.push_back(1300);
  //signal_mass.push_back(1600);
  //signal_mass.push_back(1900);
  //signal_mass.push_back(2500);
  
  vector<vector<double> > signal_evts;
  vector<vector<double> > background_evts;
  vector<vector<double> > significance;
  vector<vector<double> > signal_error;
  vector<vector<double> > background_error;

  GetSignal(signal_evts,signal_error,ana_suffix,s_b_filename,Hist_name,SR_name,path); //gets the only signal histos and saves the values+errors
  GetBackground(background_evts,background_error,ana_suffix,bkg_filename,Hist_name,SR_name,path); //gets the only bkg histos and saves the values+errors 
  CalculateSignificance(signal_evts,background_evts,significance); //calculates and saves the significance
  WriteTable(signal_evts,background_evts,significance,signal_error,background_error,signal_mass,ana_name); //prepares a tex table with the values
  ComparisonPlot(significance,signal_mass,ana_name,SR_name); //plots the significance of the different analysis for the different values of the masses
}


void GetSignal(vector<vector<double> > &signal_vect,vector<vector<double> > &signal_err, vector<TString> &ana_suffix, vector<TString> &signal_filename, TString histname, TString SR_in, TString file_path){
  
  cout<<"Getting the S+B files..."<<endl;
  double signal = 0;
  double err = 0;

  for(unsigned int n_file=0; n_file<signal_filename.size(); n_file++){
    TFile f1(file_path+"/"+signal_filename[n_file]);
    if(f1.IsOpen()) cout<<"File "<<signal_filename[n_file]<<" opened successfully!"<<endl;
    else
      {
	cout<<signal_filename[n_file]<<" not found!"<<endl;
      }
    signal_vect.push_back(vector<double>());
    signal_err.push_back(vector<double>());

    for(unsigned int n_ana=0; n_ana<ana_suffix.size(); n_ana++){
  
      //Getting the histos of the 4 analysis
      TH1F *h1 = (TH1F*)f1.Get(histname+"_"+SR_in+"_"+ana_suffix[n_ana]);
      cout<<"Getting "+histname+"_"+SR_in+"_"+ana_suffix[n_ana]<<endl;
      if (!h1){
	cout<<"ERROR! S+B histo not found!"<<endl;
	return;
      }
      else cout<<"S+B histos found!"<<endl;
      
      signal = h1->IntegralAndError(0,h1->GetNbinsX(),err);
      signal_vect[n_file].push_back(signal);
      signal_err[n_file].push_back(err);
    }  
    f1.Close();
  }
  
  return;
  } 

void GetBackground(vector<vector<double> > &bkg, vector<vector<double> > &bkg_err,vector<TString> &ana_suffix,TString &bkg_filename, TString histname, TString SR_in, TString file_path){

  cout<<"Getting the Bkg files..."<<endl;
  double num_bkg = 0;
  double err = 0;

  TFile f1(file_path+"/"+bkg_filename);
  if(f1.IsOpen()) cout<<"File "<<bkg_filename<<" opened successfully!"<<endl;
  else
    {
      cout<<bkg_filename<<" not found!"<<endl;
    }
  bkg.push_back(vector<double>());
  bkg_err.push_back(vector<double>());

  for(unsigned int n_ana=0; n_ana<ana_suffix.size(); n_ana++){
    
    //Getting the histos of the 4 analysis
    TH1F *h1 = (TH1F*)f1.Get(histname+"_"+SR_in+"_"+ana_suffix[n_ana]);
    cout<<"Getting "+histname+"_"+SR_in+"_"+ana_suffix[n_ana]<<endl;
    if (!h1){
      cout<<"ERROR! Histo not found!"<<endl;
      return;
    }
    
    num_bkg = h1->IntegralAndError(0,h1->GetNbinsX(),err);
    bkg[0].push_back(num_bkg);
    bkg_err[0].push_back(err);
  }  
  f1.Close();

  return;
}

void CalculateSignificance(vector<vector<double> > &signal, vector<vector<double> > &background, vector<vector<double> > &significance){
  double sign=0;

  for(unsigned int n_sign=0; n_sign<signal.size(); n_sign++){
    significance.push_back(vector<double>());
    for(unsigned int n_ana=0; n_ana<signal[n_sign].size(); n_ana++){
      sign=signal[n_sign][n_ana]/TMath::Sqrt(background[0][n_ana]);
      significance[n_sign].push_back(sign);
    }
  }
  return;
}

void WriteTable(vector<vector<double> > &signal, vector<vector<double> > &background, vector<vector<double> > &significance, vector<vector<double> > &sig_err, vector<vector<double> > &bkg_err,vector<double> &mass, vector<TString> &ana_name){
  cout<<"Writing table.."<<endl;
  ofstream MyFile("significance_table.txt");

  for(unsigned int i_mass=0; i_mass<signal.size(); i_mass++){
    MyFile<<fixed<<setprecision(0)<<"\\begin{table}[htdp] \n\\centering \n\\begin{tabular}{|c|c|c|c|} \n\\hline \n Gtt("<<mass[i_mass]<<",5000,100) & Background & Signal & Significance\\\\ \n\\hline \n"<<ana_name[0]<<fixed<<setprecision(2)<<" & "<<background[0][0]<<"$\\pm$"<<bkg_err[0][0]<<" & "<<signal[i_mass][0]<<"$\\pm$"<<sig_err[i_mass][0]<<" & "<<significance[i_mass][0]<<" \\\\ \n"<<ana_name[1]<<" & "<<background[0][1]<<"$\\pm$"<<bkg_err[0][1]<<" & "<<signal[i_mass][1]<<"$\\pm$"<<sig_err[i_mass][1]<<" & "<<significance[i_mass][1]<<"\\\\ \n"<<ana_name[2]<<" & "<<background[0][2]<<"$\\pm$"<<bkg_err[0][2]<<" & "<<signal[i_mass][2]<<"$\\pm$"<<sig_err[i_mass][2]<<" & "<<significance[i_mass][2]<<"\\\\ \n"<<ana_name[3]<<" & "<<background[0][3]<<"$\\pm$"<<bkg_err[0][3]<<" & "<<signal[i_mass][3]<<"$\\pm$"<<sig_err[i_mass][3]<<" & "<<significance[i_mass][3]<<"\\\\ \n\\hline \n\\end{tabular} \n\\caption{Significance table} \n\\end{table}"<<endl;
    MyFile<<"\n"<<endl;
  }
  MyFile.close();
  cout<<"Table written!"<<endl;
    return;
}

void ComparisonPlot(vector<vector<double> > &significance, vector<double> &signal_mass, vector<TString> &ana_name, TString SR_in){
  cout<<"Plotting..."<<endl;
  vector<double> vect_sign;

  TMultiGraph *mg_1 = new TMultiGraph("mg_1","mg_1");
  TLegend *leg1 = new TLegend(1.,.9,0.71,0.6);
  for(unsigned int n_ana=0; n_ana<ana_name.size(); n_ana++){     
    for(unsigned int n_mass=0; n_mass<signal_mass.size(); n_mass++){
      vect_sign.push_back(significance[n_mass][n_ana]);
    }
    TGraphErrors *gr1 = new TGraphErrors(signal_mass.size(),signal_mass.data(),vect_sign.data(),0,0);
    gr1->SetName(ana_name[n_ana]);
    gr1->SetMarkerColor(n_ana+1);
    gr1->SetLineWidth(2.);
    gr1->SetLineColor(n_ana+1);
    gr1->SetMarkerStyle(15);
    gr1->SetTitle("Significance vs Mass;Mass [GeV];Significance");
    TCanvas *c1 = new TCanvas("c1","Grafico_1");
    gr1->Draw("AP");
    mg_1->Add(gr1);
    leg1->AddEntry(ana_name[n_ana],ana_name[n_ana],"lep");
    vect_sign.clear();
    delete c1;
  }
  cout<<"Finished!"<<endl;
  
  TCanvas *c2 = new TCanvas("c2","Grafico_2");
  
  mg_1->SetTitle(SR_in+";Mass [GeV];Significance");
  mg_1->Draw("AHIST");;
  mg_1->GetHistogram()->Rebin(100/signal_mass.size());
  mg_1->GetXaxis()->SetBinLabel(1,"Gtt(1000,5000,100)");
  mg_1->GetXaxis()->SetBinLabel(2,"Gtt(1300,5000,100)");
  /* mg_1->GetXaxis()->SetBinLabel(3,"Gtt(1600,5000,100)");
  mg_1->GetXaxis()->SetBinLabel(4,"Gtt(1900,5000,100)");
  mg_1->GetXaxis()->SetBinLabel(5,"Gtt(2500,5000,100)");
  */
  leg1->Draw("SAME");
  
  return;
}
