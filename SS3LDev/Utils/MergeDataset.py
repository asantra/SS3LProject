import os
import sys
import time
import commands
import shutil
from glob import glob
from argparse import ArgumentParser

parser = ArgumentParser(description="Script for merging root files in container")
parser.add_argument("InDir",    help="Directory with input files", default ="")
parser.add_argument("--Outdir", help="Directory with output files", default="Merged")
parser.add_argument("--Prefix", help="Container prefixs", default="user.")
parser.add_argument("--Suffix", help="Suffix for merged files", default="_merged")
options = parser.parse_args()

directory = options.InDir
if not len(directory):
    print "Please specify a directory"; sys.exit(1)

if not os.path.exists(directory):
    print "Directory {0} does not exist".format(directory); sys.exit(1)

# Make a new outdir or clean the old one
if len(glob(directory+"/"+options.Outdir+"/*")) > 0:
    print "\033[1m -- Cleaning {0}/{1} \033[0m".format(directory,options.Outdir) 
    shutil.rmtree(directory+"/"+options.Outdir, True)

os.mkdir(directory+"/"+options.Outdir)
print "\033[1m -- Created {0}/{1} \033[0m".format(directory,options.Outdir) 

print "\033[1m -- Searching in {0} for containers with {1}* \033[0m".format(directory,options.Prefix)
containers = sorted(glob(directory+"/"+options.Prefix+"*"))

for container in containers:
    print "\033[92m -- Looking in {0} \033[0m".format(container)
    Outname = str(container.replace(".root",""))+options.Suffix+".root"
    filename = str(container)+"/*.root*"
    filelist = glob(filename)
    if not len(filelist):
        print "no .root file found"; continue
    else: 
        print "--> found",len(filelist),".root files in",str(container)

    command = "hadd {0} {1}".format(Outname, filename)
    print command
    os.system(command)
    time.sleep(0.5)

    shutil.move(Outname, directory+"/"+options.Outdir)
    time.sleep(0.5)
    print

print "..done"
