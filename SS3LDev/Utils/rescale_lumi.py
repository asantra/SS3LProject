import os
import sys
import shutil
from glob import glob
from ROOT import *
from optparse import OptionParser

gROOT.ProcessLine( "gErrorIgnoreLevel = 3001;")
debug = False

parser = OptionParser()
parser.add_option("--xSecFile", type="string", help="File with crosssections",   default="susy_crosssections_13TeV.txt")
parser.add_option("--Suffix",   type="string", help="Suffix for output files",   default="scaled")
parser.add_option("--Lumi",     type=float,    help="Luminosity to be rescaled", default=3000.)
(options, args) = parser.parse_args()

if len(sys.argv) < 2:
    print "Please specify a directory"
    sys.exit(1)

directory = sys.argv[1]
if len(directory) == 0:
    print "Please specify a directory"
    sys.exit(1)
if not os.path.exists(directory):
    print "Directory",directory,"does not exist"
    sys.exit(1)

if not os.environ.get('SUSYDIR'):
    print "SUSYDIR is not set. Please set it"
    sys.exit(1)

print "--> Taking crosssections from", options.xSecFile
print

#histo with number of generated events
EventsHist = "hEventsTotal"

#removing old files
inputfilenames = glob(directory+"*.root")
for inputfilename in inputfilenames:
    if inputfilename.split("/")[-1].find(options.Suffix)>-1:
        print "-- Removing old", inputfilename
        os.remove(inputfilename)


print
print "\033[1m"+"-- Found",len(inputfilenames),"files"+"\033[0m"
if not os.path.exists(directory+"scaled"): os.makedirs(directory+"scaled")

inputfilenames = glob(directory+"*.root")
for inputfilename in inputfilenames:

    if inputfilename.find("physics")>-1:
        print "-- Not rescaling",inputfilename," ==> is data file"
        print
        continue   

    isScaled = True    
    # Open input and output files
    inputfilebackup = TFile(inputfilename)
    outputfilename = inputfilename.replace('.root', "_"+options.Suffix+".root")
    outputfile = TFile(outputfilename, 'RECREATE')

    print "-- Reading", inputfilename
    print "-- Writing", outputfilename

    # Calculate scale factor
    mcid = int(inputfilename.split("/")[-1].split(".")[2])

    # number of weighted events
    histnbevents = TH1F( inputfilebackup.Get(EventsHist) )
    nbevents = float(histnbevents.GetBinContent( histnbevents.GetXaxis().FindBin(1.) ))
    if nbevents<=0: 
        print "\033[91m"+"ERROR \t "+EventsHist+" is empty "+"\033[0m"
        sys.exit(1)

    # Calculate scaling factor        
    luminosity = options.Lumi
    xsection = -1.

    # Xsections from SUSYTools
    for line in open(os.environ['SUSYDIR']+"/data/"+options.xSecFile):
        if line.strip()=="": continue
        elif line.strip()[0] == '#': continue
        elif str(mcid) in line:
            ID = line.split()[0]
            name = line.split()[1]
            xSection = float(line.split()[2])
            kFactor = float(line.split()[3])
            Eff = float(line.split()[4])
            print "process ", name," with id:",ID," has xSection:",xSection," kfac:", kFactor," and eff:", Eff
            xsection = xSection * kFactor * Eff
            break

    if xsection < 0:
        print "\033[91m"+"Error scaling",inputfilename+"\033[0m"
        print "\033[91m"+"X-Section for mcid",mcid,"unknown"+"\033[0m"
            
        #moving files with missing crosssection to dir unscaled 
        if not os.path.exists(directory+"unscaled"): os.makedirs(directory+"unscaled")
        newname = os.path.join(directory+"unscaled", os.path.basename(inputfilename))
        print "    Moving file",inputfilename,"to",newname
        os.rename(inputfilename,newname)
        isScaled = False

    scale = luminosity / nbevents * xsection

    if isScaled:
        print "   sf = Xsec * L/sum(w*N)"; 
        print "   \033[1m"+"Xsec:",xsection,"nbevents:",nbevents,"luminosity",luminosity," :: Scale:",scale,"\033[0m"
        print
                
        # Loop over input histograms
        inputfilebackup.cd()
        histkeysiter = TIter(gDirectory.GetListOfKeys())
        histkey = histkeysiter.Next()
        while histkey != None:
            myhist = histkey.ReadObj()
            histname = myhist.GetName()
            
            if(debug): print histname
            if histname.find("EventLoop")>-1: histkey = histkeysiter.Next()
            if not myhist.IsA().InheritsFrom("TH1"): continue 
 
            if not myhist.IsA().InheritsFrom("TH2"):
                if not 'heventstotal' in histname.lower():
                    myhist.Sumw2()
                    myhist.Scale(scale)
                    outputfile.cd()
                    myhist.Write()
            
            else:
                hx = myhist.ProjectionX()
                hy = myhist.ProjectionY()
                hx.SetName(myhist.GetName()+"_x")
                hy.SetName(myhist.GetName()+"_y")
                hx.Sumw2()
                hy.Sumw2()
                hx.Scale(scale)
                hy.Scale(scale)
                outputfile.cd()
                hx.Write()
                hy.Write()

            histkey = histkeysiter.Next()

    outputfile.Close()
    inputfilebackup.Close()
    
    if isScaled: shutil.move(outputfilename, directory+"scaled")
    else: os.remove(outputfilename)
    print

#Move data files to scaled directory
print "-- Copying datafiles to "+directory+"scaled"
datafiles = glob(directory+"*physics*.root")
for d in datafiles:
    shutil.copy2(d, directory+"scaled")

#move succesfully scaled files to backup directory
if not os.path.exists(directory+"backup"): os.makedirs(directory+"backup")
oldfiles = glob(directory+"*.root")
for f in oldfiles: 
    shutil.move(f, directory+"backup")
    
print "DONE"
