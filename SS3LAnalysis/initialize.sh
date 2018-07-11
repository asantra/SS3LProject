#Assume you are in the project folder (SS3LAnalysis)
cd ..
setupATLAS
echo 'in dir ' $PWD 

if [ ! -d "build/" ]; then

    mkdir build
    cd build/

    lsetup "asetup 21.2.35,AnalysisBase" rucio panda
    # lsetup "asetup 21.2,AnalysisBase,latest" rucio panda
	# https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AnalysisRelease
    
    cmake ../SS3LAnalysis
    cmake --build .
    source $CMTCONFIG/setup.sh

else
    cd build/
    lsetup "asetup 21.2.35,AnalysisBase" rucio panda
    # lsetup "asetup 21.2,AnalysisBase,latest" rucio panda
    echo 'in dir ' $PWD 
    source $CMTCONFIG/setup.sh
    cmake --clean .
    cmake --build .
fi

cd ..

if [ ! -d "Run/" ]; then
    mkdir Run;
    cd Run;
    ln -s ../SS3LAnalysis/SS3LAnalysis/scripts/*py .;
    #If the SUSYTools package is checked out we link its run scripts as well 
    if [ -d "../SS3LAnalysis/SUSYTools/" ]; then
        ln -s ../SS3LAnalysis/SUSYTools/util/*py .;
    fi
    cd -;
fi

cd SS3LAnalysis