#!/usr/bin/env python
import os

tag = "Rel21_AB21p2p27_newISOWPs_v0"
# rucio list-dids 'mc16_13TeV:mc16_13TeV.*4topSM*  --filter type=container | sort
# rucio list-dids mc16_13TeV:mc16_13TeV*MGPy8EG_A14N_GG_ttn1*SUSY2*r9315* --filter type=container | sort



# rucio list-dids mc16_13TeV:mc16_13TeV.*SUSY2*r9315*p35*  --filter type=container | sort
datasets = [
'mc16_13TeV:mc16_13TeV.361100.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wplusenu.deriv.DAOD_SUSY2.e3601_s3126_r9364_r9315_p3529/',
'mc16_13TeV:mc16_13TeV.361101.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wplusmunu.deriv.DAOD_SUSY2.e3601_s3126_r9364_r9315_p3529/',
'mc16_13TeV:mc16_13TeV.361102.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wplustaunu.deriv.DAOD_SUSY2.e3601_s3126_r9364_r9315_p3529/',
'mc16_13TeV:mc16_13TeV.361103.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wminusenu.deriv.DAOD_SUSY2.e3601_s3126_r9364_r9315_p3529/',
'mc16_13TeV:mc16_13TeV.361104.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wminusmunu.deriv.DAOD_SUSY2.e3601_s3126_r9364_r9315_p3529/',
'mc16_13TeV:mc16_13TeV.361105.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wminustaunu.deriv.DAOD_SUSY2.e3601_s3126_r9364_r9315_p3529/',
##
'mc16_13TeV:mc16_13TeV.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.deriv.DAOD_SUSY2.e3601_s3126_r9364_r9315_p3529/',
'mc16_13TeV:mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.deriv.DAOD_SUSY2.e3601_s3126_r9364_r9315_p3529/',
'mc16_13TeV:mc16_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.deriv.DAOD_SUSY2.e3601_s3126_r9364_r9315_p3529/',
##
'mc16_13TeV:mc16_13TeV.410155.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttW.deriv.DAOD_SUSY2.e5070_s3126_r9364_r9315_p3529/',
'mc16_13TeV:mc16_13TeV.410218.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttee.deriv.DAOD_SUSY2.e5070_s3126_r9364_r9315_p3529/',
'mc16_13TeV:mc16_13TeV.410219.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttmumu.deriv.DAOD_SUSY2.e5070_s3126_r9364_r9315_p3529/',
'mc16_13TeV:mc16_13TeV.410220.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_tttautau.deriv.DAOD_SUSY2.e5070_s3126_r9364_r9315_p3529/',
##
]
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16a'
	
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16a 1 --isAtlfast 1 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)
	

	
# rucio list-dids mc16_13TeV:mc16_13TeV.*SUSY2*r10210*p35*  --filter type=container | sort
datasets = [
'mc16_13TeV:mc16_13TeV.361100.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wplusenu.deriv.DAOD_SUSY2.e3601_e5984_s3126_r10201_r10210_p3529/',
'mc16_13TeV:mc16_13TeV.361101.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wplusmunu.deriv.DAOD_SUSY2.e3601_e5984_s3126_r10201_r10210_p3529/',
'mc16_13TeV:mc16_13TeV.361102.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wplustaunu.deriv.DAOD_SUSY2.e3601_e5984_s3126_r10201_r10210_p3529/',
'mc16_13TeV:mc16_13TeV.361103.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wminusenu.deriv.DAOD_SUSY2.e3601_e5984_s3126_r10201_r10210_p3529/',
'mc16_13TeV:mc16_13TeV.361104.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wminusmunu.deriv.DAOD_SUSY2.e3601_e5984_s3126_r10201_r10210_p3529/',
'mc16_13TeV:mc16_13TeV.361105.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wminustaunu.deriv.DAOD_SUSY2.e3601_e5984_s3126_r10201_r10210_p3529/',
##
'mc16_13TeV:mc16_13TeV.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.deriv.DAOD_SUSY2.e3601_e5984_s3126_r10201_r10210_p3529/',
'mc16_13TeV:mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.deriv.DAOD_SUSY2.e3601_e5984_s3126_r10201_r10210_p3529/',
'mc16_13TeV:mc16_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.deriv.DAOD_SUSY2.e3601_e5984_a875_r10201_r10210_p3529/',
##
'mc16_13TeV:mc16_13TeV.410155.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttW.deriv.DAOD_SUSY2.e5070_e5984_s3126_r10201_r10210_p3529/',
'mc16_13TeV:mc16_13TeV.410218.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttee.deriv.DAOD_SUSY2.e5070_e5984_s3126_r10201_r10210_p3529/',
'mc16_13TeV:mc16_13TeV.410219.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttmumu.deriv.DAOD_SUSY2.e5070_e5984_s3126_r10201_r10210_p3529/',
'mc16_13TeV:mc16_13TeV.410220.aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_tttautau.deriv.DAOD_SUSY2.e5070_e5984_s3126_r10201_r10210_p3529/',
##
'mc16_13TeV:mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_SUSY2.e6337_e5984_s3126_r10201_r10210_p3529/',
]
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16d'
	
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16a 1 --isAtlfast 1 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)
	