#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "TF1.h"
#include "TFile.h"
#include "TH1F.h"
#include "TKey.h"
#include "TTree.h"

#include "SUSYTools/SUSYCrossSection.h"
#include "SUSYTools/SUSYCrossSectionPMG.h"

#include <ROOT/TDataFrame.hxx>

#include <ROOT/RDataFrame.hxx>

using TDataFrame = ROOT::RDataFrame;
using namespace std;

void Export(string in_file_name, string out_file_name) {

  // SUSY::CrossSectionDBPMG *m_CrossSectionDBPMG = new
  // SUSY::CrossSectionDBPMG("/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.44/InstallArea/x86_64-slc6-gcc62-opt/data/SUSYTools/mc15_13TeV");

  // SUSY::CrossSectionDB *m_CrossSectionDB = new
  // SUSY::CrossSectionDB("/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.44/InstallArea/x86_64-slc6-gcc62-opt/data/SUSYTools/mc15_13TeV");

  SUSY::CrossSectionDB *m_CrossSectionDB = new SUSY::CrossSectionDB(
      "SUSYTools/data/mc15_13TeV/", false, false, false);

  m_CrossSectionDB->loadFile("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/"
                             "dev/SUSYTools/mc15_13TeV/Backgrounds.txt");

  map<string, double> unfilteredevs;

  string tree_name = "DiLeptonTree_SRall";
  string control_tree = "ControlTree";

  std::cout << "File : " << in_file_name << std::endl;
  std::cout << "tree name : " << tree_name << std::endl;
  bool isdata = false;
  string str_data("data");
  if (in_file_name.find(str_data) != std::string::npos)
    isdata = true;

  std::cout << "isData: " << isdata << std::endl;

  TFile *f_in = new TFile(in_file_name.c_str());

  if (f_in->IsZombie()) {
    cout << "file failed " << endl;
    return;
  }

  TTree *eventsbeforefilter = (TTree *)f_in->Get("ControlTree");
  double unfilterednev = eventsbeforefilter->GetEntries();
  cout << "Total Number of Entries: " << unfilterednev << endl;

  TDataFrame d(control_tree.c_str(), f_in);
  double Sum_MCWeights = d.Sum("MCWeight").GetValue();
  cout << "Total Number of Weighted Entries: " << Sum_MCWeights << endl;

  TIter nextkey(f_in->GetListOfKeys());
  TKey *key;

  bool dumpedcounter = false;

  while (key = (TKey *)nextkey()) {

    tree_name = (string)key->GetName();
    //string bla2(tree_name, 0, 8); // uncomment these 2 lines to run on all the systematics!!
    //if (bla2 == "HFntuple")
     string bla2(tree_name,0,18); // use this to run only on nominal
     if (bla2=="DiLeptonTree_SRall")
    {
      cout << ((string)key->GetName()).find("DiLeptonTree_SRall") << endl;
      cout << "Systtreename " << key->GetName() << endl;

      cout << " processing tree " << tree_name << endl;

      TDataFrame df(tree_name.c_str(), f_in);


      auto colNames = df.GetColumnNames();

      std::vector<std::string> allCols;

      for (auto &&colName : colNames) {
        std::cout << colName << std::endl;
        if (colName.find(".") == std::string::npos)
          allCols.push_back(colName);
      }

      allCols.push_back("XSecWeight_N");
      allCols.push_back("XSecWeight_SW");

      auto xsecweightfn_N = [=](int MCId) {

        double xsec = m_CrossSectionDB->xsectTimesEff(MCId);
        double lumi = 1.0 * unfilterednev / (xsec);
        double scale_unf = 1.0 / lumi;

        if (isdata)
          scale_unf = 1.0;

        return scale_unf;
      };

      auto xsecweightfn_SW = [=](int MCId) {

        double xsec = m_CrossSectionDB->xsectTimesEff(MCId);
        double lumi = 1.0 * Sum_MCWeights / (xsec);
        double scale_unf = 1.0 / lumi;

        if (isdata)
          scale_unf = 1.0;

        return scale_unf;
      };


      df.Define("XSecWeight_N", xsecweightfn_N, {"MCId"})
        .Define("XSecWeight_SW", xsecweightfn_SW, {"MCId"})
        .Snapshot(tree_name.c_str(), out_file_name.c_str(), allCols);
    }
  }
}

void ExportMyTree(string tmp_file) {
  ifstream in0;
  std::cout << "tmp_file is : " << tmp_file << std::endl;
  in0.open(tmp_file.c_str());
  string in_file, out_file;

  while (true) {
    in0 >> in_file >> out_file;
    if (!in0.good())
      break;
    Export(in_file, out_file);
  }
  in0.close();
}

int main(int argc, char *argv[]) {

  ExportMyTree(argv[1]);

  return 0;
}
