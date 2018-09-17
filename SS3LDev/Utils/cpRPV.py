import os
import sys
import ROOT
import time
import shutil
import commands
from argparse import ArgumentParser

def main():
    parser = ArgumentParser(description="Creates copies of RPV signal files with artifical MCId") 
    parser.add_argument("dir",    help="Directory with input files", default ="")
    parser.add_argument("--copy", type=int, help="Copy files[1/0]", default=1)
    parser.add_argument("--pre", help="File prefix",  default="user.fcardill.")
    parser.add_argument("--suf", help="File suffix",  default="._15120316_V47_output_merged.root")
    options = parser.parse_args()
    hDir = os.getcwd()

    inDir =  options.dir
    if not len(inDir) or not os.path.exists(inDir):  
        print red("Please specify an input directory"); sys.exit(1)

    print green("Creating new samples in {0}".format(inDir))
    os.chdir(inDir)
    doCp = options.copy

    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998105.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998091.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998078.MGPy8EG_A14N23LO_DD_Rpvlampp331_1700_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998066.MGPy8EG_A14N23LO_DD_Rpvlampp331_1600_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998055.MGPy8EG_A14N23LO_DD_Rpvlampp331_1500_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998045.MGPy8EG_A14N23LO_DD_Rpvlampp331_1400_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998036.MGPy8EG_A14N23LO_DD_Rpvlampp331_1300_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998028.MGPy8EG_A14N23LO_DD_Rpvlampp331_1200_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998021.MGPy8EG_A14N23LO_DD_Rpvlampp331_1100_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998015.MGPy8EG_A14N23LO_DD_Rpvlampp331_1000_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998010.MGPy8EG_A14N23LO_DD_Rpvlampp331_900_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998006.MGPy8EG_A14N23LO_DD_Rpvlampp331_800_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998003.MGPy8EG_A14N23LO_DD_Rpvlampp331_700_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998001.MGPy8EG_A14N23LO_DD_Rpvlampp331_600_400"+options.suf, doCp) 
    cpFile(options.pre+"403395.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_400"+options.suf, options.pre+"998000.MGPy8EG_A14N23LO_DD_Rpvlampp331_500_400"+options.suf, doCp) 

    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998106.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_500"+options.suf, doCp) 
    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998092.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_500"+options.suf, doCp) 
    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998079.MGPy8EG_A14N23LO_DD_Rpvlampp331_1700_500"+options.suf, doCp) 
    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998067.MGPy8EG_A14N23LO_DD_Rpvlampp331_1600_500"+options.suf, doCp) 
    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998056.MGPy8EG_A14N23LO_DD_Rpvlampp331_1500_500"+options.suf, doCp) 
    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998046.MGPy8EG_A14N23LO_DD_Rpvlampp331_1400_500"+options.suf, doCp) 
    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998037.MGPy8EG_A14N23LO_DD_Rpvlampp331_1300_500"+options.suf, doCp) 
    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998029.MGPy8EG_A14N23LO_DD_Rpvlampp331_1200_500"+options.suf, doCp) 
    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998022.MGPy8EG_A14N23LO_DD_Rpvlampp331_1100_500"+options.suf, doCp) 
    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998016.MGPy8EG_A14N23LO_DD_Rpvlampp331_1000_500"+options.suf, doCp) 
    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998011.MGPy8EG_A14N23LO_DD_Rpvlampp331_900_500"+options.suf, doCp) 
    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998007.MGPy8EG_A14N23LO_DD_Rpvlampp331_800_500"+options.suf, doCp) 
    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998004.MGPy8EG_A14N23LO_DD_Rpvlampp331_700_500"+options.suf, doCp) 
    cpFile(options.pre+"403396.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_500"+options.suf, options.pre+"998002.MGPy8EG_A14N23LO_DD_Rpvlampp331_600_500"+options.suf, doCp) 

    cpFile(options.pre+"403397.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_600"+options.suf,  options.pre+"998107.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_600"+options.suf, doCp)
    cpFile(options.pre+"403397.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_600"+options.suf,  options.pre+"998093.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_600"+options.suf, doCp)
    cpFile(options.pre+"403397.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_600"+options.suf,  options.pre+"998080.MGPy8EG_A14N23LO_DD_Rpvlampp331_1700_600"+options.suf, doCp)
    cpFile(options.pre+"403397.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_600"+options.suf,  options.pre+"998068.MGPy8EG_A14N23LO_DD_Rpvlampp331_1600_600"+options.suf, doCp)
    cpFile(options.pre+"403397.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_600"+options.suf,  options.pre+"998057.MGPy8EG_A14N23LO_DD_Rpvlampp331_1500_600"+options.suf, doCp)
    cpFile(options.pre+"403397.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_600"+options.suf,  options.pre+"998047.MGPy8EG_A14N23LO_DD_Rpvlampp331_1400_600"+options.suf, doCp)
    cpFile(options.pre+"403397.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_600"+options.suf,  options.pre+"998038.MGPy8EG_A14N23LO_DD_Rpvlampp331_1300_600"+options.suf, doCp)
    cpFile(options.pre+"403397.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_600"+options.suf,  options.pre+"998030.MGPy8EG_A14N23LO_DD_Rpvlampp331_1200_600"+options.suf, doCp)
    cpFile(options.pre+"403397.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_600"+options.suf,  options.pre+"998023.MGPy8EG_A14N23LO_DD_Rpvlampp331_1100_600"+options.suf, doCp)
    cpFile(options.pre+"403397.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_600"+options.suf,  options.pre+"998017.MGPy8EG_A14N23LO_DD_Rpvlampp331_1000_600"+options.suf, doCp)
    cpFile(options.pre+"403397.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_600"+options.suf,  options.pre+"998012.MGPy8EG_A14N23LO_DD_Rpvlampp331_900_600"+options.suf, doCp)
    cpFile(options.pre+"403397.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_600"+options.suf,  options.pre+"998008.MGPy8EG_A14N23LO_DD_Rpvlampp331_800_600"+options.suf, doCp)
    cpFile(options.pre+"403397.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_600"+options.suf,  options.pre+"998005.MGPy8EG_A14N23LO_DD_Rpvlampp331_700_600"+options.suf, doCp)

    cpFile(options.pre+"403398.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_700"+options.suf, options.pre+"998108.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_700"+options.suf, doCp)
    cpFile(options.pre+"403398.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_700"+options.suf, options.pre+"998094.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_700"+options.suf, doCp) 
    cpFile(options.pre+"403398.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_700"+options.suf, options.pre+"998081.MGPy8EG_A14N23LO_DD_Rpvlampp331_1700_700"+options.suf, doCp) 
    cpFile(options.pre+"403398.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_700"+options.suf, options.pre+"998069.MGPy8EG_A14N23LO_DD_Rpvlampp331_1600_700"+options.suf, doCp) 
    cpFile(options.pre+"403398.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_700"+options.suf, options.pre+"998058.MGPy8EG_A14N23LO_DD_Rpvlampp331_1500_700"+options.suf, doCp) 
    cpFile(options.pre+"403398.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_700"+options.suf, options.pre+"998048.MGPy8EG_A14N23LO_DD_Rpvlampp331_1400_700"+options.suf, doCp) 
    cpFile(options.pre+"403398.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_700"+options.suf, options.pre+"998039.MGPy8EG_A14N23LO_DD_Rpvlampp331_1300_700"+options.suf, doCp) 
    cpFile(options.pre+"403398.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_700"+options.suf, options.pre+"998031.MGPy8EG_A14N23LO_DD_Rpvlampp331_1200_700"+options.suf, doCp) 
    cpFile(options.pre+"403398.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_700"+options.suf, options.pre+"998024.MGPy8EG_A14N23LO_DD_Rpvlampp331_1100_700"+options.suf, doCp) 
    cpFile(options.pre+"403398.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_700"+options.suf, options.pre+"998018.MGPy8EG_A14N23LO_DD_Rpvlampp331_1000_700"+options.suf, doCp) 
    cpFile(options.pre+"403398.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_700"+options.suf, options.pre+"998013.MGPy8EG_A14N23LO_DD_Rpvlampp331_900_700"+options.suf, doCp) 
    cpFile(options.pre+"403398.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_700"+options.suf, options.pre+"998009.MGPy8EG_A14N23LO_DD_Rpvlampp331_800_700"+options.suf, doCp)

    cpFile(options.pre+"403399.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_800"+options.suf, options.pre+"998109.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_800"+options.suf, doCp)
    cpFile(options.pre+"403399.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_800"+options.suf, options.pre+"998095.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_800"+options.suf, doCp)
    cpFile(options.pre+"403399.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_800"+options.suf, options.pre+"998082.MGPy8EG_A14N23LO_DD_Rpvlampp331_1700_800"+options.suf, doCp)
    cpFile(options.pre+"403399.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_800"+options.suf, options.pre+"998070.MGPy8EG_A14N23LO_DD_Rpvlampp331_1600_800"+options.suf, doCp)
    cpFile(options.pre+"403399.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_800"+options.suf, options.pre+"998059.MGPy8EG_A14N23LO_DD_Rpvlampp331_1500_800"+options.suf, doCp)
    cpFile(options.pre+"403399.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_800"+options.suf, options.pre+"998049.MGPy8EG_A14N23LO_DD_Rpvlampp331_1400_800"+options.suf, doCp)
    cpFile(options.pre+"403399.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_800"+options.suf, options.pre+"998040.MGPy8EG_A14N23LO_DD_Rpvlampp331_1300_800"+options.suf, doCp)
    cpFile(options.pre+"403399.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_800"+options.suf, options.pre+"998032.MGPy8EG_A14N23LO_DD_Rpvlampp331_1200_800"+options.suf, doCp)
    cpFile(options.pre+"403399.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_800"+options.suf, options.pre+"998025.MGPy8EG_A14N23LO_DD_Rpvlampp331_1100_800"+options.suf, doCp)
    cpFile(options.pre+"403399.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_800"+options.suf, options.pre+"998019.MGPy8EG_A14N23LO_DD_Rpvlampp331_1000_800"+options.suf, doCp)
    cpFile(options.pre+"403399.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_800"+options.suf, options.pre+"998014.MGPy8EG_A14N23LO_DD_Rpvlampp331_900_800"+options.suf, doCp)

    cpFile(options.pre+"403400.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_900"+options.suf, options.pre+"998110.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_900"+options.suf, doCp)
    cpFile(options.pre+"403400.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_900"+options.suf, options.pre+"998096.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_900"+options.suf, doCp)
    cpFile(options.pre+"403400.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_900"+options.suf, options.pre+"998083.MGPy8EG_A14N23LO_DD_Rpvlampp331_1700_900"+options.suf, doCp)
    cpFile(options.pre+"403400.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_900"+options.suf, options.pre+"998071.MGPy8EG_A14N23LO_DD_Rpvlampp331_1600_900"+options.suf, doCp)
    cpFile(options.pre+"403400.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_900"+options.suf, options.pre+"998060.MGPy8EG_A14N23LO_DD_Rpvlampp331_1500_900"+options.suf, doCp)
    cpFile(options.pre+"403400.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_900"+options.suf, options.pre+"998050.MGPy8EG_A14N23LO_DD_Rpvlampp331_1400_900"+options.suf, doCp)
    cpFile(options.pre+"403400.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_900"+options.suf, options.pre+"998041.MGPy8EG_A14N23LO_DD_Rpvlampp331_1300_900"+options.suf, doCp)
    cpFile(options.pre+"403400.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_900"+options.suf, options.pre+"998033.MGPy8EG_A14N23LO_DD_Rpvlampp331_1200_900"+options.suf, doCp)
    cpFile(options.pre+"403400.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_900"+options.suf, options.pre+"998026.MGPy8EG_A14N23LO_DD_Rpvlampp331_1100_900"+options.suf, doCp)
    cpFile(options.pre+"403400.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_900"+options.suf, options.pre+"998020.MGPy8EG_A14N23LO_DD_Rpvlampp331_1000_900"+options.suf, doCp)

    cpFile(options.pre+"403401.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1000"+options.suf, options.pre+"998111.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_1000"+options.suf, doCp)
    cpFile(options.pre+"403401.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1000"+options.suf, options.pre+"998097.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_1000"+options.suf, doCp)
    cpFile(options.pre+"403401.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1000"+options.suf, options.pre+"998084.MGPy8EG_A14N23LO_DD_Rpvlampp331_1700_1000"+options.suf, doCp)
    cpFile(options.pre+"403401.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1000"+options.suf, options.pre+"998072.MGPy8EG_A14N23LO_DD_Rpvlampp331_1600_1000"+options.suf, doCp)
    cpFile(options.pre+"403401.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1000"+options.suf, options.pre+"998061.MGPy8EG_A14N23LO_DD_Rpvlampp331_1500_1000"+options.suf, doCp)
    cpFile(options.pre+"403401.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1000"+options.suf, options.pre+"998051.MGPy8EG_A14N23LO_DD_Rpvlampp331_1400_1000"+options.suf, doCp)
    cpFile(options.pre+"403401.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1000"+options.suf, options.pre+"998042.MGPy8EG_A14N23LO_DD_Rpvlampp331_1300_1000"+options.suf, doCp)
    cpFile(options.pre+"403401.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1000"+options.suf, options.pre+"998034.MGPy8EG_A14N23LO_DD_Rpvlampp331_1200_1000"+options.suf, doCp)
    cpFile(options.pre+"403401.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1000"+options.suf, options.pre+"998027.MGPy8EG_A14N23LO_DD_Rpvlampp331_1100_1000"+options.suf, doCp)

    cpFile(options.pre+"403402.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1100"+options.suf, options.pre+"998112.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_1100"+options.suf, doCp)
    cpFile(options.pre+"403402.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1100"+options.suf, options.pre+"998098.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_1100"+options.suf, doCp)
    cpFile(options.pre+"403402.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1100"+options.suf, options.pre+"998085.MGPy8EG_A14N23LO_DD_Rpvlampp331_1700_1100"+options.suf, doCp)
    cpFile(options.pre+"403402.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1100"+options.suf, options.pre+"998073.MGPy8EG_A14N23LO_DD_Rpvlampp331_1600_1100"+options.suf, doCp)
    cpFile(options.pre+"403402.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1100"+options.suf, options.pre+"998062.MGPy8EG_A14N23LO_DD_Rpvlampp331_1500_1100"+options.suf, doCp)
    cpFile(options.pre+"403402.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1100"+options.suf, options.pre+"998052.MGPy8EG_A14N23LO_DD_Rpvlampp331_1400_1100"+options.suf, doCp)
    cpFile(options.pre+"403402.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1100"+options.suf, options.pre+"998043.MGPy8EG_A14N23LO_DD_Rpvlampp331_1300_1100"+options.suf, doCp)
    cpFile(options.pre+"403402.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1100"+options.suf, options.pre+"998035.MGPy8EG_A14N23LO_DD_Rpvlampp331_1200_1100"+options.suf, doCp)

    cpFile(options.pre+"403403.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1200"+options.suf, options.pre+"998113.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_1200"+options.suf, doCp)
    cpFile(options.pre+"403403.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1200"+options.suf, options.pre+"998099.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_1200"+options.suf, doCp)
    cpFile(options.pre+"403403.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1200"+options.suf, options.pre+"998086.MGPy8EG_A14N23LO_DD_Rpvlampp331_1700_1200"+options.suf, doCp)
    cpFile(options.pre+"403403.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1200"+options.suf, options.pre+"998074.MGPy8EG_A14N23LO_DD_Rpvlampp331_1600_1200"+options.suf, doCp)
    cpFile(options.pre+"403403.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1200"+options.suf, options.pre+"998063.MGPy8EG_A14N23LO_DD_Rpvlampp331_1500_1200"+options.suf, doCp)
    cpFile(options.pre+"403403.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1200"+options.suf, options.pre+"998053.MGPy8EG_A14N23LO_DD_Rpvlampp331_1400_1200"+options.suf, doCp)
    cpFile(options.pre+"403403.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1200"+options.suf, options.pre+"998044.MGPy8EG_A14N23LO_DD_Rpvlampp331_1300_1200"+options.suf, doCp)

    cpFile(options.pre+"403404.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1300"+options.suf, options.pre+"998114.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_1300"+options.suf, doCp)
    cpFile(options.pre+"403404.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1300"+options.suf, options.pre+"998100.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_1300"+options.suf, doCp)
    cpFile(options.pre+"403404.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1300"+options.suf, options.pre+"998087.MGPy8EG_A14N23LO_DD_Rpvlampp331_1700_1300"+options.suf, doCp)
    cpFile(options.pre+"403404.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1300"+options.suf, options.pre+"998075.MGPy8EG_A14N23LO_DD_Rpvlampp331_1600_1300"+options.suf, doCp)
    cpFile(options.pre+"403404.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1300"+options.suf, options.pre+"998064.MGPy8EG_A14N23LO_DD_Rpvlampp331_1500_1300"+options.suf, doCp)
    cpFile(options.pre+"403404.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1300"+options.suf, options.pre+"998054.MGPy8EG_A14N23LO_DD_Rpvlampp331_1400_1300"+options.suf, doCp)

    cpFile(options.pre+"403405.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1400"+options.suf, options.pre+"998115.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_1400"+options.suf, doCp)
    cpFile(options.pre+"403405.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1400"+options.suf, options.pre+"998101.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_1400"+options.suf, doCp)
    cpFile(options.pre+"403405.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1400"+options.suf, options.pre+"998088.MGPy8EG_A14N23LO_DD_Rpvlampp331_1700_1400"+options.suf, doCp)
    cpFile(options.pre+"403405.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1400"+options.suf, options.pre+"998076.MGPy8EG_A14N23LO_DD_Rpvlampp331_1600_1400"+options.suf, doCp)
    cpFile(options.pre+"403405.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1400"+options.suf, options.pre+"998065.MGPy8EG_A14N23LO_DD_Rpvlampp331_1500_1400"+options.suf, doCp)

    cpFile(options.pre+"403406.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1500"+options.suf, options.pre+"998116.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_1500"+options.suf, doCp)
    cpFile(options.pre+"403406.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1500"+options.suf, options.pre+"998102.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_1500"+options.suf, doCp)
    cpFile(options.pre+"403406.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1500"+options.suf, options.pre+"998089.MGPy8EG_A14N23LO_DD_Rpvlampp331_1700_1500"+options.suf, doCp)
    cpFile(options.pre+"403406.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1500"+options.suf, options.pre+"998077.MGPy8EG_A14N23LO_DD_Rpvlampp331_1600_1500"+options.suf, doCp)

    cpFile(options.pre+"403407.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1600"+options.suf, options.pre+"998117.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_1600"+options.suf, doCp)
    cpFile(options.pre+"403407.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1600"+options.suf, options.pre+"998103.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_1600"+options.suf, doCp)
    cpFile(options.pre+"403407.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1600"+options.suf, options.pre+"998090.MGPy8EG_A14N23LO_DD_Rpvlampp331_1700_1600"+options.suf, doCp)

    cpFile(options.pre+"403408.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1700"+options.suf, options.pre+"998118.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_1700"+options.suf, doCp) 
    cpFile(options.pre+"403408.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1700"+options.suf, options.pre+"998104.MGPy8EG_A14N23LO_DD_Rpvlampp331_1800_1700"+options.suf, doCp) 

    cpFile(options.pre+"403409.MGPy8EG_A14N23LO_DD_Rpvlampp331_2000_1800"+options.suf, options.pre+"998119.MGPy8EG_A14N23LO_DD_Rpvlampp331_1900_1800"+options.suf, doCp)

    os.chdir(hDir)
    return



def cpFile(inFile, outFile, doCopy):
    if not os.path.exists(inFile): 
        print red("ERROR {0} does not exist".format(inFile)); return
 
    print "Copy {0} --> {1}".format(inFile,outFile)
    if doCopy: 
        shutil.copy(inFile, outFile)   
        if not os.path.exists(outFile): print red("ERROR {0} could not be copied".format(outFile))
    return


def green(msg): return "\033[0;32m{0}\033[0m".format(msg);
def red(msg): return "\033[0;31m{0}\033[0m".format(msg); 
def bold(msg): return "\033[1m{0}\033[0m".format(msg);


if __name__ == "__main__": 
    main()
