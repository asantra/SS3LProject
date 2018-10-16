# $Id: CMakeLists.txt 780993 2016-10-28 13:22:53Z krasznaa $

# The name of the package:
atlas_subdir( AddXSecBranch )


# The dependencies of the package:

atlas_depends_on_subdirs(
   PUBLIC
   SUSYTools
   )

# External dependencies

find_package( ROOT COMPONENTS )

find_package( Boost COMPONENTS program_options )

atlas_add_executable( AddVariables
      util/AddVariables.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_executable( AddXSecBranch
      util/AddXsecBranch.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} SUSYToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
