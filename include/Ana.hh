#ifndef ANA_h
#define ANA_h 1



#include "G4Version.hh"

#if G4VERSION_NUMBER >= 1100
	#include "G4AnalysisManager.hh"
#else
	#include "g4root.hh"
//	#include "g4xml.hh"
#endif



#endif
