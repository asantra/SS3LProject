#include <SS3LAnalysis/MyxAODAnalysis.h>
#include "SS3LAnalysis/CutClass.h"
/// ttbar+HF classification
#include "SS3LAnalysis/ClassifyAndCalculateHF.h"
/// #include <valgrind/callgrind.h>
#include "SS3LAnalysis/chargeflip.h"


#include <TTreeFormula.h>

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include "EventLoop/OutputStream.h"

/// Amg include
#include "EventPrimitives/EventPrimitivesHelpers.h"

/// PathResolver
#include <PathResolver/PathResolver.h>

/// PAT
#include "PATCore/PATCoreEnums.h"
#include "PATInterfaces/SystematicsUtil.h"

/// for metaData
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaNames.h>

#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"

/// EDM includes:
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODRootAccess/tools/TFileAccessTracer.h"

/// OR includes (local includes)
#include <AssociationUtils/OverlapRemovalInit.h>
#include <AssociationUtils/IOverlapRemovalTool.h>
#include <AssociationUtils/OverlapRemovalTool.h>



/// this is needed to distribute the algorithm to the workers
ClassImp(MyxAODAnalysis)


MyxAODAnalysis :: MyxAODAnalysis ()
{
  /// Here you put any code for the base initialization of variables,
  /// e.g. initialize all pointers to 0.  Note that you should only put
  /// the most basic initialization here, since this method will be
  /// called on both the submission and the worker node.  Most of your
  /// initialization code will go into histInitialize() and
  /// initialize().
}



EL::StatusCode MyxAODAnalysis :: setupJob (EL::Job& job)
{
  /// Here you put code that sets up the job on the submission object
  /// so that it is ready to work with your algorithm, e.g. you can
  /// request the D3PDReader service or add output files.  Any code you
  /// put here could instead also go into the submission script.  The
  /// sole advantage of putting it here is that it gets automatically
  /// activated/deactivated when you add/remove the algorithm from your
  /// job, which may or may not be of value to you.

  /// const char* APP_NAME = "MyxAODAnalysis::setupJob()";   
  /// Info(APP_NAME, "Start of method..." );   

  job.useXAOD ();

  /// let's initialize the algorithm to use the xAODRootAccess package
  xAOD::Init( "MyxAODAnalysis" ).ignore(); /// call before opening first file

  /// tell EventLoop about our output:
  EL::OutputStream out ("output");
  job.outputAdd (out);

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: histInitialize ()
{
  /// Here you do everything that needs to be done at the very
  /// beginning on each worker node, e.g. create histograms and output
  /// trees.  This method gets called before any input files are
  /// connected.
   
  /// const char* APP_NAME = "MyxAODAnalysis::histInitialize()";   
  /// Info(APP_NAME, "Start of method..." );   

  hWeights = new TH1D("DerivationStat_Weights", "", 3, 0, 3);
  hWeights->GetXaxis()->SetBinLabel(1, "original xAOD");
  hWeights->GetXaxis()->SetBinLabel(2, "DxAOD");
  hWeights->GetXaxis()->SetBinLabel(3, "Ntup");

  hEvents = new TH1D("DerivationStat_RawEvents", "", 3, 0, 3);
  hEvents->GetXaxis()->SetBinLabel(1, "original xAOD");
  hEvents->GetXaxis()->SetBinLabel(2, "DxAOD");
  hEvents->GetXaxis()->SetBinLabel(3, "Ntup");

  hWeightsSquared = new TH1D("DerivationStat_Weights_Squared", "", 3, 0, 3);
  hWeightsSquared->GetXaxis()->SetBinLabel(1, "original xAOD");
  hWeightsSquared->GetXaxis()->SetBinLabel(2, "DxAOD");
  hWeightsSquared->GetXaxis()->SetBinLabel(3, "Ntup");
  
  ///  wk()->addOutput(hEvents);
  ///  wk()->addOutput(hWeights);
  
  /// Info(APP_NAME, "End of method..." );   

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: fileExecute ()
{
  /// Here you do everything that needs to be done exactly once for every
  /// single file, e.g. collect a list of all lumi-blocks processed
  
  /// const char* APP_NAME = "MyxAODAnalysis::fileExecute()";   
  /// Info(APP_NAME, "Start of method..." );
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: changeInput (bool /**firstFile*/)
{
  /// Here you do everything you need to do when we change input files,
  /// e.g. resetting branch addresses on trees.  If you are using
  /// D3PDReader or a similar service this method is not needed.
  
  /// get the MetaData tree once a new file is opened, with
  const char* APP_NAME = "MyxAODAnalysis::changeInput()";   
  /// Info(APP_NAME, "Start of method..." );   

  Info("changeInput()", "About to get metaData....");
  
  
  /// read event tree and print number of events
  m_event = wk()->xaodEvent();
  Info("changeInput()", "Number of events in this file = %lli", m_event->getEntries() ); // print long long int
  /// cout << __LINE__<<endl;

  /// Read the CutBookkeeper container
  const xAOD::CutBookkeeperContainer* completeCBC = 0;
  if (!m_event->retrieveMetaInput(completeCBC, "CutBookkeepers").isSuccess()) {
    Error( "changeInput()", "Failed to retrieve CutBookkeepers from MetaData! Exiting.");
  }

  const xAOD::CutBookkeeper* allEventsCBK = 0;
  int maxcycle=-1;
  /// let's find the right CBK (latest with StreaAOD input before derivations)
  for ( auto cbk : *completeCBC ) {
    if ( cbk->name() == "AllExecutedEvents" && cbk->inputStream() == "StreamAOD" && cbk->cycle() > maxcycle){
      maxcycle = cbk->cycle();
      allEventsCBK = cbk;
    }
  }
  
  uint64_t nEventsProcessed  = -1;
  double sumOfWeights        = -1;
  double sumOfWeightsSquared = -1;
  if (allEventsCBK) {
    nEventsProcessed  = allEventsCBK->nAcceptedEvents();
    sumOfWeights        = allEventsCBK->sumOfEventWeights();
    sumOfWeightsSquared = allEventsCBK->sumOfEventWeightsSquared();
    Info( "changeInput()", "CutBookkeepers Accepted %lu SumWei %f sumWei2 %f ",nEventsProcessed,sumOfWeights,sumOfWeightsSquared);
  } else { 
    Info( APP_NAME, "No relevent CutBookKeepers found" ); 
  }
    
  hWeights->Fill(0.,sumOfWeights);
  hEvents->Fill(0.,nEventsProcessed);
  hWeightsSquared->Fill(0.,sumOfWeightsSquared);
  
  Info(APP_NAME, "Out of method..." );   

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: initialize ()
{
  /// Here you do everything that you need to do after the first input
  /// file has been connected and before the first event is processed,
  /// e.g. create additional histograms based on which variables are
  /// available in the input files.  You can also create all of your
  /// histograms and trees in here, but be aware that this method
  /// doesn't get called if no events are processed.  So any objects
  /// you create here won't be available in the output if you have no
  /// input events.

  xAOD::TFileAccessTracer::enableDataSubmission( false );  

  DEBUG = false;
  const char* APP_NAME = "MyxAODAnalysis::initialise()";   
  m_event = wk()->xaodEvent();
  
  /// eventInfo for various configuration purposes
  const xAOD::EventInfo* eventInfo(nullptr);
  ANA_CHECK( m_event->retrieve( eventInfo, "EventInfo" ) );

  /// Dump Flags:
  cout << "IsData:"  << isData    << endl;
  cout << "IsAF:"    << isAtlfast << endl;
  cout << "IsMC16a:" << ismc16a   << endl;
  cout << "IsMC16d:" << ismc16d   << endl;
  
  

  /// ////////////////////////////////////////////////////////////////////////////////////////
  /// ANA_CHECK(objTool.setProperty("DebugMode", msgLvl(MSG::VERBOSE))); /// MSG::INFO MSG::ERROR
  ANA_CHECK(objTool.setProperty("OutputLevel", msg().level()));
  
  ST::ISUSYObjDef_xAODTool::DataSource datasource = 
		isData ? ST::ISUSYObjDef_xAODTool::Data : (isAtlfast ? ST::ISUSYObjDef_xAODTool::AtlfastII : ST::ISUSYObjDef_xAODTool::FullSim);
  ANA_CHECK( objTool.setProperty("DataSource",datasource) ) ;
  if(DEBUG) cout << __LINE__ << ": " << (int) datasource << endl;  

  ANA_CHECK( objTool.setProperty("ConfigFile", "SS3LAnalysis/SS3L_Default.conf") );
  if(DEBUG) std::cout << " after <objTool->setProperty> " << std::endl << std::endl;

  int ShowerType = !isData ? ST::getMCShowerType(wk()->metaData()->castString(SH::MetaFields::sampleName)) : 0;
  cout << "ShowerType = " << ShowerType << endl;
  ANA_CHECK( objTool.setProperty("ShowerType", ShowerType) );
  
  
  std::vector<std::string> lumicalcFiles;
  std::vector<std::string> confFiles;
  std::vector<std::string> confFiles_MC16a;
  std::vector<std::string> confFiles_MC16d;
  string prw_config_loc = "dev/SUSYTools/PRW_AUTOCONFIG_SIM/files/"; /// /cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/PRW_AUTOCONFIG_SIM/files/
  string prw_config = PathResolverFindCalibDirectory(prw_config_loc);
  if(prw_config=="") {
    cout << "get_auto_configured_prw    "
            "ERROR Could not locate PRW_AUTOCONFIG location (=" << prw_config_loc << ")" << endl;
     exit(1);
  }  
  
  if(!isData)
  {
	string confFiles_a = prw_config;
	string confFiles_d = prw_config;
	
	string isSimType = "";
	if(isAtlfast) isSimType = "_AFII";
	else isSimType = "_FS";

	uint32_t runNum = eventInfo->runNumber(); /// can use it to select the year
	cout << "runNum = " << runNum << ", mcChannelNumber = " << eventInfo->mcChannelNumber() << endl;
	
	if(runNum==284500) 
	{
		confFiles_a += "pileup_mc16a_dsid" + std::to_string(eventInfo->mcChannelNumber()) + isSimType + ".root"; /// e.g pileup_mc16a_dsid304014
		confFiles_a = PathResolverFindCalibFile(confFiles_a);

		TFile testF(confFiles_a.c_str(),"read");
		if(testF.IsZombie()) 
		{
		  ANA_MSG_ERROR( "getPRWConfigFile(): file not found -> " << confFiles_a );
		  return EL::StatusCode::FAILURE;
		} 
		else 
		{
		  confFiles_MC16a.push_back(confFiles_a);
		  ANA_MSG_INFO( "getPRWConfigFile(): setting PRW conf to " << confFiles_a );
		}
	}
	else if(runNum==300000)
	{
		confFiles_d += "pileup_mc16d_dsid" + std::to_string(eventInfo->mcChannelNumber()) + isSimType + ".root"; /// e.g pileup_mc16d_dsid304014
		confFiles_d = PathResolverFindCalibFile(confFiles_d);

		TFile testF(confFiles_d.c_str(),"read");
		if(testF.IsZombie()) 
		{
		  ANA_MSG_ERROR( "getPRWConfigFile(): file not found -> " << confFiles_d );
		  return EL::StatusCode::FAILURE;
		} 
		else 
		{
		  confFiles_MC16d.push_back(confFiles_d);
		  ANA_MSG_INFO( "getPRWConfigFile(): setting PRW conf to " << confFiles_d );
		}
	}
	else
	{
		cout << "ERROR!!  Cannot take the files for the PRW!! exiting..." << endl;
		return EL::StatusCode::FAILURE;
	}	

	if(ismc16a && runNum==284500) /// should be the same, othewise the input MC is wrong
	{
		cout << "!!! I am here in mc16a !" << endl << endl << endl << endl;;
		lumicalcFiles.push_back(PathResolverFindCalibFile("SS3LAnalysis/PRW/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root"));  /// 2015 data
		lumicalcFiles.push_back(PathResolverFindCalibFile("SS3LAnalysis/PRW/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root"));  /// 2016 data
		ANA_CHECK( objTool.setProperty("PRWLumiCalcFiles", lumicalcFiles) ); 
		ANA_CHECK( objTool.setProperty("PRWConfigFiles", confFiles_MC16a) ); /// remove if using AutoconfigurePRWTool
	}
	else if(ismc16d && runNum==300000) /// should be the same, othewise the input MC is wrong
	{
		cout << "!!! I am here in mc16d!" << endl << endl << endl << endl;;
		lumicalcFiles.push_back(PathResolverFindCalibFile("SS3LAnalysis/PRW/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root"));  /// 2017 data
		confFiles.push_back(PathResolverFindCalibFile("SS3LAnalysis/PRW/physics_25ns_Triggerno17e33prim.actualMu.OflLumi-13TeV-010.root"));  /// 2017 data /// only for mc16d
		ANA_CHECK( objTool.setProperty("PRWLumiCalcFiles", lumicalcFiles) ); 
		ANA_CHECK( objTool.setProperty("PRWConfigFiles", confFiles) ); 
		ANA_CHECK( objTool.setProperty("PRWConfigFiles", confFiles_MC16d) ); /// remove if using AutoconfigurePRWTool
    }
	else if (runNum==284500 || runNum==300000)/// combine 2015-2017 data
	{
		cout << "!!! I am here (2015-2017 data all together)" << endl << endl << endl << endl;;
		
		lumicalcFiles.push_back(PathResolverFindCalibFile("SS3LAnalysis/PRW/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root"));  /// 2015 data
		lumicalcFiles.push_back(PathResolverFindCalibFile("SS3LAnalysis/PRW/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root"));  /// 2016 data 
		lumicalcFiles.push_back(PathResolverFindCalibFile("SS3LAnalysis/PRW/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root"));  /// 2017 data
		confFiles.push_back(PathResolverFindCalibFile("SS3LAnalysis/PRW/physics_25ns_Triggerno17e33prim.actualMu.OflLumi-13TeV-010.root"));  /// 2017 data /// only for mc16
		ANA_CHECK( objTool.setProperty("PRWLumiCalcFiles", lumicalcFiles) ); 
		ANA_CHECK( objTool.setProperty("PRWConfigFiles", confFiles) ); 
		if(runNum==284500) { ANA_CHECK( objTool.setProperty("PRWConfigFiles", confFiles_MC16a) ); }
		else if(runNum==300000) { ANA_CHECK( objTool.setProperty("PRWConfigFiles", confFiles_MC16d) ); }
		else
		{
			cout << "ERROR!!  You are doing craps!! exiting..." << endl;
			return EL::StatusCode::FAILURE;
		}	
	}
	else
	{
		cout << "ERROR!!  Cannot take the files for the PRW!! exiting..." << endl;
		return EL::StatusCode::FAILURE;
	}
	/// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ExtendedPileupReweighting#Generating_the_actual_pileup_fil
	/// https://indico.cern.ch/event/712774/contributions/2928042/attachments/1614637/2565496/prw_mc16d.pdf
	/// 2015+16 with average mu, 2017 with actual mu is possible 
	/// note that in order to compare to 2015+2016 only with mc16a and 2017 with only mc16d you must also store the "period_weights"  
	/// (slide 8 of : https://indico.cern.ch/event/679306/contributions/2784652/attachments/1556491/2447942/PRWForMC16.pdf )
  }
  
  /// ANA_CHECK( objTool.setProperty("PRWLumiCalcFiles", lumicalcFiles) );   
  /// if(!ismc16a) ANA_CHECK( objTool.setProperty("PRWConfigFiles", confFiles) ); 
  if(DEBUG) Info(APP_NAME,"SUSYObjDef_xAOD before initialize()... " );
  /// ANA_CHECK( objTool->setBoolProperty("AutoconfigurePRWTool", true ) );   /// !!! remove setProperty with confFiles_a / confFiles_d 
  /// ANA_CHECK( objTool.ST::ISUSYObjDef_xAODTool::setBoolProperty("AutoconfigurePRWTool", true) ); /// !!! remove setProperty with confFiles_a / confFiles_d 
  

  /// if I want to change the OR default properties (before initialise):
  /// CHECK(objTool->setProperty("DoBoostedElectronOR", false));
  
  ANA_CHECK(objTool.initialize());
  ANA_MSG_DEBUG("SUSYObjDef_xAOD initialized... " );
  if(DEBUG) std::cout << "SUSYObjDef_xAOD initialized... " << endl << std::endl; 
  
  
  
  /// ////////////////////////////////////////////////////////////////////////////////////////
  /// set ORtool 
  /**auto toolName = "SS3Lep_MyORTool";
  ORUtils::ORFlags orFlags(toolName, "baseline", "passMyOR");
  orFlags.bJetLabel= "bjet_loose";
  orFlags.boostedLeptons = false; /// no boosted OR in isMyOR
  orFlags.outputPassValue = true;
  orFlags.doEleEleOR = false;
  orFlags.doElectrons = true;
  orFlags.doMuons = true;
  orFlags.doJets = true;
  orFlags.doTaus = false;
  orFlags.doPhotons = false;
  orFlags.doFatJets  = false;
  ANA_CHECK( ORUtils::recommendedTools(orFlags, my_orToolbox));
  my_orToolbox.msg().setLevel( MSG::INFO );
  ANA_CHECK(my_orToolbox.initialize());
  ANA_MSG_DEBUG("my_orToolbox initialized... ");
  if(DEBUG) std::cout << " done with my OR tool " << std::endl << std::endl;*/
  
  

  /// ////////////////////////////////////////////////////////////////////////////////////////
  /// initialise GRL tool using latest GRL for 2015, 2016 and 2017 data
  /// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BackgroundStudies
  std::vector<std::string> vecStringGRL; 
  vecStringGRL.push_back(PathResolverFindCalibFile("SS3LAnalysis/GRL/data15_13TeV.periodAllYear_DetStatus-v89-pro21-02_Unknown_PHYS_StandardGRL_All_Good_25ns.xml"));  
  vecStringGRL.push_back(PathResolverFindCalibFile("SS3LAnalysis/GRL/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml"));
  vecStringGRL.push_back
		      (PathResolverFindCalibFile("SS3LAnalysis/GRL/data17_13TeV.periodAllYear_DetStatus-v97-pro21-13_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml"));
			  /// for reprocessed data (not containers yet)
		      // (PathResolverFindCalibFile("SS3LAnalysis/GRL/data17_13TeV.periodAllYear_DetStatus-v99-pro22-01_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml"));
  ANA_CHECK( m_grl.setProperty( "GoodRunsListVec", vecStringGRL) );
  ANA_CHECK( m_grl.setProperty( "PassThrough", false) ); /// if true (default) will ignore result of GRL and will just pass all events
  ANA_CHECK( m_grl.initialize() );
  ANA_MSG_DEBUG("GRL initialized... ");
  if(DEBUG) Info(APP_NAME, "GoodRunsListSelectionTool initialized" );
  
   
   
  /// ////////////////////////////////////////////////////////////////////////////////////////
  /// Initialise electron selector tools
  
  /// http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronEfficiencyCorrection/2015_2017/rel21.2/Summer2017_Prerec_v1/isolation/
  /// std::string egMapFile = "ElectronEfficiencyCorrection/2015_2017/rel21.2/Moriond_February2018_v1/map1.txt";
  std::string egMapFile = "ElectronEfficiencyCorrection/2015_2017/rel21.2/Moriond_February2018_v2/map5.txt";
  
  PATCore::ParticleDataType::DataType data_type(PATCore::ParticleDataType::Data);
  if (!isData) {
    if (isAtlfast) data_type = PATCore::ParticleDataType::Fast;
    else data_type = PATCore::ParticleDataType::Full;
  }
  
  ANA_CHECK( m_elecSelLikelihood_LhLoose.setProperty("WorkingPoint", "LooseBLLHElectron"));
  ANA_CHECK( m_elecSelLikelihood_LhLoose.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( m_elecSelLikelihood_LhLoose.retrieve() );  
  ANA_CHECK( m_elecSelLikelihood_LhMed.setProperty("WorkingPoint", "MediumLHElectron"));
  ANA_CHECK( m_elecSelLikelihood_LhMed.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( m_elecSelLikelihood_LhMed.retrieve() );
  ANA_CHECK( m_elecSelLikelihood_LhTight.setProperty("WorkingPoint", "TightLHElectron"));
  ANA_CHECK( m_elecSelLikelihood_LhTight.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( m_elecSelLikelihood_LhTight.retrieve() );
  if(DEBUG) std::cout << " Done with electron ID OPs " << std::endl << std::endl;
 

  ANA_CHECK( m_electronSF_reco.setProperty("MapFilePath", egMapFile) );
  ANA_CHECK( m_electronSF_reco.setProperty("RecoKey", "Reconstruction"));
  ANA_CHECK( m_electronSF_reco.setProperty("CorrelationModel", "TOTAL" ) );
  ANA_CHECK( m_electronSF_reco.setProperty("OutputLevel", MSG::INFO));
  if(!isData) ANA_CHECK( m_electronSF_reco.setProperty("ForceDataType", (int)data_type));
  ANA_CHECK( m_electronSF_reco.initialize() );
  
  ANA_CHECK( m_electronSF_LhLoose.setProperty("MapFilePath", egMapFile) );
  ANA_CHECK( m_electronSF_LhLoose.setProperty("IdKey", "LooseBLayer"));
  if(!isData) ANA_CHECK( m_electronSF_LhLoose.setProperty("ForceDataType",(int)data_type));
  ANA_CHECK( m_electronSF_LhLoose.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( m_electronSF_LhLoose.initialize() );
  ANA_CHECK( m_electronSF_LhMed.setProperty("MapFilePath", egMapFile) );
  ANA_CHECK( m_electronSF_LhMed.setProperty("IdKey", "Medium"));
  if(!isData) ANA_CHECK( m_electronSF_LhMed.setProperty("ForceDataType",(int)data_type));
  // ANA_CHECK( m_electronSF_LhMed.setProperty("CorrelationModel", "SIMPLIFIED" ) );
  ANA_CHECK( m_electronSF_LhMed.setProperty("CorrelationModel", "TOTAL" ) );
  ANA_CHECK( m_electronSF_LhMed.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( m_electronSF_LhMed.initialize() );
  ANA_CHECK( m_electronSF_LhTight.setProperty("MapFilePath", egMapFile) );
  ANA_CHECK( m_electronSF_LhTight.setProperty("IdKey", "Tight"));
  ANA_CHECK( m_electronSF_LhTight.setProperty("OutputLevel", MSG::INFO));
  if(!isData) ANA_CHECK( m_electronSF_LhTight.setProperty("ForceDataType",(int)data_type));
  ANA_CHECK( m_electronSF_LhTight.initialize() );
  if(DEBUG) std::cout << "Electron reco & ID efficiency tools are initialized"  << std::endl << std::endl;


  /// Scale factor needed for the mis-id tool ( https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EgammaChargeMisIdentificationTool )
  std::string inputFile_CFID("ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/charge_misID/ChargeCorrectionSF.Medium_FixedCutTight.root");
  ANA_CHECK( m_electronSF_CFID.setProperty("CorrectionFileName", inputFile_CFID) );
  if(!isData) ANA_CHECK( m_electronSF_CFID.setProperty("ForceDataType",(int)data_type));
  ANA_CHECK( m_electronSF_CFID.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( m_electronSF_CFID.retrieve() );
  
  /// Scale factor needed for the Charge Flip Tagger (killer, https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ElectronChargeFlipTaggerTool ) 
  std::vector< std::string > corrFileNameList_CFT;
  corrFileNameList_CFT.push_back(PathResolverFindCalibFile("ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/charge_misID/efficiencySF.ChargeID.MediumLLH_d0z0_v11_isolFixedCutTight_MediumCFT.root"));
  ANA_CHECK( m_electronSF_CFT.setProperty("CorrectionFileNameList",corrFileNameList_CFT) );
  ANA_CHECK( m_electronSF_CFT.setProperty("CorrelationModel", "TOTAL" ) );
  if(!isData) ANA_CHECK( m_electronSF_CFT.setProperty("ForceDataType",(int)data_type));
  ANA_CHECK( m_electronSF_CFT.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( m_electronSF_CFT.retrieve() );  
  if(DEBUG) std::cout << "Charge flip SFs taken "  << std::endl << std::endl;

  
  std::string trainingfile    = PathResolverFindCalibFile("ElectronPhotonSelectorTools/ChargeID/ECIDS_20161125for2017Moriond.root");
  float BDT_OP=0; /// we don't care about the BDT OP, we want to store the BDT weight and apply the cut offline
  ANA_CHECK( CFTToolmedium.setProperty("TrainingFile",trainingfile) );
  ANA_CHECK( CFTToolmedium.setProperty("WorkingPoint", "MediumLHElectron"));
  ANA_CHECK( CFTToolmedium.setProperty("CutOnBDT",BDT_OP) );
  ANA_CHECK( CFTToolmedium.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( CFTToolmedium.retrieve() );  ///initializes the tool
  ///  std::string trainingfile="SS3LAnalysis/CFT_tight.root";
  ///  std::string trainingfile    = PathResolverFindCalibFile("ElectronPhotonSelectorTools/ChargeID/ECIDS_20161125for2017Moriond.root");
  BDT_OP=0; /// we don't care about the BDT OP, we want to store the BDT weight and apply the cut offline
  ANA_CHECK( CFTToolLoose.setProperty("TrainingFile",trainingfile) );
  ANA_CHECK( CFTToolLoose.setProperty("WorkingPoint", "LooseLHElectron"));
  ANA_CHECK( CFTToolLoose.setProperty("CutOnBDT",BDT_OP) );
  ANA_CHECK( CFTToolLoose.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( CFTToolLoose.retrieve() );  ///initializes the tool
  if(DEBUG) std::cout << "Charge Flip related tools initialized... "  << std::endl << std::endl; 

   /// electrons - iso sf  
  ANA_CHECK( m_electronSF_isoLhMed.setProperty("MapFilePath", egMapFile) );
  ANA_CHECK( m_electronSF_isoLhMed.setProperty("IdKey", "Medium"));
  ANA_CHECK( m_electronSF_isoLhMed.setProperty("IsoKey", "FixedCutTight"));
  ANA_CHECK( m_electronSF_isoLhMed.setProperty("CorrelationModel", "TOTAL" ) );
  if(!isData) ANA_CHECK( m_electronSF_isoLhMed.setProperty("ForceDataType",(int)data_type));
  ANA_CHECK( m_electronSF_isoLhMed.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( m_electronSF_isoLhMed.initialize() ); 
  if(DEBUG) std::cout << "Electron iso efficiency tool is initialized"  << std::endl << std::endl;
  
  

  /// ////////////////////////////////////////////////////////////////////////////////////////
  /// Initialise muon selector tools
  
  ///  muonSelectionTool->msg().setLevel( MSG::VERBOSE );
  ANA_CHECK( m_muonSelectionTool.setProperty( "MaxEta", 2.5) );
  ANA_CHECK( m_muonSelectionTool.setProperty( "MuQuality", int(xAOD::Muon::Tight) ) );
  ANA_CHECK( m_muonSelectionTool.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( m_muonSelectionTool.initialize() );
  
  ANA_CHECK( m_muonEffSFs.setProperty( "WorkingPoint", "Medium" ) );
  ANA_CHECK( m_muonEffSFs.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( m_muonEffSFs.initialize() );
  
  ANA_CHECK( m_muonISOSFs.setProperty( "WorkingPoint", "FixedCutTightTrackOnlyIso" ) );
  ANA_CHECK( m_muonISOSFs.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( m_muonISOSFs.initialize() );
  if(DEBUG) std::cout << "muon tools initialized... "  << std::endl << endl; 
  

  
  /// ////////////////////////////////////////////////////////////////////////////////////////
  /// Isolation
  ANA_CHECK( isoLooseTO.setProperty("ElectronWP","LooseTrackOnly") );
  ANA_CHECK( isoLooseTO.setProperty("MuonWP","LooseTrackOnly") );
  ANA_CHECK( isoLooseTO.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( isoLooseTO.initialize() ); 
  ANA_CHECK( isoLoose.setProperty("ElectronWP","Loose") );
  ANA_CHECK( isoLoose.setProperty("MuonWP","Loose") );
  ANA_CHECK( isoLoose.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( isoLoose.initialize() ); 
  ANA_CHECK( isoTight.setProperty("ElectronWP","Tight") );
  ANA_CHECK( isoTight.setProperty("MuonWP","Tight") );
  ANA_CHECK( isoTight.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( isoTight.initialize() ); 

  ANA_CHECK( isoGrad.setProperty("ElectronWP","Gradient") );
  ANA_CHECK( isoGrad.setProperty("MuonWP","Gradient") );
  ANA_CHECK( isoGrad.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( isoGrad.initialize() ); 
  ANA_CHECK( isoGradLoose.setProperty("ElectronWP","GradientLoose") );
  ANA_CHECK( isoGradLoose.setProperty("MuonWP","GradientLoose") );
  ANA_CHECK( isoGradLoose.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( isoGradLoose.initialize() ); 

  std::vector< std::pair<xAOD::Iso::IsolationType, std::string> > myCuts;
  myCuts.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::ptvarcone20, "0.1713*x+88.71"));
  myCuts.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::topoetcone20, "0.1713*x+88.71"));
  std::vector< std::pair<xAOD::Iso::IsolationType, std::string> > myCutsMu;
  myCutsMu.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::ptvarcone30, "0.1713*x+88.71"));
  myCutsMu.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::topoetcone20, "0.1713*x+88.71"));
  ANA_CHECK( isoGradCustom.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( isoGradCustom.initialize() );
  ANA_CHECK( isoGradCustom->addUserDefinedWP("GradientCustom", xAOD::Type::Muon, myCutsMu));
  ANA_CHECK( isoGradCustom->addUserDefinedWP("GradientCustom", xAOD::Type::Electron, myCuts));

  std::vector< std::pair<xAOD::Iso::IsolationType, std::string> > myCutsT;
  myCutsT.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::ptvarcone20, "0.2283*x+85.28"));
  myCutsT.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::topoetcone20, "0.2283*x+85.28"));
  std::vector< std::pair<xAOD::Iso::IsolationType, std::string> > myCutsTMu;
  myCutsTMu.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::ptvarcone30, "0.2283*x+85.28"));
  myCutsTMu.push_back(std::make_pair<xAOD::Iso::IsolationType, std::string>(xAOD::Iso::topoetcone20, "0.2283*x+85.28"));
  ANA_CHECK( isoGradCustomTight.setProperty("OutputLevel", MSG::INFO));
  ANA_CHECK( isoGradCustomTight.initialize() );
  ANA_CHECK( isoGradCustomTight->addUserDefinedWP("GradientCustomTight", xAOD::Type::Electron, myCutsT));
  ANA_CHECK( isoGradCustomTight->addUserDefinedWP("GradientCustomTight", xAOD::Type::Muon, myCutsTMu));

  if(DEBUG) std::cout << "isolation WPs ini done"  << std::endl << endl; 
  
  
  
  /// //////////////////////////////////////////////////////////////////////////////////////// 
  /// Trigger efficiency/scale factor CP tools for electrons and muons
  /// For property 'ListOfLegsPerTool':
  ToolHandleArray<CP::IMuonTriggerScaleFactors> muonTools; //!
  ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronEffTools; //!
  ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronSFTools; //!
  std::map<std::string,std::string> legsPerTool;
  enum{ cLEGS, cKEY };
  
    
  /// ELECTRONS
  /// 2017 data, Option (4): use 2e17_lhloose||2e24_lhvloose for RUNS when 2e17_lhvloose is unprescaled, use 2e24_lhvloose for runs when 2e17_lhvloose is prescaled.
  vector<std::array<string,2>> toolConfigs = 
  {
	/// {<list of trigger legs>, <key in map file>}
	{"e17_lhloose, e17_lhloose_nod0", "MULTI_L_2015_e17_lhloose_2016_e17_lhloose_nod0_2017_e17_lhloose_nod0"},
	{"e12_lhloose_L1EM10VH, e17_lhvloose_nod0, e24_lhvloose_nod0_L1EM20VH", "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0_2017_e24_lhvloose_nod0_L1EM20VH"},
	// {"e17_lhvloose_nod0_L1EM15VHI", "DI_E_2017_e17_lhvloose_nod0_L1EM15VHI"}   /// not available yet, pay attention at the names once available
  };

  std::string triggerEleIso = "FixedCutTight"; 
    
  for(auto& cfg : toolConfigs) /// one instance per trigger leg x working point
  {
	for(int j=0;j<2;++j) /// two instances: 0 -> MC efficiencies, 1 -> SFs
	{
	  string name = "AsgElectronEfficiencyCorrectionTool/"
		   		  + ((j? "SS3Lep_ElTrigEff_" : "SS3Lep_ElTrigSF_")
				  + std::to_string(factoryEle.size()/2));

	  auto elEffTool = factoryEle.emplace(factoryEle.end(), name);
	  elEffTool->setProperty("MapFilePath",egMapFile).ignore();
	  elEffTool->setProperty("TriggerKey",string(j?"":"Eff_") + cfg[cKEY]).ignore();
	  cout << cfg[cKEY]<< endl;
	  elEffTool->setProperty("IdKey","Medium").ignore();
	  elEffTool->setProperty("IsoKey",triggerEleIso).ignore(); 
	  elEffTool->setProperty("CorrelationModel","TOTAL").ignore();
	  if(!isData) ANA_CHECK( elEffTool->setProperty("ForceDataType",(int)data_type));
	  ANA_CHECK( elEffTool->setProperty("OutputLevel", MSG::INFO));
	  ANA_CHECK( elEffTool->initialize() );
	  
	  auto& handles = (j? electronSFTools : electronEffTools);
      handles.push_back(elEffTool->getHandle());
      /// Safer to retrieve the name from the final ToolHandle, it might be
      /// prefixed (by the parent tool name) when the handle is copied
      name = handles[handles.size()-1].name();
	  cout << name << endl;
      legsPerTool[name] = cfg[cLEGS];
	}
  }

  /// MUONS  
  muonTool.setTypeAndName("CP::MuonTriggerScaleFactors/SS3Lep_MuonTrigEff");
  muonTool.setProperty("CalibrationRelease", "180312_TriggerUpdate").ignore();
  muonTool.setProperty("MuonQuality", "Medium").ignore();
  muonTool.setProperty("AllowZeroSF", true);
  /// muonTool.setProperty("useRel207", true).ignore();
  /// muonTool.setProperty("Year","2015").ignore();
  ANA_CHECK( muonTool.setProperty("OutputLevel", MSG::INFO));
  if(muonTool.initialize() != StatusCode::SUCCESS)
  {
    Error(APP_NAME, "Unable to initialize the muon CP tool!");
  }
  muonTools.push_back(muonTool.getHandle());
 
  /// Configure the trigger SF tools
  myTriggerSFTool.setProperty("ElectronEfficiencyTools", electronEffTools).ignore();
  myTriggerSFTool.setProperty("ElectronScaleFactorTools", electronSFTools).ignore();
  myTriggerSFTool.setProperty("MuonTools", muonTools).ignore();
  
  std::map<string, string> triggers;
  /// https://gitlab.cern.ch/atlas/athena/blob/21.2/Trigger/TrigAnalysis/TrigGlobalEfficiencyCorrection/examples/TrigGlobEffCorrExample2.cxx
  triggers["2015"] = "2e12_lhloose_L12EM10VH"
			   "|| mu18_mu8noL1"
			   "|| e17_lhloose_mu14";
			   
  triggers["2016"] = "2e17_lhvloose_nod0"
			   "|| e17_lhloose_nod0_mu14"
			   "|| mu22_mu8noL1";
			   
  /// for 2017 data:  Option (4): use 2e17_lhloose||2e24_lhvloose for RUNS when 2e17_lhvloose is unprescaled, use 2e24_lhvloose for runs when 2e17_lhvloose is prescaled.
  /// https://its.cern.ch/jira/browse/ATR-17746 
  /// the list of runs on which the L1_2EM15VHI was accidentally prescaled is
  /// 328374,328333,328263,328221,328099,328042,328017,327862,327860,327764,327761,327745,327662,327636,327582,327490,327342,327265,327103
  // put them back in once the SFs are available
  // triggers["327103-328374"] = "2e24_lhvloose_nod0"; 				/// ["firstPrescaledRun-lastPrescaledRun"]
  // triggers["325713-327057"] = "2e17_lhvloose_nod0_L12EM15VHI"		/// ["firstUnprescaledRun-lastUnprescaledRun"]
						// "|| 2e24_lhvloose_nod0"; 
  // triggers["328393-340453"] = "2e17_lhvloose_nod0_L12EM15VHI"		/// ["firstUnprescaledRun-lastUnprescaledRun"]
					    // "|| 2e24_lhvloose_nod0"; 

  myTriggerSFTool.setProperty("TriggerCombination", triggers).ignore(); 
  myTriggerSFTool.setProperty("ListOfLegsPerTool", legsPerTool).ignore();

  ///if(DEBUG) myTriggerSFTool.setProperty("OutputLevel", MSG::DEBUG).ignore();
  ANA_CHECK( myTriggerSFTool.setProperty("OutputLevel", MSG::INFO));
  if(myTriggerSFTool.initialize() != StatusCode::SUCCESS)
  {
    Error(APP_NAME, "Unable to initialize the TrigGlob tool!");
  }
  
	
  if(DEBUG) std::cout << "Done with trigger SFs"  << std::endl << std::endl;
  
  
  
  /// //////////////////////////////////////////////////////////////////////////////////////// 
  
  TFile *file = wk()->getOutputFile ("output");
  MyTree = new TTree ("AnaNtup", "AnaNtup");
  MyTree->SetDirectory (file);  

  for(int t=0; t<nTrig;t++){
    trig_map.insert (std::pair<std::string, bool>(nameTrig[t].Data(), false));
    MyTree->Branch(nameTrig[t].Data(), &trig_map[nameTrig[t].Data()]);
  }

  /// https://root.cern.ch/doc/master/classTTree.html
  
  /// general 
  MyTree->Branch("EventNumber",&tEventNumber,"EventNumber/l");
  MyTree->Branch("ChannelNumber",&tChannelNumber,"ChannelNumber/I");
  MyTree->Branch("AvgMu",&tAvgMu,"AvgMu/D");
  MyTree->Branch("EventWeight",&EventWeight,"EventWeight/D");
  MyTree->Branch("PRWWeight",&tPRWWeight,"PRWWeight/D");
  MyTree->Branch("TriggerDileptonSF",&tTriggerDileptonSF,"TriggerDileptonSF/D");
  MyTree->Branch("bcid",&bcid,"bcid/I");
  MyTree->Branch("LB",&LB,"LB/I");
  MyTree->Branch("passGRL",&tpassGRL,"passGRL/I");
  MyTree->Branch("RunNb",&RunNb,"RunNb/I");
  MyTree->Branch("PRWrandomRunNumber",&tPRWrandomRunNumber,"PRWrandomRunNumber/i"); 
  MyTree->Branch("DetError",&tDetError,"DetError/I");
  MyTree->Branch("Nvtx",&Nvtx,"Nvtx/I");
  /// MyTree->Branch("PV_x",&primvertex_x,"PV_x/F");
  /// MyTree->Branch("PV_y",&primvertex_y,"PV_y/F");
  MyTree->Branch("PV_z",&primvertex_z,"PV_z/F");

  
  /// muons 
  MyTree->Branch("NMu", &NMu, "NMu/I");
  MyTree->Branch("Mu_eta",&Mu_eta);
  MyTree->Branch("Mu_phi",&Mu_phi);
  MyTree->Branch("Mu_pT",&Mu_pT);
  MyTree->Branch("Mu_charge",&Mu_charge);
  MyTree->Branch("Mu_sigd0",&Mu_sigd0);
  MyTree->Branch("Mu_d0pvtx",&Mu_d0pvtx);
  MyTree->Branch("Mu_d0pvtxerr",&Mu_d0pvtxerr);
  MyTree->Branch("Mu_z0sinTheta",&Mu_z0pvtx);
  MyTree->Branch("Mu_isBad",&Mu_isBad);
  MyTree->Branch("Mu_isCosmic",&Mu_isCosmic);
  MyTree->Branch("Mu_passOR",&Mu_passOR);
  MyTree->Branch("Mu_isTight",&Mu_isTight);
  MyTree->Branch("Mu_isSig2017",&Mu_isSig);
  MyTree->Branch("Mu_PromptLepTaggerBDTweight",&Mu_PromptLepTaggerBDTweight);
  if(!isData)
  {
    MyTree->Branch("Mu_type",&Mu_type);
    MyTree->Branch("Mu_origin",&Mu_origin);
  }
  /// muon isolation 
  MyTree->Branch("Mu_ptcone20",&Mu_ptcone20);
  MyTree->Branch("Mu_ptcone20_TightTTVA_pt500",&Mu_ptcone20_TightTTVA_pt500);
  MyTree->Branch("Mu_ptcone20_TightTTVA_pt1000",&Mu_ptcone20_TightTTVA_pt1000);
  MyTree->Branch("Mu_ptcone30",&Mu_ptcone30);
  MyTree->Branch("Mu_ptcone40",&Mu_ptcone40);
  MyTree->Branch("Mu_ptvarcone20",&Mu_ptvarcone20);
  MyTree->Branch("Mu_ptvarcone30",&Mu_ptvarcone30);
  MyTree->Branch("Mu_ptvarcone30_TightTTVA_pt500",&Mu_ptvarcone30_TightTTVA_pt500);
  MyTree->Branch("Mu_ptvarcone30_TightTTVA_pt1000",&Mu_ptvarcone30_TightTTVA_pt1000);
  MyTree->Branch("Mu_ptvarcone40",&Mu_ptvarcone40);
  MyTree->Branch("Mu_topoetcone20",&Mu_topoetcone20);
  MyTree->Branch("Mu_topoetcone30",&Mu_topoetcone30);
  MyTree->Branch("Mu_topoetcone40",&Mu_topoetcone40);
  MyTree->Branch("Mu_neflowisol20",&Mu_neflowisol20);
  MyTree->Branch("Mu_passIsoLooseTO",&Mu_passIsoLooseTO);
  MyTree->Branch("Mu_passIsoLoose",&Mu_passIsoLoose);
  MyTree->Branch("Mu_passIsoTight",&Mu_passIsoTight);
  MyTree->Branch("Mu_passIsoGrad", &Mu_passIsoGrad);
  MyTree->Branch("Mu_passIsoGradCustomTight", &Mu_passIsoGradCustomTight);
  MyTree->Branch("Mu_passIsoGradCustom", &Mu_passIsoGradCustom);
  MyTree->Branch("Mu_passIsoGradLoose", &Mu_passIsoGradLoose);
  /// muon reco & ID scale factors 
  MyTree->Branch("Mu_SFw",&Mu_SFw);
  MyTree->Branch("Mu_IsoSFw",&Mu_IsoSFw);
  MyTree->Branch("Mu_StatUncReco",&Mu_StatUncReco);
  MyTree->Branch("Mu_SystUncReco",&Mu_SystUncReco);
  MyTree->Branch("Mu_StatUncReco_LOWPT",&Mu_StatUncReco_LOWPT);
  MyTree->Branch("Mu_SystUncReco_LOWPT",&Mu_SystUncReco_LOWPT);
  MyTree->Branch("Mu_StatUncISO",&Mu_StatUncISO);
  MyTree->Branch("Mu_SystUncISO",&Mu_SystUncISO);
  /// muon trigger matching
  MyTree->Branch("Mu_trigMatch_mu26_ivarmedium", &Mu_TrigMatch_mu26_ivarmedium );
  MyTree->Branch("Mu_trigMatch_mu20_iloose_L1MU15", &Mu_TrigMatch_mu20_iloose_L1MU15 );
  MyTree->Branch("Mu_trigMatch_mu40", &Mu_TrigMatch_mu40);
  MyTree->Branch("Mu_trigMatch_mu50", &Mu_TrigMatch_mu50);
  MyTree->Branch("Mu_trigMatch_mu8noL1", &Mu_TrigMatch_mu8noL1);
  MyTree->Branch("Mu_trigMatch_mu14", &Mu_TrigMatch_mu14);
  MyTree->Branch("Mu_trigMatch_mu18", &Mu_TrigMatch_mu18);
  MyTree->Branch("Mu_trigMatch_mu18_mu8noL1", &Mu_TrigMatch_mu18_mu8noL1);
  MyTree->Branch("Mu_trigMatch_e17_lhloose_mu14", &Mu_TrigMatch_e17_lhloose_mu14);
  MyTree->Branch("Mu_trigMatch_e17_lhloose_nod0_mu14", &Mu_TrigMatch_e17_lhloose_nod0_mu14);
  MyTree->Branch("Mu_trigMatch_mu20_mu8noL1", &Mu_TrigMatch_mu20_mu8noL1);
  MyTree->Branch("Mu_trigMatch_mu22_mu8noL1", &Mu_TrigMatch_mu22_mu8noL1);
  MyTree->Branch("Mu_trigMatch_mu24_iloose"          , &Mu_TrigMatch_mu24_iloose );
  MyTree->Branch("Mu_trigMatch_mu24_ivarloose"	     , &Mu_TrigMatch_mu24_ivarloose	);  
  MyTree->Branch("Mu_trigMatch_mu24_iloose_L1MU15"   , &Mu_TrigMatch_mu24_iloose_L1MU15	); 
  MyTree->Branch("Mu_trigMatch_mu24_ivarloose_L1MU15", &Mu_TrigMatch_mu24_ivarloose_L1MU15);
  MyTree->Branch("Mu_trigMatchPair_mu18_mu8noL1", &Mu_TrigMatchPair_mu18_mu8noL1);
  MyTree->Branch("Mu_trigMatchPair_mu20_mu8noL1", &Mu_TrigMatchPair_mu20_mu8noL1);
  MyTree->Branch("Mu_trigMatchPair_mu22_mu8noL1", &Mu_TrigMatchPair_mu22_mu8noL1);
  
  
  /// electrons
  MyTree->Branch("NEl",&NEl,"NEl/I");
  MyTree->Branch("El_eta",&El_eta);
  MyTree->Branch("El_etaclus",&El_etaclus);
  MyTree->Branch("El_phi",&El_phi);
  MyTree->Branch("El_pT",&El_pT);
  MyTree->Branch("El_E",&El_E);
  MyTree->Branch("El_charge",&El_charge);
  MyTree->Branch("El_sigd0",&El_sigd0);
  MyTree->Branch("El_d0pvtx",&El_d0pvtx);
  MyTree->Branch("El_d0pvtxerr",&El_d0pvtxerr);
  MyTree->Branch("El_z0sinTheta",&El_z0pvtx);
  MyTree->Branch("El_isLooseAndBLayerLH_baseline",&El_isLooseLH_baseline);
  MyTree->Branch("El_isLooseAndBLayerLH_fromTool", &El_isLooseLH_fromTool);
  MyTree->Branch("El_isMediumLH",&El_isMediumLH);
  MyTree->Branch("El_isTightLH",&El_isTightLH);
  MyTree->Branch("El_isSigNoCFT2017",&El_isSig);
  MyTree->Branch("El_nBLayerHits",&El_nBLayerHits);
  MyTree->Branch("El_expectBLayerHit",&El_expectBLayerHit);
  MyTree->Branch("El_passOR",&El_passOR); 
  MyTree->Branch("El_passChargeFlipTaggerBDTmedium", &El_passChargeFlipTaggerBDTmedium);
  MyTree->Branch("El_passChargeFlipTaggerBDTloose", &El_passChargeFlipTaggerBDTloose);
  MyTree->Branch("El_PromptLepTaggerBDTweight", &El_PromptLepTaggerBDTweight );
  if(!isData)
  {
    MyTree->Branch("El_truthType",&El_truthType);
    MyTree->Branch("El_truthOrigin",&El_truthOrigin); 
    MyTree->Branch("El_truthPdgId",&El_truthPdgId); 
    MyTree->Branch("El_bkgTruthType",&El_bkgTruthType);
    MyTree->Branch("El_bkgTruthOrigin",&El_bkgTruthOrigin); 
    MyTree->Branch("El_bkgMotherPdgId",&El_bkgMotherPdgId);
    MyTree->Branch("El_firstEgMotherTruthType",&El_firstEgMotherTruthType); 
    MyTree->Branch("El_firstEgMotherTruthOrigin",&El_firstEgMotherTruthOrigin);
    MyTree->Branch("El_firstEgMotherPdgId",&El_firstEgMotherPdgId);
	MyTree->Branch("El_lastEgMotherTruthType",&El_lastEgMotherTruthType); 
    MyTree->Branch("El_lastEgMotherTruthOrigin",&El_lastEgMotherTruthOrigin);
    MyTree->Branch("El_lastEgMotherPdgId",&El_lastEgMotherPdgId);
    MyTree->Branch("El_chFlip",&El_chFlip);
  }
  
  /// electron isolation
  MyTree->Branch("El_ptcone20",&El_ptcone20);
  MyTree->Branch("El_ptcone20_TightTTVA_pt500",&El_ptcone20_TightTTVA_pt500);
  MyTree->Branch("El_ptcone20_TightTTVA_pt1000",&El_ptcone20_TightTTVA_pt1000);
  MyTree->Branch("El_ptcone30",&El_ptcone30);
  MyTree->Branch("El_ptcone40",&El_ptcone40);
  MyTree->Branch("El_ptvarcone20",&El_ptvarcone20);
  MyTree->Branch("El_ptvarcone20_TightTTVA_pt1000",&El_ptvarcone20_TightTTVA_pt1000);
  MyTree->Branch("El_ptvarcone30",&El_ptvarcone30);
  MyTree->Branch("El_ptvarcone30_TightTTVA_pt1000",&El_ptvarcone30_TightTTVA_pt1000);
  MyTree->Branch("El_ptvarcone40",&El_ptvarcone40);
  MyTree->Branch("El_topoetcone20",&El_topoetcone20);
  MyTree->Branch("El_topoetcone30",&El_topoetcone30);
  MyTree->Branch("El_topoetcone40",&El_topoetcone40);
  MyTree->Branch("El_neflowisol20",&El_neflowisol20);
  MyTree->Branch("El_passIsoLooseTO",&El_passIsoLooseTO);
  MyTree->Branch("El_passIsoLoose",&El_passIsoLoose);
  MyTree->Branch("El_passIsoTight",&El_passIsoTight);
  MyTree->Branch("El_passIsoGrad", &El_passIsoGrad);
  MyTree->Branch("El_passIsoGradCustomTight", &El_passIsoGradCustomTight);
  MyTree->Branch("El_passIsoGradCustom", &El_passIsoGradCustom);
  MyTree->Branch("El_passIsoGradLoose", &El_passIsoGradLoose);
  /// electrons reco & ID & Iso scale & trigger factors
  MyTree->Branch("El_SFwUncReco", &El_SFwUncReco);
  MyTree->Branch("El_SFwTightLH", &El_SFwTightLH);
  MyTree->Branch("El_SFwMediumLH",&El_SFwMediumLH);
  MyTree->Branch("El_SFwUncMediumLH",&El_SFwUncMediumLH);
  MyTree->Branch("El_SFwLooseAndBLayerLH", &El_SFwLooseLH);
  MyTree->Branch("El_SFwUncLooseAndBLayerLH", &El_SFwUncLooseLH);
  MyTree->Branch("El_SFweightCFT", &El_SFweightCFT);  
  MyTree->Branch("El_SFUncweightCFT", &El_SFUncweightCFT);  
  MyTree->Branch("El_SFweightCFID", &El_SFweightCFID);  
  MyTree->Branch("El_SFStatweightCFID", &El_SFStatweightCFID);  
  MyTree->Branch("El_SFSystweightCFID", &El_SFSystweightCFID);  
  MyTree->Branch("El_IsoSFwMediumLH", &El_IsoSFwMediumLH);
  MyTree->Branch("El_IsoSFwUncMediumLH", &El_IsoSFwUncMediumLH);
  /// electron trigger matching
  MyTree->Branch("El_trigMatch_e12_lhloose_L1EM10VH", &El_TrigMatch_e12_lhloose_L1EM10VH );
  MyTree->Branch("El_trigMatch_e17_lhloose", &El_TrigMatch_e17_lhloose );
  MyTree->Branch("El_trigMatch_e24_lhmedium_L1EM20VH", &El_TrigMatch_e24_lhmedium_L1EM20VH);
  MyTree->Branch("El_trigMatch_e24_lhmedium_iloose_L1EM20VH", &El_TrigMatch_e24_lhmedium_iloose_L1EM20VH);
  MyTree->Branch("El_trigMatch_e24_lhmedium_nod0_ivarloose", &El_TrigMatch_e24_lhmedium_nod0_ivarloose );
  MyTree->Branch("El_trigMatch_e24_lhtight_nod0_ivarloose",  &El_TrigMatch_e24_lhtight_nod0_ivarloose );
  MyTree->Branch("El_trigMatch_e26_lhtight_nod0_ivarloose", &El_TrigMatch_e26_lhtight_nod0_ivarloose);
  MyTree->Branch("El_trigMatch_e60_lhmedium", &El_TrigMatch_e60_lhmedium );
  MyTree->Branch("El_trigMatch_e60_lhmedium_nod0", &El_TrigMatch_e60_lhmedium_nod0 );
  MyTree->Branch("El_trigMatch_2e12_lhloose_L12EM10VH", &El_TrigMatch_2e12_lhloose_L12EM10VH);
  MyTree->Branch("El_trigMatch_2e15_lhloose_L12EM10VH", &El_TrigMatch_2e15_lhloose_L12EM10VH);
  MyTree->Branch("El_trigMatch_2e15_lhvloose_L12EM13VH",&El_TrigMatch_2e15_lhvloose_L12EM13VH);
  MyTree->Branch("El_trigMatch_2e15_lhvloose_nod0_L12EM13VH", &El_TrigMatch_2e15_lhvloose_nod0_L12EM13VH);
  MyTree->Branch("El_trigMatch_2e17_lhvloose_nod0", &El_TrigMatch_2e17_lhvloose_nod0);
  MyTree->Branch("El_TrigMatch_2e17_lhvloose_nod0_L12EM15VHI", &El_TrigMatch_2e17_lhvloose_nod0_L12EM15VHI);
  MyTree->Branch("El_TrigMatch_2e24_lhvloose_nod0", &El_TrigMatch_2e24_lhvloose_nod0);
  MyTree->Branch("El_trigMatch_e17_lhloose_mu14", &El_TrigMatch_e17_lhloose_mu14);
  MyTree->Branch("El_trigMatch_e17_lhloose_nod0_mu14", &El_TrigMatch_e17_lhloose_nod0_mu14);

  /// JETS
  MyTree->Branch("NJet",&NJet,"NJet/I");
  MyTree->Branch("Jet_eta",&Jet_eta);
  MyTree->Branch("Jet_phi",&Jet_phi);
  MyTree->Branch("Jet_pT",&Jet_pT);
  MyTree->Branch("Jet_E",&Jet_E);
  MyTree->Branch("Jet_nTrk",&Jet_nTrk);
  MyTree->Branch("Jet_quality",&Jet_quality);
  MyTree->Branch("Jet_EMFrac",&Jet_EMFrac);
  MyTree->Branch("Jet_HECFrac",&Jet_HECFrac);
  MyTree->Branch("Jet_JVT",&Jet_JVT);
  MyTree->Branch("Jet_JVTsf",&Jet_JVTsf); /// per jet
  MyTree->Branch("totalJVTsf",&totalJVTsf,"totalJVTsf/F");
  MyTree->Branch("Jet_MV2c10",&Jet_MV2c10);
  MyTree->Branch("Jet_SFw",&Jet_SFw);
  MyTree->Branch("Jet_passOR",&Jet_passOR);
  if(!isData)
  {
    MyTree->Branch("Jet_ConeTruthLabel",  &Jet_ConeTruthLabel);
    MyTree->Branch("Jet_PartonTruthLabel",&Jet_PartonTruthLabel);
    MyTree->Branch("Jet_HadronConeExclTruthLabel",&Jet_HadronConeExclTruthLabel);
    MyTree->Branch("Jet_deltaR",&Jet_deltaR);
  }
  

  /// Missing Energy
  MyTree->Branch("Etmiss_TST_Etx",&Etmiss_TST_Etx,"Etmiss_TST_Etx/F");
  MyTree->Branch("Etmiss_TST_Ety",&Etmiss_TST_Ety,"Etmiss_TST_Ety/F");
  MyTree->Branch("Etmiss_TST_Et", &Etmiss_TST_Et, "Etmiss_TST_Et/F");

  
  /// Truth business
  if(!isData)
  { 
	/// truth missing transverse energy
	MyTree->Branch("Etmiss_Truth_Etx",&Etmiss_Truth_Etx,"Etmiss_Truth_Etx/F");
	MyTree->Branch("Etmiss_Truth_Ety",&Etmiss_Truth_Ety,"Etmiss_Truth_Ety/F");
	MyTree->Branch("Etmiss_Truth_Et", &Etmiss_Truth_Et, "Etmiss_Truth_Et/F");
  
	/// truthJets from "AntiKt4TruthJets" collection
  	MyTree->Branch("NTruthAntiktJet",&NTruthAntiktJet,"NTruthAntiktJet/I");
    MyTree->Branch("TruthAntiktJet_eta",&TruthAntiktJet_eta);
    MyTree->Branch("TruthAntiktJet_phi",&TruthAntiktJet_phi);
    MyTree->Branch("TruthAntiktJet_pT", &TruthAntiktJet_pT);
    MyTree->Branch("TruthAntiktJet_E",  &TruthAntiktJet_E);
    MyTree->Branch("TruthAntiktJet_ConeTruthLabel",  &TruthAntiktJet_ConeTruthLabel);
    MyTree->Branch("TruthAntiktJet_HadronConeExclTruthLabelID",  &TruthAntiktJet_HadronConeExclTruthLabelID);
    MyTree->Branch("TruthAntiktJet_PartonTruthLabel",&TruthAntiktJet_PartonTruthLabel);
	
	/// need these branches for ttV HF / tt Hf studies (2b / 3b regions)
	MyTree->Branch("TruthAntiktJetJet_ClassHF",  &TruthAntiktJetJet_ClassHF);
	MyTree->Branch("extraB",&extraB,"extraB/I");
	MyTree->Branch("extraC",&extraC,"extraC/I");
	
	/// jets from "TruthParticles" collection
    MyTree->Branch("NTruthJet",&NTruthJet,"NTruthJet/I");
    MyTree->Branch("TruthJet_eta",&TruthJet_eta);
    MyTree->Branch("TruthJet_phi",&TruthJet_phi);
    MyTree->Branch("TruthJet_pT", &TruthJet_pT);
    MyTree->Branch("TruthJet_E",  &TruthJet_E);
    MyTree->Branch("TruthJet_id",  &TruthJet_id);
    MyTree->Branch("TruthJet_origin",&TruthJet_origin);

	/// only real lepton (from t/W/Z/SUSY/etc) from "TruthParticles" collection
    MyTree->Branch("NTruthRealL",  &NTruthRealL,"NTruthRealL/I");
    MyTree->Branch("TruthRealL_eta",&TruthRealL_eta);
    MyTree->Branch("TruthRealL_phi",&TruthRealL_phi);
    MyTree->Branch("TruthRealL_pT", &TruthRealL_pT);
    MyTree->Branch("TruthRealL_id", &TruthRealL_id);
    MyTree->Branch("TruthRealL_origin", &TruthRealL_origin);
  }

  /// susy particles
 if(!isData)
 {  
    MyTree->Branch("SUSY_Spart_pdgId1",&pdgid1,"SUSY_Spart_pdgId1/I");
    MyTree->Branch("SUSY_Spart_pdgId2",&pdgid2,"SUSY_Spart_pdgId2/I");
    /// 
    MyTree->Branch("SUSY_Gluino_decay1",&gluDecay1,"SUSY_Gluino_decay1/I");
    MyTree->Branch("SUSY_Gluino_decay2",&gluDecay2,"SUSY_Gluino_decay2/I");
  }
  
   /// other
  if(!isData)
  {
	MyTree->Branch("GenFiltHT", &GenFiltHT, "GenFiltHT/F");
    MyTree->Branch("GenFiltMET",&GenFiltMET,"GenFiltMET/F");
    
	/// PDF INFO!!!
    /// Get the Truth Event information from the event:
	MyTree->Branch("TruthX1",&TruthX1,"TruthX1/F");
	MyTree->Branch("TruthX2",&TruthX2,"TruthX2/F"); 
	/// MyTree->Branch("TruthXF1",&TruthXF1,"TruthXF1/F");
	/// MyTree->Branch("TruthXF2",&TruthXF2,"TruthXF2/F"); 
	MyTree->Branch("TruthQ",  &TruthQ,  "TruthQ/F"); 
	MyTree->Branch("TruthPDGID1",&TruthPDGID1,"TruthPDGID1/I");
	MyTree->Branch("TruthPDGID2",&TruthPDGID2,"TruthPDGID2/I");
	/// MyTree->Branch("TruthPDFID1",&TruthPDFID1,"TruthPDFID1/I");
	/// MyTree->Branch("TruthPDFID2",&TruthPDFID2,"TruthPDFID2/I");
	
	MyTree->Branch("SherpaNjetWeight",  &SherpaNjetWeight,  "SherpaNjetWeight/F");
  }
  
  Info(APP_NAME, "Tree booking done..." );   
  
  TDirectory *out_TDir = (TDirectory*) wk()->getOutputFile ("output");
  hWeights->SetDirectory(out_TDir);
  hEvents->SetDirectory(out_TDir);
  hWeightsSquared->SetDirectory(out_TDir);

  Info(APP_NAME, "Out of method..." );   

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: execute ()
{
  ///  CALLGRIND_START_INSTRUMENTATION;

  /// Here you do everything that needs to be done on every single
  /// events, e.g. read input variables, apply cuts, and fill
  /// histograms and trees.  This is where most of your actual analysis
  /// code will go.
 

  /**isMyOR = false;
  pT_treshold = 50000.; */
  
  DEBUG = false;
  static int count=0;
  bool displ = (count++<10);

  /// only for debugging!! remove this when running on grid!!!
  if(false && count > 200) return EL::StatusCode::SUCCESS;

  
  if(DEBUG && false) 
  {
	std::cout << "All OK here: exiting" <<std::endl;
	return EL::StatusCode::SUCCESS;
  }
  
  
  const char* APP_NAME = "MyxAODAnalysis::execute()";   
  if(DEBUG) Info(APP_NAME, "Start of method..." );  

  
  ///  std::vector<CP::SystematicSet>::const_iterator sysListItr;


  ///----------------------------
  /// Event information
  ///--------------------------- 
  const xAOD::EventInfo* eventInfo = nullptr;
  if( ! m_event->retrieve( eventInfo, "EventInfo").isSuccess() ){
    Error(APP_NAME, "Failed to retrieve event info collection. Exiting." );
    return EL::StatusCode::FAILURE;
  }

   if(DEBUG) Info(APP_NAME, "EventInfo retrieved..." );   

  /// check if the event is data or MC
  /// (many tools are applied either to data or MC)
  bool isMC = false;
  /// check if the event is MC
  if(eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
    isMC = true; /// can do something with this later
  }   
  
  tEventNumber = eventInfo->eventNumber();
  RunNb = eventInfo->runNumber();
  
  /// for debugging !!!!
  if(false && !(tEventNumber==199427)) return EL::StatusCode::SUCCESS; /// go to next event

  if(DEBUG)
  {
	std::cout << "Working on run/evt: "<<  RunNb << " / " << tEventNumber << std::endl;
  }
	
  LB = eventInfo->lumiBlock();
  bcid = eventInfo->bcid();
  tAvgMu = eventInfo->averageInteractionsPerCrossing();
  
  if(isMC) 
  {
	tChannelNumber = eventInfo->mcChannelNumber();
	EventWeight = (eventInfo->mcEventWeights()).at(0);
  }
  else
  {
	tChannelNumber = -1;
	EventWeight = 1;
  }
  if(DEBUG) std::cout << "tChannelNumber / EventWeight stored\n";

  /// UInt_t runNumber_forMu = 0;
  if(isMC){
    GenFiltHT  = eventInfo->auxdata<float>("GenFiltHT");
    GenFiltMET = eventInfo->auxdata<float>("GenFiltMET");

	/// I was using this with my version of PRW tool
	/// let's take the PW weight! https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ExtendedPileupReweighting#Combining_MC16a_and_MC16c_d_mult
	/// CP::SystematicSet defaultSet;
	/// m_pileupReweightingTool->applySystematicVariation(defaultSet);
	/// m_pileupReweightingTool->apply(*eventInfo, true); 
	///tPRWWeight = eventInfo->auxdecor<float>("PileupWeight"); /// or,comments this line and the one above and just use: m_pileupReweightingTool->getCombinedWeight(*eventInfo);  
	///tPRWrandomRunNumber = eventInfo->auxdata<unsigned int>("RandomRunNumber");

	/// using SUSY Tools:
	objTool->ApplyPRWTool();
	/// take the random number (you'll need this to apply the correct triggers in each data run/year)
	tPRWrandomRunNumber = objTool->GetRandomRunNumber(); /// objTool->GetRandomRunNumber(); /// eventInfo->auxdata<unsigned int>("RandomRunNumber");
	tPRWWeight = objTool->GetPileupWeight();/// eventInfo->auxdata< float >( "PileupWeight" );
	if(DEBUG) std::cout << "tPRWWeight = " << tPRWWeight << std::endl;

	/// tAvgMu = eventInfo->auxdata< float >( "corrected_averageInteractionsPerCrossing" );  // before
	tAvgMu = objTool->GetCorrectedAverageInteractionsPerCrossing();
  }
  else
  {
	tPRWWeight = 1.;
	tPRWrandomRunNumber = RunNb;
  }
  if(DEBUG) std::cout << "Done with PWT and RandomRunNumber\n";
 
 
  tpassGRL = true;
  if(!isMC){ /// it's data!
	if(!m_grl->passRunLB(*eventInfo))
	{
	  tpassGRL = false;
      return EL::StatusCode::SUCCESS; /// go to next event
    }
  }
  if(!tpassGRL) return EL::StatusCode::SUCCESS; /// no point to continue   
  if(DEBUG) std::cout << "GRL applied! \n";
  
   
  tDetError = false;
  if(!isMC)
  {
    if( (eventInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error ) || 
		(eventInfo->errorState(xAOD::EventInfo::Tile)==xAOD::EventInfo::Error ) || 
		(eventInfo->errorState(xAOD::EventInfo::SCT)==xAOD::EventInfo::Error ) || 
		(eventInfo->isEventFlagBitSet(xAOD::EventInfo::Core, 18) )  )
	{
      Info(APP_NAME,"Event %llu with detector errors", tEventNumber );   
	  tDetError = true;     
    }
  }
  if(tDetError) return EL::StatusCode::SUCCESS; /// no point to continue
  if(DEBUG) std::cout << "Done with DetError\n";
  
  
  const xAOD::Vertex* pv = objTool->GetPrimVtx();
  if(!pv) return EL::StatusCode::SUCCESS; /// if no PV, continue to avoid several crashes
	
  primvertex_z = -999;
  /// primvertex_x = 0;
  /// primvertex_y = 0;
  Nvtx = 0;
  const xAOD::VertexContainer* vertices(0);
  if( m_event->retrieve( vertices, "PrimaryVertices" ).isSuccess() ) 
  {
    for( const auto& vx : *vertices ) {
      Nvtx++;
      if(vx->vertexType() == xAOD::VxType::PriVtx){ /// Found THE vertex
		primvertex_z = vx->z(); 
		/// primvertex_x = vx->y(); 
		/// primvertex_y = vx->x(); 
      }
    }
  } else 
  { 
    Error(APP_NAME, "Failed to retrieve PV container. Exiting." );
    return EL::StatusCode::FAILURE;
  } 
  if(DEBUG) std::cout << __LINE__ << " " << Nvtx << std::endl;
  if(Nvtx==0) return EL::StatusCode::SUCCESS;

  
  ///------------
  /// Muon Block
  ///------------
  NMu=0;
  Mu_eta.clear();
  Mu_phi.clear();
  Mu_pT.clear();
  Mu_charge.clear();
  Mu_sigd0.clear();
  Mu_d0pvtx.clear();
  Mu_d0pvtxerr.clear();
  Mu_z0pvtx.clear();
  Mu_isBad.clear();
  Mu_isCosmic.clear();
  Mu_passOR.clear();
  Mu_isTight.clear();
  Mu_isSig.clear();
  Mu_PromptLepTaggerBDTweight.clear();
  /// muon truth
  Mu_type.clear();
  Mu_origin.clear();
  /// muon isolation
  Mu_ptcone20.clear();
  Mu_ptcone20_TightTTVA_pt500.clear();
  Mu_ptcone20_TightTTVA_pt1000.clear();
  Mu_ptcone30.clear();
  Mu_ptcone40.clear();
  Mu_ptvarcone20.clear();
  Mu_ptvarcone30.clear();
  Mu_ptvarcone30_TightTTVA_pt500.clear();
  Mu_ptvarcone30_TightTTVA_pt1000.clear();
  Mu_ptvarcone40.clear();
  Mu_topoetcone20.clear();
  Mu_topoetcone30.clear();
  Mu_topoetcone40.clear();
  Mu_neflowisol20.clear();
  Mu_passIsoLooseTO.clear();
  Mu_passIsoLoose.clear();
  Mu_passIsoTight.clear();
  Mu_passIsoGrad.clear();
  Mu_passIsoGradCustom.clear();
  Mu_passIsoGradCustomTight.clear();
  Mu_passIsoGradLoose.clear();
  /// muon reco & ID scale factors 
  Mu_SFw.clear();
  Mu_IsoSFw.clear();
  Mu_StatUncReco.clear();
  Mu_SystUncReco.clear();
  Mu_StatUncReco_LOWPT.clear();
  Mu_SystUncReco_LOWPT.clear();
  Mu_StatUncISO.clear();
  Mu_SystUncISO.clear();
  /// muon trigger matching
  Mu_TrigMatch_mu26_ivarmedium.clear();  
  Mu_TrigMatch_mu20_iloose_L1MU15.clear();
  Mu_TrigMatch_mu40.clear();
  Mu_TrigMatch_mu50.clear();
  Mu_TrigMatch_mu8noL1.clear();
  Mu_TrigMatch_mu14.clear();
  Mu_TrigMatch_mu18.clear();
  Mu_TrigMatch_mu18_mu8noL1.clear();
  Mu_TrigMatch_e17_lhloose_mu14.clear();
  Mu_TrigMatch_e17_lhloose_nod0_mu14.clear();
  Mu_TrigMatch_mu20_mu8noL1.clear();
  Mu_TrigMatch_mu22_mu8noL1.clear();
  Mu_TrigMatch_mu24_iloose.clear(); 
  Mu_TrigMatch_mu24_ivarloose.clear(); 
  Mu_TrigMatch_mu24_iloose_L1MU15.clear();  
  Mu_TrigMatch_mu24_ivarloose_L1MU15.clear(); 
  Mu_TrigMatchPair_mu18_mu8noL1.clear();
  Mu_TrigMatchPair_mu20_mu8noL1.clear();
  Mu_TrigMatchPair_mu22_mu8noL1.clear();
 
  /// needed for some sanity checks
  // EtaMuons .clear();
  // pTMuons  .clear();
  // MassMuons.clear();
  // PhiMuons .clear();

  
  ///----------------
  /// Electron Block
  ///----------------
  NEl=0;
  El_eta.clear();
  El_etaclus.clear();
  El_phi.clear();
  El_pT.clear();
  El_E.clear();
  El_charge.clear();
  El_sigd0.clear();
  El_d0pvtx.clear();
  El_d0pvtxerr.clear();
  El_z0pvtx.clear();
  El_isLooseLH_baseline.clear();
  El_isLooseLH_fromTool.clear();
  El_isMediumLH.clear();
  El_isTightLH.clear();
  El_isSig.clear();
  El_nBLayerHits.clear();
  El_expectBLayerHit.clear();
  El_passOR.clear();
  /// El_passChargeFlipTagger.clear();
  El_passChargeFlipTaggerBDTmedium.clear();
  El_passChargeFlipTaggerBDTloose.clear();
  El_PromptLepTaggerBDTweight.clear(); 
  /// electron truth
  El_truthType.clear();
  El_truthOrigin.clear();
  El_truthPdgId.clear();
  El_bkgTruthType.clear();
  El_bkgTruthOrigin.clear();
  El_bkgMotherPdgId.clear();
  El_firstEgMotherTruthType.clear();
  El_firstEgMotherTruthOrigin.clear();
  El_firstEgMotherPdgId.clear();
  El_lastEgMotherTruthType.clear();
  El_lastEgMotherTruthOrigin.clear();
  El_lastEgMotherPdgId.clear();
  El_chFlip.clear();
  
  /// electron isolation
  El_ptcone20.clear();
  El_ptcone20_TightTTVA_pt500.clear();
  El_ptcone20_TightTTVA_pt1000.clear();
  El_ptcone30.clear();
  El_ptcone40.clear();
  El_ptvarcone20.clear();
  El_ptvarcone20_TightTTVA_pt1000.clear();
  El_ptvarcone30.clear();
  El_ptvarcone30_TightTTVA_pt1000.clear();
  El_ptvarcone40.clear();
  El_topoetcone20.clear();
  El_topoetcone30.clear();
  El_topoetcone40.clear();
  El_neflowisol20.clear();
  El_passIsoLooseTO.clear();
  El_passIsoLoose.clear();
  El_passIsoTight.clear();
  El_passIsoGrad.clear();
  El_passIsoGradCustom.clear();
  El_passIsoGradCustomTight.clear();
  El_passIsoGradLoose.clear();
  /// electrons reco & ID & Iso & trigger scale factors
  El_SFwUncReco.clear();
  El_SFwTightLH.clear();
  El_SFwMediumLH.clear();
  El_SFwUncMediumLH.clear();
  El_SFwLooseLH.clear();
  El_SFweightCFT.clear();
  El_SFUncweightCFT.clear();
  El_SFweightCFID.clear();
  El_SFStatweightCFID.clear();
  El_SFSystweightCFID.clear();
  El_IsoSFwMediumLH.clear();
  El_IsoSFwUncMediumLH.clear();
  /// electron trigger matching  
  El_TrigMatch_e12_lhloose_L1EM10VH.clear();
  El_TrigMatch_e17_lhloose.clear();
  El_TrigMatch_e24_lhmedium_L1EM20VH.clear();
  El_TrigMatch_e24_lhmedium_iloose_L1EM20VH.clear();
  El_TrigMatch_e24_lhmedium_nod0_ivarloose.clear(); 
  El_TrigMatch_e24_lhtight_nod0_ivarloose.clear(); 
  El_TrigMatch_e26_lhtight_nod0_ivarloose.clear();
  El_TrigMatch_e60_lhmedium.clear();
  El_TrigMatch_e60_lhmedium_nod0.clear(); 
  El_TrigMatch_2e12_lhloose_L12EM10VH.clear();
  El_TrigMatch_2e15_lhloose_L12EM10VH.clear();
  El_TrigMatch_2e15_lhvloose_L12EM13VH.clear();
  El_TrigMatch_2e15_lhvloose_nod0_L12EM13VH.clear();
  El_TrigMatch_2e17_lhvloose_nod0.clear();
  El_TrigMatch_2e17_lhvloose_nod0_L12EM15VHI.clear();
  El_TrigMatch_2e24_lhvloose_nod0.clear();
  El_TrigMatch_e17_lhloose_mu14.clear();
  El_TrigMatch_e17_lhloose_nod0_mu14.clear();
  
  
  ///------------
  /// JETS
  ///------------
  NJet=0;
  Jet_eta.clear();
  Jet_phi.clear();
  Jet_pT.clear();
  Jet_E.clear();
  Jet_nTrk.clear();
  Jet_quality.clear();
  Jet_EMFrac.clear();
  Jet_HECFrac.clear();
  Jet_JVT.clear();
  Jet_JVTsf.clear();
  totalJVTsf=0;
  Jet_MV2c10.clear();
  Jet_SFw.clear();
  Jet_passOR.clear();
  /// truth jets
  Jet_ConeTruthLabel.clear();
  Jet_PartonTruthLabel.clear();
  Jet_HadronConeExclTruthLabel.clear();  
  Jet_deltaR.clear();
  
  
  ///----------------
  /// Missing Energy
  ///----------------
  Etmiss_TST_Etx=0.;
  Etmiss_TST_Ety=0.;
  Etmiss_TST_Et=0.; 
  
  
  ///---------------
  /// Truth business 
  ///---------------
 
  /// truth missing transverse energy
  Etmiss_Truth_Etx=0.;
  Etmiss_Truth_Ety=0.;
  Etmiss_Truth_Et=0.;

  /// truthJets from "AntiKt4TruthJets" collection
  NTruthAntiktJet=0;
  TruthAntiktJet_eta.clear();
  TruthAntiktJet_phi.clear();
  TruthAntiktJet_pT.clear();
  TruthAntiktJet_E.clear();
  TruthAntiktJet_ConeTruthLabel.clear();
  TruthAntiktJet_HadronConeExclTruthLabelID.clear();
  TruthAntiktJet_PartonTruthLabel.clear();
  
  /// need these branches for ttV HF / tt Hf studies (2b / 3b regions)
  TruthAntiktJetJet_ClassHF.clear();
  extraB=0;
  extraC=0;
  
  /// jets from "TruthParticles" collection
  NTruthJet=0;
  TruthJet_eta.clear();
  TruthJet_phi.clear();
  TruthJet_pT.clear();
  TruthJet_E.clear();
  TruthJet_id.clear();
  TruthJet_origin.clear();
  
  /// only real lepton (from t/W/Z/SUSY/etc) from "TruthParticles" collection
  NTruthRealL = 0;
  TruthRealL_eta.clear();
  TruthRealL_phi.clear();
  TruthRealL_pT.clear();
  TruthRealL_id.clear();
  TruthRealL_origin.clear();

  
  ///------------
  /// other
  ///------------
  
  /// PDF INFO
  TruthX1=0.;
  TruthX2=0.;
  /// TruthXF1=0.;
  /// TruthXF2=0.;
  TruthQ=0.;
  
  
  if(DEBUG) cout << "DEBUG :: End my variables initialization " << endl;
   
  static SG::AuxElement::Accessor<int> dec_type("truthType");
  static SG::AuxElement::Accessor<int> dec_origin("truthOrigin");
  if(DEBUG) cout << "took truth type / origin Accessors\n";

  
  ///------------
  /// JETS
  ///------------
  
  CutClass::Particle theJet; /// Conditional jump or move depends on uninitialised value(s)
  xAOD::JetContainer* jets_copy(0);
  xAOD::ShallowAuxContainer* jets_copyaux(0);
  ANA_CHECK( objTool->GetJets(jets_copy,jets_copyaux,true));
  xAOD::JetContainer::iterator jet_itr = (jets_copy)->begin();
  xAOD::JetContainer::iterator jet_end = (jets_copy)->end();
  if(DEBUG) cout << "objTool->GetJets\n";
   
   
  
  ///------------
  /// MUONS
  ///------------  
  
  CutClass::Particle theMu;
  std::vector<const xAOD::IParticle*> stored_muon_particles;
  
  xAOD::MuonContainer* muons_copy(0);
  xAOD::ShallowAuxContainer* muons_copyaux(0);
  ANA_CHECK( objTool->GetMuons(muons_copy,muons_copyaux,true) );
  xAOD::MuonContainer::iterator mu_itr = (muons_copy)->begin();
  xAOD::MuonContainer::iterator mu_end  = (muons_copy)->end();
  if(DEBUG) cout << "objTool->GetMuons\n";
  

  
  ///------------
  /// ELECTRONS
  ///------------   
  CutClass::Particle theEl;
  
  xAOD::ElectronContainer* electrons_copy(0);
  xAOD::ShallowAuxContainer* electrons_copyaux(0);
  ANA_CHECK( objTool->GetElectrons(electrons_copy,electrons_copyaux, true) );
  xAOD::ElectronContainer::iterator el_itr = (electrons_copy)->begin();
  xAOD::ElectronContainer::iterator el_end = (electrons_copy)->end();
  if(DEBUG) cout << "objTool->GetElectrons\n";
 
  
  ///----------------------------
  /// Prompt lepton tagger BLOCK
  ///----------------------------	

  /// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PromptLeptonTagging#Release_21)
  std::string alg_name = "PromptLeptonVeto";
  /// Instance of ConstAccessor for the BDT weight
  SG::AuxElement::ConstAccessor<float> accessPromptVar(alg_name);
 
  
  
  ///--------------
  /// PHOTONS
  ///--------------  
  xAOD::PhotonContainer* photons_copy(0);
  xAOD::ShallowAuxContainer* photons_copyaux(0);
  ANA_CHECK( objTool->GetPhotons(photons_copy,photons_copyaux,true) );
  xAOD::PhotonContainer::iterator ph_itr = (photons_copy)->begin();
  xAOD::PhotonContainer::iterator ph_end = (photons_copy)->end();
  if(DEBUG) cout << "objTool->GetPhotons\n";
  
  
  
  ///------------
  /// OTHER
  ///------------  
  
  std::vector<const xAOD::Electron*> myTriggeringElectrons;
  std::vector<const xAOD::Muon*> myTriggeringMuons;
  int ne20=0, nmu20=0, nmu21=0, nmu23=0, nmu10=0;

  std::vector<CP::SystematicSet> sysList_muRECO = CP::make_systematics_vector(m_muonEffSFs->recommendedSystematics());
  std::vector<CP::SystematicSet> sysList_muISO = CP::make_systematics_vector(m_muonISOSFs->recommendedSystematics());
  ///
  CP::SystematicSet recSysts = m_electronSF_CFID->recommendedSystematics();   
  std::vector<CP::SystematicSet> sysList = CP::make_systematics_vector(recSysts);
  CP::SystematicSet recSysts_eleID = m_electronSF_LhMed->recommendedSystematics();  
  std::vector<CP::SystematicSet> sysList_eleID = CP::make_systematics_vector(recSysts_eleID);
  CP::SystematicSet recSysts_eleReco = m_electronSF_reco->recommendedSystematics();  
  std::vector<CP::SystematicSet> sysList_eleReco = CP::make_systematics_vector(recSysts_eleReco);
  CP::SystematicSet recSysts_eleISO = m_electronSF_isoLhMed->recommendedSystematics();  
  std::vector<CP::SystematicSet> sysList_eleISO = CP::make_systematics_vector(recSysts_eleISO);
  CP::SystematicSet recSysts_eleCFT = m_electronSF_CFT->recommendedSystematics();  
  std::vector<CP::SystematicSet> sysList_eleCFT = CP::make_systematics_vector(recSysts_eleCFT);


  if(DEBUG) cout << "All objects done\n";
  
 


  ///------------
  /// OVERLAP REMOVAL
  ///------------
  
  /// SUSYTool overlap removal : take the decorations; iterators / containers not changed
  ANA_CHECK( objTool->OverlapRemoval(electrons_copy, muons_copy, jets_copy) );
  /*** auto& my_orTool = my_orToolbox.masterTool;
  ANA_CHECK( my_orTool->removeOverlaps(electrons_copy, muons_copy, jets_copy) );*/

  
  /// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/SusyObjectDefinitionsr2113TeV
  
 
  ///------------
  /// JET Block 
  ///------------
 
  /// xAOD::JetContainer* goodJets = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
  /// ANA_CHECK( store.record(goodJets, "MySelJets" ) );
  for (const auto& jet : *jets_copy) 
  {
	/// if (jet->auxdata<char>("baseline") == 1  &&
	/// jet->auxdata<char>("passOR") == 1  &&
	///	jet->pt() > 20000.  && ( fabs( jet->eta()) < 2.8) ) {
	/// goodJets->push_back(jet);
	/// }
    objTool->IsBJet( *jet );  
  }
  if(DEBUG) cout << "DEBUG :: objTool->IsBJet" << endl;
  
  if(isMC){
    objTool->BtagSF( jets_copy );
    totalJVTsf = objTool->JVT_SF( jets_copy );
  }
  if(DEBUG) cout << "DEBUG :: objTool->JVT_SF" << endl;
  

  /// Get the TruthJets from the event:
  const xAOD::JetContainer* truthJets = 0;
  if(isMC) 
  {
	ANA_CHECK( m_event->retrieve( truthJets, "AntiKt4TruthJets" ) );  ///  AntiKt4TruthWZJets are in D-SUSY2 ?
    if(DEBUG) Info("execute()", "  number of truth jets selected in ST = %lu", truthJets->size());
  }
  if(DEBUG) Info("execute()", "  number of jets selected in ST = %lu", jets_copy->size());
  
  
  jet_itr = (jets_copy)->begin();
  jet_end = (jets_copy)->end();
  for( ; jet_itr != jet_end; ++jet_itr ) 
  {   
    theJet.FourVector.SetPtEtaPhiE( (*jet_itr)->pt(), (*jet_itr)->eta(), (*jet_itr)->phi(), (*jet_itr)->e() );

	/// if(theJet.FourVector.Pt()<20000. ) continue;   /// should be applied for signal jets only
	
	Jet_eta.push_back(theJet.FourVector.Eta());
	Jet_phi.push_back(theJet.FourVector.Phi());
	Jet_pT.push_back(theJet.FourVector.Pt());
	Jet_E.push_back(theJet.FourVector.E());
      
	if(isMC){
	  float min_delR = 10.;
	  for (const auto& truthJet : *truthJets) {
		float deltaEta = fabs((*jet_itr)->eta() - truthJet->eta());
		float deltaPhi = fabs((*jet_itr)->phi() - truthJet->phi());
		if (deltaPhi > TMath::Pi() ){
		  deltaPhi = 2*TMath::Pi() - deltaPhi;
		}
		float _deltaR = sqrt(pow(deltaEta,2) + pow(deltaPhi,2));
		if(_deltaR < min_delR)
		  min_delR = _deltaR;
	  }
	  Jet_deltaR.push_back( min_delR );
	}

	std::vector<int> nTrkVec;
	(*jet_itr)->getAttribute(xAOD::JetAttribute::NumTrkPt500, nTrkVec);
	int jet_nTrk = (objTool->GetPrimVtx() == 0 || nTrkVec.size() == 0) ? 0 : nTrkVec[objTool->GetPrimVtx()->index()];
	Jet_nTrk.push_back( jet_nTrk );
	
	Jet_quality.push_back(  (int)(*jet_itr)->auxdata< char >("bad")  );
	if(DEBUG && false) cout << "After Jet_quality\n";
	
	
	float emfrac, hecfrac;
    (*jet_itr)->getAttribute(xAOD::JetAttribute::EMFrac, emfrac);
    (*jet_itr)->getAttribute(xAOD::JetAttribute::HECFrac, hecfrac);
    Jet_EMFrac.push_back(emfrac);
	Jet_HECFrac.push_back(hecfrac);	
	
	
	/// bool passJVT = objTool->JetPassJVT(**jet_itr, true);
	float jvt = (*jet_itr)->auxdata<float>("Jvt");
	Jet_JVT.push_back( jvt );
	float jvtsf = (*jet_itr)->auxdata<float>("jvtscalefact");
	Jet_JVTsf.push_back( jvtsf ); 
	if (false && tEventNumber == 147934)
	{
		/// some debugging
		std::cout << " -> jet pT=" << theJet.FourVector.Pt() << ", jvt=" << jvt << std::endl;
	}
	if(DEBUG && false) cout << "After Jet_JVTsf\n";

	
	/// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingBenchmarksRelease21
	Double_t weight_mv2c10 = -1;
	if ( !(*jet_itr)->btagging()->MVx_discriminant("MV2c10", weight_mv2c10)) {
	  Error(APP_NAME,"Failed to retrieve MV2c20 weight!");
	}
	Jet_MV2c10.push_back(weight_mv2c10);
	
	Jet_SFw.push_back( isMC ? (*jet_itr)->auxdata< double >("effscalefact") : 1 );
	
	Jet_passOR.push_back( (int)(*jet_itr)->auxdata< char >("passOR") );

	int iret = -1;
	if(isMC)
	{
	  iret = (*jet_itr)->auxdata<int>("ConeTruthLabelID") ;
	}
	Jet_ConeTruthLabel.push_back( iret );
	iret = -1;
	if(isMC)
	{
	  iret = (*jet_itr)->auxdata<int>("PartonTruthLabelID") ;
	}
	Jet_PartonTruthLabel.push_back( iret );
	iret = -1;
	if(isMC)
	{
	  iret = (*jet_itr)->auxdata<int>("HadronConeExclTruthLabelID") ;
	}
	Jet_HadronConeExclTruthLabel.push_back( iret );
    
	NJet++;
  }
  if(DEBUG) cout << "DEBUG :: End jet block, with NJet = " << NJet<< endl;

  ///------------
  /// MUON BLOCK 
  ///------------

  /// to store baseline muons for matching
  ///   xAOD::MuonContainer* goodMu = new xAOD::MuonContainer(SG::VIEW_ELEMENTS);
  ///   ANA_CHECK( store.record(goodMu, "MySelMu") );

  static SG::AuxElement::Accessor<int> acc_truthType("truthType");
  static SG::AuxElement::Accessor<int> acc_truthOrigin("truthOrigin");
  
  mu_itr = (muons_copy)->begin();
  mu_end = (muons_copy)->end();
  for( ; mu_itr != mu_end; ++mu_itr ) 
  {
    theMu.FourVector.SetPtEtaPhiM( (*mu_itr)->pt(), (*mu_itr)->eta(), (*mu_itr)->phi(), 105.65837 );

    const xAOD::TrackParticle* track;	
    if ((*mu_itr)->muonType() == xAOD::Muon::SiliconAssociatedForwardMuon) {
      track = (*mu_itr)->trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
      if (!track) continue; /// don't treat SAF muons without ME track further
    }
	else{
      track = (*mu_itr)->primaryTrackParticle();
    }
      
    theMu.ID = track->charge()*-13; /// mu-:13, mu+:-13	
	theMu.sigd0 = (*mu_itr)->auxdata< float >("d0sig");
    theMu.d0pvtx = track->d0();
	theMu.d0pvtxerr = Amg::error( track->definingParametersCovMatrix(),0);
    theMu.z0pvtx = (*mu_itr)->auxdata< float >("z0sinTheta") ; 
    
	if ((*mu_itr)->auxdata<char >("baseline")== 1)
	{
	  if(theMu.FourVector.Pt()<10000. || fabs(theMu.FourVector.Eta())>2.7 ) continue;
	
	  Mu_eta.push_back(theMu.FourVector.Eta());
	  Mu_phi.push_back(theMu.FourVector.Phi());
	  Mu_pT.push_back(theMu.FourVector.Pt());
	 
	  if(theMu.ID < 0) Mu_charge.push_back( 1 );
	  else Mu_charge.push_back( -1 );
	  
	  Mu_sigd0.push_back(theMu.sigd0);
	  Mu_d0pvtx.push_back(theMu.d0pvtx);
	  Mu_d0pvtxerr.push_back(theMu.d0pvtxerr);	  
	  Mu_z0pvtx.push_back(theMu.z0pvtx);
	
	  Mu_isBad.push_back( (int)(*mu_itr)->auxdata< char >("bad") );
	  Mu_isCosmic.push_back( (int)(*mu_itr)->auxdata< char >("cosmic") );
	
	
	  /// play with OR here!
	  Mu_passOR.push_back( (int)(*mu_itr)->auxdata< char >("passOR") );
	  /** if(isMyOR==false || (isMyOR==true && theMu.FourVector.Pt()>pT_treshold)){
	  Mu_passOR.push_back( (int)(*mu_itr)->auxdata< char >("passOR") );  /// boosted
	  }
	  else {
	  Mu_passOR.push_back( (int)(*mu_itr)->auxdata< char >("passMyOR") ); /// standard OR
	  }*/	  
	  
	
   	  bool isTight = false;
	  isTight = m_muonSelectionTool->accept(**mu_itr);
	  Mu_isTight.push_back( (int)isTight );
	  
	  Mu_isSig.push_back( (int)(*mu_itr)->auxdata< char >("signal") );
	  
	  if(accessPromptVar.isAvailable(**mu_itr)) Mu_PromptLepTaggerBDTweight.push_back(accessPromptVar(**mu_itr));
	  else ATH_MSG_WARNING(alg_name << " not found in auxiliary store");
	  
	  int muonTruthType = -1;
	  int muonTruthOrigin = -1;
	  if(isMC){
		/// access MC type/origin
		const xAOD::TrackParticle* trackParticle = track;
		if (trackParticle) {
		  if (acc_truthType.isAvailable(*trackParticle)  ) muonTruthType   = acc_truthType(*trackParticle);
		  if (acc_truthOrigin.isAvailable(*trackParticle)) muonTruthOrigin = acc_truthOrigin(*trackParticle);
		}   
		Mu_type.push_back(  muonTruthType );
		Mu_origin.push_back( muonTruthOrigin );
	  }
	  
	  
	  Mu_ptcone20.push_back( (*mu_itr)->auxdata<float>("ptcone20") );
	  Mu_ptcone20_TightTTVA_pt500.push_back( (*mu_itr)->auxdata<float>("ptcone20_TightTTVA_pt500") );
	  Mu_ptcone20_TightTTVA_pt1000.push_back( (*mu_itr)->auxdata<float>("ptcone20_TightTTVA_pt1000") );
	  Mu_ptcone30.push_back( (*mu_itr)->auxdata<float>("ptcone30") );
	  Mu_ptcone40.push_back( (*mu_itr)->auxdata<float>("ptcone40") );
	  Mu_ptvarcone20.push_back( (*mu_itr)->auxdata<float>("ptvarcone20") );
	  Mu_ptvarcone30.push_back( (*mu_itr)->auxdata<float>("ptvarcone30") );
	  Mu_ptvarcone30_TightTTVA_pt500.push_back( (*mu_itr)->auxdata<float>("ptvarcone30_TightTTVA_pt500") );
	  Mu_ptvarcone30_TightTTVA_pt1000.push_back( (*mu_itr)->auxdata<float>("ptvarcone30_TightTTVA_pt1000") );
	  Mu_ptvarcone40.push_back( (*mu_itr)->auxdata<float>("ptvarcone40") );
	  Mu_topoetcone20.push_back( (*mu_itr)->auxdata<float>("topoetcone20") );
	  Mu_topoetcone30.push_back( (*mu_itr)->auxdata<float>("topoetcone30") );
	  Mu_topoetcone40.push_back( (*mu_itr)->auxdata<float>("topoetcone40") );
	  Mu_neflowisol20.push_back( (*mu_itr)->auxdata<float>("neflowisol20") );
	  bool passLooseTO = false;
	  if (isoLooseTO->accept( **mu_itr )) passLooseTO = true;
	  Mu_passIsoLooseTO.push_back((int)passLooseTO);
	  bool passLoose = false;
	  if (isoLoose->accept( **mu_itr )) passLoose = true;
	  Mu_passIsoLoose.push_back((int)passLoose);
	  bool passTight = false;
	  if (isoTight->accept( **mu_itr )) passTight = true;
	  Mu_passIsoTight.push_back((int)passTight);
	  bool passGrad = false;
	  if (isoGrad->accept( **mu_itr )) passGrad = true;
	  Mu_passIsoGrad.push_back((int)passGrad);
	  bool passGradCustomTight = false;
	  if (isoGradCustomTight->accept( **mu_itr )) passGradCustomTight = true;
	  Mu_passIsoGradCustomTight.push_back((int)passGradCustomTight);
	  bool passGradCustom = false;
	  if (isoGradCustom->accept( **mu_itr )) passGradCustom = true;
	  Mu_passIsoGradCustom.push_back((int)passGradCustom);
	  bool passGradLoose = false;
	  if (isoGradLoose->accept( **mu_itr )) passGradLoose = true;
	  Mu_passIsoGradLoose.push_back((int)passGradLoose);
	  
	  
	  theMu.SFweight = isMC ? objTool->GetSignalMuonSF(**mu_itr, true, false) : 1. ;
   	  Mu_SFw.push_back(theMu.SFweight);
	  
	  float sf  = isMC ? objTool->GetSignalMuonSF(**mu_itr, false, true) : 1. ;
	  Mu_IsoSFw.push_back(sf);
	  
	  float theSf_reco(1.), theSfunc_Reco_EFF_RECO_STAT_UP(1.), theSfunc_Reco_EFF_RECO_SYST_UP(1.);
	  float theSfunc_Reco_EFF_RECO_STAT_LOWPT_UP(1.), theSfunc_Reco_EFF_RECO_SYST_LOWPT_UP(1.);
	  float theSf_ISO(1.), theSfunc_EFF_ISO_STAT__1up(1.), theSfunc_EFF_ISO_SYS__1up(1.);
	  if(isMC)
	  {
		/// https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonEfficiencyCorrections/util/MuonEfficiencyScaleFactorsTest.cxx
		/// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesMC16#Muon_trigger_efficiency_scale_fa
		if(false) for(const auto& sss : sysList_muRECO) {cout << sss.name() << endl;}
        ANA_CHECK(  m_muonEffSFs->applySystematicVariation(CP::SystematicSet("")) );
	    ANA_CHECK(  m_muonEffSFs->getEfficiencyScaleFactor (**mu_itr,theSf_reco) );	
	    ANA_CHECK(  m_muonEffSFs->applySystematicVariation(CP::SystematicSet("MUON_EFF_RECO_STAT__1up")) );
	    ANA_CHECK(  m_muonEffSFs->getEfficiencyScaleFactor (**mu_itr,theSfunc_Reco_EFF_RECO_STAT_UP) );
		ANA_CHECK(  m_muonEffSFs->applySystematicVariation(CP::SystematicSet("MUON_EFF_RECO_SYS__1up")) );
	    ANA_CHECK(  m_muonEffSFs->getEfficiencyScaleFactor (**mu_itr,theSfunc_Reco_EFF_RECO_SYST_UP) );
		ANA_CHECK(  m_muonEffSFs->applySystematicVariation(CP::SystematicSet("MUON_EFF_RECO_STAT_LOWPT__1up")) );
	    ANA_CHECK(  m_muonEffSFs->getEfficiencyScaleFactor (**mu_itr,theSfunc_Reco_EFF_RECO_STAT_LOWPT_UP) );
		ANA_CHECK(  m_muonEffSFs->applySystematicVariation(CP::SystematicSet("MUON_EFF_RECO_SYS_LOWPT__1up")) );
	    ANA_CHECK(  m_muonEffSFs->getEfficiencyScaleFactor (**mu_itr,theSfunc_Reco_EFF_RECO_SYST_LOWPT_UP) );
		
		
		if(false) for(const auto& sss : sysList_muISO) {cout << sss.name() << endl;}
        ANA_CHECK(  m_muonISOSFs->applySystematicVariation(CP::SystematicSet("")) );
	    ANA_CHECK(  m_muonISOSFs->getEfficiencyScaleFactor (**mu_itr,theSf_ISO) );	
	    ANA_CHECK(  m_muonISOSFs->applySystematicVariation(CP::SystematicSet("MUON_EFF_ISO_STAT__1up")) );
	    ANA_CHECK(  m_muonISOSFs->getEfficiencyScaleFactor (**mu_itr,theSfunc_EFF_ISO_STAT__1up) );
		ANA_CHECK(  m_muonISOSFs->applySystematicVariation(CP::SystematicSet("MUON_EFF_ISO_SYS__1up")) );
	    ANA_CHECK(  m_muonISOSFs->getEfficiencyScaleFactor (**mu_itr,theSfunc_EFF_ISO_SYS__1up) );
	
	  }
	  Mu_StatUncReco.push_back(theSfunc_Reco_EFF_RECO_STAT_UP);
	  Mu_SystUncReco.push_back(theSfunc_Reco_EFF_RECO_SYST_UP);
	  Mu_StatUncReco_LOWPT.push_back(theSfunc_Reco_EFF_RECO_STAT_LOWPT_UP);
	  Mu_SystUncReco_LOWPT.push_back(theSfunc_Reco_EFF_RECO_SYST_LOWPT_UP);
	  Mu_StatUncISO.push_back(theSfunc_EFF_ISO_STAT__1up);
	  Mu_SystUncISO.push_back(theSfunc_EFF_ISO_SYS__1up);
	  
	  // cout << "theMu.SFweight = " << theMu.SFweight << " " << theSf_reco*theSf_ISO << endl;
	  /// cout << "theSf_reco = " << theSf_reco << ", theSfunc_Reco_EFF_RECO_STAT = "<<theSf_reco-theSfunc_Reco_EFF_RECO_STAT_UP 
		   /// << ", theSfunc_Reco_EFF_RECO_SYST = "<<theSf_reco-theSfunc_Reco_EFF_RECO_SYST_UP  
		   /// << ", theSfunc_Reco_EFF_RECO_STAT_LOWPT = "<<theSf_reco-theSfunc_Reco_EFF_RECO_STAT_LOWPT_UP
		   /// << ", theSfunc_Reco_EFF_RECO_SYST_LOWPT = "<<theSf_reco-theSfunc_Reco_EFF_RECO_SYST_LOWPT_UP  << endl;;
		
		
	  Mu_TrigMatch_mu26_ivarmedium.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu26_ivarmedium"));
	  Mu_TrigMatch_mu20_iloose_L1MU15.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu20_iloose_L1MU15"));
	  Mu_TrigMatch_mu40.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu40"));
	  Mu_TrigMatch_mu50.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu50"));

	  Mu_TrigMatch_mu8noL1.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu8noL1"));
	  Mu_TrigMatch_mu14.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu14"));
	  Mu_TrigMatch_mu18.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu18"));
	  
	  /// 2015 items
	  Mu_TrigMatch_mu18_mu8noL1.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu18_mu8noL1"));
	  Mu_TrigMatch_e17_lhloose_mu14.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_e17_lhloose_mu14"));

	  /// 2016 items
	  Mu_TrigMatch_e17_lhloose_nod0_mu14.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_e17_lhloose_nod0_mu14"));
	  Mu_TrigMatch_mu20_mu8noL1.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu20_mu8noL1"));
	  Mu_TrigMatch_mu22_mu8noL1.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu22_mu8noL1"));
	  
	  /// 2016 single lepton
	  Mu_TrigMatch_mu24_iloose.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu24_iloose") );
	  Mu_TrigMatch_mu24_ivarloose.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu24_ivarloose") );
	  Mu_TrigMatch_mu24_iloose_L1MU15.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu24_iloose_L1MU15") );
	  Mu_TrigMatch_mu24_ivarloose_L1MU15.push_back(objTool->IsTrigMatched(*mu_itr, "HLT_mu24_ivarloose_L1MU15") );
	  
	  /// pair matching --> do after main loop
	  stored_muon_particles.push_back(*mu_itr);

	  /// Analysis level signal muons (need this for trigger matching):
	  bool signal_muon = true;
	  signal_muon &= theMu.FourVector.Pt() > 10000;
	  signal_muon &= fabs(theMu.FourVector.Eta()) < 2.5;  /// change to 2.7 if decided by the group! (need some studies...)
	  signal_muon &= (*mu_itr)->auxdata<float>("ptvarcone30")/theMu.FourVector.Pt() < 0.06;
	  signal_muon &= fabs(theMu.sigd0) < 3;
	  signal_muon &= fabs(theMu.z0pvtx) < 0.5;

	  if ( signal_muon )
	  {
		myTriggeringMuons.push_back(*mu_itr);
		
		nmu10++;
		if (theMu.FourVector.Pt() > 20000) nmu20++;
		if (theMu.FourVector.Pt() > 21000) nmu21++;
		if (theMu.FourVector.Pt() > 23000) nmu23++;
	  }
	  
	  NMu++;
    }
  }

  /// now fill trigger matching pairs
  for(auto mu1 : stored_muon_particles)
  {
    Mu_TrigMatchPair_mu18_mu8noL1.emplace_back();
    Mu_TrigMatchPair_mu20_mu8noL1.emplace_back();
    Mu_TrigMatchPair_mu22_mu8noL1.emplace_back();
    for(auto mu2 : stored_muon_particles)
	{
      Mu_TrigMatchPair_mu18_mu8noL1.back().push_back((mu1!=mu2)? objTool->IsTrigMatched(mu1,mu2,"HLT_mu18_mu8noL1") : false);
      Mu_TrigMatchPair_mu20_mu8noL1.back().push_back((mu1!=mu2)? objTool->IsTrigMatched(mu1,mu2,"HLT_mu20_mu8noL1") : false);
      Mu_TrigMatchPair_mu22_mu8noL1.back().push_back((mu1!=mu2)? objTool->IsTrigMatched(mu1,mu2,"HLT_mu22_mu8noL1") : false);
    }
  }
  if(DEBUG) cout << "Done with muons!" << endl;

  
  ///----------------
  /// ELECTRON BLOCK
  ///----------------

  /// To fill Electron charge misId info
  const xAOD::TruthParticleContainer* truthP = 0;
  if (isMC) {
    ANA_CHECK( m_event->retrieve( truthP, "TruthParticles" ) );
    fillElectronChargeFlip(electrons_copy, truthP, eventInfo->mcChannelNumber() );
  }

  el_itr = (electrons_copy)->begin();
  el_end = (electrons_copy)->end();
  for( ; el_itr != el_end; ++el_itr ) 
  {
    theEl.FourVector.SetPtEtaPhiM( (*el_itr)->pt(), (*el_itr)->eta(), (*el_itr)->phi(), 0.5109989 );
       
    theEl.ID = (*el_itr)->trackParticle()->charge()*-11; /// e-:11, e+:-11
	
	theEl.sigd0 = xAOD::TrackingHelpers::d0significance( (*el_itr)->trackParticle(),
										 eventInfo->beamPosSigmaX(), 
										 eventInfo->beamPosSigmaY(), 
										 eventInfo->beamPosSigmaXY() );
	/// theEl.sigd0old = (*el_itr)->trackParticle()->d0() / Amg::error((*el_itr)->trackParticle()->definingParametersCovMatrix(),0);
    theEl.d0pvtx = (*el_itr)->trackParticle()->d0();
	theEl.d0pvtxerr = Amg::error((*el_itr)->trackParticle()->definingParametersCovMatrix(),0);
	
    theEl.z0pvtx = (*el_itr)->auxdata< float >("z0sinTheta"); 
    /// theEl.z0pvtx = (*el_itr)->trackParticle()->z0() + (*el_itr)->trackParticle()->vz() - primvertex_z; 
    
   
    int isLooseLH_baseline = (int)(*el_itr)->auxdata< char >("baseline");

    if ( isLooseLH_baseline )
	{
      if(theEl.FourVector.Pt()<10000. || fabs(theEl.FourVector.Eta())>2.47 ) continue;
   	  
      El_eta.push_back(theEl.FourVector.Eta());
      El_etaclus.push_back( (*el_itr)->caloCluster()->eta() );
      El_phi.push_back(theEl.FourVector.Phi());
	  El_pT.push_back(theEl.FourVector.Pt());
      El_E.push_back(theEl.FourVector.E());
	  
	  if(theEl.ID < 0) El_charge.push_back( 1 );
      else El_charge.push_back( -1 );
	  El_sigd0.push_back(theEl.sigd0);
      El_d0pvtx.push_back(theEl.d0pvtx);
      El_d0pvtxerr.push_back(theEl.d0pvtxerr);
      El_z0pvtx.push_back(theEl.z0pvtx);
	  
	  
      El_isLooseLH_baseline.push_back( isLooseLH_baseline );
	  /// int pass = -1; pass = (int)m_elecSelLikelihood_LhLoose->accept(*el_itr) ; 
      El_isLooseLH_fromTool.push_back( (*el_itr)->auxdata< char >("DFCommonElectronsLHLoose") );
      /// pass = -1; pass = (int)m_elecSelLikelihood_LhMed->accept(*el_itr) ; 
      El_isMediumLH.push_back( (*el_itr)->auxdata< char >("DFCommonElectronsLHMedium") );	  
      /// pass = -1; pass = (int)m_elecSelLikelihood_LhTight->accept(*el_itr) ; 
      El_isTightLH.push_back( (*el_itr)->auxdata< char >("DFCommonElectronsLHTight") ); 
	  
	  
	  El_isSig.push_back( (int)(*el_itr)->auxdata< char >("signal") );
	  
	  
	  uint8_t nBLHits = 0;
      uint8_t expectBlayer(1);
      (*el_itr)->trackParticle()->summaryValue( nBLHits, xAOD::numberOfBLayerHits);
      (*el_itr)->trackParticle()->summaryValue( expectBlayer, xAOD::expectBLayerHit);
      El_nBLayerHits.push_back( (int)nBLHits );
      El_expectBLayerHit.push_back( (int)expectBlayer );
	  
	  
	  /// play with OR here!
	  El_passOR.push_back( (*el_itr)->auxdata<char>("passOR") );
	  /**if(isMyOR==false || (isMyOR==true && theEl.FourVector.Pt()>pT_treshold)){
		El_passOR.push_back( (int)(*el_itr)->auxdata< char >("passOR") );   /// boosted
		}
		else{
		El_passOR.push_back( (int)(*el_itr)->auxdata< char >("passMyOR") ); /// standard OR
		}		
	  */
	  
  
	  /// bool CFT_accept = false;
	  /// if ( CFTToolmedium->accept( **el_itr )) CFT_accept = true;
	  /// El_passChargeFlipTagger.push_back((int)CFT_accept);
      El_passChargeFlipTaggerBDTmedium.push_back((double)CFTToolmedium->calculate( *el_itr));  
	  /// bool CFTLoose_accept = false;
	  /// if ( CFTToolLoose->accept( **el_itr )) CFTLoose_accept = true;
	  /// El_passChargeFlipTagger.push_back((int)CFTLoose_accept);
      El_passChargeFlipTaggerBDTloose.push_back((double)CFTToolLoose->calculate( *el_itr));
	  
	  if(accessPromptVar.isAvailable(**el_itr)) El_PromptLepTaggerBDTweight.push_back(accessPromptVar(**el_itr));
	  else ATH_MSG_WARNING(alg_name << " not found in auxiliary store");
	  
	   
	  if(isMC){
        El_truthType.push_back( (*el_itr)->auxdata<int>("truthType") );
        El_truthOrigin.push_back(  (*el_itr)->auxdata<int>("truthOrigin") );
        El_truthPdgId.push_back(  (*el_itr)->auxdata<int>("truthPdgId") );
        El_bkgTruthType.push_back( (*el_itr)->auxdata<int>("bkgTruthType") );
        El_bkgTruthOrigin.push_back(  (*el_itr)->auxdata<int>("bkgTruthOrigin") );
        El_bkgMotherPdgId.push_back( (*el_itr)->auxdata<int>("bkgMotherPdgId") );
        El_firstEgMotherTruthType.push_back( (*el_itr)->auxdata<int>("firstEgMotherTruthType") );
        El_firstEgMotherTruthOrigin.push_back(  (*el_itr)->auxdata<int>("firstEgMotherTruthOrigin") );
        El_firstEgMotherPdgId.push_back( (*el_itr)->auxdata<int>("firstEgMotherPdgId") );
		El_lastEgMotherTruthType.push_back( (*el_itr)->auxdata<int>("lastEgMotherTruthType") );
        El_lastEgMotherTruthOrigin.push_back(  (*el_itr)->auxdata<int>("lastEgMotherTruthOrigin") );
        El_lastEgMotherPdgId.push_back( (*el_itr)->auxdata<int>("lastEgMotherPdgId") );
		El_chFlip.push_back(  (*el_itr)->auxdata< int >("chargeFlip") );
      }
	 
	  
	  El_ptcone20.push_back( (*el_itr)->auxdata<float>("ptcone20") );
	  El_ptcone20_TightTTVA_pt500.push_back( (*el_itr)->auxdata<float>("ptcone20_TightTTVA_pt500") );
	  El_ptcone20_TightTTVA_pt1000.push_back( (*el_itr)->auxdata<float>("ptcone20_TightTTVA_pt1000") );
      El_ptcone30.push_back( (*el_itr)->auxdata<float>("ptcone30") );
      El_ptcone40.push_back( (*el_itr)->auxdata<float>("ptcone40") );
      El_ptvarcone20.push_back( (*el_itr)->auxdata<float>("ptvarcone20") );
      El_ptvarcone20_TightTTVA_pt1000.push_back( (*el_itr)->auxdata<float>("ptvarcone20_TightTTVA_pt1000") );
      El_ptvarcone30.push_back( (*el_itr)->auxdata<float>("ptvarcone30") );
      El_ptvarcone30_TightTTVA_pt1000.push_back( (*el_itr)->auxdata<float>("ptvarcone30_TightTTVA_pt1000") );
      El_ptvarcone40.push_back( (*el_itr)->auxdata<float>("ptvarcone40") );
      El_topoetcone20.push_back( (*el_itr)->auxdata<float>("topoetcone20") );
      El_topoetcone30.push_back( (*el_itr)->auxdata<float>("topoetcone30") );
      El_topoetcone40.push_back( (*el_itr)->auxdata<float>("topoetcone40") );
      El_neflowisol20.push_back( (*el_itr)->auxdata<float>("neflowisol20") );
	   bool passLooseTO = false;
      if (isoLooseTO->accept( **el_itr )) passLooseTO = true;
      El_passIsoLooseTO.push_back((int)passLooseTO);
      bool passLoose = false;
      if (isoLoose->accept( **el_itr )) passLoose = true;
      El_passIsoLoose.push_back((int)passLoose);      
      bool passTight = false;
      if (isoTight->accept( **el_itr )) passTight = true;
      El_passIsoTight.push_back((int)passTight);      
      bool passGrad = false;
      if (isoGrad->accept( **el_itr )) passGrad = true;
      El_passIsoGrad.push_back((int)passGrad);
      bool passGradCustom = false;
      if (isoGradCustom->accept( **el_itr )) passGradCustom = true;
      El_passIsoGradCustom.push_back((int)passGradCustom);
      bool passGradCustomTight = false;
      if (isoGradCustomTight->accept( **el_itr )) passGradCustomTight = true;
      El_passIsoGradCustomTight.push_back((int)passGradCustomTight);
      bool passGradLoose = false;
      if (isoGradLoose->accept( **el_itr )) passGradLoose = true;
      El_passIsoGradLoose.push_back((int)passGradLoose);
	  
  
      double theSf_reco(1.),theSf_LhTight(1.),theSf_LhMedium(1.),theSf_LhLoose(1.),theSf_CFT(1.);
	  double theSfunc_LhMedium_ID_TOTAL_1NPCOR_PLUS_UNCOR__1up(1.);
	  double theSfunc_LhLoose_ID_TOTAL_1NPCOR_PLUS_UNCOR__1up(1.);
	  double theSfunc_Reco_TOTAL_1NPCOR_PLUS_UNCOR__1up(1.);	  
	  double theSf_isoLhMed(1.);
	  double theSfunc_Iso_TOTAL_1NPCOR_PLUS_UNCOR__1up(1.);
	  double theSfunc_ChargeIDSel_TOTAL_1NPCOR_PLUS_UNCOR__1up(1.);
	  
      if(/**!isAtlfast && */isMC ){		  
		if(false) for(const auto& sss : sysList_eleReco) {cout << sss.name() << endl;}
		ANA_CHECK(  m_electronSF_reco->applySystematicVariation(CP::SystematicSet("")) );
		ANA_CHECK(  m_electronSF_reco->getEfficiencyScaleFactor (**el_itr,theSf_reco) );	
		ANA_CHECK(  m_electronSF_reco->applySystematicVariation(CP::SystematicSet("EL_EFF_Reco_TOTAL_1NPCOR_PLUS_UNCOR__1up")) );
		ANA_CHECK(  m_electronSF_reco->getEfficiencyScaleFactor (**el_itr,theSfunc_Reco_TOTAL_1NPCOR_PLUS_UNCOR__1up) );
		/// theSfunc_Reco = theSfunc_Reco_TOTAL_1NPCOR_PLUS_UNCOR__1up - theSf_reco;
		
      	ANA_CHECK(  m_electronSF_LhTight->getEfficiencyScaleFactor (**el_itr,theSf_LhTight) );
		
		ANA_CHECK(  m_electronSF_LhLoose->applySystematicVariation(CP::SystematicSet("")) );
		ANA_CHECK(  m_electronSF_LhLoose->getEfficiencyScaleFactor (**el_itr,theSf_LhLoose) );
		ANA_CHECK(  m_electronSF_LhLoose->applySystematicVariation(CP::SystematicSet("EL_EFF_ID_TOTAL_1NPCOR_PLUS_UNCOR__1up")) );
		ANA_CHECK(  m_electronSF_LhLoose->getEfficiencyScaleFactor (**el_itr,theSfunc_LhLoose_ID_TOTAL_1NPCOR_PLUS_UNCOR__1up) );
		/// theSfunc_LhLoose = theSfunc_LhLoose_ID_TOTAL_1NPCOR_PLUS_UNCOR__1up - theSf_LhLoose;
		
		if(false) for(const auto& sss : sysList_eleID){ cout << sss.name() << endl; }
		ANA_CHECK(  m_electronSF_LhMed->applySystematicVariation(CP::SystematicSet("")) );
		ANA_CHECK(  m_electronSF_LhMed->getEfficiencyScaleFactor (**el_itr,theSf_LhMedium) );
		ANA_CHECK(  m_electronSF_LhMed->applySystematicVariation(CP::SystematicSet("EL_EFF_ID_TOTAL_1NPCOR_PLUS_UNCOR__1up")) );
		ANA_CHECK(  m_electronSF_LhMed->getEfficiencyScaleFactor (**el_itr,theSfunc_LhMedium_ID_TOTAL_1NPCOR_PLUS_UNCOR__1up) );
		/// theSfunc_LhMedium = theSfunc_LhMedium_ID_TOTAL_1NPCOR_PLUS_UNCOR__1up - theSf_LhMedium;
	
	  
		if(ismc16d || RunNb==300000)  /// ortherwise I cannot take the SFs in mc16d
		{
			/// cout << " tPRWrandomRunNumber = " <<  tPRWrandomRunNumber <<  endl;
			eventInfo->auxdecor<unsigned>("RandomRunNumber") = 300000;				
		}
		if(false) for(const auto& sss : sysList_eleCFT) { cout << sss.name() << endl;}
		if(!isAtlfast && theEl.FourVector.Pt() > 15000.) 
		{
			ANA_CHECK(  m_electronSF_CFT->applySystematicVariation(CP::SystematicSet("")) );
			ANA_CHECK(  m_electronSF_CFT->getEfficiencyScaleFactor (**el_itr,theSf_CFT) );
			ANA_CHECK(  m_electronSF_CFT->applySystematicVariation(CP::SystematicSet("EL_EFF_ChargeIDSel_TOTAL_1NPCOR_PLUS_UNCOR__1up")) );
			ANA_CHECK(  m_electronSF_CFT->getEfficiencyScaleFactor (**el_itr,theSfunc_ChargeIDSel_TOTAL_1NPCOR_PLUS_UNCOR__1up) );
			/// theSfunc_ChargeIDSel = theSfunc_ChargeIDSel_TOTAL_1NPCOR_PLUS_UNCOR__1up - theSf_CFT;
		}
		if(ismc16d || RunNb==300000)  /// ortherwise I cannot take the SFs in mc16d
		{	
			eventInfo->auxdecor<unsigned>("RandomRunNumber") = tPRWrandomRunNumber;			
		}
		
		
		if(false) for(const auto& sss : sysList_eleISO) { cout << sss.name() << endl; }
		ANA_CHECK(  m_electronSF_isoLhMed->applySystematicVariation(CP::SystematicSet("")) );
		ANA_CHECK(  m_electronSF_isoLhMed->getEfficiencyScaleFactor (**el_itr,theSf_isoLhMed) );	
		ANA_CHECK(  m_electronSF_isoLhMed->applySystematicVariation(CP::SystematicSet("EL_EFF_Iso_TOTAL_1NPCOR_PLUS_UNCOR__1up")) );
		ANA_CHECK(  m_electronSF_isoLhMed->getEfficiencyScaleFactor (**el_itr,theSfunc_Iso_TOTAL_1NPCOR_PLUS_UNCOR__1up) );
		/// theSfunc_Iso = theSfunc_Iso_TOTAL_1NPCOR_PLUS_UNCOR__1up - theSf_isoLhMed;
      }
      theEl.SFweightTightLH  = (isMC)  ? theSf_reco*theSf_LhTight : 1.;
	  theEl.SFweightMediumLH = (isMC ) ? theSf_reco*theSf_LhMedium : 1.;
	  theEl.SFweightLooseLH  = (isMC ) ? theSf_reco*theSf_LhLoose : 1.;
	  /// http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/charge_misID/
	  double tmpCFT = (isMC  ? theSf_CFT : 1.); 
	  if(false) std::cout << "CFT:: el pt=" << theEl.FourVector.Pt() << ", SFweightCFT=" << tmpCFT << std::endl;	 

	  El_SFwUncReco.push_back(theSfunc_Reco_TOTAL_1NPCOR_PLUS_UNCOR__1up);
	  El_SFwTightLH.push_back(theEl.SFweightTightLH);
      El_SFwMediumLH.push_back(theEl.SFweightMediumLH);
	  El_SFwUncMediumLH.push_back(theSfunc_LhMedium_ID_TOTAL_1NPCOR_PLUS_UNCOR__1up); /// (SFwMediumLH_unc)	  
      El_SFwLooseLH.push_back(theEl.SFweightLooseLH);
      El_SFwUncLooseLH.push_back(theSfunc_LhLoose_ID_TOTAL_1NPCOR_PLUS_UNCOR__1up);
	  El_SFweightCFT.push_back(tmpCFT);   /// Scale factor needed for the Charge Flip Tagger (charge flip killer)
	  El_SFUncweightCFT.push_back(theSfunc_ChargeIDSel_TOTAL_1NPCOR_PLUS_UNCOR__1up); /// (theSfunc_ChargeIDSel);  
	  
	  
	  double theSf_CFID(1.), theSf_stat1up_CFID(1.), theSf_syst1up_CFID(1.);
	  double theSf_stat_CFID(1.), theSf_syst_CFID(1.);
	  if(!isAtlfast && isMC && theEl.FourVector.Pt() > 25000.)
	  {
		if(ismc16d || RunNb==300000)  /// ortherwise I cannot take the SFs in mc16d
		{
			eventInfo->auxdecor<unsigned>("RandomRunNumber") = 300000;				
		}		
		ANA_CHECK( m_electronSF_CFID->applySystematicVariation(CP::SystematicSet("")) );
		ANA_CHECK( m_electronSF_CFID->getEfficiencyScaleFactor (**el_itr,theSf_CFID) );
		ANA_CHECK( m_electronSF_CFID->applySystematicVariation(CP::SystematicSet("EL_CHARGEID_STAT__1up")) );
		ANA_CHECK( m_electronSF_CFID->getEfficiencyScaleFactor (**el_itr,theSf_stat1up_CFID) );
		ANA_CHECK( m_electronSF_CFID->applySystematicVariation(CP::SystematicSet("EL_CHARGEID_SYStotal__1up")) );
		ANA_CHECK( m_electronSF_CFID->getEfficiencyScaleFactor (**el_itr,theSf_syst1up_CFID) );
		if(ismc16d || RunNb==300000)  /// ortherwise I cannot take the SFs in mc16d
		{	
			eventInfo->auxdecor<unsigned>("RandomRunNumber") = tPRWrandomRunNumber;			
		}

		theSf_stat_CFID = theSf_stat1up_CFID; ///-theSf_CFID;
		theSf_syst_CFID = theSf_syst1up_CFID; ///-theSf_CFID;
 	  }
	  else if(!isAtlfast && isMC && theEl.FourVector.Pt() < 25000.)
	  {
		/// not correct, but trying to avoid a crash (and set the sys to 0)
		theSf_CFID = 1.;
		theSf_stat_CFID = 0;
		theSf_syst_CFID = 1.02;		
	  }		  
	 
	  /// http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/charge_misID/
	  /// https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/ElectronPhotonID/ElectronEfficiencyCorrection/Root/ElectronChargeEfficiencyCorrectionTool.cxx
	  double tmpCFID = (isMC ? theSf_CFID : 1.);
	  if(false) std::cout << "CFID:: el pt=" << theEl.FourVector.Pt() << ", SFweightCFID=" << tmpCFID << std::endl;	
	  El_SFweightCFID.push_back(tmpCFID); /// Scale factor needed for the Charge ID Tool (later we can use it to take the charge flip bkg)	  
	  double tmpCFID_stat = (isMC ? theSf_stat_CFID : 1.);
	  El_SFStatweightCFID.push_back(tmpCFID_stat); /// stat unc	  
	  double tmpCFID_syst = (isMC ? theSf_syst_CFID : 1.);
	  El_SFSystweightCFID.push_back(tmpCFID_syst); /// syst unc
	  
	   
	  /// float sf  = isMC ? objTool->GetSignalElecSF( **el_itr, false, false, false, true) : 1. ;
      El_IsoSFwMediumLH.push_back(theSf_isoLhMed);
      El_IsoSFwUncMediumLH.push_back(theSfunc_Iso_TOTAL_1NPCOR_PLUS_UNCOR__1up); /// (theSfunc_Iso);
	  
     
      El_TrigMatch_e12_lhloose_L1EM10VH.push_back(objTool->IsTrigMatched(*el_itr, "HLT_e12_lhloose_L1EM10VH"));
      El_TrigMatch_e17_lhloose.push_back(objTool->IsTrigMatched(*el_itr, "HLT_e17_lhloose"));
      El_TrigMatch_e24_lhmedium_L1EM20VH.push_back(objTool->IsTrigMatched(*el_itr, "HLT_e24_lhmedium_L1EM20VH"));   
      El_TrigMatch_e24_lhmedium_iloose_L1EM20VH.push_back(objTool->IsTrigMatched(*el_itr, "HLT_e24_lhmedium_iloose_L1EM20VH"));   
	  El_TrigMatch_e24_lhmedium_nod0_ivarloose.push_back( objTool->IsTrigMatched(*el_itr,"HLT_e24_lhmedium_nod0_ivarloose") ); 
      El_TrigMatch_e24_lhtight_nod0_ivarloose.push_back( objTool->IsTrigMatched(*el_itr,"HLT_e24_lhtight_nod0_ivarloose") ); 
	  El_TrigMatch_e26_lhtight_nod0_ivarloose.push_back(objTool->IsTrigMatched(*el_itr, "HLT_e26_lhtight_nod0_ivarloose")); 
	  El_TrigMatch_e60_lhmedium.push_back(objTool->IsTrigMatched(*el_itr, "HLT_e60_lhmedium"));
	  El_TrigMatch_e60_lhmedium_nod0.push_back( objTool->IsTrigMatched(*el_itr,"HLT_e60_lhmedium_nod0") ); 
      El_TrigMatch_2e12_lhloose_L12EM10VH.push_back(objTool->IsTrigMatched(*el_itr,"HLT_2e12_lhloose_L12EM10VH")); 
      El_TrigMatch_2e15_lhloose_L12EM10VH.push_back(objTool->IsTrigMatched(*el_itr,"HLT_2e15_lhloose_L12EM10VH")); 
      El_TrigMatch_2e15_lhvloose_L12EM13VH.push_back(objTool->IsTrigMatched(*el_itr,"HLT_2e15_lhvloose_L12EM13VH")); 
      El_TrigMatch_2e15_lhvloose_nod0_L12EM13VH.push_back(objTool->IsTrigMatched(*el_itr,"HLT_2e15_lhvloose_nod0_L12EM13VH")); 
      El_TrigMatch_2e17_lhvloose_nod0.push_back(objTool->IsTrigMatched(*el_itr,"HLT_2e17_lhvloose_nod0")); 
      El_TrigMatch_2e17_lhvloose_nod0_L12EM15VHI.push_back(objTool->IsTrigMatched(*el_itr,"HLT_2e17_lhvloose_nod0_L12EM15VHI")); 
	  El_TrigMatch_2e24_lhvloose_nod0.push_back(objTool->IsTrigMatched(*el_itr, "HLT_2e24_lhvloose_nod0"));   
      El_TrigMatch_e17_lhloose_mu14.push_back(objTool->IsTrigMatched(*el_itr,"HLT_e17_lhloose_mu14")); 
      El_TrigMatch_e17_lhloose_nod0_mu14.push_back(objTool->IsTrigMatched(*el_itr,"HLT_e17_lhloose_nod0_mu14")); 
	  

	  
      /// Analysis level signal electrons (needed for trigger matching):
	  bool signal_electron = true;
	  signal_electron &= (int)(*el_itr)->auxdata< char >("signal");
	  signal_electron &= theEl.FourVector.Pt() > 10000;
	  signal_electron &= fabs(theEl.FourVector.Eta()) <=2.47;
	  signal_electron &= ((fabs((*el_itr)->caloCluster()->eta()) >= 1.52 || fabs((*el_itr)->caloCluster()->eta()) <= 1.37));
	  signal_electron &= (*el_itr)->auxdata<float>("ptvarcone20")/theEl.FourVector.Pt() < 0.06;
	  signal_electron &= (*el_itr)->auxdata<float>("topoetcone20")/theEl.FourVector.Pt() < 0.06;
	  signal_electron &= (double)CFTToolmedium->calculate( *el_itr) >-0.28087; 
	  signal_electron &= fabs(theEl.z0pvtx) < 0.5;
	  signal_electron &= fabs(theEl.sigd0) < 5;

	  if ( signal_electron ){ 
		myTriggeringElectrons.push_back(*el_itr);
		if (theEl.FourVector.Pt() > 20000) ne20++;
	  }

      NEl++;
    }
    ///    ++el_itr_med;    
  }
  if(DEBUG) cout << "Done with electrons!" << endl;

  
  
  /// Dilepton trigger scale factors
  /// and finally retrieve the trigger scale factor
  /// need to change the nEle cut if e24 !!!!
  /**double trigger_sf = 1.;
  bool triggermatch = (ne20>=2 || (ne20>=1 && nmu20>=1) || (nmu21>=1 && nmu10>=2 && tPRWrandomRunNumber<290000) || (nmu23>=1 && nmu10>=2 && tPRWrandomRunNumber>290000));
  if(isMC && triggermatch){
	auto cc = myTriggerSFTool->getEfficiencyScaleFactor(myTriggeringElectrons, myTriggeringMuons, trigger_sf);
	if(cc != CP::CorrectionCode::Ok)
	{
		Warning(APP_NAME, "Scale factor evaluation failed");
	}
	tTriggerDileptonSF = trigger_sf;

	//	if( tEventNumber == 158230 || tEventNumber == 160679 || tEventNumber == 159403 || tEventNumber ==  160593) 
	//	  std::cout << tEventNumber << " " << trigger_sf << std::endl;
  }*/
  /// change this once all the SFs are avaialble 
  tTriggerDileptonSF = 1.;
  
  if(DEBUG) cout << "Done with trigger SFs!" << endl;
  
  
  /// Trigger
  for(int t=0; t<nTrig;t++)
  {
    ///cout << "debug " << nameTrig[t] << ": "  << objTool->isTrigPassed(nameTrig[t].Data()) << endl;  
    trig_map[ nameTrig[t].Data() ] = objTool->IsTrigPassed(nameTrig[t].Data());
  }
  if(DEBUG) cout << "Done with triggers!" << endl;
  


  ///--------------
  /// PHOTON BLOCK
  ///--------------
    
  /// Get the Photons from the event
  /// const xAOD::PhotonContainer* photons = 0;
  /// ANA_CHECK( m_event->retrieve( photons, "PhotonCollection" ) );
  /// Info( APP_NAME, "Number of photons: %i",
  /// static_cast< int >( photons->size() ) );
  


  ///-----------------
  /// GET REBUILT MET
  ///-----------------
  
  double mpx = 0;
  double mpy = 0;
  xAOD::MissingETContainer* mettst = new xAOD::MissingETContainer;
  xAOD::MissingETAuxContainer* mettst_aux = new xAOD::MissingETAuxContainer;
  mettst->setStore(mettst_aux);
	
  /// Temporary protection against events with no PV (needed to avoid MET crashes)
  /// const xAOD::Vertex* pv = objTool->GetPrimVtx(); done earlier to avoid OR crash
  if(!pv)
  {
    Info( APP_NAME,"No primary vertex found, skip event" );
  }
  else
  {
	ANA_CHECK( objTool->GetMET(*mettst,
						   jets_copy,
						   electrons_copy,
						   muons_copy,
						   photons_copy, 
						   NULL,
						   true) );

	/// when debugging I use these variables: 
	int Nmu  = muons_copy ? muons_copy->size() : 0;
	int Nel  = electrons_copy ? electrons_copy->size() : 0;
	int Njet = jets_copy ? jets_copy->size() : 0;
	int Ntau = 0; ///tau ? tau->size() : 0;
	int Nph  = photons_copy ? photons_copy->size() : 0;
	bool doTST = true;
	bool doJVT = true;

    mpx = (*mettst)["Final"]->mpx();
    mpy = (*mettst)["Final"]->mpy();

    Etmiss_TST_Etx = mpx;
    Etmiss_TST_Ety = mpy;
    Etmiss_TST_Et = sqrt(Etmiss_TST_Etx*Etmiss_TST_Etx + Etmiss_TST_Ety*Etmiss_TST_Ety);
	
	if(false)
	{
	  Info( APP_NAME, " MET TST px = %f py = %f ", (*mettst)["Final"]->mpx(), (*mettst)["Final"]->mpy() );
	  std::cout<<APP_NAME<<Form(" DEBUG EvtNbr = %i\t Objects for MET: %i mu, %i el, %i jets, %i ph, %i tau [JVT=%i|TST=%i]",tEventNumber,Nmu,Nel,Njet,Nph,Ntau,doTST,doJVT);
	  cout<<endl;
		
	  cout << "tEventNumber = " << tEventNumber << endl;
	  cout << "Nmu = " << Nmu << ", Nel = " << Nel << ", Njet = " << Njet << ", Ntau = " << Ntau << ", Nph = " << Nph << endl;
	  cout << "mpx = " << mpx << ", mpy = " << mpy  << endl;
	  cout << "Etmiss_TST_Et = " << Etmiss_TST_Et << endl;
	  
	  mu_itr = (muons_copy)->begin();
      mu_end = (muons_copy)->end();
	  for( ; mu_itr != mu_end; ++mu_itr ) 
	  {
		cout << "muon, pt = " << (*mu_itr)->pt() << ", eta = " << (*mu_itr)->eta() << ", phi = " << (*mu_itr)->phi() << endl;
		cout << "mu_z0sinTheta = " << (*mu_itr)->auxdata< float >("z0sinTheta") << endl;
	  }
	  
	  el_itr = (electrons_copy)->begin();
      el_end = (electrons_copy)->end();
	  for( ; el_itr != el_end; ++el_itr ) 
	  {
		cout << "electron, pt = " << (*el_itr)->pt() << ", eta = " << (*el_itr)->eta() << ", phi = " << (*el_itr)->phi() << endl;
		cout << "el_z0sinTheta = " << (*el_itr)->auxdata< float >("z0sinTheta") << endl;
	  }
	  
	  jet_itr = (jets_copy)->begin();
	  jet_end = (jets_copy)->end();
	  for( ; jet_itr != jet_end; ++jet_itr ) 
	  {
		cout << "jet, pt = " << (*jet_itr)->pt() << ", eta = " << (*jet_itr)->eta() << ", phi = " << (*jet_itr)->phi() << endl;
	  }
	  
	  ph_itr = (photons_copy)->begin();
	  ph_end = (photons_copy)->end();
	  for( ; ph_itr != ph_end; ++ph_itr ) 
	  {
		cout << "photon, pt = " << (*ph_itr)->pt() << ", eta = " << (*ph_itr)->eta() << ", phi = " << (*ph_itr)->phi() << endl;
	  }
	}
  }


  ///------------
  /// Truth business 
  ///------------
  
  if(DEBUG)  Info( APP_NAME, "Starting truth...." );
  
 
  /// need these branches for ttV HF / tt Hf studies (2b / 3b regions)
  int ttbarHF(-999), ttbarHF_ext(-999), ttbarHF_prompt(-999); 
  std::map<int,int> mapJetClassHF;
  mapJetClassHF.clear();
  if (isMC) 
  {
	/// Ttbar + HF classification
	bool debugme=false;
	if(debugme) std::cout << "\nEventNumber=" << tEventNumber << std::endl;

	ClassifyAndCalculateHF classHF(m_event,"TruthParticles","AntiKt4TruthJets");
	classHF.apply(ttbarHF, ttbarHF_ext, ttbarHF_prompt, mapJetClassHF);

	if(debugme && false)
	{
		int counter=0;
		int jet_n=truthJets->size();
		for(auto jptr : *truthJets)
		{
		  std::cout << " MyxAODAna -> jet " << counter << "/" << jet_n << ", pT=" << jptr->pt() << ", eta=" << jptr->eta() << ", classHF=" << mapJetClassHF[counter]<< std::endl;
		  counter++;
		}
	}

	int extB = 0; 
	int extC = 0;   
	///  ext_code = 1000*b+100*B+10*c+1*C;
	int itr=0;
	int ttbarHF_ext_tmp = ttbarHF_ext;
	while (ttbarHF_ext_tmp > 0) 
	{
		int digit = ttbarHF_ext_tmp % 10;
		if(itr == 0 || itr == 1) extC += digit;
		if(itr == 2 || itr == 3) extB += digit;
		ttbarHF_ext_tmp /= 10; 
		itr++;
	}

	if(debugme){
	///	if(ttbarHF_ext != 0){
	/// std::cout << "====>  ttbarHF=" << ttbarHF << ", ttbarHF_ext=" << ttbarHF_ext << ", ttbarHF_prompt=" << ttbarHF_prompt << std::endl;
	std ::cout << "============>  #b=" << extB << ", #c=" << extC << std::endl;
	///	}
	}

	extraB = extB;
	extraC = extC;
  
  
	/// Cross check classification
	if(debugme) std::cout << "------------- cross check -------------" << std::endl;
	/// Get the Truth Particles from the event:
	const xAOD::TruthParticleContainer* truthPximo = 0;
	ANA_CHECK( m_event->retrieve( truthPximo, "TruthParticles" ) );
	if(debugme) Info( APP_NAME, "Number of TRUTH particles: %i", static_cast< int >( truthPximo->size() ) );

	int extratestB = 0;
	int extratestC = 0;
	for (const auto& tp : *truthPximo)
	{
		if( tp->pt() < 10E03) continue; /// omit low-pT stuff
			
		if( fabs(tp->pdgId())==5 || fabs(tp->pdgId())==4 )
		{
		  int firstMo = 0;
		  if(tp->nParents() >0)
		  {
			for (const auto& tpC : *truthPximo) 
			{
			  if (tpC->barcode() == tp->parent(0)->barcode() ) 
			  {
				firstMo = tpC->pdgId();
			  }
			}
		  }

		  if(firstMo == tp->pdgId()) continue;

		  if(fabs(tp->pdgId())==5)
		  {
			if( fabs(firstMo)!=6) extratestB++;
		  }
		  
		  if(fabs(tp->pdgId())==4)
		  {
			if( fabs(firstMo)!=24 ) extratestC++;
		  }

		  if(debugme)
		  {
			Info( APP_NAME, "TRUTH pdgID:%d   pT/eta/phi: %g/%g/%g   barcode: %d status: %d  parents: %d %d",
				tp->pdgId(),  tp->pt(),   tp->eta(),   tp->phi(),
				tp->barcode(),    tp->status(),
				tp->nParents(), firstMo
				);
		  }
		}
	}
	if(DEBUG) std ::cout << "Ximo:     #b=" << extratestB << ", #c=" << extratestC << std::endl;
	if(DEBUG) std ::cout << "Othmane:  #b=" << extB << ", #c=" << extC << std::endl;
    if(DEBUG) cout << "End Ttbar + HF classification\n";
  }

 
  /// find Z,W and decay products
  vector<CutClass::Particle> vTruthJets; vTruthJets.clear();
  vector<CutClass::Particle> vTruthRealL; vTruthRealL.clear();
  CutClass::Particle theTruthAntikTJet;

  if (isMC) 
  {
	/// take the truth jets from "AntiKt4TruthJets" collection 
	for (const auto& truthJet : *truthJets) 
	{   
      theTruthAntikTJet.FourVector.SetPtEtaPhiE( truthJet->pt(),truthJet->eta(),truthJet->phi(),truthJet->e() );
    
      if(theTruthAntikTJet.FourVector.Pt()<10000. ) continue; /// as used to fill the mapJetClassHF

      int ijet = -1;
      ijet = truthJet->auxdata<int>("PartonTruthLabelID") ;
      TruthAntiktJet_PartonTruthLabel.push_back( ijet );
      ijet = -1;
      ijet = truthJet->auxdata<int>("ConeTruthLabelID") ;
      TruthAntiktJet_ConeTruthLabel.push_back( ijet );
	  /// ijet = -1;
      /// ijet = truthJet->auxdata<int>("HadronConeExclTruthLabelID") ;
      /// TruthAntiktJet_HadronConeExclTruthLabelID.push_back( ijet );
	
      TruthAntiktJet_eta.push_back(theTruthAntikTJet.FourVector.Eta());
      TruthAntiktJet_phi.push_back(theTruthAntikTJet.FourVector.Phi());
      TruthAntiktJet_pT.push_back (theTruthAntikTJet.FourVector.Pt());
      TruthAntiktJet_E.push_back  (theTruthAntikTJet.FourVector.E());
	  
	  /// need these branches for ttV HF / tt Hf studies (2b / 3b regions)
      TruthAntiktJetJet_ClassHF.push_back(mapJetClassHF[NTruthAntiktJet]);
	  
      NTruthAntiktJet++;      
    }	
    if(DEBUG) cout << "DEBUG :: End truth antikt jet part " << endl;
    

	/// take truth missing transverse energy
    const xAOD::MissingETContainer* TruthMET = 0;
    ANA_CHECK(m_event->retrieve( TruthMET, "MET_Truth" ) );
    
    xAOD::MissingETContainer::const_iterator TruthMET_it = TruthMET->find("NonInt");
    if (TruthMET_it == TruthMET->end()) std::cout << "No NonINT inside MET container" << std::endl;
    xAOD::MissingET* TruthMET_NonInt = *TruthMET_it;

    /// const xAOD::MissingETContainer* truth_met = 0 ;
    /// CHECK(m_event->retrieve(truth_met, "MET_Truth"));
    /// xAOD::MissingETContainer* met= new xAOD::MissingETContainer();
    /// xAOD::MissingETAuxContainer* met_aux = new xAOD::MissingETAuxContainer();
    /// met->setStore(met_aux);
    /// met->push_back(new xAOD::MissingET(truth_met->at(0)->mpx(),truth_met->at(0)->mpy(),truth_met->at(0)->sumet(),"Final",MissingETBase::Source::total()));
	
    Etmiss_Truth_Etx = TruthMET_NonInt->mpx(); 
    Etmiss_Truth_Ety = TruthMET_NonInt->mpy(); 
    Etmiss_Truth_Et  = sqrt(Etmiss_Truth_Etx*Etmiss_Truth_Etx+Etmiss_Truth_Ety*Etmiss_Truth_Ety); 
    
	/// cout << " MEt: " << Etmiss_Truth_Etx << " / " << Etmiss_Truth_Ety << " / " << Etmiss_Truth_Et << endl; 
	if(DEBUG) cout << "DEBUG :: End truth met part " << endl;

	
	/// fill variables associated to SUSY particles
    pdgid1 = 0;
    pdgid2 = 0;
    ANA_CHECK(objTool->FindSusyHP(truthP, pdgid1, pdgid2) );
    if(DEBUG) Info( APP_NAME, "Initial SUSY particles: %i %i",pdgid1, pdgid2 );
          
    int nGoodL = 0;
    CutClass::Particle theRealL, theRealJet;
    for (auto parent : *truthP)
	{
	  /// as Ximo!	 
	  /**if( parent->pt() > 10000. && parent->nParents()>0 && 
	      (fabs( parent->pdgId() ) == 11 || fabs( parent->pdgId() ) == 13 || fabs( tp->pdgId() ) ==12 || fabs( tp->pdgId() ) ==14 || fabs( tp->pdgId() ) ==16))
	  {
	    for (const auto& tpC : *truthP) 
		{
		  if (tpC->barcode() == parent->parent(0)->barcode()) 
		  {
			if( fabs(tpC->pdgId()) > 1000000 || fabs(tpC->pdgId())==23 || fabs(tpC->pdgId()) ==24)
			{
			  ++nGoodL;
		      if(displ)cout << count << " (ximo) child ID  " << parent->pdgId() << " parent id " << tpC->pdgId() << endl;
			}
		  }
	    }		
	  }*/ 
 	  
	  if(!parent->hasDecayVtx()) continue;
   
      auto pid = parent->pdgId();
	  auto vtx = parent->decayVtx();
	  const int ndec = vtx->nOutgoingParticles(); /// or parent->nChildren()
  
	  if(!ndec) continue;
	  if(parent->p4().M()<12000.) continue;
	  if(parent->pt() < 10000.) continue;
     
  	 
	  /// keep the quarks here - truth OR studies 
	  for(int j=0;j<ndec;++j)
	  {
		if(fabs(pid)!=6 && (fabs(pid)<21 || fabs(pid)>37) && (fabs(pid)<1000001 || fabs(pid)>2999999)) continue;
	    /// gtt, parents : gluino(1000021) - top(6) - top(6) - neutralino1(1000022) ; top(6) - W(24) b(5) ; W - jets

		auto child = vtx->outgoingParticle(j);
		if(!child) continue;
		int cid = child->pdgId();
	
		if(fabs(cid)==fabs(pid)) continue;
	
		if(displ)
		{
		  cout << "Quark loop : " << count << ", j= " << j << " parent ID " << pid << ", with nch= " << ndec << ", and child quark id= " << cid << endl;
		}
		if(child->pt() < 10000.) continue;	
		
		if(fabs(cid)>=1 && fabs(cid)<=5)
		{
	       theRealJet.FourVector.SetPtEtaPhiE(child->pt(),child->eta(),child->phi(),child->e());
		   theRealJet.ID=(int)(cid);
		   theRealJet.Origin=(int)(parent->pdgId());
		   vTruthJets.push_back(theRealJet);
		   
		   if(displ) 
		   {	
			cout << count << " keep the jet with ID " << theRealJet.ID << ", and origin " << theRealJet.Origin << endl;
			cout << "pt = " << child->pt() << ", eta = " << child->eta() << ", phi = " << child->phi() << endl;
		   }	   
		}
	  }
	  
	  /// take the leptons here
	  bool isParentTau = false;
	  if(fabs(pid) > 1000000 || fabs(pid)==24 || fabs(pid)==23) /// keep only W, Z and tau as parent -- final parent in the decay chain
	  for(int i=0;i<ndec;i++)
	  {
		auto child = vtx->outgoingParticle(i);
		if(!child) continue;
		int cid = child->pdgId();

		if(fabs(cid)==fabs(pid)) continue;
		if(child->pt() < 10000.) continue;

		isParentTau = false;
		while(fabs(cid)==15) /// catch tau-mediated prompt electrons
		{
		   if(!child->hasDecayVtx()) break;

		   auto tauvtx = child->decayVtx();
		   const int ntaudec = tauvtx->nOutgoingParticles();
		   
		   if(ntaudec==1 && tauvtx->outgoingParticle(0)->absPdgId()==15) break; /// need only the tau child - e or mum

		   for(int k=0;k<ntaudec;++k)
		   {  
			auto tauchild = tauvtx->outgoingParticle(k);
			if(tauchild) 
			{
			 if(DEBUG) cout << "Working on run/evt: "<<  RunNb << " / " << tEventNumber << endl;
			
			 cid = tauchild->pdgId();			
			 if(displ) cout << "index = "<< k << " :: tau with n= " << ntaudec << " decays, and child = " << cid << endl; 
			
			 /// if(fabs(cid)==11 || fabs(cid)==13 || fabs(cid)==12 || fabs(cid)==14 || fabs(cid)==16)
			 if(fabs(cid)==11 || fabs(cid)==13)
			 {
			  child = tauchild;
			  isParentTau = true;
			  if(displ) cout <<"isParentTau = true with child = " << tauchild->pdgId() << " (pt=" << child->pt() << ")" << endl;
			  break;
			 }
			}
			else 
		     break;
		   }
		   
           break;		   
		}
		
		/// and further we take the info only for the leptons coming from W / Z  / tau
		/// if(fabs(cid)==11 || fabs(cid)==13 || fabs(cid)==12 || fabs(cid)==14 || fabs(cid)==16)
		if(fabs(cid)==11 || fabs(cid)==13)
		{
			theRealL.FourVector.SetPtEtaPhiM(child->pt(),child->eta(),child->phi(),child->m() );
			theRealL.ID=(int)(cid);
			if(isParentTau)
			{
				theRealL.Origin=(int)(vtx->outgoingParticle(i)->pdgId()); // tau PiD always +/- 15
				if(displ) cout <<"isParentTau : child " <<child << " stored (pt=" << child->pt() << ")" << endl;
			}
			else
				theRealL.Origin=(int)(parent->pdgId());
			vTruthRealL.push_back(theRealL);	
		
			if(displ)
			{	
			  cout << count << " keep the lepton with ID " << theRealL.ID << ", and origin " << theRealL.Origin << endl;
			  cout << "pt = " << child->pt() << ", eta = " << child->eta() << ", phi = " << child->phi() << endl;
		    }	
		}
	  }
	   
	}   
	if(displ) cout << "DEBUG :: End get truth particles info part " << endl;



    gluDecay1 = 0;
    gluDecay2 = 0;	 
    for (const auto& tp : *truthP) 
	{
	  ///check "if gluino"
	  if (tp->pdgId()==1000021)
	  {
		int _ch[2]={0}; int ich=0;
		bool foundSUSY = false;
		
		if (tp->nChildren() == 3) 
		{
			for(unsigned int ch=0; ch<tp->nChildren(); ch++)
			{
			  for (const auto& tpC : *truthP) 
			  {
				if (tpC->barcode() == tp->child(ch)->barcode() ) 
				{
				  if((abs(tpC->pdgId()) > 1000020 && abs(tpC->pdgId()) < 1000040))
				  {
					foundSUSY=true;
				  }
				  else
				  {
					_ch[ich]=tpC->pdgId(); 
					ich++;
					///  cout << " `- AA Ch: " <<tpC->pdgId() << " " << tpC->barcode() <<  " " << tpC->nChildren() << endl;
				  }
				}
			  }
			}
		}
		  
		if(foundSUSY && _ch[0]==-1*_ch[1])
		{
			if(gluDecay1==0) gluDecay1=abs(_ch[0]);
			else gluDecay2=abs(_ch[0]);
		}
	  } /// END "if gluino"
    }
    /// Info( APP_NAME, "Gluino decays: %i %i",gluDecay1, gluDecay2 );

 
 
    NTruthJet = vTruthJets.size(); /// any parent, final state quarks
    for(Int_t i=0;i<NTruthJet;i++)
    {
     TruthJet_eta.push_back(vTruthJets[i].FourVector.Eta());
     TruthJet_phi.push_back(vTruthJets[i].FourVector.Phi());
     TruthJet_pT.push_back(vTruthJets[i].FourVector.Pt());
	 TruthJet_E.push_back(vTruthJets[i].FourVector.E());
     TruthJet_id.push_back((int)vTruthJets[i].ID);
     TruthJet_origin.push_back((int)vTruthJets[i].Origin);
    }
	///
	NTruthRealL = vTruthRealL.size(); /// only leptons from W, Z and tau
	for(Int_t i=0;i<NTruthRealL;i++)
    {
     TruthRealL_eta.push_back(vTruthRealL[i].FourVector.Eta());
     TruthRealL_phi.push_back(vTruthRealL[i].FourVector.Phi());
     TruthRealL_pT.push_back(vTruthRealL[i].FourVector.Pt());
     TruthRealL_id.push_back((int)vTruthRealL[i].ID);
     TruthRealL_origin.push_back((int)vTruthRealL[i].Origin);
    }
	if(displ)
		cout << count << " nGoodL = " << nGoodL << ", NTruthRealL = " << NTruthRealL << ", NTruthJet = " << NTruthJet << endl;
	/// nGoodL -> not including tau children!!! (too lazy to take care of this, as it's not used...)
	
	if(DEBUG) cout << "DEBUG :: End fill truth variables part " << endl;
    


  
    /// PDF INFO!!!
    /// Get the Truth Event information from the event:
    const xAOD::TruthEventContainer* truthE = 0;
    ANA_CHECK( m_event->retrieve( truthE, "TruthEvents" ) );
    
	xAOD::TruthEventContainer::const_iterator truthE_itr = truthE->begin();
	xAOD::TruthEventContainer::const_iterator truthE_end = truthE->end();
	for( ; truthE_itr != truthE_end; ++truthE_itr ) 
	{
     
	  if((*truthE_itr)->isAvailable< float >( "X1" ) )
	  {
		TruthX1 = (*truthE_itr)->auxdata<float>("X1");
		TruthX2 = (*truthE_itr)->auxdata<float>("X2");
		/// TruthXF1 = (*truthE_itr)->auxdata<float>("XF1");
		/// TruthXF2 = (*truthE_itr)->auxdata<float>("XF2");
		TruthQ   = (*truthE_itr)->auxdata<float>("Q");
		TruthPDGID1 = (*truthE_itr)->auxdata<int>("PDGID1");
		TruthPDGID2 = (*truthE_itr)->auxdata<int>("PDGID2");
		/// TruthPDFID1 = (*truthE_itr)->auxdata<int>("PDFID1");
		/// TruthPDFID2 = (*truthE_itr)->auxdata<int>("PDFID2");
	  }
	  else
	  { /// Somehow it does not exist
		TruthX1     = -1;
		TruthX2     = -1;
		/// TruthXF1    = -1;
		/// TruthXF2    = -1;
		TruthQ      = -1;
		TruthPDGID1 = -1;
		TruthPDGID2 = -1;
		/// TruthPDFID1 = -1;
		/// TruthPDFID2 = -1;    
	  }
	}
	if(DEBUG) Info( APP_NAME, "Ended PDF...." );

    ///SherpaNjetWeight =  objTool->getSherpaVjetsNjetsWeight(njet); 
	/// Default SUSYTools :
	SherpaNjetWeight =  objTool->getSherpaVjetsNjetsWeight();
  }
  if(DEBUG) cout << "DEBUG :: End truth / MC block " << endl;

  

  /// ----------------------------
  /// keep track of all the events
  /// ----------------------------
  hWeights->Fill(1., EventWeight);
  hEvents-> Fill(1., 1.);
  hWeightsSquared->Fill(1., EventWeight*EventWeight);
  if(DEBUG) cout << "DEBUG :: histo with initial EventWeight filled " << endl;
  
    
  /// -------------
  ///Fill our tree    
  /// -------------  
  if( isMC || (NMu+NEl)>0 )
  {
    hWeights->Fill(2., EventWeight);
    hEvents-> Fill(2., 1.);
    hWeightsSquared->Fill(2., EventWeight*EventWeight);
	if(DEBUG) cout << "DEBUG :: histo with stored EventWeight filled " << endl;
    
    MyTree->Fill();
	if(DEBUG) cout << "DEBUG :: MyTree filled " << endl;
  }

  if(DEBUG) Info( APP_NAME, "Filled, time to delete...." );

  ///  return EL::StatusCode::SUCCESS;

  /// The containers created by the shallow copy are owned by you. Remember to delete them
  /// delete jets_copy;
  /// delete jets_copyaux;
  /// delete muons_copy;
  /// delete muons_copyaux;
  /// delete electrons_copy;
  /// delete electrons_copyaux;
  /// delete photons_copy;
  /// delete photons_copyaux;
  delete mettst;
  delete mettst_aux;
  ///  delete goodJets;
  if(DEBUG) cout << "DEBUG :: end delete the shallow copy " << endl;
  
  if(DEBUG && false) store.print();
  if(DEBUG && false) cout << "DEBUG :: this was store.print()" << endl; 
  store.clear();   
  if(DEBUG) cout << "DEBUG :: the store is cleaned " << endl; 

  
  ///  CALLGRIND_STOP_INSTRUMENTATION;

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: postExecute ()
{
  /// Here you do everything that needs to be done after the main event
  /// processing.  This is typically very rare, particularly in user
  /// code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: finalize ()
{
  /// This method is the mirror image of initialize(), meaning it gets
  /// called after the last event has been processed on the worker node
  /// and allows you to finish up any objects you created in
  /// initialize() before they are written to disk.  This is actually
  /// fairly rare, since this happens separately for each worker node.
  /// Most of the time you want to do your post-processing on the
  /// submission node after all your histogram outputs have been
  /// merged.  This is different from histFinalize() in that it only
  /// gets called on worker nodes that processed input events.

  DEBUG = false;
  
  const char* APP_NAME = "MyxAODAnalysis::finalize()";   
  Info(APP_NAME, "Deleting all tools");
  
  Info(APP_NAME, "All done, bye!" );
  /// MyTree->Print();

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: histFinalize ()
{
  /// This method is the mirror image of histInitialize(), meaning it
  /// gets called after the last event has been processed on the worker
  /// node and allows you to finish up any objects you created in
  /// histInitialize() before they are written to disk.  This is
  /// actually fairly rare, since this happens separately for each
  /// worker node.  Most of the time you want to do your
  /// post-processing on the submission node after all your histogram
  /// outputs have been merged.  This is different from finalize() in
  /// that it gets called on all worker nodes regardless of whether
  /// they processed input events.

  ///   hEvents->SetBinContent(1, m_initialNevents);
  ///   hEvents->SetBinContent(2, m_finalNevents);
  
  ///   hWeights->SetBinContent(1, m_finalSumOfWeights);
  ///   hWeights->SetBinContent(2, m_initialSumOfWeights);
  


  return EL::StatusCode::SUCCESS;
}
