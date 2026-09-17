////////////////////////////////////////////////////////////////////////////////
//   SteAct.cc
//
//   Definitions of SteAct class's member functions. Details of user
// actions are here.
//
//                        - Feb 5th 2025, Hoyong Jeong (hoyong5419@korea.ac.kr)
////////////////////////////////////////////////////////////////////////////////



#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "SteAct.hh"
#include "EveAct.hh"



SteAct::SteAct(EveAct* EA) : G4UserSteppingAction(), m_EA(EA)
{
}



SteAct::~SteAct()
{
}



void SteAct::UserSteppingAction(const G4Step* step)
{
	//------------------------------------------------
	// Who am I? Where am I? What am I undergoing?
	//------------------------------------------------
	// Track ID
//	G4int trackID = step -> GetTrack() -> GetTrackID();

	// Particle name
	G4String parName = step -> GetTrack() -> GetDefinition() -> GetParticleName();

	// Particle ID
//	G4int parID = step -> GetTrack() -> GetDefinition() -> GetPDGEncoding();

	// Particle charge
//	G4int parCharge = step -> GetTrack() -> GetDefinition() -> GetPDGCharge();

	// Process name
//	const G4VProcess* creProc = step -> GetTrack() -> GetCreatorProcess();
//	G4String procName = step -> GetPostStepPoint() -> GetProcessDefinedStep() -> GetProcessName();

	// Physical volume
	G4String namePrePV = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetName();
	G4String namePostPV;
	G4VPhysicalVolume* postPV = step -> GetPostStepPoint() -> GetPhysicalVolume();
	if ( postPV != 0 ) namePostPV = postPV -> GetName();
	else namePostPV = "outside";

	// Status
//	G4StepStatus preStat = step -> GetPreStepPoint() -> GetStepStatus();
//	G4StepStatus postStat = step -> GetPostStepPoint() -> GetStepStatus();

	// Position
	G4ThreeVector prePos = step -> GetPreStepPoint() -> GetPosition();
//	G4ThreeVector postPos = step -> GetPostStepPoint() -> GetPosition();

	// Momentum
//	G4ThreeVector preMom = step -> GetPreStepPoint() -> GetMomentum();
//	G4ThreeVector postMom = step -> GetPostStepPoint() -> GetMomentum();

	// Kinetic energy
	G4double preKinEgy = step -> GetPreStepPoint() -> GetKineticEnergy();
	G4double postKinEgy = step -> GetPostStepPoint() -> GetKineticEnergy();

	// Energy deposition
	G4double eDep = step -> GetTotalEnergyDeposit() / MeV;

	// Global time	
	G4double preTime  = step -> GetPreStepPoint()  -> GetGlobalTime();
	G4double postTime = step -> GetPostStepPoint() -> GetGlobalTime();

//	if ( namePostPV == "outside" && parName == "He3" )
//	{
//		G4cout << "[Info] He3 final energy = " << postKinEgy / MeV << " MeV" << G4endl;
//	}

//	if ( parName == "alpha" )
//	{	
//		G4cout << "[Info] He4 detected with kinetic energy " << postKinEgy / MeV << " MeV" << G4endl;
//	}



	// We only care about strips
    if ( namePrePV != "Strip_phys" ) return;
	if ( parName != "He3" && parName != "alpha" ) return;

	    const G4Event* event = G4RunManager::GetRunManager()->GetCurrentEvent();
    G4int eventID = event->GetEventID();

	G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4TouchableHandle touchable = preStepPoint->GetTouchableHandle();

    // Strip number (0~34)
    G4int stripID = touchable -> GetCopyNumber();

    // Left or right? 
    G4VPhysicalVolume* motherVolume = touchable -> GetVolume(1);
    const G4String& motherName = motherVolume->GetName(); // "SiDetector_Left" or "SiDetector_Right"
 G4double theta = std::atan2(std::hypot(prePos.x(), prePos.y()), prePos.z());

    // 3) Combine ID
    G4int side = (motherName == "SiDetector_Left") ? 0 : 1;
	if ( side == 1 ) stripID = 34 - stripID;
    G4int uniqueStripID = side * 100 + stripID;  // ex: Left strip 5 -> 5, Right strip 5 -> 105

	if      ( parName == "He3"   ) m_EA -> SetHe3(uniqueStripID, preTime / ns, theta / degree, preKinEgy / MeV);
	else if ( parName == "alpha" ) m_EA -> SetHe4(uniqueStripID, preTime / ns, theta / degree, preKinEgy / MeV);



//        G4cout << "Hit: side=" << motherName
//		<< " eID=" << eventID 
//		<< " par=" << parName
  //             << " stripID=" << stripID
    //           << " uniqueID=" << uniqueStripID
      //         << " T=" << preKinEgy / MeV << " MeV"
        //       << " x=" << prePos / mm << " mm"
          //     << " theta=" << theta / degree << " deg"
            //   << " t=" << preTime / ns << " ns" << G4endl;
}
