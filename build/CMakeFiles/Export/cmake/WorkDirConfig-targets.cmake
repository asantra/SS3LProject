# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget WorkDir::AddXSecBranchPkg WorkDir::AddXSecBranchPkgPrivate WorkDir::AddVariables WorkDir::AddXSecBranch WorkDir::DiLeptonPkg WorkDir::DiLeptonPkgPrivate WorkDir::DiLeptonLib)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
if(_IMPORT_PREFIX STREQUAL "/")
  set(_IMPORT_PREFIX "")
endif()

# Create imported target WorkDir::AddXSecBranchPkg
add_library(WorkDir::AddXSecBranchPkg INTERFACE IMPORTED)

set_target_properties(WorkDir::AddXSecBranchPkg PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/src/AddXsecBranch"
  INTERFACE_LINK_LIBRARIES "SUSYToolsPkg"
)

# Create imported target WorkDir::AddXSecBranchPkgPrivate
add_library(WorkDir::AddXSecBranchPkgPrivate INTERFACE IMPORTED)

# Create imported target WorkDir::AddVariables
add_executable(WorkDir::AddVariables IMPORTED)

# Create imported target WorkDir::AddXSecBranch
add_executable(WorkDir::AddXSecBranch IMPORTED)

# Create imported target WorkDir::DiLeptonPkg
add_library(WorkDir::DiLeptonPkg INTERFACE IMPORTED)

set_target_properties(WorkDir::DiLeptonPkg PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/src/DiLepton"
  INTERFACE_LINK_LIBRARIES "AthContainersPkg;AsgToolsPkg;xAODRootAccessPkg;GoodRunsListsPkg;FourMomUtilsPkg;xAODBasePkg;xAODCaloEventPkg;xAODCutFlowPkg;xAODEgammaPkg;xAODEventInfoPkg;xAODJetPkg;xAODMetaDataCnvPkg;xAODMissingETPkg;xAODMuonPkg;xAODTauPkg;xAODTrackingPkg;xAODTriggerPkg;xAODTriggerCnvPkg;xAODTruthPkg;EventLoopPkg;EventLoopGridPkg;EventLoopAlgsPkg;InDetTrackSelectionToolPkg;AssociationUtilsPkg;IsolationSelectionPkg;PATInterfacesPkg;PMGToolsPkg;PileupReweightingPkg;RootCoreUtilsPkg;SampleHandlerPkg;ElectronEfficiencyCorrectionPkg;ElectronPhotonFourMomentumCorrectionPkg;ElectronPhotonSelectorToolsPkg;IsolationCorrectionsPkg;ElectronPhotonShowerShapeFudgeToolPkg;PhotonEfficiencyCorrectionPkg;JetSelectorToolsPkg;xAODBTaggingEfficiencyPkg;MuonMomentumCorrectionsPkg;MuonEfficiencyCorrectionsPkg;MuonSelectorToolsPkg;SUSYToolsPkg;TauAnalysisToolsPkg;TriggeranalysisInterfacesPkg;JetTileCorrectionPkg;JetCalibToolsPkg;JetJvtEfficiencyPkg;JetMomentToolsPkg;JetResolutionPkg;JetUncertaintiesPkg;JetReclusteringPkg;METUtilitiesPkg;METInterfacePkg;PathResolverPkg;TrigDecisionToolPkg;TriggerMatchingToolPkg;TrigConfxAODPkg"
)

# Create imported target WorkDir::DiLeptonPkgPrivate
add_library(WorkDir::DiLeptonPkgPrivate INTERFACE IMPORTED)

# Create imported target WorkDir::DiLeptonLib
add_library(WorkDir::DiLeptonLib SHARED IMPORTED)

set_target_properties(WorkDir::DiLeptonLib PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "\$<TARGET_PROPERTY:WorkDir::DiLeptonPkg,INTERFACE_INCLUDE_DIRECTORIES>;/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.48/InstallArea/x86_64-slc6-gcc62-opt/include"
  INTERFACE_LINK_LIBRARIES "/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.48/InstallArea/x86_64-slc6-gcc62-opt/lib/libCore.so;/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.48/InstallArea/x86_64-slc6-gcc62-opt/lib/libRooFitCore.so;/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.48/InstallArea/x86_64-slc6-gcc62-opt/lib/libRooFit.so;/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.48/InstallArea/x86_64-slc6-gcc62-opt/lib/libGraf.so;/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.48/InstallArea/x86_64-slc6-gcc62-opt/lib/libMinuit.so;EventLoop;xAODBase;xAODRootAccess;xAODEventInfo;GoodRunsListsLib;PileupReweightingLib;PATInterfaces;PathResolver;xAODTau;xAODJet;xAODMuon;xAODEgamma;xAODCaloEvent;xAODTracking;xAODTruth;MuonMomentumCorrectionsLib;FourMomUtils;MuonEfficiencyCorrectionsLib;MuonSelectorToolsLib;JetCalibToolsLib;JetSelectorToolsLib;AthContainers;ElectronPhotonFourMomentumCorrectionLib;ElectronEfficiencyCorrectionLib;ElectronPhotonSelectorToolsLib;IsolationSelectionLib;IsolationCorrectionsLib;ElectronPhotonShowerShapeFudgeToolLib;PhotonEfficiencyCorrectionLib;METUtilitiesLib;METInterface;SUSYToolsLib;TauAnalysisToolsLib;AsgTools;xAODMissingET;JetResolutionLib;AssociationUtilsLib;JetEDM;JetUncertaintiesLib;JetReclusteringLib;JetCPInterfaces;xAODBTaggingEfficiencyLib;TrigConfxAODLib;TrigDecisionToolLib;xAODCutFlow;JetMomentToolsLib;TriggerMatchingToolLib;xAODMetaDataCnv;xAODTrigger;xAODMetaData;JetJvtEfficiencyLib;PMGToolsLib;JetTileCorrectionLib"
  INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.48/InstallArea/x86_64-slc6-gcc62-opt/include"
)

if(CMAKE_VERSION VERSION_LESS 3.0.0)
  message(FATAL_ERROR "This file relies on consumers using CMake 3.0.0 or greater.")
endif()

# Load information for each installed configuration.
get_filename_component(_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(GLOB CONFIG_FILES "${_DIR}/WorkDirConfig-targets-*.cmake")
foreach(f ${CONFIG_FILES})
  include(${f})
endforeach()

# Cleanup temporary variables.
set(_IMPORT_PREFIX)

# Loop over all imported files and verify that they actually exist
foreach(target ${_IMPORT_CHECK_TARGETS} )
  foreach(file ${_IMPORT_CHECK_FILES_FOR_${target}} )
    if(NOT EXISTS "${file}" )
      message(FATAL_ERROR "The imported target \"${target}\" references the file
   \"${file}\"
but this file does not exist.  Possible reasons include:
* The file was deleted, renamed, or moved to another location.
* An install or uninstall procedure did not complete successfully.
* The installation package was faulty and contained
   \"${CMAKE_CURRENT_LIST_FILE}\"
but not all the files it references.
")
    endif()
  endforeach()
  unset(_IMPORT_CHECK_FILES_FOR_${target})
endforeach()
unset(_IMPORT_CHECK_TARGETS)

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)