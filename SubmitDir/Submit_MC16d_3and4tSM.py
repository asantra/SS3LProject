#!/usr/bin/env python
import os

tag = "Rel21_AB21p2p35_v0"


datasets = [
'mc16_13TeV:mc16_13TeV.304014.MadGraphPythia8EvtGen_A14NNPDF23_3top_SM.deriv.DAOD_SUSY2.e4324_e5984_s3126_r10201_r10210_p3387/',
'mc16_13TeV:mc16_13TeV.410080.MadGraphPythia8EvtGen_A14NNPDF23_4topSM.deriv.DAOD_SUSY2.e4111_e5984_s3126_r10201_r10210_p3387/'
]
# Loop over datasets 
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16d'

    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16d 1 --isAtlfast 0 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)