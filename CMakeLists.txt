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
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
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
target_link_libraries(he3pol ${Geant4_LIBRARIES})



#-------------------------------------------------------------------------------
#   Copy all scripts to the build directory, i.e. the directory in which we
# build he3pol. This is so that we can run the executable directly because
# it relies on these scripts being in the current working directory.
#-------------------------------------------------------------------------------
set(HE3POL_RESOURCES
	init_vis.mac
	vis.mac
	config.conf
	data
	geometry
	scripts
)

foreach(_res ${HE3POL_RESOURCES})
	if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${_res})
		add_custom_command(TARGET he3pol POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				${PROJECT_SOURCE_DIR}/${_res}
				${PROJECT_BINARY_DIR}/${_res}
			COMMENT "Copying directory ${_res}")
	else()
		add_custom_command(TARGET he3pol POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				${PROJECT_SOURCE_DIR}/${_res}
				${PROJECT_BINARY_DIR}/${_res}
			COMMENT "Copying file ${_res}")
	endif()
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
