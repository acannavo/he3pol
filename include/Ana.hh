////////////////////////////////////////////////////////////////////////////////
//   Ana.hh
//
// - Author: Hoyong   Jeong   (hoyong5419@korea.ac.kr)
//           Antonino Cannavo (  acannavo@bnl.gov    )
////////////////////////////////////////////////////////////////////////////////



#ifndef ANA_h
#define ANA_h 1



//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
#include "G4Version.hh"



//------------------------------------------------------------------------------
// Decide analysis manager depending on Geant4 version
//------------------------------------------------------------------------------
#if G4VERSION_NUMBER >= 1100
	#include "G4AnalysisManager.hh"
#else
	#include "g4root.hh"
//	#include "g4xml.hh"
#endif



#endif
