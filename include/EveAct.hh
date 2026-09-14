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
	void AddEDepL(G4double, G4int);
	void SetTL(G4double, G4int);
	void AddEDepR(G4double, G4int);
	void SetTR(G4double, G4int);


	private:
	///---------------------------------------------------------
	/// 16-deg-pol-related private members
	///---------------------------------------------------------
	G4double eDepP16Sum;
	G4double eDepP16LSum;
	G4double eDepP16LS0;
	G4double eDepP16LS1;
	G4double eDepP16LS2;
	G4double eDepP16LS3;
	G4double eDepP16RSum;
	G4double eDepP16RS0;
	G4double eDepP16RS1;
	G4double eDepP16RS2;
	G4double eDepP16RS3;


	///---------------------------------------------------------
	/// 12-deg-pol-related private members
	///---------------------------------------------------------
	G4double eDepP12Sum;
	G4double eDepP12LSum;
	G4double eDepP12LS0;
	G4double eDepP12LS1;
	G4double eDepP12LS2;
	G4double eDepP12RSum;
	G4double eDepP12RS0;
	G4double eDepP12RS1;
	G4double eDepP12RS2;
};



#endif
