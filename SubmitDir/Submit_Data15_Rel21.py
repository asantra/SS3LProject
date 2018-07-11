#!/usr/bin/env python
import os

tag = "Rel21_AB21p2p35_v0"


# rucio list-dids data15_13TeV:data15_13TeV.*PhysCont.DAOD_SUSY2.*/ --filter type=container | sort

datasets = [
'data15_13TeV:data15_13TeV.periodD.physics_Main.PhysCont.DAOD_SUSY2.grp15_v03_p3372/',
'data15_13TeV:data15_13TeV.periodE.physics_Main.PhysCont.DAOD_SUSY2.grp15_v03_p3372/',
'data15_13TeV:data15_13TeV.periodF.physics_Main.PhysCont.DAOD_SUSY2.grp15_v03_p3372/',
'data15_13TeV:data15_13TeV.periodG.physics_Main.PhysCont.DAOD_SUSY2.grp15_v03_p3372/',
'data15_13TeV:data15_13TeV.periodH.physics_Main.PhysCont.DAOD_SUSY2.grp15_v03_p3372/',
'data15_13TeV:data15_13TeV.periodJ.physics_Main.PhysCont.DAOD_SUSY2.grp15_v03_p3372/',
]
# Loop over (75/65GRL) datasets 
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('data15_13TeV:data15_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.Data15'
		
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 1 --isData15 1 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)