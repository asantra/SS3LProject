#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "TSystem.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2F.h"
#include "TString.h"
#include "TCollection.h"
#include "TKey.h"
#include "TMath.h"

bool isSignal(float channel){
  if (channel >= 370000. && channel <= 380000.) return true;
  return false; 
}

double getError(std::vector< float > v){
  double sum2(0);
  for(unsigned int i(1); i<v.size(); i++) sum2 += TMath::Power(v[i]-v[0], 2); 
  return TMath::Sqrt(sum2);
}

double getTotalError( std::vector< TH1* > V, bool Nom, bool useUD, bool debug){

  std::vector< float > v;
  std::vector< TString > s;
  for(unsigned int i(0); i<V.size(); i++){
    TH1 *h = (TH1*)V.at(i);
    TString name = h->GetName();
    if( name.Contains("Nom") ){ 
      s.push_back( name );
      v.push_back( h->GetBinContent(1) );
    }
  }
  if(!useUD){
    for(unsigned int i(0); i<V.size(); i++){
      TH1 *h = (TH1*)V.at(i);
      TString name = h->GetName();
      if( !name.Contains("Nom") && !name.Contains("mcstat") ){  
	s.push_back( name );
	v.push_back( h->GetBinContent(1) );  
      }
    }
  }
  else{
    std::vector< float > vh,vl;
    for(unsigned int i(0); i<V.size(); i++){
      TH1 *h = (TH1*)V.at(i);
      TString name = h->GetName();
      if( !name.Contains("Nom") && !name.Contains("mcstat") ){  	
	if(name.Contains("High")){
	  TString n = name;
	  n.ReplaceAll("High","");
	  s.push_back( n );
	  vh.push_back( h->GetBinContent(1) );  
	}
	if(name.Contains("Low")){
	  vl.push_back( h->GetBinContent(1) ); 	
	}	 
      }
    }
    for(unsigned int j(0); j<vh.size(); j++) v.push_back( v[0] + TMath::Max( TMath::Abs(vh[j]-v[0]),TMath::Abs(vl[j]-v[0]) ) );  
  }
  if(debug){ for(unsigned int i(0); i<v.size(); i++) std::cout << Form("Name %s \t \t Value %f \t \t Diff %f", s[i].Data(), v[i], v[i]-v[0]) << std::endl;}
  
  float result = Nom ? v[0] : getError(v);
  return result;
}

std::vector< std::vector<float> > readSignalPoints(TString filename, bool debug){
  std::cout << "Reading table " << filename << std::endl;

  std::ifstream File;
  File.open(filename);
  if( !File.is_open() ){ cout << " -- " << filename << " not found... aborting" << endl; abort(); }

  std::vector< std::vector<float> > info;

  while(true){
    if( File.eof() ) break;
    std::vector< float > point;
    std::vector< std::string > names;  
    for(int i(0); i<5; i++){
      std::string n; float p;
      File >> n;
      File >> p;
      names.push_back(n);
      point.push_back(p);
    }
    if( isSignal(point[0]) ) info.push_back(point);
  }

  if(debug){
    for(unsigned int i(0); i<info.size(); i++)
      std::cout << Form("%.0f \t %.0f \t %.0f \t %f \t %f", info[i][0], info[i][1], info[i][2], info[i][3], info[i][4]) << std::endl;	 
  }
  return info;
}


void plotsMCError(TString grid = "") {

  bool useUD   = true;
  bool doDebug = false;

  int G(0);
  if(grid=="Gtt")     G = 0;
  if(grid=="2stepWZ") G = 1;
  if(grid=="BB1step") G = 2;
  if(grid=="SLN1")    G = 3;  
  std::cout << "Option " << grid << " plotting grid no " << G << std::endl; 

  TString path = "/home/fc12/HistFitter/Run/FITRESULTS/20-01-2016/";
  TString exclude = "";

  ofstream outfile1;
  TFile   *outfile2;

  const int N = 4;
  TString filenames[N] = {"Mc15SusyGtt_nom_Test_NoElIso_3.21fb_V14_Excl_Job_winter2015SameSign.root", 
			  "Mc15SusyGG2StepWZ_nom_Test_NoElIso_3.21fb_V14_Excl_Job_winter2015SameSign.root", 
			  "Mc15SusyBtt_nom_Test_NoElIso_3.21fb_V14_Excl_Job_winter2015SameSign.root", 
			  "Mc15SusyGSL_nom_Test_NoElIso_3.21fb_V14_Excl_Job_winter2015SameSign.root"};
  TString tables[N] = {"Mc15SusyGtt_CL.txt",
		       "Mc15SusyGG2StepWZ_CL.txt",
		       "Mc15SusyBtt_CL.txt",
		       "Mc15SusyGSL_CL.txt"};
  TString gridnames[N] = {"Gtt", "2stepWZ", "BB1step", "SLN1"};

  TFile *f_grid = new TFile(path+filenames[G]);

  outfile2 = new TFile("HistoMC_"+gridnames[G]+".root", "RECREATE");

  TH2F * h_MCnom = new TH2F("MCnom", "MCnom", 500, 0, 2000, 500, 0, 2000);
  TH2F * h_MCerr = new TH2F("MCerr", "MCerr", 500, 0, 2000, 500, 0, 2000);

  outfile1.open("TableMC_"+gridnames[G]+".txt");
  outfile1 << "Grid:"+gridnames[G]+"\n";

  std::vector< std::vector< float > > info = readSignalPoints(tables[G], doDebug);
  for(unsigned int it(0); it<info.size(); it++){ 
    std::vector< TH1* >  hVec;
    TIter next(f_grid->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)next())) {
      TH1 *h = (TH1*)key->ReadObj();
      TString name = h->GetName();
      if(!name.Contains("hsignal")) continue;
      TString ch = Form ("%i", (int)info[it][0]);
      bool pass = name.Contains(ch) && !name.Contains("Norm");
      if(exclude.Length()) pass = pass && !name.Contains(exclude);
      if(pass) hVec.push_back(h);
    }
   
    double MCnom = getTotalError(hVec, 1, useUD, 0);
    double MCerr = getTotalError(hVec, 0, useUD, doDebug);
    
    h_MCnom->Fill(info[it][1], info[it][2], MCnom);
    h_MCerr->Fill(info[it][1], info[it][2], MCerr);

    TString line = Form("Channel %i \t m0 %i \t m12 %i \t MC(nom) %.3f \t MC(Err) %.4f", (int)info[it][0], (int)info[it][1], (int)info[it][2], MCnom, MCerr );
    std::cout << line << std::endl;
    line +="\n";
    outfile1 << line;
  }

  outfile2->cd();
  h_MCnom->Write();
  h_MCerr->Write();

  return;
}


