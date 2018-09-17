{
  gROOT->LoadMacro("/home/fc12/plotter/atlasstyle-00-03-04/AtlasStyle.C");
  SetAtlasStyle();                                                                   
  gStyle->SetOptStat(000000); 
  gStyle->SetOptTitle(0);
  gStyle->SetHatchesLineWidth(2);

  int rebin = 20;
  TString name = "Meff", SR = "SRall", pairOpt = "A";
  const unsigned int N = 6, L = 3;
  bool log = true;
  float xMin=0., xMax=2800000., yMin=0.01;
  TString xlabel = "M_{eff} [MeV]";

  histname = name+"_"+SR+"_"+pairOpt;
  TString path[L] = {"/storage/users/fc12/DiLepton-13-06-2015/LEPALL/Merged/scaled/",
		     "/storage/users/fc12/DiLepton-13-06-2015/ELECTRONS/Merged/scaled/",
		     "/storage/users/fc12/DiLepton-13-06-2015/MU_AND_EL/Merged/scaled/"};

  TString filenames[N] = {"MC_jetjet_Pythia.root", 
			  "MC_Wt_PowhegPythia.root", 
			  "MC_WqqZ_Sherpa.root", 
			  "MC_Top_PowhegPythia.root", 
			  "MC_W_PowhegPythia.root",
			  "MC_Z_PowhegPythia.root"};


  TFile *f[N][L];
  TH1F *h[N][L];

  for(int i=0; i<N; i++){
    for(int j=0; j<L; j++){
      std::cout << "Reading from "<< path[j]+filenames[i] << std::endl;
      f[i][j] = new TFile(path[j]+filenames[i]);
      h[i][j] = (TH1F*)f[i][j]->Get(histname);
      if(rebin) h[i][j]->Rebin(rebin);
    }
  }



  TH1F *h_[L];
  for(int j=0; j<L; j++){
    h_[j] = (TH1F*) h[0][j]->Clone( Form("h_%i",j) );
    h_[j]->Reset();
    for(int i=0; i<N; i++) h_[j]->Add(h[i][j]);
  }

  TCanvas *c = new TCanvas("c","c");
  c->cd();
  if(log) c->SetLogy();

  if(log) h_[0]->GetYaxis()->SetRangeUser(yMin, h_[0]->GetMaximum()*10);
  else h_[0]->GetYaxis()->SetRangeUser(0., h_[0]->GetMaximum()*1.3);
  h_[0]->GetXaxis()->SetRangeUser(xMin, xMax);
  h_[0]->GetYaxis()->SetLabelSize(0.03);;
  h_[0]->GetYaxis()->SetTitleSize(0.03);
  h_[0]->GetYaxis()->SetTitleOffset(1.5);
  h_[0]->GetYaxis()->SetTitle("#events");
  h_[0]->GetXaxis()->SetLabelSize(0.03);
  h_[0]->GetXaxis()->SetLabelOffset(0.017);
  h_[0]->GetXaxis()->SetTitle(xlabel);
  h_[0]->GetXaxis()->SetTitleSize(0.03);
  h_[0]->GetXaxis()->SetTitleOffset(1.5);
  h_[0]->SetLineWidth(1);
  h_[0]->SetLineColor(kBlack);

  h_[0]->SetLineColor(kBlue);
  for (int j=0; j<L; j++) h_[j]->SetMarkerSize(0);
  h_[0]->Draw("hist");
  h_[1]->SetLineColor(kRed);
  h_[1]->SetLineWidth(1);
  h_[1]->Draw("histsame");
  h_[2]->SetLineColor(kGreen);
  h_[2]->SetLineWidth(1);
  h_[2]->Draw("histsame");

  TLegend *leg = new TLegend(0.72, 0.65, 0.80, 0.85);
  leg->SetBorderSize(0); 
  leg->SetTextFont(42);
  leg->SetTextSize(0.03);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetLineColor(0);
  leg->AddEntry(h_[0], "all lepton pairs", "l");
  leg->AddEntry(h_[2], "ee or #mu#mu pair", "l");
  leg->AddEntry(h_[1], "ee pair", "l");
  leg->Draw("same");

  TLatex n;
  n.SetNDC();
  n.SetTextColor(kBlack);;
  n.SetTextFont(72);
  n.SetTextSize(0.04);
  n.DrawLatex(0.40,0.86,histname.ReplaceAll("_"," "));
  n.SetTextSize(0.03);
  n.SetTextFont(42);
  n.DrawLatex(0.40,0.80, Form(" Total SM background"));  
  

}
