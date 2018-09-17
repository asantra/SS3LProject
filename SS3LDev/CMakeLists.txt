#
# Project file for building the SS3LAnalysis code.
#

# Set the minimum required CMake version:
cmake_minimum_required( VERSION 3.2 FATAL_ERROR )

# Find the project that we depend on:
find_package( AnalysisBase REQUIRED)

# Set up CTest:
atlas_ctest_setup()

# Set up the analysis project:
atlas_project( WorkDir 1.0.0
   	       USE AnalysisBase 21.2.13 )

# Set up the runtime environment setup script(s):
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )
install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh
   DESTINATION . )

# Set up CPack:
atlas_cpack_setup()
