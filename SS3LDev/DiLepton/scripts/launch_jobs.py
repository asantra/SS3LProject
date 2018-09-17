import os
import re
import sys
import commands
import time
from glob import glob
from argparse import ArgumentParser

parser = ArgumentParser(description="Script for mutiple grid job submission")
parser.add_argument("--DQ2",      help="List of datasets to submitt", default="none")
parser.add_argument("--Submitt",  type=int, help="Launch submission command [1/0]", default=1)
parser.add_argument("--Suffix",   help="Suffix for ouput files", default="")
parser.add_argument("--doSys",    type=int, help="Enables systematic variantions [0/1]", default=0)
parser.add_argument("--Trigger",  help="Trigger application. Default dilepton+EtMiss",   default="DILEPTON_MET")
parser.add_argument("--SysList",  help="Comma separated systematic variations", default="")
parser.add_argument("--makeHist", type=int, help="Add Histos to output [0/1]",   default=0)
parser.add_argument("--makeTree", type=int, help="Add TTree to output  [1/0/2]", default=1) 
parser.add_argument("--R1Jets",   type=int, help="Add R1Jets to output [No=0/FatJets=1/RecJets=2]", default=0)
parser.add_argument("--setLeptons",     type=int, help="lepton merging option", default=0)
parser.add_argument("--NSignalLeptons", type=int, help="How many signal leptons are needed", default=3)
parser.add_argument("--SignalRegion",   help="Process only one Signal Region", default="SRall")
options = parser.parse_args()

if options.DQ2 == "none":
    print "please specify a DQ2 file"; sys.exit(1)

if not os.environ.get('BASEDIR'):
    print "BASEDIR is not set. Please set it"; sys.exit(1)

Suffix = "_{0}{1}".format(time.strftime("%d%H%M%S"), options.Suffix)

os.chdir(os.environ["BASEDIR"]+"/DiLepton/scripts")
table = "datasets/{0}".format(options.DQ2)
print "Looking for datasets in {0}".format(table)

if not len(table) or not os.path.exists(table):
    print "Please specify a DQ2 file"; sys.exit(1)

NJobs = 0
for line in open(table):
    dataset = line.split()[0]
    if dataset[0]=="#": continue
    if dataset.find(":")>-1: dataset = dataset.split(":")[1]
    MCId = dataset.split(".")[1]    
    isData     = 0
    isAtlfast  = 0
    isMC15     = 0
    if dataset.find("data")>-1: isData = 1
    if re.search("_a\d{3,4}_", dataset): isAtlfast = 1
    if not isData: 
        if (dataset.find("a810")>-1 or dataset.find("r7326")>-1 or dataset.find("r7267")>-1 or dataset.find("r7360")>-1): isMC15 = 2
        if (dataset.find("p26")>-1 or dataset.find("p27")>-1 or dataset.find("p28")>-1 or dataset.find("p29")>-1): isMC15 = 3 
        
    command = "python run.py"\
        " --inputDS={0} --isData={1} --isAtlfast={2} --isMC15={3} --doSys={4}"\
        " --Trigger={5} --SysList={6} --makeHist={7} --makeTree={8} --R1Jets={9}"\
        " --setLeptons={10} --NSignalLeptons={11} --SignalRegion={12} --suffix={13} --driver=GRID".format(dataset, isData, isAtlfast, isMC15, options.doSys, options.Trigger, 
                                                                                                          options.SysList, options.makeHist, options.makeTree, options.R1Jets, 
                                                                                                          options.setLeptons, options.NSignalLeptons, options.SignalRegion, Suffix)
    print command

    if options.Submitt: 
        os.system(command)
        NJobs = NJobs+1
        time.sleep(1)
            
print
time.sleep(1)

if options.Submitt: 
    print "... submitted {0} jobs with suffix {1}".format(NJobs, Suffix)

print "...done"    
