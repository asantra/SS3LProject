export LOCALDIR=$(pwd)
export LIST=${1}
shift

echo "--> get voms-proxy"
voms-proxy-init -voms atlas

echo "--> setupATLAS from cvmfs"
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh

echo "--> setupAthena"
asetup 20.7.8.7,AtlasDerivation,here

echo "--> lsetup panda"
lsetup "panda"

echo "--> lsetup rucio"
lsetup rucio

echo "Sample list: $LIST"
if [ "$LIST" = "" ]; then return; fi

for i in `cat $LIST`; do
    echo pathena --trf \"Reco_tf.py --inputAODFile=%IN --outputNTUP_PILEUPFile=%OUT.pileup.root\" --outDS user.`whoami`.pileup_v0.`echo ${i} | gawk '{split($1,a,".");print a[2]"."a[3]"."a[6]}'` --inDS ${i} --long --noBuild;
    pathena --trf "Reco_tf.py --inputAODFile=%IN --outputNTUP_PILEUPFile=%OUT.pileup.root" --outDS user.`whoami`.pileup_v0.`echo ${i} | gawk '{split($1,a,".");print a[2]"."a[3]"."a[6]}'` --inDS ${i} --long --noBuild;
done

echo "...done"
