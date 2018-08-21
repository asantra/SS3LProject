#!/usr/bin/env python
import os

tag = "Rel21_AB21p2p35_v0"


datasets = [
# 'mc16_13TeV:mc16_13TeV.364242.Sherpa_222_NNPDF30NNLO_WWW_3l3v_EW6.deriv.DAOD_SUSY2.e5887_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.364243.Sherpa_222_NNPDF30NNLO_WWZ_4l2v_EW6.deriv.DAOD_SUSY2.e5887_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.364244.Sherpa_222_NNPDF30NNLO_WWZ_2l4v_EW6.deriv.DAOD_SUSY2.e5887_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.364245.Sherpa_222_NNPDF30NNLO_WZZ_5l1v_EW6.deriv.DAOD_SUSY2.e5887_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.364246.Sherpa_222_NNPDF30NNLO_WZZ_3l3v_EW6.deriv.DAOD_SUSY2.e5887_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.364247.Sherpa_222_NNPDF30NNLO_ZZZ_6l0v_EW6.deriv.DAOD_SUSY2.e5887_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.364248.Sherpa_222_NNPDF30NNLO_ZZZ_4l2v_EW6.deriv.DAOD_SUSY2.e5887_s3126_r9364_r9315_p3387/',
]
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16a'
	
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16a 1 --isAtlfast 0 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)

	
datasets = [  
# 'mc16_13TeV:mc16_13TeV.407311.Sherpa_221_NNPDF30NNLO_6l0v_EW6.deriv.DAOD_SUSY2.e5473_e5984_s3126_s3136_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.407312.Sherpa_221_NNPDF30NNLO_5l1v_EW6.deriv.DAOD_SUSY2.e5473_e5984_s3126_s3136_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.407313.Sherpa_221_NNPDF30NNLO_4l2v_EW6.deriv.DAOD_SUSY2.e5473_e5984_s3126_s3136_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.407314.Sherpa_221_NNPDF30NNLO_3l3v_EW6.deriv.DAOD_SUSY2.e5473_e5984_s3126_s3136_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.407315.Sherpa_221_NNPDF30NNLO_2l4v_EW6.deriv.DAOD_SUSY2.e5655_e5984_s3126_s3136_r9364_r9315_p3387/',
]
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16a'
	
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16a 1 --isAtlfast 0 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)
	

datasets = [
# 'mc16_13TeV:mc16_13TeV.364250.Sherpa_222_NNPDF30NNLO_llll.deriv.DAOD_SUSY2.e5894_s3126_r9364_r9315_p3387/',  
# 'mc16_13TeV:mc16_13TeV.364288.Sherpa_222_NNPDF30NNLO_llll_lowMllPtComplement.deriv.DAOD_SUSY2.e6096_e5984_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.364253.Sherpa_222_NNPDF30NNLO_lllv.deriv.DAOD_SUSY2.e5916_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.364289.Sherpa_222_NNPDF30NNLO_lllv_lowMllPtComplement.deriv.DAOD_SUSY2.e6133_e5984_s3126_r9364_r9315_p3387/',
]
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16a'
	
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16a 1 --isAtlfast 0 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)
	
	
datasets = [
# 'mc16_13TeV:mc16_13TeV.345705.Sherpa_222_NNPDF30NNLO_ggllll_0M4l130.deriv.DAOD_SUSY2.e6213_e5984_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.345706.Sherpa_222_NNPDF30NNLO_ggllll_130M4l.deriv.DAOD_SUSY2.e6213_e5984_a875_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.364283.Sherpa_222_NNPDF30NNLO_lllljj_EW6.deriv.DAOD_SUSY2.e6055_e5984_s3126_r9364_r9315_p3387/',
# 'mc16_13TeV:mc16_13TeV.364284.Sherpa_222_NNPDF30NNLO_lllvjj_EW6.deriv.DAOD_SUSY2.e6055_e5984_s3126_r9364_r9315_p3387/',
'mc16_13TeV:mc16_13TeV.364286.Sherpa_222_NNPDF30NNLO_llvvjj_ss_EW4.deriv.DAOD_SUSY2.e6055_s3126_r9364_p3596/',
'mc16_13TeV:mc16_13TeV.364287.Sherpa_222_NNPDF30NNLO_llvvjj_ss_EW6.deriv.DAOD_SUSY2.e6055_s3126_r9364_p3596/'
]
for inds in datasets:
    inds = inds.strip()
    _outds = inds.replace('mc16_13TeV:mc16_13TeV.','user.oducu.'+tag+'.')
    outds = 'user.oducu.'+tag+'.' + _outds.split(".")[3]+'.'+_outds.split(".")[4] + '.MC16a'
	
    cmd = 'python /home/oducu/Rel21/SS3Lep/Run/Run.py --isData 0 --ismc16a 1 --isAtlfast 0 --driver grid --inputDS %s --outputDS %s'%(inds,outds)
    print cmd
    os.system(cmd)