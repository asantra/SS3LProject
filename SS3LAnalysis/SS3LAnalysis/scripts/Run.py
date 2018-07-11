import ROOT
import logging
import datetime
from glob import glob

logging.basicConfig(level=logging.INFO)
from optparse import OptionParser

now = datetime.datetime.now()
tag="Rel21_AB21p2p35_v0"
MyWorkDir ='/home/oducu/Rel21/SS3Lep/OUTPUTFILES/' + tag + '/'

parser = OptionParser()
parser.add_option("--submitDir", help="dir to store the output", default="submit_dir")
parser.add_option("--inputDS", help="input DS from DQ2", default="none")
parser.add_option("--outputDS", help="output DS name", default="default")
parser.add_option("--driver", help="select where to run", choices=("direct", "prooflite", "grid"), default="direct")
parser.add_option("--nevents", type=int, help="number of events to process for all the datasets")
parser.add_option("--skip-events", type=int, help="skip the first n events")
parser.add_option("-w", "--overwrite", action='store_true', default=True, help="overwrite previous submitDir")
parser.add_option("--isData", help="Data flag [0/1]", default=0)
parser.add_option("--isData15", help="Data15 flag [0/1]", default=0)
parser.add_option("--isData16", help="Data16 flag [0/1]", default=0)
parser.add_option("--isData17", help="Data17 flag [0/1]", default=0)
parser.add_option("--isFirstWeek", help="First week flag [0/1]", default=0)
#parser.add_option("--is50ns", help="50ns flag [0/1]", default=0)
parser.add_option("--isAtlfast", help="ATLFAST flag [0/1]", default=0)
parser.add_option("--ismc16a", help="mc16a flag [0/1]", default=0)
parser.add_option("--ismc16d", help="mc16d flag [0/1]", default=0)
parser.add_option("--doSyst", help="Create Trees with systemtic variations [0/1]", default=0)

(options, args) = parser.parse_args()

import atexit
@atexit.register
def quite_exit():
    ROOT.gSystem.Exit(0)

logging.info("loading packages")
# ROOT.gROOT.Macro("$ROOTCOREDIR/scripts/load_packages.C")

if options.overwrite:
    import shutil
    shutil.rmtree(options.submitDir, True)


#Set up the job for xAOD access:
ROOT.xAOD.Init().ignore();

# create a new sample handler to describe the data files we use
logging.info("creating new sample handler")
sh_all = ROOT.SH.SampleHandler()


if options.outputDS == "notDefault" :  # run on baaf or lxplus or ...
  search_directories = []
  print "isData: ", options.isData
  if int(options.isData)==1:
      search_directories = ("//data/oducu/DAOD_SUSY2/Rel21/data/p3372/",)
      name_directory     = "/data/oducu/DAOD_SUSY2/Rel21/data/p3372/"
  else:
      search_directories = ("/data/oducu/DAOD_SUSY2/Rel21/p3387_MC16a_4t/",)  # p3387_MC16a_4t Gtt_p3575
      name_directory     = "/data/oducu/DAOD_SUSY2/Rel21/p3387_MC16a_4t/"
      # search_directories = ("/home/oducu/DFSUSY_R21.2/run/WithEgammaBr_version_June6/",)  # p3387_MC16a_4t WithEgammaBr_version_June6 Initial
      # name_directory     = "/home/oducu/DFSUSY_R21.2/run/WithEgammaBr_version_June6/"
	  
  # scan for datasets in the given directories
  for directory in search_directories:
    ROOT.SH.scanDir(sh_all, directory)
    SampleName = options.inputDS # glob(name_directory+"/*")[-1]

  print "Search directories:", search_directories
  
  options.submitDir = MyWorkDir + 'TASK_' +now.strftime('%b%d_%Hh%Mm%Ss')
  
else :  # run on grid
  ROOT.SH.scanRucio (sh_all, options.inputDS);
  SampleName = options.inputDS
  name_directory = ""
  options.submitDir = 'TASK_' +now.strftime('%b%d_%Hh%Mm%Ss')
  
  
# print out the samples we found
logging.info("%d different datasets found scanning all directories", len(sh_all))


# SampleName = SampleName.split("/")[-1].split(".")[1] # [2] - to keep only the MC name
if int(options.isData)==1 and int(options.isData15)==1:
    SampleName = SampleName.replace(name_directory+'data15_13TeV:data15_13TeV.','user.oducu.data15_13TeV.'+tag+'.').replace('deriv.DAOD_SUSY2.','')
elif int(options.isData)==1 and int(options.isData16)==1:
    SampleName = SampleName.replace(name_directory+'data16_13TeV:data16_13TeV.','user.oducu.data16_13TeV.'+tag+'.').replace('deriv.DAOD_SUSY2.','')
elif int(options.isData)==1 and int(options.isData17)==1:
    SampleName = SampleName.replace(name_directory+'data17_13TeV:data17_13TeV.','user.oducu.data17_13TeV.'+tag+'.').replace('deriv.DAOD_SUSY2.','')
else:
    SampleName = SampleName.replace(name_directory+'mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.').replace('deriv.DAOD_SUSY2.','')
 
if options.outputDS == "default": 
   options.outputDS = SampleName
    
logging.info("Sample Name: %s", SampleName)


# set the name of the tree in our files
sh_all.setMetaString("nc_tree", "CollectionTree")

sh_all.setMetaString("nc_grid_filter", "*")

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


# add our algorithm to the job
logging.info("creating algorithms")
alg = ROOT.MyxAODAnalysis()
print "isData?",options.isData
print "isData15?",options.isData15
print "isData16?",options.isData16
print "isData17?",options.isData17
print "isAF?",options.isAtlfast
print "ismc16a?",options.ismc16a
print "ismc16d?",options.ismc16d

logging.info("adding data")
alg.isData = int(options.isData)
alg.isData15 = int(options.isData15)
alg.isData16 = int(options.isData16)
alg.isData17 = int(options.isData17)
logging.info("adding AF")
alg.isAtlfast = int(options.isAtlfast)
logging.info("adding mc16a")
alg.ismc16a = int(options.ismc16a )
alg.ismc16d = int(options.ismc16d )
alg.doSyst = options.doSyst
alg.inputDS = options.inputDS

logging.info("adding algorithms")
job.algsAdd(alg)

# make the driver we want to use:
# this one works by running the algorithm directly
logging.info("creating driver")
driver = None
if (options.driver == "direct"):
    logging.info("running on direct")
    driver = ROOT.EL.DirectDriver()
    logging.info("submit job")
    driver.submit(job, options.submitDir)
    #print "submit dir:", options.submitDir
    logging.info("done submit job")
    logging.info("outputDS %s", options.outputDS)
    driver.options().setString("nc_outputSampleName", options.outputDS)
    job.options().setString(ROOT.EL.Job.optXaodAccessMode, ROOT.EL.Job.optXaodAccessMode_athena); 
elif (options.driver == "prooflite"):
    logging.info("running on prooflite")
    driver = ROOT.EL.ProofDriver()
    driver.numWorkers = 4;
    job.options().setString(ROOT.EL.Job.optXaodAccessMode, ROOT.EL.Job.optXaodAccessMode_athena); 
    logging.info("submit job")
    driver.submit(job, options.submitDir)
elif (options.driver == "grid"):
    logging.info("running on Grid") 
    driver = ROOT.EL.PrunDriver()   
    #driver.options().setString("nc_outputSampleName", "user.jpoveda.t0607_v07.%in:name[2]%.%in:name[3].%in:name[6]%")
    driver.options().setString("nc_outputSampleName", options.outputDS)
    # driver.options().setString("nc_outputSampleName", "user.jpoveda.pilot.test")
    driver.options().setDouble("nc_disableAutoRetry", 1)
    # driver.options().setDouble("nc_nGBPerJob", 5)
    # driver.options().setString("nc_nGBPerJob", "5")
    driver.options().setString (ROOT.EL.Job.optSubmitFlags, "--forceStaged");
    # if int(options.isData)==1:
       # driver.options().setDouble("nc_nFilesPerJob", 5)
       # driver.options().setString("nc_nFilesPerJob", "5")
    driver.options().setDouble("nc_nFilesPerJob", 5)
    driver.options().setString("nc_nFilesPerJob", "5")
    job.options().setString(ROOT.EL.Job.optXaodAccessMode, ROOT.EL.Job.optXaodAccessMode_athena);
    # If willing to specify an output Storage element for your job
    # driver.options().setString("nc_destSE", "CERN-PROD_LOCALGROUPDISK")
    # If willing to veto certain sites
    driver.options().setString('nc_excludedSite','ANALY_SiGNET,ANALY_RAL_SL6,ANALY_PIC_SL6,ANALY_BU_ATLAS_Tier2_SL6')    
    logging.info("submit job")
    driver.submitOnly(job, options.submitDir)
    logging.info("done submit job")


