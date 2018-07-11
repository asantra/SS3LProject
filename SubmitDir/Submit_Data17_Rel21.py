#!/usr/bin/env python
import os

tag = "Rel21_AB21p2p35_v0"


# rucio list-dids data17_13TeV:data17_13TeV.*PhysCont.DAOD_SUSY2.*/ --filter type=container | sort

datasets = [
'data17_13TeV:data17_13TeV.periodB.physics_Main.PhysCont.DAOD_SUSY2.grp17_v03_p3372/',
'data17_13TeV:data17_13TeV.periodC.physics_Main.PhysCont.DAOD_SUSY2.grp17_v03_p3372/',
'data17_13TeV:data17_13TeV.periodD.physics_Main.PhysCont.DAOD_SUSY2.grp17_v03_p3372/',
'data17_13TeV:data17_13TeV.periodE.physics_Main.PhysCont.DAOD_SUSY2.grp17_v03_p3372/',
'data17_13TeV:data17_13TeV.periodF.physics_Main.PhysCont.DAOD_SUSY2.grp17_v03_p3372/',
'data17_13TeV:data17_13TeV.periodH.physics_Main.PhysCont.DAOD_SUSY2.grp17_v03_p3372/',
'data17_13TeV:data17_13TeV.periodI.physics_Main.PhysCont.DAOD_SUSY2.grp17_v03_p3372/',
'data17_13TeV:data17_13TeV.periodK.physics_Main.PhysCont.DAOD_SUSY2.grp17_v03_p3372/',
]
# Loop over (200/184GRL) datasets 
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('data17_13TeV:data17_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.Data17'
	
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 1 --isData17 1 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)