#Assume you are in the project folder (SS3LAnalysis)
cd ..
setupATLAS
echo 'in dir ' $PWD 

if [ ! -d "build/" ]; then

    mkdir build
    cd build/

    lsetup "asetup 21.2.35,AnalysisBase" rucio panda
	# https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AnalysisRelease
    
    source /cvmfs/sft.cern.ch/lcg/external/gcc/6.2.0/x86_64-slc6/setup.sh
    cmake ../DiLeptonRel21
    cmake --build .
    source $CMTCONFIG/setup.sh

else
    cd build/
    lsetup "asetup 21.2.35,AnalysisBase" rucio panda
    echo 'in dir ' $PWD 
    source $CMTCONFIG/setup.sh
    cmake --clean .
    cmake --build .
fi

cd ..

if [ ! -d "Run/" ]; then
    mkdir Run;
    #cd Run;
    #ln -s ../DiLepton/DiLepton/scripts/*py .;
    #If the ElectronIDDevelopment package is checked out we link its run scripts as well 
    #if [ -d "../DiLepton/ElectronIDDevelopment/" ]; then
    #    ln -s ../DiLepton/ElectronIDDevelopment/util/*py .;
    #fi
    #cd -;
fi

cd DiLeptonRel21
