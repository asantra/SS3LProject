echo "--> Setup of diLepton package"
export SETUPOPT=${1}
shift
export BASEREL="21.2"

export LOCALDIR=$(pwd)
export SETUPDIR=$( cd "$( dirname "${BASH_SOURCE:-$0}" )" && pwd )

export BASEDIR=${SETUPDIR}/../../
if [ ! -d $BASEDIR ]; then echo "BASEDIR does not exist, exiting"; return; fi

cd ${BASEDIR}; export BASEDIR=$(pwd)
echo "--> AnalysisBase directory ${BASEDIR}"

#export SUSYDIR=${BASEDIR}/SUSYTools 
#if [ ! -d $SUSYDIR ]; then echo "SUSYTools cannot be found, exiting"; return; fi
#echo "--> SUSYTool direcory ${SUSYDIR}"; echo

echo "--> get voms-proxy"
voms-proxy-init -voms atlas

export ALRB_userMenuFmtSkip=true
echo "--> setupATLAS from cvmfs"
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh

echo "--> setup rucio"
lsetup "rucio"

echo "--> lsetup panda"
lsetup "panda"

#echo "--> setup AnalysisBase ${BASEREL}"
#lsetup "rcsetup Base,${BASEREL}"

#rc find_packages
#if [[ $SETUPOPT = "-clean" ]]; then rc clean; fi 
#rc compile

cd ${LOCALDIR}
echo " ...done"
