import os
import sys
import ROOT
import shutil
import logging
import commands
from glob import glob
from argparse import ArgumentParser

logging.basicConfig(level=logging.INFO)

#LocalSample = "Data"
#InPath = "/afs/cern.ch/work/p/ptornamb/SS3L-Full/Samples/"
LocalSample = "mc16_13TeV"
InPath = "/lustre/scratch/epp/atlas/na337/workdir/Analysis2018/SS3L/Framework/Rel21-2-v35/DiLeptonRel21/"

if not os.environ.get('BASEDIR'):
    print "BASEDIR is not set. Please set it"; sys.exit(1)

LocalInputDir = "{0}/{1}".format(InPath,LocalSample)
if not os.path.exists(LocalInputDir):
    print "LocalInputDir does not exist, aborting"; sys.exit(1)

LocalOutputDir = "{0}/Output".format(os.environ['BASEDIR'])
if not os.path.exists(LocalOutputDir):
    print "LocalOutputDir does not exist, creating"; os.makedirs(LocalOutputDir)

logging.info("LocalInputDir  %s", LocalInputDir)
logging.info("LocalOutputDir %s", LocalOutputDir)

parser = ArgumentParser(description="Runs job locally or creates prun command for grid submission")
parser.add_argument("--submitDir", help="dir to store the output", default=LocalOutputDir)
parser.add_argument("--inputDS",   help="input DS from DQ2",       default="none")
#parser.add_argument("--driver",    help="select where to run",     choices=("LOCAL", "PROOF", "GRID"), default="GRID")
parser.add_argument("--driver",    help="select where to run",     choices=("LOCAL", "PROOF", "GRID"), default="LOCAL")
parser.add_argument("--Debug",       type=int, help="Print debug messages", default=0)
parser.add_argument("--nevents",     type=int, help="number of events to process for all the datasets")
parser.add_argument("--skip-events", type=int, help="skip the first n events")
parser.add_argument("--overwrite", action='store_true', default=True, help="overwrite previous submitDir")
parser.add_argument("--isData",    type=int, help="Data flag [0/1]",    default=0)
parser.add_argument("--isAtlfast", type=int, help="ATLFAST flag [0/1]", default=0)
parser.add_argument("--isMC15",    type=int, help="mc15 configuration of prw [A=1/B=2/C=3/D=4]", default=0)
parser.add_argument("--isMC16",    type=int, help="mc16 configuration of prw [2015/2016/2017/2018]", default=2016)
parser.add_argument("--makeHist",  type=int, help="Add Histos to output  [0/1]", default=0)
parser.add_argument("--makeTree",  type=int, help="Add TTree to output [1/0/2]", default=1)
parser.add_argument("--R1Jets",    type=int, help="Add R1Jets to output[No=0/FatJets=1/RecJets=2]",default=0)
parser.add_argument("--setLeptons",type=int, help="Leptons for SS 0=Leptons, 1=Muons, 2=Electrons", default=0)
parser.add_argument("--doSys",     type=int, help="Enables systematic variantions", default=0)
parser.add_argument("--Trigger",   help="Set trigger name",  default="DILEPTON_MET")
parser.add_argument("--SysList",   help="Comma-separated list of systematics", default="")
#parser.add_argument("--SignalRegion", help="Comma-separated list of Signal Regions", default="SRall,SR1b1,SR1b2")
parser.add_argument("--SignalRegion", help="Comma-separated list of Signal Regions", default="SRall")
parser.add_argument("--NSignalLeptons", type=int, help="How many leptons have to be signal type", default=3)
parser.add_argument("--PileupReweighting", type=int, help="Do PileupReweighting for MC [1/0]", default=1)
parser.add_argument("--useSite", help="Site name where jobs are sent", default="")
parser.add_argument("--excludeSites", help="Blacklist analysis sites", default="")
parser.add_argument("--nFilesPerJob", type=int, help="Number of files processed per job", default=0)
parser.add_argument("--suffix",  help="suffix for output", default="")
options = parser.parse_args()

import atexit
@atexit.register
def quite_exit():
    ROOT.gSystem.Exit(0)

#logging.info("loading packages")
ROOT.gROOT.Macro("$ROOTCOREDIR/scripts/load_packages.C")

if options.overwrite:
    logging.info("overwrite directory %s", options.submitDir)
    shutil.rmtree(options.submitDir, True)

prefix = "Oct1"
#f= open("download.txt","a")
#f.write("------------------------------------------------------------")

#Set up the job for xAOD access:
ROOT.xAOD.Init().ignore();

# create a new sample handler to describe the data files we use
logging.info("creating new sample handler")
sh_all = ROOT.SH.SampleHandler()

if options.inputDS != "none":
  ROOT.SH.scanDQ2 (sh_all, options.inputDS);

  current=-1
  for z in range(1,5):
      current = (options.inputDS).find(".",current+1)
      if z==1: first = current+1
      if z==3:last = current
  short_name = options.inputDS[first:last]
  print "Short Name: ", short_name
  if short_name.find("*")>-1: short_name = short_name.replace("*","")
  logging.info("Outname %s", short_name)
  SampleName = options.inputDS
  #print "SampleName: ", SampleName

else :
  search_directories = []
  search_directories = (LocalInputDir,)

  # scan for datasets in the given directories
  short_name = "output"
  for directory in search_directories:
    ROOT.SH.scanDir(sh_all, directory)
    SampleName = glob(LocalInputDir+"/*")[-1]


# print out the samples we found
logging.info("%d different datasets found scanning all directories", len(sh_all))

runnumber  = SampleName.split("/")[-1].split(".")[1]
mc16v      = SampleName.split("/")[-1].split(".")[5]
SampleName = SampleName.split("/")[-1].split(".")[2]

logging.info("Sample Name: %s", SampleName)
logging.info("Run Number: %s", runnumber)
logging.info("MC16 version: %s", mc16v)

#downloadname = "user.nabraham.{0}_{1}.{2}.{3}/".format(prefix,runnumber,SampleName,mc16v)
#f.write(downloadname+"\n")

# set the name of the tree in our files
sh_all.setMetaString("nc_tree", "CollectionTree")

# this is the basic description of our job
logging.info("creating new job")
job = ROOT.EL.Job()
job.sampleHandler(sh_all)

if options.nevents:
    logging.info("processing only %d events", options.nevents)
    job.options().setDouble(ROOT.EL.Job.optMaxEvents, options.nevents)

if options.skip_events:
    logging.info("skipping first %d events", options.skip_events)
    job.options().setDouble(ROOT.EL.Job.optSkipEvents, options.skip_events)

if len(options.useSite):
    logging.info("send jobs to %s", options.useSite)
    job.options().setString(ROOT.EL.Job.optGridSite, options.useSite)

if len(options.excludeSites):
    logging.info("exclude sites %s", options.excludeSites)
    job.options().setString(ROOT.EL.Job.optGridExcludedSite, options.excludeSites)

if options.nFilesPerJob:
    logging.info("%i files per job",options.nFilesPerJob)
    job.options().setDouble(ROOT.EL.Job.optGridNFilesPerJob, options.nFilesPerJob)
    job.options().setDouble(ROOT.EL.Job.optGridMaxNFilesPerJob, options.nFilesPerJob)


# add our algorithm to the job
logging.info("creating algorithms")
alg = ROOT.DiLepton()
alg.Debug = options.Debug
alg.isData = options.isData
alg.isAtlfast = options.isAtlfast
alg.SysList = options.SysList
alg.NSignalLeptons = options.NSignalLeptons;
alg.doSys = options.doSys
alg.mc15 = options.isMC15
alg.mc16 = options.isMC16
alg.makeHist = options.makeHist
alg.makeTree = options.makeTree
alg.getR1Jets = options.R1Jets
alg.setLeptons = options.setLeptons
alg.SampleName = SampleName
alg.TriggerName = options.Trigger
alg.SigRegion = options.SignalRegion
alg.PileupReweighting = options.PileupReweighting

logging.info("adding algorithms")
job.algsAdd(alg)

# make the driver we want to use:
logging.info("creating driver")
driver = None
if (options.driver == "LOCAL"):
    logging.info("running on direct")
    driver = ROOT.EL.DirectDriver()
    logging.info("submit job")
    driver.submit(job, options.submitDir)
elif (options.driver == "PROOF"):
    logging.info("running on prooflite")
    driver = ROOT.EL.ProofDriver()
    logging.info("submit job")
    driver.submit(job, options.submitDir)
elif (options.driver == "GRID"):
    logging.info("running on Grid")
    driver = ROOT.EL.PrunDriver()
    command = "voms-proxy-info -all | grep nickname"
    output = commands.getoutput(command)
    cernname = output.split()[4]
    logging.info("CERN user = %s", cernname)
    outname = "user.{0}.{1}_{2}.{3}.{4}/".format(cernname,prefix,runnumber,SampleName,mc16v)
    logging.info("--OutDS = %s", outname)
    driver.options().setString("nc_outputSampleName", outname)
    driver.options().setDouble("nc_disableAutoRetry", 1)
    logging.info("submit job")
    driver.submitOnly(job, options.submitDir)

if(options.driver == "LOCAL"):
    logging.info("sent output to %s", LocalOutputDir)

#f.close()
print "...done"
