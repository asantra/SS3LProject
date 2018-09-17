#define DiLeptonTree_SRall_cxx

#include "DiLeptonTree_SRall.h"
#include <TH2.h>
#include <TStyle.h>
#include <iostream>
#include <fstream>

void DiLeptonTree_SRall::Begin(TTree * /*tree*/)
{
  TString option = GetOption();
}

void DiLeptonTree_SRall::SlaveBegin(TTree * /*tree*/)
{
  debug   = false;
  showPDF = false;

  lumi   = 3209.;
  NTotal = 20165.389;
  float factor = 0.64;
  NTotal *= factor;
  xSec    = 0.0073556864*1.*0.66247000; 

  TString option = GetOption();
  meffcut  = 0;
  metcut   = 0;
  njets    = 0;
  nbjets   = 0;
  nbmax    = -1;
  nlep     = 0;
   
  int SR(0);
  if( option=="SR1b" )  SR = 1;
  if( option=="SR3b" )  SR = 2;
  if( option=="SR0b3j") SR = 3;
  if( option=="SR0b5j") SR = 4;

  switch( SR ){
  case 1:
    njets   = 4;
    nbjets  = 1;
    meffcut = 550000.;
    metcut  = 150000.;
    break;   
   
  case 2:
    njets   = 0;
    nbjets  = 3;
    meffcut = 650000.;
    metcut  = 125000.;
    break;
   
  case 3:
    njets = 3;
    meffcut = 550000;
    metcut = 200000;
    nbmax = 0;
    nlep   = 3;
    break;
   
  case 4:
    njets = 5;
    meffcut = 650000.;
    metcut  = 125000.;
    nbmax = 0;
    break;
   
  default:
    std::cout << "No SR selected, using SRall" << std::endl; 
    break;
  }

  RAW  = 0;
  sumW = 0;
  outfile.open("Events_"+option+".txt");

  std::cout << "Signal Region " << option << std::endl;
  TString line = "Signal Region: "+option + "\n"; 
  outfile << line;

  if(GlFilter) std::cout << "GlFilter applied " << std::endl;
}

Bool_t DiLeptonTree_SRall::Process(Long64_t entry)
{
  
  fChain->GetTree()->GetEntry(entry);

  if(Meff   < meffcut) return kTRUE;
  if(EtMiss < metcut)  return kTRUE;
  if(Njet50 < njets)   return kTRUE;
  if(Nbjet  < nbjets)  return kTRUE;
  if(nbmax>-1 && Nbjet>nbmax)    return kTRUE;
  if((int)lepTLV->size() < nlep) return kTRUE;

  if(GlFilter && (GLDec1==5 || GLDec2==5) ) return kTRUE;
  

  TString ch("");
  switch(SSChannel){
  case 1: ch = "mm"; break;
  case 2: ch = "ee"; break;
  case 3: ch = "em"; break;
  }
  float w = TotalWeight;
  float w_pdfUp(w);
  float w_pdfDown(w);
  if(showPDF){
     w_pdfUp = w * PDFWeights->at(1);
     w_pdfDown = w * PDFWeights->at(2);
  }

  TString line = Form("EventNumber %i \t Meff %.0f \t EtMiss %.0f \t Njet50 %i \t Nbjet %i \t Nlep %i \t Ch %s \t w %.3f \t FileEntry %i ", 
		      (int)EventNumber, Meff, EtMiss, Njet50, Nbjet, (int)lepTLV->size(), ch.Data(), w, (int)entry);

  std::cout << line << std::endl;
  line += " \n";
  outfile << line;

  if(debug){
    for (unsigned int i(0); i<lepTLV->size(); i++){
      TLorentzVector v = lepTLV->at(i);
      TString type = v.M()>50 ? "m" : "e";
      std::cout << "Lepton " << i << " type " << type << Form("\t Pt|Eta|Phi|E = %.1f | %.2f | %.2f | %.2f",v.Pt(),v.Eta(),v.Phi(),v.E()) << std::endl;
    }
    std::cout << std::endl;
  }

  RAW++;
  sumW += w;

  if(showPDF){
    sumW_pdfUp   += w_pdfUp;
    sumW_pdfDown += w_pdfDown;
  }

  return kTRUE;
}

void DiLeptonTree_SRall::SlaveTerminate()
{
}

void DiLeptonTree_SRall::Terminate()
{

  double W = getNorm(sumW);

  TString line = Form("RAW %i \t sumW %.6f \t normW %.6f", RAW, sumW, W);
  line += "\n";
  outfile << line;
  std::cout << line << std::endl;

  if(showPDF){
    std::cout << "sumW(PDFup) " << sumW_pdfUp << " sumW(PDFdown) " << sumW_pdfDown << std::endl; 
    std::cout << "normW(PDFup) " << getNorm(sumW_pdfUp) << " norm(PDFdown) " << getNorm(sumW_pdfDown) << std::endl;
  }  

}

double DiLeptonTree_SRall::getNorm(double v){
  TString line = Form("NTotal %.1f \t lumi %.1f \t xSec %.3f",NTotal, lumi, xSec );
  line += "\n";
  outfile << line;
  std::cout << line << std::endl;

  return (v/NTotal)*lumi*xSec;
}
