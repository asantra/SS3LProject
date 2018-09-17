{
  gSystem->Load("DiLeptonPlotter_cxx.so");
  gROOT->ProcessLine("DiLeptonPlotter p");
  gROOT->LoadMacro("~/plotter/atlasstyle-00-03-04/AtlasStyle.C");
  SetAtlasStyle();

  p.setInDir("/storage/users/fc12/DiLepton-24-08-2015/hist-output/Merged/scaled/");
  p.setOutDir("/home/fc12/");
  p.doPrint(false);

  p.setBkgFile("MC_4Top_MadGraphPythia.root", "4 Top");
  p.setBkgFile("MC_Wt_PowhegPythia.root",     "W Top");
  p.setBkgFile("MC_ttV_MadGraphPythia.root",  "ttV");
  p.setBkgFile("MC_Vgamma_Sherpa.root",       "V#gamma");
  p.setBkgFile("MC_DiBoson_Sherpa.root",      "diBoson");
  p.setBkgFile("MC_Top_PowhegPythia.root",    "Top");
  p.setBkgFile("MC_W_PowhegPythia.root",      "W+jets");
  p.setBkgFile("MC_Z_PowhegPythia.root",      "Z+jets");

  p.setSigFile("MC_GG_ttn1_1000_5000_400_MGPyEG.root",  "Gtt(1000,5000,400)");
  p.setSigFile("MC_GG_ttn1_1200_5000_100_MGPyEG.root",  "Gtt(1200,5000,100)");
  p.setSigFile("MC_GG_ttn1_1400_5000_100_MGPyEG.root",  "Gtt(1400,5000,100)");
  

  double lumi = 78.62;
  double histlumi = 3000.;

  p.setLumiScale(lumi, histlumi);
  p.useData(true);

  p.setPairing("B");
  p.setSR("SRall");
  p.plot("Meff",   0, 3200000, 0.003, 50);
  p.plot("ETmiss", 1, 800000,  0.003, 50);
  p.setSR("SR1b");
  p.plot("Meff",   0, 3200000, 0.003, 20);
  p.plot("ETmiss", 1, 800000,  0.003, 20);
  p.setSR("SR3b");
  p.plot("Meff",   0, 3200000, 0.003, 20);
  p.plot("ETmiss", 1, 800000,  0.003, 20);
  p.setSR("SR0b3j");
  p.plot("Meff",   0, 3200000, 0.003, 20);
  p.plot("ETmiss", 1, 800000,  0.003, 20);
  p.setSR("SR0b5j"); 
  p.plot("Meff",   0, 3200000, 0.003, 20);
  p.plot("ETmiss", 1, 800000,  0.003, 20);

}
