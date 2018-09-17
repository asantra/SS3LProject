#include "DiLeptonPlotter.h"

using namespace std;

void DiLeptonPlotter::doDebug(bool debug){
  Debug = debug;
  std::cout << CNAME << " :: Debug \t" << Debug << std::endl;
}

void DiLeptonPlotter::doPrint(bool print){
  Print = print;
  std::cout << CNAME << " :: Print \t" << Print << std::endl;
}

void DiLeptonPlotter::setSR(TString SRname){
  SR = SRname;
  std::cout << CNAME << " :: Set SR \t" << SR << std::endl;
}

void DiLeptonPlotter::setPairing(TString opt){
  pOpt = opt;
  std::cout << CNAME << " :: Set pairing \t"<< pOpt << std::endl;
}

void DiLeptonPlotter::setInDir(TString indir){
  inpath = indir;
  std::cout << CNAME << " :: Set input dir \t"<< inpath << std::endl;
}

void DiLeptonPlotter::setOutDir(TString outdir){    
  outpath = outdir;  
  std::cout << CNAME << " :: Set output dir \t"<< outpath << std::endl; 
}

void DiLeptonPlotter::setBkgFile(TString name, TString label){

  TFile *f = new TFile(inpath+name);
  this->check(f);
  BkgFiles.push_back(f);
  BkgNames.push_back(label);
  std::cout << CNAME << " :: Set Background "<< label << "\t"<< f->GetName() << std::endl;
}

void DiLeptonPlotter::setSigFile(TString name, TString label){
  
  TFile *s = new TFile(inpath+name);
  this->check(s);
  SigFiles.push_back(s);
  SigNames.push_back(label);
  std::cout << CNAME << " :: Set Signal "<< label << "\t"<< s->GetName() << std::endl;
}

bool DiLeptonPlotter::check(TFile *f){
  if(f) return true;
  else{std::cout << CNAME << "\t ERROR no files selected" << std::endl; abort();}
}

void DiLeptonPlotter::setLumiScale(double LumiScale, double HistLumi){
  
  lumi  = LumiScale;
  scale = lumi/HistLumi;
  std::cout << CNAME << " :: Set lumi "<< lumi <<" \t scale "<< scale << std::endl; 
}

void DiLeptonPlotter::useData(bool usedata){

  data = usedata;
  if(!data) fDATA=0;
  else fDATA = new TFile(inpath+"DATA_physics_Main.root");
  std::cout << CNAME << " :: Use data \t"<< data << std::endl; 
}

void DiLeptonPlotter::plot(TString name, float xMin, float xMax, float yMin, int rebin, bool log){

  gStyle->SetOptStat(000000);
  gStyle->SetOptTitle(0);
  gStyle->SetHatchesLineWidth(2);

  TString histname = name + "_" + SR + "_" + pOpt;
  std::cout << CNAME << " :: Plotting \t" << histname << std::endl;
  if(inpath==""){std::cout << "Input directory not set... abort " << std::endl; abort();}

  double rMin = 0., rMax=2.08;  
  TString xlabel; 
  if(histname.Contains("meff") || histname.Contains("Meff")) xlabel = "M_{eff} [MeV]";
  else xlabel = "E_{T}^{miss} [MeV]";
  rebin = rebin ? rebin : 1;

  const int N = BkgFiles.size();
  const int S = SigFiles.size();
  TFile *f[N]; TFile *f_sig[S];
  for(int i=0; i<N; i++) f[i] = (TFile*)BkgFiles.at(i);
  for(int i=0; i<S; i++) f_sig[i] = (TFile*)SigFiles.at(i);

  TH1F* h[N];
  for(int i=0; i<N; i++){ 
    h[i] = (TH1F*)f[i]->Get(histname);  
    h[i]->Rebin(rebin);
    h[i]->Scale(scale);
  }
  if(!h[0]){std::cout << "ERROR \t Histo not found" << std::endl; abort();}

  TH1F *h_ = (TH1F*)h[0]->Clone( Form("h_%s", histname.Data()) );
  h_->Reset();
  for(int i=0; i<N; i++) h_->Add(h[i]);

  TH1F *h_sig[S]; TH1F *sig[S];
  for(int i=0; i<S; i++) h_sig[i] = (TH1F*)h_->Clone( Form("h_sig_%s_%i",histname.Data(),i) ) ;

  for(int i=0; i<S; i++){
    sig[i] = (TH1F*)f_sig[i]->Get(histname);
    sig[i]->Rebin(rebin);
    sig[i]->Scale(scale);
    h_sig[i]->Add(sig[i]);
  }
  if(data){
    hDATA = (TH1F*)fDATA->Get(histname);
    hDATA->Rebin(rebin); 
  }

  TString cname = histname;
  TCanvas *c = new TCanvas(cname,cname, 200, 10, 800, 600);
  TPad *pad1 = new TPad(cname+"_p1", cname+"_p1", 0., 0.28 , 1., 1., -1, 0, 0 );
  pad1->SetBottomMargin( 0.0002 );
  pad1->SetTopMargin( c->GetTopMargin() );
  pad1->SetLeftMargin( 0.11 );
  pad1->SetRightMargin( c->GetRightMargin() );
  if(log) pad1->SetLogy();
  //pad1->SetGridx();
  pad1->Draw();
  TPad *pad2 = new TPad(cname+"_p2", cname+"_p2", 0., 0., 1., 0.28 , -1, 0, 0 );
  pad2->SetTopMargin( 0. );
  pad2->SetBottomMargin( c->GetBottomMargin() * 3 );
  pad2->SetTopMargin( 0.0002 );
  pad2->SetLeftMargin( 0.11 );
  pad2->SetRightMargin( c->GetRightMargin() );
  //pad2->SetGridx();
  pad2->Draw();
  
  pad1->cd();
  h_->Draw("hist");
  if(log) h_->GetYaxis()->SetRangeUser(yMin, h_->GetMaximum()*100);
  else h_->GetYaxis()->SetRangeUser(0., h_->GetMaximum()*1.3);
  h_->GetXaxis()->SetRangeUser(xMin, xMax);
  h_->GetYaxis()->SetLabelSize(0.04);;
  h_->GetYaxis()->SetTitleSize(0.04);
  h_->GetYaxis()->SetTitleOffset(1.);
  h_->GetYaxis()->SetTitle("# events");
  h_->GetXaxis()->SetLabelSize(0.03);
  h_->GetXaxis()->SetLabelOffset(0.017);
  h_->GetXaxis()->SetTitle(xlabel);
  h_->GetXaxis()->SetTitleSize(0.03);
  h_->GetXaxis()->SetTitleOffset(1.5);
  h_->SetLineWidth(1);
  h_->SetLineColor(kBlack);

  for(int i=0; i<S; i++){
    this->setColor(h_sig[i], i, "l");
    h_sig[i]->SetLineWidth(2.);
    h_sig[i]->SetLineStyle(2);
    h_sig[i]->SetMarkerSize(0);
    h_sig[i]->Draw("histsame");
  }  

  THStack* bkg = new THStack(histname+"_bkg",histname+"_bkg");
  for(int i=0; i<N; i++){ 
    this->setColor(h[i], i, "f"); 
    h[i]->SetLineWidth(1); 
    bkg->Add(h[i]);
  } 
  bkg->Draw("hist same");
  h_->Draw("hist same a");
  gPad->RedrawAxis();

  if(data){
    TH1F *hDATA_ = (TH1F*)hDATA->Clone( "hDATA_" );
    hDATA_->SetLineColor(kWhite);
    hDATA_->SetMarkerColor(kWhite);
    hDATA_->SetMarkerStyle(20);
    hDATA_->SetMarkerSize(1.2);
    hDATA_->SetLineWidth(3);
    hDATA_->Draw("PEX0SAME");
    hDATA->SetLineColor(kBlack);
    hDATA->SetMarkerStyle(20);
    hDATA->SetLineWidth(1);
    hDATA->SetMarkerSize(0.9);
    hDATA->Draw("PEX0SAME");
  } 

  TLegend *leg = new TLegend(0.70, 0.40, 0.78, 0.85);
  leg->SetBorderSize(0); 
  leg->SetTextFont(42);
  leg->SetTextSize(0.03);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetLineColor(0);
  if(data) leg->AddEntry(hDATA, "DATA 2015 #sqrt{s} = 13 TeV");
  for(int i=0; i<N; i++) leg->AddEntry(h[i], " "+BkgNames.at(i), "f");
  for(int i=0; i<S; i++) leg->AddEntry( h_sig[i], " "+SigNames[i], "l");
  leg->Draw("same");

  TLatex n;
  n.SetNDC();
  n.SetTextColor(kBlack);;
  n.SetTextFont(72);
  n.SetTextSize(0.05);
  n.DrawLatex(0.40,0.82,histname.ReplaceAll("_"," "));
  n.SetTextSize(0.03);
  n.SetTextFont(42);
  n.DrawLatex(0.40,0.77, Form("SM total %.2f", h_->Integral() ));  
  if(data) n.DrawLatex(0.60,0.77, Form("Data %.0f", hDATA->Integral() )); 
  for(int i=0; i<S; i++) n.DrawLatex(0.40, (float) 0.77-((float)(i+1)*0.04), Form("%s: %.2f", SigNames[i].Data(), h_sig[i]->Integral() )) ;
  n.SetTextSize(0.04);
  if(data) n.DrawLatex(0.20,0.77, Form("#int L dt = %.2f pb^{-1}", lumi) );

  pad2->cd();  
  TH1F* h_ratio[S]; 
  for(int i=0; i<S; i++){ 
    h_ratio[i] = (TH1F*)h_->Clone( Form("ratio_%s_%i",histname.Data(),i) );
    h_ratio[i]->Reset();
 
    for(int bin=0; bin<h_ratio[i]->GetNbinsX(); bin++){ 
      float ratio = h_->GetBinContent(bin)>0 ? h_sig[i]->GetBinContent(bin)/h_->GetBinContent(bin) : 0;
      h_ratio[i]->SetBinContent(bin, ratio);
    }
    h_ratio[i]->SetLineColor(h_sig[i]->GetLineColor());
    h_ratio[i]->SetLineWidth(1);
    h_ratio[i]->SetLineStyle(7);
  }
  h_ratio[0]->Draw("hist");
  h_ratio[0]->GetYaxis()->SetRangeUser(rMin, rMax);
  h_ratio[0]->GetYaxis()->SetNdivisions(10);
  h_ratio[0]->GetXaxis()->SetTitleSize(0.10);
  h_ratio[0]->GetXaxis()->SetTitleOffset(1.25);
  h_ratio[0]->GetXaxis()->SetLabelSize(0.09);
  h_ratio[0]->GetXaxis()->SetLabelOffset(0.03);
  h_ratio[0]->GetYaxis()->SetTitle("Ratio");
  h_ratio[0]->GetYaxis()->SetTitleSize(0.08);
  h_ratio[0]->GetYaxis()->SetTitleOffset(0.48);
  h_ratio[0]->GetYaxis()->SetLabelSize(0.07);
  h_ratio[0]->GetYaxis()->SetLabelOffset(0.008);
  for(int i=1; i<S; i++) h_ratio[i]->Draw("hist same");

  TLine line;
  line.SetLineColor(kBlack);
  line.SetLineStyle(3);
  line.SetLineWidth(2);
  line.DrawLine(xMin, 1., xMax, 1.);
  gPad->RedrawAxis();

  if(data){
    TH1F * h_ratioDATA = (TH1F*) hDATA->Clone("hratioDATA");
    h_ratioDATA->Reset();
    for(int bin=0; bin<h_ratioDATA->GetNbinsX(); bin++){ 
      float ratio = h_->GetBinContent(bin)>0 ? hDATA->GetBinContent(bin)/h_->GetBinContent(bin) : 0;
      h_ratioDATA->SetBinContent(bin, ratio);
    }
    h_ratioDATA->SetLineColor(hDATA->GetLineColor());
    h_ratioDATA->SetLineWidth(1);
    h_ratioDATA->Draw("PE X0 SAME");
  }  
  if(Print){
    c->Print(outpath+cname+".pdf");
    cout << " -- plot saved in " << outpath+cname<<".pdf" << endl;
  } 

  return;
}


void DiLeptonPlotter::setColor(TH1F *h, int i, TString option){

  if(option.Contains("f")){
    if(i==0)      h->SetFillColor(kOrange-6);
    else if(i==1) h->SetFillColor(kViolet);
    else if(i==2) h->SetFillColor(kOrange);
    else if(i==3) h->SetFillColor(kYellow);
    else if(i==4) h->SetFillColor(kGreen-1);
    else if(i==5) h->SetFillColor(kOrange-9);
    else if(i==6) h->SetFillColor(kAzure+10);
    else if(i==7) h->SetFillColor(kBlue-4);
    else          h->SetFillColor(kBlue);
  }
  if(option.Contains("l")){
    if(i==0)      h->SetLineColor(kOrange-6);
    else if(i==1) h->SetLineColor(kViolet);
    else if(i==2) h->SetLineColor(kOrange);
    else if(i==3) h->SetLineColor(kYellow);
    else if(i==4) h->SetLineColor(kGreen-1);
    else if(i==5) h->SetLineColor(kOrange-9);
    else if(i==6) h->SetLineColor(kAzure+10);
    else if(i==7) h->SetLineColor(kBlue-4);
    else          h->SetLineColor(kBlue);
  }
  return;
}
