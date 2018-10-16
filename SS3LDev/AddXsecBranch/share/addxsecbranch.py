import os, sys
import re

import math

import subprocess

args=sys.argv

sample_list_name = args[1]

indir = args[2]

outdir = args[3]

suffix=""

if (len(sys.argv)>=5):
	suffix =args[4]

#sys.exit(1)

grid=""

if (len(args)>5):
    grid=args[5]

tempf=open('tmp.txt', 'w')

#f=file("samples_lists/"+sample_list_name)

f=file(sample_list_name)

currdir = os.getcwd()

for l in f:
    #line=re.split('\t',l)
    line=l.split()
    #print line
    if len(line)>1:
	    #print line[0]," ",line[1]," ",line[2]," ",line[3]," ",line[4]

	    sample = line[0]


    else:
	    sample= line[0]
	    tempf.write(currdir+"/"+indir+"/"+sample+suffix+"\t"+outdir+"/"+sample+suffix+"\n")
	    #tempf.write(currdir+"/"+indir+"/"+sample+suffix+".root\t"+outdir+"/"+sample+suffix+".root\n")

tempf.close()

cmdline = "AddXSecBranch "+os.getcwd()+"/tmp.txt"
#cmdline = "AddXsecBranch13TeV/ExportMyTree "+os.getcwd()+"/tmp.txt 2>/dev/null"
#cmdline = "AddXsecBranch13TeV/AddXsecBranch13TeV "+os.getcwd()+"/tmp.txt 2>/dev/null"

print os.getcwd()
print cmdline

subprocess.call(cmdline,shell=True)
