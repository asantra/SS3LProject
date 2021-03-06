#!/usr/bin/env python
import os

tag = "Rel21_AB21p2p35_v0"


## 'mc16_13TeV:mc16_13TeV.410217.aMcAtNloHerwigppEvtGen_UEEE5_CTEQ6L1_CT10ME_260000_tWZDR.deriv.DAOD_SUSY2.e4851_s3126_r9364_r9315_p3401/',  #not yet in mc16d, use the one with filter
datasets = [
# 'mc16_13TeV:mc16_13TeV.410560.MadGraphPythia8EvtGen_A14_tZ_4fl_tchan_noAllHad.deriv.DAOD_SUSY2.e5803_e5984_s3126_r10201_r10210_p3401/',
# 'mc16_13TeV:mc16_13TeV.407321.MadGraphPythia8EvtGen_A14NNPDF23LO_ttbarWll.deriv.DAOD_SUSY2.e5536_e5984_s3126_r10201_r10210_p3387/',
'mc16_13TeV:mc16_13TeV.410408.aMcAtNloPythia8EvtGen_tWZ_Ztoll_minDR1.deriv.DAOD_SUSY2.e6423_s3126_r10201_p3563/', 
]
# Loop over datasets 
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16d'
	
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16d 1 --isAtlfast 0 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)