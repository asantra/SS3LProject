{
  gROOT->LoadMacro("/home/fc12/plotter/atlasstyle-00-03-04/AtlasStyle.C");
  SetAtlasStyle();
  gStyle->SetOptStat(000000);
  gStyle->SetOptTitle(0);
  gStyle->SetHatchesLineWidth(2);


  TString name = "Meff", SR = "SR1b", pairOpt = "A";

  bool log = true;
  int rebin = 40;
  float xMin=0., xMax=3200000., yMin=0.003;
  double rMin = 0., rMax=2.08;
  TString xlabel = "M_{eff} [MeV]";

  TString path = "/storage/users/fc12/DiLepton-13-06-2015/LEPALL/Merged/scaled/";
  std::cout << " -- plotting from " << path << endl;

  const unsigned int N = 6;
  TFile *f[N];
  TString filenames[N] = {"MC_jetjet_Pythia.root", 
			  "MC_Wt_PowhegPythia.root", 
			  "MC_WqqZ_Sherpa.root", 
			  "MC_Top_PowhegPythia.root", 
			  "MC_W_PowhegPythia.root",
			  "MC_Z_PowhegPythia.root"};


  const unsigned int S = 3;
  TFile *f_sig[S];
  TString signal[S] = {"user.fcardill.204533.Herwigpp_UEEE4_Gtt_G1000_T5000_L100_LEPALL_hist-output_merged_scaled.root",
		       "user.fcardill.204534.Herwigpp_UEEE4_Gtt_G1300_T5000_L100_LEPALL_hist-output_merged_scaled.root",
		       "user.fcardill.204535.Herwigpp_UEEE4_Gtt_G1600_T5000_L100_LEPALL_hist-output_merged_scaled.root" };

  for(int i=0; i<S; i++) std:: cout << "Signal " << i << "  "<< signal[i] << std::endl;

  for(int i=0; i<N; i++) f[i] = new TFile(path+filenames[i]);
  for(int i=0; i<S; i++) f_sig[i] = new TFile(path+signal[i]);

  TH1F* h[N];
  histname = name+"_"+SR+"_"+pairOpt;
  for(int i=0; i<N; i++) h[i] = (TH1F*)f[i]->Get(histname); 
  if(rebin) for(int i=0; i<N; i++) h[i]->Rebin(rebin);

  TH1F *h_ = h[0]->Clone("h_");
  h_->Reset();
  for(int i=0; i<N; i++) h_->Add(h[i]);

  TH1F *h_sig[S]; 
  TH1F *sig[S];
  for(int i=0; i<S; i++) h_sig[i] = (TH1F*)h_->Clone( Form("h_sig_%i",i) ) ;

  for(int i=0; i<S; i++){
    sig[i] = (TH1F*)f_sig[i]->Get(histname);
    if(rebin) sig[i]->Rebin(rebin);
    h_sig[i]->Add(sig[i]);
  }

  TCanvas *c = new TCanvas("c","c", 200, 10, 800, 600);
  TPad *pad1 = new TPad("pad1", "pad1", 0., 0.33 , 1., 1., -1, 0, 0 );
  pad1->SetBottomMargin( 0.0005 );
  pad1->SetTopMargin( c->GetTopMargin() );
  pad1->SetLeftMargin( 0.11 );
  pad1->SetRightMargin( c->GetRightMargin() );
  if(log) pad1->SetLogy();
  pad1->SetGridx();
  pad1->Draw();
  TPad *pad2 = new TPad( "pad2", "pad2", 0., 0., 1., 0.2 + c->GetBottomMargin() / 600 + 0.13, -1, 0, 0 );
  pad2->SetTopMargin( 0. );
  pad2->SetBottomMargin( c->GetBottomMargin() * 2.4 );
  pad2->SetTopMargin( 0.0005 );
  pad2->SetLeftMargin( 0.11 );
  pad2->SetRightMargin( c->GetRightMargin() );
  pad2->SetGridx();
  pad2->Draw();
  
  pad1->cd();
  h_->Draw("hist");
  if(log) h_->GetYaxis()->SetRangeUser(yMin, h_->GetMaximum()*100);
  else h_->GetYaxis()->SetRangeUser(0., h_->GetMaximum()*1.3);
  h_->GetXaxis()->SetRangeUser(xMin, xMax);
  h_->GetYaxis()->SetLabelSize(0.04);;
  h_->GetYaxis()->SetTitleSize(0.04);
  h_->GetYaxis()->SetTitleOffset(1.);
  h_->GetYaxis()->SetTitle("#events");
  h_->GetXaxis()->SetLabelSize(0.03);
  h_->GetXaxis()->SetLabelOffset(0.017);
  h_->GetXaxis()->SetTitle(xlabel);
  h_->GetXaxis()->SetTitleSize(0.03);
  h_->GetXaxis()->SetTitleOffset(1.5);
  h_->SetLineWidth(1);
  h_->SetLineColor(kBlack);

  for(int i=0; i<S; i++){
    h_sig[i]->SetLineWidth(2.);
    h_sig[i]->SetLineStyle(7);
    h_sig[i]->SetMarkerSize(0);
    h_sig[i]->Draw("histsame");
  }
  h_sig[0]->SetLineColor(kRed);
  h_sig[1]->SetLineColor(kGreen);
  h_sig[2]->SetLineColor(kViolet);


  THStack* bkg = new THStack("bkg","bkg");
  h[0]->SetFillColor(kOrange-6);
  h[1]->SetFillColor(kMagenta);
  h[2]->SetFillColor(kGreen-1);
  h[3]->SetFillColor(kOrange-9);
  h[4]->SetFillColor(kAzure+10);
  h[5]->SetFillColor(kBlue-4);
  for(int i=0; i<N; i++) h[i]->SetLineWidth(1);
  for(int i=0; i<N; i++) bkg->Add(h[i]);
  bkg->Draw("histsame");

  h_->Draw("hist same a");
  gPad->RedrawAxis(); 


  TLegend *leg = new TLegend(0.72, 0.48, 0.80, 0.88);
  leg->SetBorderSize(0);
  leg->SetHeader("Backgrounds"); 
  leg->SetTextFont(42);
  leg->SetTextSize(0.03);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetLineColor(0);
  for(int i=0; i<N; i++){ 
    TString bkg_name = filenames[i];
    bkg_name.ReplaceAll(".root","").ReplaceAll("MC","").ReplaceAll("_"," ");
    leg->AddEntry(h[i], bkg_name, "f");
  }
  TString signames[S] = {" Gtt(1000,5000,100)", " Gtt(1300,5000,100)", " Gtt(1600,5000,100)"};
  for(int i=0; i<S; i++) leg->AddEntry( h_sig[i], signames[i], "l");
  leg->Draw("same");

  TLatex n;
  n.SetNDC();
  n.SetTextColor(kBlack);;
  n.SetTextFont(72);
  n.SetTextSize(0.05);
  n.DrawLatex(0.40,0.86,histname.ReplaceAll("_"," "));
  n.SetTextSize(0.04);
  n.SetTextFont(42);
  n.DrawLatex(0.40,0.80, Form(" SM total. %.2f", h_->Integral() ));  
  
  for(int i=0; i<S; i++) n.DrawLatex(0.40, (float) 0.80-((float)(i+1)*0.05), Form("%s: %.2f", signames[i].Data(), h_sig[i]->Integral() )) ;
  

  pad2->cd();  
  TH1F* h_ratio[S]; 
  for(int i=0; i<S; i++){ 
    h_ratio[i] = (TH1F*)h_->Clone( Form("ratio_%i",i) );
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
  h_ratio[0]->GetXaxis()->SetTitleSize(0.09);
  h_ratio[0]->GetXaxis()->SetTitleOffset(1.4);
  h_ratio[0]->GetXaxis()->SetLabelSize(0.08);
  h_ratio[0]->GetXaxis()->SetLabelOffset(0.035);
  h_ratio[0]->GetYaxis()->SetTitle("Sig/Bkg");
  h_ratio[0]->GetYaxis()->SetTitleSize(0.06);
  h_ratio[0]->GetYaxis()->SetTitleOffset(0.65);
  h_ratio[0]->GetYaxis()->SetLabelSize(0.06);
  h_ratio[0]->GetYaxis()->SetLabelOffset(0.008);
  for(int i=1; i<S; i++) h_ratio[i]->Draw("hist same");

  TLine line;
  line.SetLineColor(kBlack);
  line.SetLineStyle(3);
  line.SetLineWidth(2);
  line.DrawLine(xMin, 1., xMax, 1.);
  gPad->RedrawAxis();

}

