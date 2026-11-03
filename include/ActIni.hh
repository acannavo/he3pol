////////////////////////////////////////////////////////////////////////////////
//   ActIni.hh
//
//   This file is a header for ActIni class. Every actions are initialized
// through this class.
//
// - Author: Hoyong   Jeong   (hoyong5419@korea.ac.kr)
//           Antonino Cannavo (  acannavo@bnl.gov    )
////////////////////////////////////////////////////////////////////////////////



#ifndef ACTINI_h
#define ACTINI_h 1



//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
#include "G4VUserActionInitialization.hh"



//------------------------------------------------------------------------------
// Class declaration
//------------------------------------------------------------------------------
class ActIni : public G4VUserActionInitialization
{
public:
	ActIni();
	virtual ~ActIni();


	virtual void BuildForMaster() const;
	virtual void Build() const;
};



#endif
