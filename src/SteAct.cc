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
#include "HitData.hh"



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
	G4double eDepTotal = step -> GetTotalEnergyDeposit();
	G4double eDepNoIon = step -> GetNonIonizingEnergyDeposit();
	G4double eDep      = eDepTotal - eDepNoIon;

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
	if ( namePrePV != "Strip_phys"                ) return;
//	if ( parName   != "He3" && parName != "alpha" ) return;

	G4StepPoint* preStepPoint = step -> GetPreStepPoint();
	G4TouchableHandle touchable = preStepPoint -> GetTouchableHandle();

	// Strip number (0 to 34)
	G4int stripID = touchable -> GetCopyNumber();

	// Left or right? 
	G4VPhysicalVolume* motherVolume = touchable -> GetVolume(1);
	const G4String& motherName = motherVolume -> GetName(); // "SiDetector_Left" or "SiDetector_Right"
	G4double theta = std::atan2(std::hypot(prePos.x(), prePos.y()), prePos.z());

	// Revert ID for the right
	G4int side = (motherName == "SiDetector_Left") ? 0 : 1;
	if ( side == 1 ) stripID = 34 - stripID;

	// Strip channel grouping
	if      ( stripID ==  0 | stripID ==  1 | stripID ==  2 ) stripID =  0;
	else if ( stripID ==  3 | stripID ==  4 | stripID ==  5 ) stripID =  1;
	else if ( stripID ==  6 | stripID ==  7 | stripID ==  8 ) stripID =  2;
	else if ( stripID ==  9 | stripID == 10 | stripID == 11 ) stripID =  3;
	else if ( stripID == 12 | stripID == 13 | stripID == 14 ) stripID =  4;
	else if ( stripID == 15 | stripID == 16 | stripID == 17 ) stripID =  5;
	else if ( stripID == 18 | stripID == 19 | stripID == 20 ) stripID =  6;
	else if ( stripID == 21 | stripID == 22 | stripID == 23 ) stripID =  7;
	else if ( stripID == 24 | stripID == 25 | stripID == 26 ) stripID =  8;
	else if ( stripID == 27 | stripID == 28 | stripID == 29 ) stripID =  9;
	else if ( stripID == 30 | stripID == 31 | stripID == 32 ) stripID = 10;
	else if ( stripID == 33 | stripID == 34                 ) stripID = 11;

	// Combine ID
    G4int uniqueStripID = side * 100 + stripID;  // ex: Left strip 5 -> 5, Right strip 5 -> 105

	// Record MC truth
	if      ( parName == "He3"   ) m_EA -> SetHe3(uniqueStripID, preTime / ns, theta / degree, preKinEgy / MeV);
	else if ( parName == "alpha" ) m_EA -> SetHe4(uniqueStripID, preTime / ns, theta / degree, preKinEgy / MeV);

	// Record sensor signal
	gHit . stripID . push_back(uniqueStripID);
	gHit . t       . push_back(preTime / ns );
	gHit . dE      . push_back(eDep    / MeV);


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
