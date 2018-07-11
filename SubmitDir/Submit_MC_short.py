#!/usr/bin/env python
import os

# rucio list-dids 'mc16_13TeV:mc16_13TeV.*4topSM*  --filter type=container | sort

datasets = [
'mc16_13TeV:mc16_13TeV.410080.MadGraphPythia8EvtGen_A14NNPDF23_4topSM.deriv.DAOD_SUSY2.e4111_s3126_r9364_r9315_p3387/'
]
# Loop over datasets 
for inds in datasets:
    inds = inds.strip()
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --isAtlfast 0 --driver grid --inputDS %s'%(inds)
    print cmd
    os.system(cmd)