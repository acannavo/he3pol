////////////////////////////////////////////////////////////////////////////////
//   FirstElasticBiasOperator.hh
//
//   Cross-section bias on one process (default "hadElastic") in the attached volume:
// - at the start of every event the bias is ON (factor, e.g. 1e7),
// - once a biased interaction has actually occurred in the event, the bias is
//    switched OFF (analog cross section) until the next event begins.
//
//   The flag is event-wide (all tracks). For primary-only behaviour, uncomment the
// ParentID line in ProposeOccurenceBiasingOperation.
//
//   One operator instance per worker thread (create it in ConstructSDandField()).
//
// - Author: Hoyong   Jeong   (hoyong5419@korea.ac.kr)
//           Antonino Cannavo (  acannavo@bnl.gov    )
////////////////////////////////////////////////////////////////////////////////



#ifndef FirstElasticBiasOperator_h
#define FirstElasticBiasOperator_h 1



//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
#include "G4BOptnChangeCrossSection.hh"
#include "G4BiasingProcessInterface.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4VBiasingOperator.hh"
#include "globals.hh"

#include <cfloat>
#include <map>



//------------------------------------------------------------------------------
// Class declaration
//------------------------------------------------------------------------------
class FirstElasticBiasOperator : public G4VBiasingOperator
{
	public:
	FirstElasticBiasOperator(const G4String& name, G4double factor = 1.e7, const G4String& processName = "hadElastic")
	    : G4VBiasingOperator(name), fFactor(factor), fProcessName(processName)
	{ }

	~FirstElasticBiasOperator() override
	{
		for ( auto& kv : fOps ) delete kv . second;
	}

	G4VBiasingOperation* ProposeOccurenceBiasingOperation(const G4Track* /*track*/, const G4BiasingProcessInterface* callingProcess) override
	{
		// Other processes: analog
		if ( callingProcess -> GetWrappedProcess() -> GetProcessName() != fProcessName ) return nullptr;  

		// Primary-only variant
//		if ( track -> GetParentID() != 0 ) return nullptr;

		SyncEvent();

		// Already biased once in this event: analog
		if ( fDoneThisEvent ) return nullptr;  

		G4BOptnChangeCrossSection*& op = fOps[callingProcess];
		if ( op == nullptr ) op = new G4BOptnChangeCrossSection("xsecBias-" + fProcessName);

		const G4double analogLength = callingProcess -> GetWrappedProcess() -> GetCurrentInteractionLength();
		if ( analogLength > DBL_MAX / 10. ) return nullptr;

		op -> SetBiasedCrossSection(fFactor / analogLength);  // factor * analog xs
		op -> Sample();

		return op;
	}

	void OperationApplied(const G4BiasingProcessInterface* callingProcess,
	                      G4BiasingAppliedCase /*biasingCase*/,
	                      G4VBiasingOperation* occurenceOperationApplied,
	                      G4double /*weightForOccurenceInteraction*/,
	                      G4VBiasingOperation* /*finalStateOperationApplied*/,
	                      const G4VParticleChange* /*particleChangeProduced*/) override
	{
		auto it = fOps . find(callingProcess);
		if ( it != fOps . end() && it -> second == occurenceOperationApplied )
		{
			it -> second -> SetInteractionOccured();
			fDoneThisEvent = true;  // switch the bias off for the rest of this event
		}
	}

	//----------------------------------------------------------
	// Keep the other OperationApplied overloads visible
	//----------------------------------------------------------
	using G4VBiasingOperator::OperationApplied;

	G4VBiasingOperation* ProposeFinalStateBiasingOperation(const G4Track*, const G4BiasingProcessInterface*) override
	{
		return nullptr;
	}

	G4VBiasingOperation* ProposeNonPhysicsBiasingOperation(const G4Track*, const G4BiasingProcessInterface*) override
	{
		return nullptr;
	}


	private:
	//----------------------------------------------------------
	// Re-arm the bias when a new event (or a new run) begins
	//----------------------------------------------------------
	void SyncEvent()
	{
		const G4Run*   run = G4RunManager::GetRunManager() -> GetCurrentRun();
		const G4Event* evt = G4EventManager::GetEventManager() -> GetConstCurrentEvent();
		const G4int runID = (run != nullptr) ? run -> GetRunID()   : -1;
		const G4int evtID = (evt != nullptr) ? evt -> GetEventID() : -1;

		if ( runID != fRunID || evtID != fEventID )
		{
			fRunID         = runID;
			fEventID       = evtID;
			fDoneThisEvent = false;
		}
	}

	G4double fFactor;
	G4String fProcessName;
	std::map<const G4BiasingProcessInterface*, G4BOptnChangeCrossSection*> fOps;

	G4bool fDoneThisEvent = false;
	G4int fRunID          = -1;
	G4int fEventID        = -1;
};



#endif
