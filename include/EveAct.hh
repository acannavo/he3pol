////////////////////////////////////////////////////////////////////////////////
//   EveAct.hh
//
//   This file is a header for EveAct class. User can add user-defined event
// action in this class. So this class works at every event.
//
// - Author: Hoyong   Jeong   (hoyong5419@korea.ac.kr)
//           Antonino Cannavo (  acannavo@bnl.gov    )
////////////////////////////////////////////////////////////////////////////////



#ifndef EVEACT_h
#define EVEACT_h 1



///-----------------------------------------------------------------------------
/// Headers
///-----------------------------------------------------------------------------
#include "G4UserEventAction.hh" 
#include <map>



class G4Event;



///-----------------------------------------------------------------------------
/// Class declaration
///-----------------------------------------------------------------------------
class EveAct: public G4UserEventAction
{
	public:
	EveAct();
	virtual ~EveAct();

	virtual void BeginOfEventAction(const G4Event*);
	virtual void EndOfEventAction(const G4Event*);


	///---------------------------------------------------------
	/// Methods accumulating E and set T
	///---------------------------------------------------------
	void SetHe3(G4int stripID, G4double time, G4double theta, G4double energy);
	void SetHe4(G4int stripID, G4double time, G4double theta, G4double energy);


	private:
	///---------------------------------------------------------
	/// Private members for He3
	///---------------------------------------------------------
	G4bool   b3;
	G4int    stripID3;
	G4double time3;
	G4double theta3;
	G4double energy3;


	///---------------------------------------------------------
	/// Private members for He4
	///---------------------------------------------------------
	G4bool   b4;
	G4int    stripID4;
	G4double time4;
	G4double theta4;
	G4double energy4;
};



#endif
