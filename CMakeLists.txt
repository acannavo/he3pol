################################################################################
#
#   CMakeLists.txt for HE3POL.
#
# - Authors: Hoyong   Jeong   (hoyong5419@korea.ac.kr)
#            Antonino Cannavo (  acannavo@bnl.gov    )
#
################################################################################



#-------------------------------------------------------------------------------
#   Setup the project
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.9 FATAL_ERROR)
project(HE3POL)



#-------------------------------------------------------------------------------
#   Find Geant4 package, activating all available UI and Vis drivers by default.
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable.
#-------------------------------------------------------------------------------
option(WITH_GEANT4_UIVIS "Build he3pol with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
	find_package(Geant4 REQUIRED ui_all vis_all)
else()
	find_package(Geant4 REQUIRED)
endif()



#-------------------------------------------------------------------------------
#   Setup Geant4 include directories and compile definitions
#   Setup include directory for this project
#-------------------------------------------------------------------------------
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)



#------------------------------------------------------------------------------#
#   Find ROOT package.                                                         #
#------------------------------------------------------------------------------#
find_package(ROOT REQUIRED EG)



#------------------------------------------------------------------------------#
#   Setup ROOT include directories and compile definitions.                    #
#------------------------------------------------------------------------------#
include(${ROOT_USE_FILE})



#------------------------------------------------------------------------------#
#   Setup PLUTO using user's environment variable "PLUTOSYS"                   #
# You can edit directory path depending on your system.                        #
#------------------------------------------------------------------------------#
link_directories($ENV{PLUTOSYS}/lib)
include_directories($ENV{PLUTOSYS}/include)



#-------------------------------------------------------------------------------
#   Locate sources and headers for this project
#   NB: headers are included so they will show up in IDEs
#-------------------------------------------------------------------------------
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)



#-------------------------------------------------------------------------------
#   Add the executable, and link it to the Geant4 libraries
#-------------------------------------------------------------------------------
add_executable(he3pol main.cc ${sources} ${headers})
target_link_libraries(he3pol ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} Pluto)



#-------------------------------------------------------------------------------
#   Copy all scripts to the build directory, i.e. the directory in which we
# build he3pol. This is so that we can run the executable directly because
# it relies on these scripts being in the current working directory.
#-------------------------------------------------------------------------------
set(HE3POL_SCRIPTS
	init_vis.mac
	vis.mac
	config.conf
)

foreach(_script ${HE3POL_SCRIPTS})
	configure_file(
		${PROJECT_SOURCE_DIR}/${_script}
		${PROJECT_BINARY_DIR}/${_script}
		COPYONLY
	)
endforeach()



#-------------------------------------------------------------------------------
#   For internal Geant4 use - but has no effect if you build this he3pol
# standalone
#-------------------------------------------------------------------------------
add_custom_target(HE3POL DEPENDS he3pol)



#-------------------------------------------------------------------------------
#   Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#-------------------------------------------------------------------------------
install(TARGETS he3pol DESTINATION bin)
