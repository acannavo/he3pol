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
	eDepP16Sum  = 0.;
	eDepP16LSum = 0.;
	eDepP16LS0  = 0.;
	eDepP16LS1  = 0.;
	eDepP16LS2  = 0.;
	eDepP16LS3  = 0.;
	eDepP16RSum = 0.;
	eDepP16RS0  = 0.;
	eDepP16RS1  = 0.;
	eDepP16RS2  = 0.;
	eDepP16RS3  = 0.;

	eDepP12Sum  = 0.;
	eDepP12LSum = 0.;
	eDepP12LS0  = 0.;
	eDepP12LS1  = 0.;
	eDepP12LS2  = 0.;
	eDepP12RSum = 0.;
	eDepP12RS0  = 0.;
	eDepP12RS1  = 0.;
	eDepP12RS2  = 0.;
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
	/// Add row if something detected
	///---------------------------------------------------------
	if ( eDepP16Sum > 0. || eDepP12Sum > 0. )
	{
		AM -> FillNtupleIColumn( 0, anEvent -> GetEventID());
		AM -> FillNtupleDColumn( 1, eDepP16Sum             );
		AM -> FillNtupleDColumn( 2, eDepP16LSum            );
		AM -> FillNtupleDColumn( 3, eDepP16LS0             );
		AM -> FillNtupleDColumn( 4, eDepP16LS1             );
		AM -> FillNtupleDColumn( 5, eDepP16LS2             );
		AM -> FillNtupleDColumn( 6, eDepP16LS3             );
		AM -> FillNtupleDColumn( 7, eDepP16RSum            );
		AM -> FillNtupleDColumn( 8, eDepP16RS0             );
		AM -> FillNtupleDColumn( 9, eDepP16RS1             );
		AM -> FillNtupleDColumn(10, eDepP16RS2             );
		AM -> FillNtupleDColumn(11, eDepP16RS3             );

		AM -> FillNtupleDColumn(12, eDepP12Sum             );
		AM -> FillNtupleDColumn(13, eDepP12LSum            );
		AM -> FillNtupleDColumn(14, eDepP12LS0             );
		AM -> FillNtupleDColumn(15, eDepP12LS1             );
		AM -> FillNtupleDColumn(16, eDepP12LS2             );
		AM -> FillNtupleDColumn(17, eDepP12RSum            );
		AM -> FillNtupleDColumn(18, eDepP12RS0             );
		AM -> FillNtupleDColumn(19, eDepP12RS1             );
		AM -> FillNtupleDColumn(20, eDepP12RS2             );

		AM -> AddNtupleRow();
	}
}
