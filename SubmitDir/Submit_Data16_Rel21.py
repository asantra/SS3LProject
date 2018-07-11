#!/usr/bin/env python
import os

tag = "Rel21_AB21p2p35_v0"


# rucio list-dids data16_13TeV:data16_13TeV.*PhysCont.DAOD_SUSY2.*/ --filter type=container | sort

datasets = [
'data16_13TeV:data16_13TeV.periodA.physics_Main.PhysCont.DAOD_SUSY2.grp16_v03_p3372/',
'data16_13TeV:data16_13TeV.periodB.physics_Main.PhysCont.DAOD_SUSY2.grp16_v03_p3372/',
'data16_13TeV:data16_13TeV.periodC.physics_Main.PhysCont.DAOD_SUSY2.grp16_v03_p3372/',
'data16_13TeV:data16_13TeV.periodD.physics_Main.PhysCont.DAOD_SUSY2.grp16_v03_p3372/',
'data16_13TeV:data16_13TeV.periodE.physics_Main.PhysCont.DAOD_SUSY2.grp16_v03_p3372/',
'data16_13TeV:data16_13TeV.periodF.physics_Main.PhysCont.DAOD_SUSY2.grp16_v03_p3372/',
'data16_13TeV:data16_13TeV.periodG.physics_Main.PhysCont.DAOD_SUSY2.grp16_v03_p3372/',
'data16_13TeV:data16_13TeV.periodI.physics_Main.PhysCont.DAOD_SUSY2.grp16_v03_p3372/',
'data16_13TeV:data16_13TeV.periodK.physics_Main.PhysCont.DAOD_SUSY2.grp16_v03_p3372/',
'data16_13TeV:data16_13TeV.periodL.physics_Main.PhysCont.DAOD_SUSY2.grp16_v03_p3372/',
]
# Loop over (156/150GRL) datasets 
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('data16_13TeV:data16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.Data16'
	
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 1 --isData16 1 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)