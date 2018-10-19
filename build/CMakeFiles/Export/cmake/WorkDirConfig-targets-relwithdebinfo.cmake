#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "WorkDir::AddVariables" for configuration "RelWithDebInfo"
set_property(TARGET WorkDir::AddVariables APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(WorkDir::AddVariables PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/AddVariables"
  )

list(APPEND _IMPORT_CHECK_TARGETS WorkDir::AddVariables )
list(APPEND _IMPORT_CHECK_FILES_FOR_WorkDir::AddVariables "${_IMPORT_PREFIX}/bin/AddVariables" )

# Import target "WorkDir::AddXSecBranch" for configuration "RelWithDebInfo"
set_property(TARGET WorkDir::AddXSecBranch APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(WorkDir::AddXSecBranch PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/AddXSecBranch"
  )

list(APPEND _IMPORT_CHECK_TARGETS WorkDir::AddXSecBranch )
list(APPEND _IMPORT_CHECK_FILES_FOR_WorkDir::AddXSecBranch "${_IMPORT_PREFIX}/bin/AddXSecBranch" )

# Import target "WorkDir::DiLeptonLib" for configuration "RelWithDebInfo"
set_property(TARGET WorkDir::DiLeptonLib APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(WorkDir::DiLeptonLib PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libDiLeptonLib.so"
  IMPORTED_SONAME_RELWITHDEBINFO "libDiLeptonLib.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS WorkDir::DiLeptonLib )
list(APPEND _IMPORT_CHECK_FILES_FOR_WorkDir::DiLeptonLib "${_IMPORT_PREFIX}/lib/libDiLeptonLib.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
