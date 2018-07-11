#!/usr/bin/env python
import os

tag = "Rel21_AB21p2p35_v0"


datasets = [
'mc16_13TeV:mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_SUSY2.e6337_e5984_s3126_r9364_r9315_p3575/', 
'mc16_13TeV:mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_SUSY2.e5458_s3126_r9364_r9315_p3387/',
]
# Loop over datasets 
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16a'

    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16a 1 --isAtlfast 0 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)


datasets = [
'mc16_13TeV:mc16_13TeV.407342.PhPy8EG_A14_ttbarHT1k5_hdamp258p75_nonallhad.deriv.DAOD_SUSY2.e6414_e5984_s3126_r9364_r9315_p3401/', 
'mc16_13TeV:mc16_13TeV.407343.PhPy8EG_A14_ttbarHT1k_1k5_hdamp258p75_nonallhad.deriv.DAOD_SUSY2.e6414_e5984_s3126_r9364_r9315_p3401/', 
'mc16_13TeV:mc16_13TeV.407344.PhPy8EG_A14_ttbarHT6c_1k_hdamp258p75_nonallhad.deriv.DAOD_SUSY2.e6414_e5984_s3126_r9364_r9315_p3401/', 
'mc16_13TeV:mc16_13TeV.407345.PhPy8EG_A14_ttbarMET200_300_hdamp258p75_nonallhad.deriv.DAOD_SUSY2.e6414_e5984_s3126_r9364_r9315_p3401/', 
'mc16_13TeV:mc16_13TeV.407346.PhPy8EG_A14_ttbarMET300_400_hdamp258p75_nonallhad.deriv.DAOD_SUSY2.e6414_e5984_s3126_r9364_r9315_p3401/', 
'mc16_13TeV:mc16_13TeV.407347.PhPy8EG_A14_ttbarMET400_hdamp258p75_nonallhad.deriv.DAOD_SUSY2.e6414_e5984_s3126_r9364_r9315_p3401/', 	
]
# Loop over datasets 
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16a'
	
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16a 1 --isAtlfast 0 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)

	
datasets = [
# 'mc16_13TeV:mc16_13TeV.410011.PowhegPythiaEvtGen_P2012_singletop_tchan_lept_top.deriv.DAOD_SUSY2.e3824_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.410012.PowhegPythiaEvtGen_P2012_singletop_tchan_lept_antitop.deriv.DAOD_SUSY2.e3824_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.410013.PowhegPythiaEvtGen_P2012_Wt_inclusive_top.deriv.DAOD_SUSY2.e3753_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.410014.PowhegPythiaEvtGen_P2012_Wt_inclusive_antitop.deriv.DAOD_SUSY2.e3753_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.410025.PowhegPythiaEvtGen_P2012_SingleTopSchan_noAllHad_top.deriv.DAOD_SUSY2.e3998_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.410026.PowhegPythiaEvtGen_P2012_SingleTopSchan_noAllHad_antitop.deriv.DAOD_SUSY2.e3998_s3126_r9364_r9315_p3387/'
]
# Loop over datasets 
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.') 
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4]  + '.MC16a'

    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16a 1 --isAtlfast 0 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)