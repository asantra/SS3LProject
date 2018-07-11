#!/usr/bin/env python
import os

tag = "Rel21_AB21p2p35_v0"


datasets = [
'mc16_13TeV:mc16_13TeV.410081.MadGraphPythia8EvtGen_A14NNPDF23_ttbarWW.deriv.DAOD_SUSY2.e4111_s3126_r9364_r9315_p3387/',
'mc16_13TeV:mc16_13TeV.410155.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttW.deriv.DAOD_SUSY2.e5070_s3126_r9364_r9315_p3575/',
'mc16_13TeV:mc16_13TeV.410218.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttee.deriv.DAOD_SUSY2.e5070_s3126_r9364_r9315_p3575/',
'mc16_13TeV:mc16_13TeV.410219.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttmumu.deriv.DAOD_SUSY2.e5070_s3126_r9364_r9315_p3575/',
'mc16_13TeV:mc16_13TeV.410220.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_tttautau.deriv.DAOD_SUSY2.e5070_s3126_r9364_r9315_p3575/',
'mc16_13TeV:mc16_13TeV.410276.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttee_mll_1_5.deriv.DAOD_SUSY2.e6087_e5984_s3126_r9364_r9315_p3401/',
'mc16_13TeV:mc16_13TeV.410277.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttmumu_mll_1_5.deriv.DAOD_SUSY2.e6087_e5984_s3126_r9364_r9315_p3401/',
'mc16_13TeV:mc16_13TeV.410278.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_tttautau_mll_1_5.deriv.DAOD_SUSY2.e6087_e5984_s3126_r9364_r9315_p3401/',
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
'mc16_13TeV:mc16_13TeV.410111.MadGraphPythia8EvtGen_A14NNPDF23LO_ttee_Np0.deriv.DAOD_SUSY2.e4265_s3126_r9364_r9315_p3387/',
'mc16_13TeV:mc16_13TeV.410112.MadGraphPythia8EvtGen_A14NNPDF23LO_ttee_Np1.deriv.DAOD_SUSY2.e4265_s3126_r9364_r9315_p3387/',
'mc16_13TeV:mc16_13TeV.410113.MadGraphPythia8EvtGen_A14NNPDF23LO_ttmumu_Np0.deriv.DAOD_SUSY2.e4265_s3126_r9364_r9315_p3387/',
'mc16_13TeV:mc16_13TeV.410114.MadGraphPythia8EvtGen_A14NNPDF23LO_ttmumu_Np1.deriv.DAOD_SUSY2.e4265_s3126_r9364_r9315_p3387/',
'mc16_13TeV:mc16_13TeV.410115.MadGraphPythia8EvtGen_A14NNPDF23LO_tttautau_Np0.deriv.DAOD_SUSY2.e4265_s3126_r9364_r9315_p3387/',
'mc16_13TeV:mc16_13TeV.410116.MadGraphPythia8EvtGen_A14NNPDF23LO_tttautau_Np1.deriv.DAOD_SUSY2.e4265_s3126_r9364_r9315_p3387/',
# 'mc15_13TeV.410066.MadGraphPythia8EvtGen_A14NNPDF23LO_ttW_Np0.merge.DAOD_SUSY2.e4111_s2608_s2183_r7725_r7676_p2879/',
# 'mc15_13TeV.410067.MadGraphPythia8EvtGen_A14NNPDF23LO_ttW_Np1.merge.DAOD_SUSY2.e4111_s2608_s2183_r7725_r7676_p2879/',
# 'mc15_13TeV.410068.MadGraphPythia8EvtGen_A14NNPDF23LO_ttW_Np2.merge.DAOD_SUSY2.e4111_s2608_s2183_r7725_r7676_p2879/',
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
'mc16_13TeV:mc16_13TeV.410144.Sherpa_NNPDF30NNLO_ttW.deriv.DAOD_SUSY2.e4686_e5984_s3126_r9364_r9315_p3387/', 
'mc16_13TeV:mc16_13TeV.410142.Sherpa_NNPDF30NNLO_ttll_mll5.deriv.DAOD_SUSY2.e4686_e5984_s3126_r9364_r9315_p3387/',
]
# Loop over datasets 
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16a'

    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16a 1 --isAtlfast 0 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)