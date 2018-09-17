{
  gStyle->SetOptStat(000000);
  gStyle->SetPaintTextFormat(".1f");
  gStyle->SetTextFont(42);

  const int G=4;
  TFile *f[G];
  f[0] = new TFile("HistoMC_Gtt.root","open");
  f[1] = new TFile("HistoMC_2stepWZ.root");
  f[2] = new TFile("HistoMC_BB1step.root");
  f[3] = new TFile("HistoMC_SLN1.root");

  TString names[G] = {"Gtt", "GG2step", "BB1step", "SLN1"};

  const int N=2;
  TH2F *h[N][G];
  TCanvas *c[N][G];

  for(unsigned int g(0); g<G; g++){
    //    if(g!=3) continue;
    h[0][g] = (TH2F*)f[g]->Get("MCnom");
    h[1][g] = (TH2F*)f[g]->Get("MCerr");

    for (int i(0); i<N; i++){
      TString cname = Form("Plot_%i_%s",i,names[g].Data()); 
      c[i][g] = new TCanvas(cname, cname);
      std::cout << "Created " << c[i][g]->GetName() << std::endl;
      c[i][g]->cd();
      
      h[i][g]->GetXaxis()->SetTitle("m_{#tilde{q}} [GeV]");
      h[i][g]->GetYaxis()->SetTitle("m_{#tilde{#Chi}_{0}^{1}} [GeV]");    
      h[i][g]->GetXaxis()->SetLabelFont(42);
      h[i][g]->GetXaxis()->SetTitleFont(42);
      h[i][g]->GetYaxis()->SetLabelFont(42);
      h[i][g]->GetYaxis()->SetTitleFont(42);
      h[i][g]->GetYaxis()->SetTitleOffset(1.23);    

      h[i][g]->Rebin2D(10,10);
      h[i][g]->GetZaxis()->SetRangeUser(1E-6,10);
      h[i][g]->SetMarkerSize(1.5);
      if(i==0) h[i][g]->SetTitle(Form("MC nominal grid %s",names[g].Data()));
      if(i==1) h[i][g]->SetTitle(Form("MC error grid %s",names[g].Data()));
      h[i][g]->Draw("COLZ TEXT45");

      TString cname2 = Form("Plot_%i_%s.pdf",i,names[g].Data());
      c[i][g]->Print(cname2);

    }

  }

}
