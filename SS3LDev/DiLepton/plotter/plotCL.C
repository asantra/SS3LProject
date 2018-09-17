{
  gStyle->SetOptStat(000000);
  gStyle->SetPaintTextFormat(".1f");
  gStyle->SetTextFont(42);

  const int G=4;
  TFile *f[G];
  f[0] = new TFile("Mc15SusyGtt_CL.root");
  f[1] = new TFile("Mc15SusyGG2StepWZ_CL.root");
  f[2] = new TFile("Mc15SusyBtt_CL.root");
  f[3] = new TFile("Mc15SusyGSL_CL.root");

  TString names[4] = {"Gtt", "GG2step", "BB1step", "SLN1"};

  const int N=2;
  TH2F *h[N][G];
  TCanvas *c[N][G];

  for(unsigned int g(0); g<G; g++){
    //    if(g!=3) continue;

    if(g==0){
      h[0][g] = (TH2F*)f[g]->Get("Mc15SusyGtt_CLObs");
      h[1][g] = (TH2F*)f[g]->Get("Mc15SusyGtt_CLExp");
    }
    if(g==1){
      h[0][g] = (TH2F*)f[g]->Get("Mc15SusyGG2StepWZ_CLObs");
      h[1][g] = (TH2F*)f[g]->Get("Mc15SusyGG2StepWZ_CLExp");
    }
    if(g==2){
      h[0][g] = (TH2F*)f[g]->Get("Mc15SusyBtt_CLObs");
      h[1][g] = (TH2F*)f[g]->Get("Mc15SusyBtt_CLExp");
    }
    if(g==3){
      h[0][g] = (TH2F*)f[g]->Get("Mc15SusyGSL_CLObs");
      h[1][g] = (TH2F*)f[g]->Get("Mc15SusyGSL_CLExp");
    }
    for (int i(0); i<N; i++){
      TString cname = Form("CL_%i_%s",i,names[g].Data()); 
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

      h[i][g]->Rebin2D(20,20);
      h[i][g]->GetZaxis()->SetRangeUser(1E-6,1);
      h[i][g]->SetMarkerSize(1.5);
      if(i==0) h[i][g]->SetTitle(Form("CL Observed grid %s",names[g].Data()));
      if(i==1) h[i][g]->SetTitle(Form("CL Expected grid %s",names[g].Data()));
      h[i][g]->Draw("COLZ TEXT45");

      TString cname2 = Form("CL_%i_%s.pdf",i,names[g].Data());
      c[i][g]->Print(cname2);

    }

  }

}
