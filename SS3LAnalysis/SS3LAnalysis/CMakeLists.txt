################################################################################
# Package: SS3LAnalysis
################################################################################

# Declare the package name:
atlas_subdir( SS3LAnalysis )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthToolSupport/AsgTools
                          Control/xAODRootAccess
                          DataQuality/GoodRunsLists
                          Event/FourMomUtils
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODCutFlow
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMetaDataCnv
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODTriggerCnv
                          Event/xAOD/xAODTruth
                          PhysicsAnalysis/D3PDTools/EventLoop
                          PhysicsAnalysis/D3PDTools/EventLoopGrid
                          PhysicsAnalysis/D3PDTools/EventLoopAlgs
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          PhysicsAnalysis/AnalysisCommon/AssociationUtils
                          PhysicsAnalysis/AnalysisCommon/IsolationSelection
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          PhysicsAnalysis/AnalysisCommon/PMGTools
                          PhysicsAnalysis/AnalysisCommon/PileupReweighting
                          PhysicsAnalysis/D3PDTools/RootCoreUtils
                          PhysicsAnalysis/D3PDTools/SampleHandler
                          PhysicsAnalysis/ElectronPhotonID/ElectronEfficiencyCorrection
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonFourMomentumCorrection
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          PhysicsAnalysis/ElectronPhotonID/IsolationCorrections
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonShowerShapeFudgeTool
                          PhysicsAnalysis/ElectronPhotonID/PhotonEfficiencyCorrection
                          PhysicsAnalysis/JetMissingEtID/JetSelectorTools
                          PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/xAODBTaggingEfficiency
                          PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections
                          PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonEfficiencyCorrections
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          PhysicsAnalysis/SUSYPhys/SUSYTools
                          PhysicsAnalysis/TauID/TauAnalysisTools
                          Reconstruction/Jet/JetAnalysisTools/JetTileCorrection
                          Reconstruction/Jet/JetCalibTools
                          Reconstruction/Jet/JetJvtEfficiency
                          Reconstruction/Jet/JetMomentTools
                          Reconstruction/Jet/JetResolution
                          Reconstruction/Jet/JetSubStructureUtils 
                          Reconstruction/Jet/JetUncertainties
                          Reconstruction/Jet/JetReclustering
                          Reconstruction/MET/METUtilities
                          Reconstruction/MET/METInterface
                          Tools/PathResolver
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TriggerMatchingTool
                          Trigger/TrigAnalysis/TrigGlobalEfficiencyCorrection
                          Trigger/TrigConfiguration/TrigConfxAOD
)


# Find the needed external(s):
find_package( ROOT COMPONENTS Core RooFitCore RooFit Graf Minuit )

# build a ROOT (old CINt style) dictionary for the library
atlas_add_root_dictionary ( SS3LAnalysisLib SS3LAnalysisCintDict
                            ROOT_HEADERS 
                            SS3LAnalysis/*.h
                            Root/LinkDef.h
                            EXTERNAL_PACKAGES ROOT
)

# build a shared library
atlas_add_library( SS3LAnalysisLib
                   SS3LAnalysis/*.h Root/*.cxx ${SS3LAnalysisCintDict}
                   PUBLIC_HEADERS SS3LAnalysis
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES}
          	       EventLoop xAODBase xAODRootAccess
                   xAODEventInfo GoodRunsListsLib PileupReweightingLib PATInterfaces
                   PathResolver xAODTau xAODJet xAODMuon xAODEgamma xAODCaloEvent
                   xAODTracking xAODTruth MuonMomentumCorrectionsLib FourMomUtils
                   MuonEfficiencyCorrectionsLib MuonSelectorToolsLib JetCalibToolsLib
                   JetSelectorToolsLib AthContainers
                   ElectronPhotonFourMomentumCorrectionLib
                   ElectronEfficiencyCorrectionLib ElectronPhotonSelectorToolsLib
                   IsolationSelectionLib IsolationCorrectionsLib
                   ElectronPhotonShowerShapeFudgeToolLib
                   PhotonEfficiencyCorrectionLib METUtilitiesLib METInterface SUSYToolsLib
                   TauAnalysisToolsLib AsgTools xAODMissingET JetResolutionLib
                   AssociationUtilsLib JetEDM JetUncertaintiesLib JetReclusteringLib
                   JetCPInterfaces xAODBTaggingEfficiencyLib TrigConfxAODLib
                   TrigDecisionToolLib xAODCutFlow JetMomentToolsLib
                   TriggerMatchingToolLib TrigGlobalEfficiencyCorrectionLib xAODMetaDataCnv xAODTrigger
                   xAODMetaData JetJvtEfficiencyLib PMGToolsLib JetSubStructureUtils
                   JetTileCorrectionLib ${release_libs}
)

# Install files from the package:
atlas_install_data( data/*
					data/GRL
					data/PRW
			       )
# atlas_install_python_modules( python/*.py
			                  # config
                             # )
# atlas_install_joboptions( share/*.py )
