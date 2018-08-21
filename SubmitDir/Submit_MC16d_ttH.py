#!/usr/bin/env python
import os

tag = "Rel21_AB21p2p35_v0"


## old ttH samples
datasets = [
## 'mc16_13TeV:mc16_13TeV.343365.aMcAtNloPythia8EvtGen_A14_NNPDF23_NNPDF30ME_ttH125_dilep.deriv.DAOD_SUSY2.e4706_e5984_s3126_r9364_r9315_p3387/',  # not yet in mc16d
# 'mc16_13TeV:mc16_13TeV.343366.aMcAtNloPythia8EvtGen_A14_NNPDF23_NNPDF30ME_ttH125_semilep.deriv.DAOD_SUSY2.e4706_e5984_s3126_r10201_r10210_p3387/',
# 'mc16_13TeV:mc16_13TeV.343367.aMcAtNloPythia8EvtGen_A14_NNPDF23_NNPDF30ME_ttH125_allhad.deriv.DAOD_SUSY2.e4706_e5984_s3126_r10201_r10210_p3387/',
]
# Loop over datasets 
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16d'
	
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16d 1 --isAtlfast 0 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)
	
	
## new ttH samples
datasets = [
'mc16_13TeV:mc16_13TeV.345875.PhPy8EG_A14NNPDF23_NNPDF30ME_ttH125_hdamp352p5_dilep.deriv.DAOD_SUSY2.e6474_s3126_r10201_p3596/',
'mc16_13TeV:mc16_13TeV.345874.PhPy8EG_A14NNPDF23_NNPDF30ME_ttH125_hdamp352p5_semilep.deriv.DAOD_SUSY2.e6474_s3126_r10201_p3596/',
]
# Loop over datasets 
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16d'
	
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16d 1 --isAtlfast 0 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)