import os
import sys
import ROOT
import shutil
from glob import glob


def main():

    if len(sys.argv) < 2:
        print "Please specify a search string"; sys.exit(1)

    searchstring = sys.argv[1]
    if not len(searchstring):
        print "Please specify a search string"; sys.exit(1)
    
    if not searchstring.find("evgen.EVNT") > -1:
        print "Please use evgen.EVNT files for search"; sys.exit(1)

    print "\033[1m -- Searching for {0} \033[0m".format(searchstring)

    if os.path.exists("ami.txt"): 
        print "-- removing old ami.txt"; os.remove("ami.txt");

    filename = searchstring.split(".")[2]
    query = "rucio list-dids {0} | sort | grep mc15_13TeV > {1}.txt".format(searchstring, filename)
    os.system(query)

    text_file = open(filename+".txt","rw")
    processnames = []
    processIDs = [] 

    for line in text_file:
        processname = line.split(".")[2]
        processID = line.split(".")[1]
        processnames.append(processname)
        processIDs.append(processID)

        inline = line.split(" ")[1]
        inline_new = inline.replace("mc15_13TeV:","ami show dataset info ")
        command = inline_new+" >> ami.txt"
        print command
        os.system(command)

    if os.path.exists(filename+"_table.txt"): 
        print "removing old",filename+"_table.txt"
        os.remove(filename+"_table.txt")

    end_file = open(filename + "_table.txt","wr")

    sections = []; eff = [];
    kfac = 1.0
    uncUP   = []; uncDOWN = [];

    getCrosssections     = "grep crossSection_mean  ami.txt > crosssections.txt"
    getEfficiencies      = "grep GenFiltEff_mean ami.txt > efficiencies.txt"
    getCrosssectionsUp   = "grep crossSection_max ami.txt > crosssectionsUp.txt"
    getCrosssectionsDown = "grep crossSection_min ami.txt > crosssectionsDown.txt"
    
    os.system(getCrosssections)
    os.system(getEfficiencies)
    os.system(getCrosssectionsUp)
    os.system(getCrosssectionsDown)

    crosssections     = open("crosssections.txt","rw")
    efficiencies      = open("efficiencies.txt","rw")
    crosssectionsUp   = open("crosssectionsUp.txt","rw")
    crosssectionsDown = open("crosssectionsDown.txt","rw")

    for line in crosssections:
        section = float(line.split(": ")[1])*1000
        sections.append(section)

    for line in efficiencies:
        efficiency = float(line.split(": ")[1])
        eff.append(efficiency)

    for line in crosssectionsUp:
        XsecUp = float(line.split(": ")[1])
        uncUP.append(XsecUp)

    for line in crosssectionsDown:
        XsecDown = float(line.split(": ")[1])
        uncDOWN.append(XsecDown)

    for i in range(len(sections)):
        txtline = str(processIDs[i])+"\t"+str(processnames[i])+"\t"\
            "%.8f" %round(sections[i],8)+"\t"+"%.5f" %kfac+" \t"\
            "%.5f" %round(eff[i],5)+" \t"+"%.5f" %RelUnc(uncUP[i],uncDOWN[i])+"\n"
        end_file.write(txtline)

    os.remove(filename+".txt")
    os.remove("crosssections.txt")
    os.remove("efficiencies.txt")
    os.remove("crosssectionsUp.txt")
    os.remove("crosssectionsDown.txt")

    end_file.close()
    print "\033[1m"+"-- Created",filename+"_table.txt with "+str(len(sections))+" entries"+"\033[0m"
    
    shutil.move(filename+"_table.txt","crosssections/"+filename+"_table.txt")

    if os.path.exists("ami.txt"): os.remove("ami.txt")

    return True

def RelUnc(up, down):
    unc = (up-down)/(up+down)
    return abs(unc)

if __name__ == "__main__":
    main()

