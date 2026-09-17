///-----------------------------------------------------------------------------
/// Headers
///-----------------------------------------------------------------------------
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"

#include "EveAct.hh"
#include "Ana.hh"



///-----------------------------------------------------------------------------
/// Costructor
///-----------------------------------------------------------------------------
EveAct::EveAct()
{
}



///-----------------------------------------------------------------------------
/// Destructor
///-----------------------------------------------------------------------------
EveAct::~EveAct()
{
}



///-----------------------------------------------------------------------------
/// Begin of event action
///-----------------------------------------------------------------------------
void EveAct::BeginOfEventAction(const G4Event* /*anEvent*/)
{
	///---------------------------------------------------------
	/// Initialize members
	///---------------------------------------------------------
	b3       = false;
	stripID3 = -1;
	time3    = -1.;
	theta3   = -1.;
	energy3  = -1.;

	b4       = false;
	stripID4 = -1;
	time4    = -1.;
	theta4   = -1.;
	energy4  = -1.;
}



///-----------------------------------------------------------------------------
/// End of event action
///-----------------------------------------------------------------------------
void EveAct::EndOfEventAction(const G4Event* anEvent)
{
	///---------------------------------------------------------
	/// Get analysis manager
	///---------------------------------------------------------
	G4AnalysisManager* AM = G4AnalysisManager::Instance();


	///---------------------------------------------------------
	/// Add row if both detected
	///---------------------------------------------------------
	if ( b3 & b4 )
	{
		AM -> FillNtupleIColumn( 0, anEvent -> GetEventID());
		AM -> FillNtupleIColumn( 1, stripID3               );
		AM -> FillNtupleIColumn( 2, stripID4               );
		AM -> FillNtupleDColumn( 3, time3                  );
		AM -> FillNtupleDColumn( 4, time4                  );
		AM -> FillNtupleDColumn( 5, theta3                 );
		AM -> FillNtupleDColumn( 6, theta4                 );
		AM -> FillNtupleDColumn( 7, energy3                );
		AM -> FillNtupleDColumn( 8, energy4                );

		AM -> AddNtupleRow();
	}
}



void EveAct::SetHe3(G4int stripID, G4double time, G4double theta, G4double energy)
{
	b3       = true;
	stripID3 = stripID;
	time3    = time;
	theta3   = theta;
	energy3  = energy;
}



void EveAct::SetHe4(G4int stripID, G4double time, G4double theta, G4double energy)
{
	b4       = true;
	stripID4 = stripID;
	time4    = time;
	theta4   = theta;
	energy4  = energy;
}

