{
  //cout<<"Starting..."<<endl;

  gSystem->Load("AnaTree_cxx.so");

  gROOT->ProcessLine("AnaTree s");

  gROOT->LoadMacro("/home/fc12/plotter/atlasstyle-00-03-04/AtlasStyle.C");
  gStyle->SetOptStat(000000);
  gStyle->SetOptTitle(0);
  gStyle->SetHatchesLineWidth(2);
  //s.SetHistPath("/storage/users/pt1009/data_file/GG_2step_Analysis/TTree/original_files/Histo/Merged/");

  TString bkg_path = "/storage/users/pt1009/data_file/Dilepton_V20/Bkg/Merged/";
  //TString sig_path = "/storage/users/pt1009/data_file/Final_study/TTree/Signal/";
  //TString sample = "MC_MGPy8EG_A14N_BB_onestepC1_500_200_100_SS2L.root ";
  TString name = "";
  TString title = "";
  TString data_path = "/storage/users/pt1009/data_file/Dilepton_V20/Data/";
  TString data = "DATA_physics_main.root";
  
  TString variable = "Meff";

  float lumi = 2015.12;
  int n_meff = -1;
  int n_MET = -1;
  int y_max;

  s.SetDebugMode();

  vector<TString> sr_name;
  TString input_name;

  //  sr_name.push_back("SRall");
  sr_name.push_back("SR1b");
  //sr_name.push_back("SR3b");
  //sr_name.push_back("SR0b3j");
  //sr_name.push_back("SR0b5j");


  for(unsigned int i_sr=0; i_sr<sr_name.size(); i_sr++){

    input_name = sr_name[i_sr];
    title = variable+" distribution - "+input_name;
    //Settings
    s.SetSR(input_name);
    s.SetKinCuts(-1,-1);
    //s.SetPtCut(50000);
    //Signal
    /*TH1D *h_signal = new TH1D("signal","Meff signal",10,0,5000000);
    
    s.SetPath(sig_path);
    s.SetFileName(sample);
    s.GetHisto(variable,h_signal);
    s.RescaleLumi(h_signal,lumi,-1);
    
    cout<<"Signal: "<<h_signal->Integral()<<endl;
    */
    //Bkg histos
    s.SetPath(bkg_path);
    s.ClearFileList();
    s.GetFilename("test.txt");
    cout<<"Num of files: "<<s.filelist.size()<<endl;
    
    TH1D *tot_bkg = new TH1D("tot_bkg","Meff total bkg",10,0,5000000);
    
    //TH1D *higgs_bkg = new TH1D("higgs_bkg","Meff higgs bkg",10,0,5000000);
    TH1D *bosons_bkg = new TH1D("bosons_bkg","Meff bosons bkg",10,0,5000000);
    TH1D *ttbar_bkg = new TH1D("ttbar_bkg","Meff ttbar bkg",10,0,5000000);
    TH1D *top_bkg = new TH1D("top_bkg","Meff Top bkg",10,0,5000000);
    TH1D *higgs_bkg = new TH1D("higgs_bkg","Meff higgs bkg",10,0,5000000);
    TH1D *w_bkg = new TH1D("w_bkg","Meff W bkg",10,0,5000000);
    //TH1D *wt_bkg = new TH1D("wt_bkg","Wt top bkg",10,0,5000000);
    TH1D *z_pythia_bkg = new TH1D("z_pythia_bkg","Z (Pythia) bkg",10,0,5000000);
    //TH1D *z_sherpa_bkg = new TH1D("z_sherpa_bkg","Z (Sherpa) bkg",10,0,5000000);
    
    
    int num_files = s.filelist.size();
    
    for(unsigned int i=0; i<num_files-1;i++){
      //cout<<"--------------"<<endl;
      TH1D *bkg_hist = new TH1D("bkg_hist","Meff",10,0,5000000);
      TH1D *bkg_hist_raw = new TH1D("bkg_hist_raw","Meff",10,0,5000000);
      cout<<s.filelist[i]<<endl;
      cout<<s.sample_type[i]<<endl;     
      s.SetFileName(s.filelist[i]);
      s.GetHisto(variable,bkg_hist);
      s.GetHistoRaw(variable,bkg_hist_raw);
      
      //cout<<"Raw: "<<bkg_hist_raw->Integral()<<endl;
      //cout<<"Before: "<<bkg_hist->Integral()<<endl;
      s.RescaleLumi(bkg_hist,lumi,s.sample_type[i]);
      //cout<<"After: "<<bkg_hist->Integral()<<endl;
      tot_bkg->Add(bkg_hist);
      
      ////////////////////////////                  
      
      if(s.sample_type[i]==1){
	TH1D *h1 = (TH1D*)bkg_hist->Clone("h1");
	higgs_bkg->Add(h1);
      }
      
      if(s.sample_type[i]==3){
	TH1D *h1 = (TH1D*)bkg_hist->Clone("h1");
	top_bkg->Add(h1);
      }
      
      if(s.sample_type[i]==2){
	TH1D *h1 = (TH1D*)bkg_hist->Clone("h1");
	ttbar_bkg->Add(h1);
      }
      
      if(s.sample_type[i]==10){
	TH1D *h1 = (TH1D*)bkg_hist->Clone("h1");
	bosons_bkg->Add(h1);
      }
      
      if(s.sample_type[i]==11){
	TH1D *h1 = (TH1D*)bkg_hist->Clone("h1");
	z_pythia_bkg->Add(h1);
      }
      
      if(s.sample_type[i]==4){
	TH1D *h1 = (TH1D*)bkg_hist->Clone("h1");
	w_bkg->Add(h1);
      }
      
      ///////////////////////////
      delete bkg_hist;
      delete bkg_hist_raw;
    }
    
    ////////////////////DATA/////////////////////
    s.SetPath(data_path);
    s.SetFileName(data);
    TH1D *hist_data = new TH1D("hist_data","Data",10,0,5000000);
    s.GetHisto(variable,hist_data);
    //cout<<"--------Data: "<<hist_data->Integral()<<endl;
    ////////////////////////////////////////////
    //cout<<"Drawing!"<<endl;
    
    double err=0;
    cout<<"---------"<<input_name<<"---------"<<endl;
    cout<<"Tot bkg: "<<tot_bkg->IntegralAndError(0,50,err)<<" +- "<<err<<endl;
    /*    cout<<"VV,VVV: "<<bosons_bkg->IntegralAndError(0,50,err)<<" +- "<<err<<endl;
    cout<<"ttbar: "<<ttbar_bkg->IntegralAndError(0,50,err)<<" +- "<<err<<endl;
    cout<<"ttbarV,tZ: "<<top_bkg->IntegralAndError(0,50,err)<<" +- "<<err<<endl;
    cout<<"ttH,ggH,VBFH: "<<higgs_bkg->IntegralAndError(0,50,err)<<" +- "<<err<<endl;
    cout<<"W: "<<w_bkg->IntegralAndError(0,50,err)<<" +- "<<err<<endl;
    cout<<"Z: "<<z_pythia_bkg->IntegralAndError(0,50,err)<<" +- "<<err<<endl;
    */
    cout<<"Data: "<<hist_data->Integral()<<endl;

    TCanvas *c = new TCanvas("c","c");
    
    THStack* bkg_stack = new THStack("bkg","bkg");
    //h[0]->SetFillColor(kOrange-6);                                                                                                          
    //higgs_bkg->SetFillColor(kViolet);
    top_bkg->SetFillColor(kGreen+1);
    ttbar_bkg->SetFillColor(kOrange-9);
    higgs_bkg->SetFillColor(kAzure+10);
    bosons_bkg->SetFillColor(kBlue-4);
    w_bkg->SetFillColor(kRed-4);
    z_pythia_bkg->SetFillColor(kViolet);
    
    bkg_stack->Add(bosons_bkg);
    bkg_stack->Add(top_bkg);
    bkg_stack->Add(ttbar_bkg);
    bkg_stack->Add(higgs_bkg);
    bkg_stack->Add(w_bkg); 
    bkg_stack->Add(z_pythia_bkg);
    
    //bkg_stack->Draw("hist");
    //bkg_stack->GetYaxis()->SetRangeUser(0.001,10);
    //c->SetLogy();
    
    
    //tot_bkg->Add(h_signal);
    
    tot_bkg->Draw("hist");
    //tot_bkg->SetLineStyle(2);
    tot_bkg->SetLineWidth(3);
    tot_bkg->SetLineColor(kRed);

    if(sr_name[i_sr]=="SRall") y_max = 10000;
    else y_max = 10;

    tot_bkg->GetYaxis()->SetRangeUser(0.01,y_max);

    bkg_stack->Draw("same hist");
    c->SetLogy();
    
    hist_data->Draw("same E");
    hist_data->SetMarkerStyle(20);
    hist_data->SetLineColor(kBlack);

    //  cout<<"E"<<endl;
    TLegend *leg = new TLegend(0.60, 0.68, 0.80, 0.40);
    leg->SetBorderSize(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.03);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    leg->SetLineColor(0);
    
    leg->AddEntry(tot_bkg,"SM Background","l");
    leg->AddEntry(top_bkg,"Top+X","f");
    leg->AddEntry(ttbar_bkg,"ttbar","f");
    leg->AddEntry(bosons_bkg,"VV,VVV","f");
    leg->AddEntry(higgs_bkg,"ttH,Higgs","f");
    leg->AddEntry(w_bkg,"W","f");
    leg->AddEntry(z_pythia_bkg,"Z","f");
    leg->Draw("same");
    
    
    TLatex n;
    n.SetNDC();
    n.SetTextColor(kBlack);
    n.SetTextFont(72);
    n.SetTextSize(0.04);
    n.DrawLatex(0.58,0.83, Form(" #int L dt = %.2f fb^{-1}", lumi/1000));
    n.SetTextSize(0.03);
    n.SetTextFont(42);
    n.DrawLatex(0.60,0.77, Form(" Bkg total. %.2f", tot_bkg->Integral()));
    //n.DrawLatex(0.60,0.76, Form(" Signal total. %.2f", h_signal->Integral()));
    n.DrawLatex(0.60,0.72, Form(" Data total. %.2f", hist_data->Integral()));
    
    n.SetTextFont(72);
    n.SetTextSize(0.05);
    n.DrawLatex(0.38,0.92,title);
    
    c->SaveAs("./sr_distributions/"+variable+"_"+sr_name[i_sr]+".pdf");
    delete hist_data;
    delete tot_bkg;
    delete bkg_stack;
    delete bosons_bkg;
    delete ttbar_bkg;
    delete top_bkg;
    delete higgs_bkg;
    delete w_bkg;                                                                         
    delete z_pythia_bkg;
  }//END LOOP SR
}
