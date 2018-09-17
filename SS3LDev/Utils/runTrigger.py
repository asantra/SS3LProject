import os
import sys
import commands
import time
import shutil
from glob import glob
import ROOT 
from optparse import OptionParser

##### simple helper to process triggers ####

def main():

    parser = OptionParser()
    parser.add_option("--TestEv", type=int, help="test run on limited events",  default=-1)
    parser.add_option("--Debug",  type=int, help="print debug messages", default=0)
    (options, args) = parser.parse_args()

    filename = "valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.AOD.e3099_s2578_r6630"
    print "Starting run on file", filename
    if filename.find(".root")<0: filename = "hist-"+filename+".root"
   
    inpath = "/home/fc12/Analysis_xAOD/Output/"
    outpath = "/home/fc12/Analysis_xAOD/Files/TriggerValidation/28-05-2015/ttbar/noMET/"
    print "All output is send to", outpath      
    logfile = "out.log"
     
    path = "/home/fc12/Analysis_xAOD/DiLepton/scripts/"
    os.chdir(path)
    for trigger in TriggerListNEW():
        print "-- using trigger", trigger

        trigName = ""
        if not trigger=="NONE": trigName = trigger

        command = "python run.py --Trigger=%(TRIG)s --Debug=%(DEBUG)i &>> /dev/null" % {"TRIG":trigName, "DEBUG":options.Debug}
        if options.TestEv>-1: command = "python run.py --nevents=%(NEV)i --Trigger=%(TRIG)s --Debug=%(DEBUG)i &>> %(LOG)s" % {"NEV":options.TestEv, "TRIG":trigName, "DEBUG":options.Debug, "LOG":logfile}
        print command

        os.system(command)
        time.sleep(1)
        checkFile(inpath+filename)
        
        if os.path.isdir(outpath+trigger):
            print "-- removing old ", outpath+trigger
            shutil.rmtree(outpath+trigger)

        os.mkdir(outpath+trigger)
        shutil.move(inpath+filename, outpath+trigger)
        print "-- moving output", inpath+filename, "\n-->",outpath+trigger+"/"+filename
        print
        time.sleep(1)

    if os.path.isfile(outpath+logfile): os.remove(outpath+logfile)
    os.system("echo FINISHED >> out.log") 

    shutil.move(logfile, outpath)
    time.sleep(1)
    print
    print "-- finished all",len(TriggerListNEW()),"runs. Output dir: ",outpath 
    

def TriggerList():

    triggers = ["NONE",
                "ALL",
                "MUON",
                "ELECTRON",
                "MET",
                "HLT_mu18",
                "HLT_mu26",
                "HLT_mu50",
                "HLT_mu24_imedium",
                "HLT_mu26_imedium", 
                "HLT_2mu10",
                "HLT_2mu14", 
                "HLT_3mu6",
                "HLT_mu18_mu8noL1",
                "HLT_mu18_2mu4noL1",
                "HLT_mu24_mu8noL1",  
                "HLT_e20_medium",
                "HLT_e60_medium",         
                "HLT_2e12_loose_L12EM10VH",
                "HLT_2e17_loose",
                "HLT_e24_tight_iloose", 
                "HLT_e26_tight_iloose",      
                "HLT_e17_loose_mu14",
                "HLT_e15_lhvloose_L1EM7",
                "HLT_e20_lhvloose_L1EM12",
                "HLT_e30_lhvloose_L1EM15",
                "HLT_e5_lhtight",
                "HLT_e3_lhloose",
                "HLT_e14_lhtight",
                "HLT_e20_lhmedium",
                "HLT_e60_lhmedium",
                "HLT_e24_lhmedium_L1EM18VH",
                "HLT_e17_lhmedium_iloose",
                "HLT_e24_lhtight_iloose",
                "HLT_e26_lhtight_iloose",
                "HLT_2e12_lhloose_L12EM10VH",
                "HLT_2e17_lhloose",
                "HLT_e12_lhloose_2mu10",
                "HLT_e12_lhmedium_2mu10",
                "HLT_e17_lhloose_2e9_lhloose", 
                "HLT_e12_loose_2mu10",
                "HLT_xe60",
                "HLT_xe70",
                "HLT_xe80",
                "HLT_xe100",
                "HLT_j80_xe80",
                "HLT_j100_xe80"]

    return triggers


def TriggerListNEW():

    triggers = ["NONE",
                "DILEPTON",
                "DILEPTON_MET",
                "DILEPTON_MET_SLEPTON"]

    return triggers



def checkFile(filename):

    f = ROOT.TFile.Open(filename)
    if not f:
        print filename, " cannot be opened"
        sys.exit(0)
    return


if __name__ == "__main__":
    main()
