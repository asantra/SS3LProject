#!/usr/bin/env python
import os

tag = "Rel21_AB21p2p35_v0"


datasets = [
'mc16_13TeV:mc16_13TeV.410560.MadGraphPythia8EvtGen_A14_tZ_4fl_tchan_noAllHad.deriv.DAOD_SUSY2.e5803_s3126_r9364_r9315_p3401/',
'mc16_13TeV:mc16_13TeV.407321.MadGraphPythia8EvtGen_A14NNPDF23LO_ttbarWll.deriv.DAOD_SUSY2.e5536_s3126_s3136_r9364_r9315_p3387/',
'mc16_13TeV:mc16_13TeV.410217.aMcAtNloHerwigppEvtGen_UEEE5_CTEQ6L1_CT10ME_260000_tWZDR.deriv.DAOD_SUSY2.e4851_s3126_r9364_r9315_p3401/',
]
# Loop over datasets 
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16a'

    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16a 1 --isAtlfast 0 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)