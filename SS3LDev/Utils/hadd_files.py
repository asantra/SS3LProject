#!/usr/bin/env python

from glob import glob
from ROOT import *
import sys
import os
import shutil
import tempfile
from runPeriods import Luminosities

gROOT.ProcessLine( "gErrorIgnoreLevel = 3001;")

print "\033[1m"+"-- Starting file merger ..."+"\033[0m"
print ""

def mergeFiles(directory, infilenames, outfilename):

    mytempdir = tempfile.mkdtemp()
    try:

        basedirname = directory.rstrip('/').split('/')[-1]
        outfilenamefull = os.path.join(directory, outfilename)
        outfilenametemp = os.path.join(mytempdir, outfilename)
        infilenamesfull = [os.path.join(directory, i) for i in infilenames]

        print "\033[1m"+"-- Outfilename  ",outfilenamefull,"\033[0m"
        print "    Infile list has %i entries" % len(infilenames)

        infiles = []
        for i in infilenamesfull:
            infiles.extend( glob( i ) )
        if len(infiles) > 0:
            print "    Merging %i files" % len(infiles)

            # Remove output file
            if os.path.exists(outfilenamefull):
                os.unlink(outfilenamefull)

            # File merger
            filemerger = TFileMerger()
            filemerger.Reset()
            filemerger.OutputFile( outfilenametemp )
            lumi = 0

            for infile in infiles:
                if not os.path.exists(infile):
                    print "File",infile,"does not exist"
                    sys.exit(1)

                tempinfile = os.path.join(mytempdir, os.path.basename(infile))
                shutil.copy(infile, mytempdir)
                filemerger.AddFile(tempinfile, False)

                if infile.find("physics_Main") >= 0: lumi += getLumi(infile)
                print "   ",infile

            print "   ... Starting to merge files"
            result = filemerger.Merge()
            if not result:
                print "Error merging",outfilenamefull
                sys.exit(1)

            filemerger.Reset()

            #Print lumi for datafiles
            if lumi > 0: print "   --> Total data lumi:", lumi

            # Copy outputfile
            print "   ... Copying output file to original location"
            shutil.copy(outfilenametemp, outfilenamefull)

        else:
            print "    No input files found"

    finally:
        shutil.rmtree(mytempdir)


def getLumi(infile):
 
    sample = infile.split("/")[-1]
    runnumber = int(sample.split(".")[2])
    if Luminosities.has_key(runnumber): return Luminosities[runnumber]
    else: return 0


if __name__ == "__main__":

    if len(sys.argv) < 2:
        print "Please specify a diretory"
        sys.exit(1)

    directory = sys.argv[1]
    if len(directory) == 0:
        print "Please specify a diretory"
        sys.exit(1)
    if not os.path.exists(directory):
        print "Directory",directory,"does not exist"
        sys.exit(1)

 
    # MC
    mergeFiles(directory, ["user.*.*.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wplus*.root",\
                               "user.*.*.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wminus*.root"], "MC_W_PowhegPythia.root")

    mergeFiles(directory, ["user.*.*.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad*.root",\
                               "user.*.*.PowhegPythiaEvtGen_P2012_singletop_tchan_lept_*.root",\
                               "user.*.*.PowhegPythiaEvtGen_P2012_SingleTopSchan_noAllHad_*.root"], "MC_Top_PowhegPythia.root")

    mergeFiles(directory, ["user.*.*.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee*.root",\
                               "user.*.*.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu*.root",\
                               "user.*.*.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau*.root"], "MC_Z_PowhegPythia.root")

    mergeFiles(directory, ["user.*.*.PowhegPythia8_AU2CT10_WpWm*.root",\
                               "user.*.*.PowhegPythia8_AU2CT10_WmZ*.root",\
                               "user.*.*.PowhegPythia8_AU2CT10_WpZ*.root",\
                               "user.*.*.PowhegPythia8_AU2CT10_ZZ*.root"], "MC_DiBoson_PowhegPythia.root")

    mergeFiles(directory, ["user.*.*.Pythia8EvtGen*H125*.root",\
                               "user.*.*.PowhegPythia8EvtGen*H125*.root"], "MC_Higgs_PowhegPythia.root")

    mergeFiles(directory, ["user.*.*.Sherpa_CT10_Zee_Pt*_C*B*.root",\
                           "user.*.*.Sherpa_CT10_Zmumu_Pt*_C*B*.root",\
                               "user.*.*.Sherpa_CT10_Ztautau_Pt*_C*B*.root"], "MC_Z_Sherpa.root")

    mergeFiles(directory, ["user.*.*.Sherpa_CT10_enugammaPt*.root",\
                               "user.*.*.Sherpa_CT10_munugammaPt*.root",\
                               "user.*.*.Sherpa_CT10_taunugammaPt*.root",\
                               "user.*.*.Sherpa_CT10_eegammaPt*.root",\
                               "user.*.*.Sherpa_CT10_mumugammaPt*.root",\
                               "user.*.*.Sherpa_CT10_tautaugammaPt*.root",\
                               "user.*.*.Sherpa_CT10_nunugammaPt*.root"], "MC_Vgamma_Sherpa.root")

    mergeFiles(directory, ["user.*.*.Sherpa_CT10_llll*.root",\
                               "user.*.*.Sherpa_CT10_lllv*.root",\
                               "user.*.*.Sherpa_CT10_llvv*.root",\
                               "user.*.*.Sherpa_CT10_ggllll*.root",\
                               "user.*.*.Sherpa_CT10_ggllvv*.root",\
                               "user.*.*.Sherpa_CT10_Z*Z*.root",\
                               "user.*.*.Sherpa_CT10_W*Z*.root",\
                               "user.*.*.Sherpa_CT10_W*W*.root"], "MC_DiBoson_Sherpa.root")

    mergeFiles(directory, ["user.*.*.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ*.root"], "MC_jetjet_Pythia.root")

    mergeFiles(directory, ["user.*.*.PowhegPythiaEvtGen_P2012_Wt_dilepton_top*.root",\
                               "user.*.*.PowhegPythiaEvtGen_P2012_Wt_dilepton_antitop*.root"], "MC_Wt_PowhegPythia.root")

    mergeFiles(directory, ["user.*.*.MadGraphPythia8EvtGen_A14NNPDF23LO_ttW_Np*.root",\
                               "user.*.*.MadGraphPythia8EvtGen_A14NNPDF23_ttbarWW*.root",\
                               "user.*.*.MadGraphPythia8EvtGen_A14NNPDF23LO_ttee_Np*.root",\
                               "user.*.*.MadGraphPythia8EvtGen_A14NNPDF23LO_ttmumu_Np*.root",\
                               "user.*.*.MadGraphPythia8EvtGen_A14NNPDF23LO_tttautau_Np*.root"], "MC_ttV_MadGraphPythia.root")

    mergeFiles(directory, ["user.*.*.MadGraphPythia8EvtGen_A14NNPDF23_4topSM.*.root"], "MC_4Top_MadGraphPythia.root")

    #Signal
    mergeFiles(directory, ["user.*.371200.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_600_350_225_100*.root"], "MC_GG_2stepWZ_600_350_225_100_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371201.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_600_450_375_300*.root"], "MC_GG_2stepWZ_600_450_375_300_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371202.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_600_500_450_400*.root"], "MC_GG_2stepWZ_600_500_450_400_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371205.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_700_650_625_600*.root"], "MC_GG_2stepWZ_700_650_625_600_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371206.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_800_450_275_100*.root"], "MC_GG_2stepWZ_800_450_275_100_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371207.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_800_550_425_300*.root"], "MC_GG_2stepWZ_800_550_425_300_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371209.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_800_650_575_500*.root"], "MC_GG_2stepWZ_800_650_575_500_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371211.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_800_750_725_700*.root"], "MC_GG_2stepWZ_800_750_725_700_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371213.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_900_750_675_600*.root"], "MC_GG_2stepWZ_900_750_675_600_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371214.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_900_800_750_700*.root"], "MC_GG_2stepWZ_900_800_750_700_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371217.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1000_650_475_300*.root"], "MC_GG_2stepWZ_1000_650_475_300_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371218.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1000_750_625_500*.root"], "MC_GG_2stepWZ_1000_750_625_500_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371219.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1000_800_700_600*.root"], "MC_GG_2stepWZ_1000_800_700_600_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371221.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1000_900_850_800*.root"], "MC_GG_2stepWZ_1000_900_850_800_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371224.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1100_800_650_500*.root"], "MC_GG_2stepWZ_1100_800_650_500_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371225.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1100_850_725_600*.root"], "MC_GG_2stepWZ_1100_850_725_600_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371226.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1100_900_800_700*.root"], "MC_GG_2stepWZ_1100_900_800_700_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371231.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1200_800_600_400*.root"], "MC_GG_2stepWZ_1200_800_600_400_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371233.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1200_900_750_600*.root"], "MC_GG_2stepWZ_1200_900_750_600_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371235.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1200_1000_900_800*.root"], "MC_GG_2stepWZ_1200_1000_900_800_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371237.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1300_700_400_100*.root"], "MC_GG_2stepWZ_1300_700_400_100_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371239.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1300_800_550_300*.root"], "MC_GG_2stepWZ_1300_800_550_300_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371241.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1300_900_700_500*.root"], "MC_GG_2stepWZ_1300_900_700_500_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371243.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1300_1000_850_700*.root"], "MC_GG_2stepWZ_1300_1000_850_700_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371247.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1400_800_500_200*.root"],  "MC_GG_2stepWZ_1400_800_500_200_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371249.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1400_900_650_400.*.root"], "MC_GG_2stepWZ_1400_900_650_400_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371251.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1400_1000_800_600*.root"], "MC_GG_2stepWZ_1400_1000_800_600_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371255.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1500_800_450_100*.root"], "MC_GG_2stepWZ_1500_800_450_100_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371256.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1500_850_525_200*.root"], "MC_GG_2stepWZ_1500_850_525_200_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371257.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1500_900_600_300*.root"], "MC_GG_2stepWZ_1500_900_600_300_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371259.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1500_1000_750_500*.root"], "MC_GG_2stepWZ_1500_1000_750_500_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371261.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1500_1100_900_700*.root"], "MC_GG_2stepWZ_1500_1100_900_700_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371265.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1600_900_550_200*.root"], "MC_GG_2stepWZ_1600_900_550_200_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371267.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1600_1000_700_400*.root"], "MC_GG_2stepWZ_1600_1000_700_400_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371269.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1600_1100_850_600*.root"], "MC_GG_2stepWZ_1600_1100_850_600_MadgraphPythia8.root")
    mergeFiles(directory, ["user.*.371277.MadGraphPythia8EvtGen_A14N_GG_2stepWZ_1700_1100_800_500*.root"], "MC_GG_2stepWZ_1700_1100_800_500_MadgraphPythia8.root")

    mergeFiles(directory, ["user.*.370102.MGPy8EG_A14N_GG_ttn1_900_5000_400*.root"], "MC_GG_ttn1_900_5000_400_MGPyEG.root")
    mergeFiles(directory, ["user.*.370106.MGPy8EG_A14N_GG_ttn1_1000_5000_400*.root"], "MC_GG_ttn1_1000_5000_400_MGPyEG.root")
    mergeFiles(directory, ["user.*.370107.MGPy8EG_A14N_GG_ttn1_1000_5000_600*.root"], "MC_GG_ttn1_1000_5000_600_MGPyEG.root")
    mergeFiles(directory, ["user.*.370110.MGPy8EG_A14N_GG_ttn1_1100_5000_200*.root"], "MC_GG_ttn1_1100_5000_200_MGPyEG.root")
    mergeFiles(directory, ["user.*.370112.MGPy8EG_A14N_GG_ttn1_1100_5000_600*.root"], "MC_GG_ttn1_1100_5000_600_MGPyEG.root")
    mergeFiles(directory, ["user.*.370115.MGPy8EG_A14N_GG_ttn1_1200_5000_100*.root"], "MC_GG_ttn1_1200_5000_100_MGPyEG.root")
    mergeFiles(directory, ["user.*.370117.MGPy8EG_A14N_GG_ttn1_1200_5000_400*.root"], "MC_GG_ttn1_1200_5000_400_MGPyEG.root")
    mergeFiles(directory, ["user.*.370118.MGPy8EG_A14N_GG_ttn1_1200_5000_600*.root"], "MC_GG_ttn1_1200_5000_600_MGPyEG.root")
    mergeFiles(directory, ["user.*.370119.MGPy8EG_A14N_GG_ttn1_1200_5000_700*.root"], "MC_GG_ttn1_1200_5000_700_MGPyEG.root")
    mergeFiles(directory, ["user.*.370120.MGPy8EG_A14N_GG_ttn1_1200_5000_800*.root"], "MC_GG_ttn1_1200_5000_800_MGPyEG.root")
    mergeFiles(directory, ["user.*.370125.MGPy8EG_A14N_GG_ttn1_1300_5000_400*.root"], "MC_GG_ttn1_1300_5000_400_MGPyEG.root")
    mergeFiles(directory, ["user.*.370126.MGPy8EG_A14N_GG_ttn1_1300_5000_600*.root"], "MC_GG_ttn1_1300_5000_600_MGPyEG.root")
    mergeFiles(directory, ["user.*.370127.MGPy8EG_A14N_GG_ttn1_1300_5000_800*.root"], "MC_GG_ttn1_1300_5000_800_MGPyEG.root")
    mergeFiles(directory, ["user.*.370130.MGPy8EG_A14N_GG_ttn1_1400_5000_100*.root"], "MC_GG_ttn1_1400_5000_100_MGPyEG.root")
    mergeFiles(directory, ["user.*.370131.MGPy8EG_A14N_GG_ttn1_1400_5000_200*.root"], "MC_GG_ttn1_1400_5000_200_MGPyEG.root")
    mergeFiles(directory, ["user.*.370133.MGPy8EG_A14N_GG_ttn1_1400_5000_600*.root"], "MC_GG_ttn1_1400_5000_600_MGPyEG.root")
    mergeFiles(directory, ["user.*.370135.MGPy8EG_A14N_GG_ttn1_1400_5000_1000*.root"], "MC_GG_ttn1_1400_5000_1000_MGPyEG.root")
    mergeFiles(directory, ["user.*.370139.MGPy8EG_A14N_GG_ttn1_1500_5000_400*.root"], "MC_GG_ttn1_1500_5000_400_MGPyEG.root")
    mergeFiles(directory, ["user.*.370141.MGPy8EG_A14N_GG_ttn1_1500_5000_800*.root"], "MC_GG_ttn1_1500_5000_800_MGPyEG.root")
    mergeFiles(directory, ["user.*.370145.MGPy8EG_A14N_GG_ttn1_1600_5000_200*.root"], "MC_GG_ttn1_1600_5000_200_MGPyEG.root")
    mergeFiles(directory, ["user.*.370147.MGPy8EG_A14N_GG_ttn1_1600_5000_600*.root"], "MC_GG_ttn1_1600_5000_600_MGPyEG.root")
    mergeFiles(directory, ["user.*.370154.MGPy8EG_A14N_GG_ttn1_1700_5000_400*.root"], "MC_GG_ttn1_1700_5000_400_MGPyEG.root")
    mergeFiles(directory, ["user.*.370155.MGPy8EG_A14N_GG_ttn1_1700_5000_600*.root"], "MC_GG_ttn1_1700_5000_600_MGPyEG.root")
    mergeFiles(directory, ["user.*.370156.MGPy8EG_A14N_GG_ttn1_1700_5000_800*.root"], "MC_GG_ttn1_1700_5000_800_MGPyEG.root")
    mergeFiles(directory, ["user.*.370158.MGPy8EG_A14N_GG_ttn1_1700_5000_1200*.root"], "MC_GG_ttn1_1700_5000_1200_MGPyEG.root")
    mergeFiles(directory, ["user.*.370163.MGPy8EG_A14N_GG_ttn1_1800_5000_600*.root"], "MC_GG_ttn1_1800_5000_600_MGPyEG.root")
    mergeFiles(directory, ["user.*.370167.MGPy8EG_A14N_GG_ttn1_1800_5000_1400*.root"], "MC_GG_ttn1_1800_5000_1400_MGPyEG.root")
    mergeFiles(directory, ["user.*.370173.MGPy8EG_A14N_GG_ttn1_1900_5000_800*.root"], "MC_GG_ttn1_1900_5000_800_MGPyEG.root")
    mergeFiles(directory, ["user.*.370174.MGPy8EG_A14N_GG_ttn1_1900_5000_1000*.root"], "MC_GG_ttn1_1900_5000_1000_MGPyEG.root")
    mergeFiles(directory, ["user.*.370176.MGPy8EG_A14N_GG_ttn1_1900_5000_1400*.root"], "MC_GG_ttn1_1900_5000_1400_MGPyEG.root")
    mergeFiles(directory, ["user.*.370183.MGPy8EG_A14N_GG_ttn1_2000_5000_1000*.root"], "MC_GG_ttn1_2000_5000_1000_MGPyEG.root")
    mergeFiles(directory, ["user.*.370424.MGPy8EG_A14N_GG_ttn1OnShellt1_1500_450_60*.root"], "MC_GG_ttn1OnShellt1_1500_450_60_MGPyEG.root")
    mergeFiles(directory, ["user.*.370429.MGPy8EG_A14N_GG_ttn1OnShellt1_1500_1320_60*.root"], "MC_GG_ttn1OnShellt1_1500_1320_60_MGPyEG.root")
    mergeFiles(directory, ["user.*.370438.MGPy8EG_A14N_GG_ttn1OnShellt1_1700_850_60*.root"], "MC_GG_ttn1OnShellt1_1700_850_60_MGPyEG.root")
    mergeFiles(directory, ["user.*.370442.MGPy8EG_A14N_GG_ttn1OnShellt1_1700_1520_60*.root"], "MC_GG_ttn1OnShellt1_1700_1520_60_MGPyEG.root")
    mergeFiles(directory, ["user.*.370524.MGPy8EG_A14N_GG_ttn1OnShellt1_1500_450_250*.root"], "MC_GG_ttn1OnShellt1_1500_450_250_MGPyEG.root")
    mergeFiles(directory, ["user.*.370529.MGPy8EG_A14N_GG_ttn1OnShellt1_1500_1320_250*.root"], "MC_GG_ttn1OnShellt1_1500_1320_250_MGPyEG.root")

    # Data
    mergeFiles(directory, ["user.*.physics_Main.*.root"], "DATA_physics_Main.root")

print "DONE"
