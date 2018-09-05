if [ ! -d "../build/" ]; then
    echo "dir build not found, running initialize"
    source initialize.sh 
else
    cd ../build
    setupATLAS
    voms-proxy-init -voms atlas
    lsetup "asetup 21.2.32,AnalysisBase" rucio panda
    source $CMTCONFIG/setup.sh
    cd ../SS3LAnalysis
fi
