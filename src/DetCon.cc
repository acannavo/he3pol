///-----------------------------------------------------------------------------
/// Headers
///-----------------------------------------------------------------------------
#include "G4VPhysicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

#include "DetCon.hh"







#include "G4VBiasingOperator.hh"
#include "G4BOptnChangeCrossSection.hh"
#include "G4BiasingProcessInterface.hh"
#include <float.h>  // for DBL_MAX

class G4VolumeBiasOperator : public G4VBiasingOperator
{
public:
    G4VolumeBiasOperator(const G4String& name, G4double factor)
        : G4VBiasingOperator(name), fXSFactor(factor)
    {}

    virtual G4VBiasingOperation*


ProposeOccurenceBiasingOperation(const G4Track* track,
                                  const G4BiasingProcessInterface* callingProcess) override
{
    // 프로세스 이름 확인 — hadElastic만 bias
    const G4String& processName = callingProcess->GetWrappedProcess()->GetProcessName();
    if (processName != "hadElastic") {
        return nullptr;  // 다른 프로세스는 원래 물리 그대로
    }

    G4BOptnChangeCrossSection* operation = fChangeXSOperations[callingProcess];
    if (operation == nullptr) {
        operation = new G4BOptnChangeCrossSection("xsecBias-" + processName);
        fChangeXSOperations[callingProcess] = operation;
    }

    G4double analogInteractionLength =
        callingProcess->GetWrappedProcess()->GetCurrentInteractionLength();
    if (analogInteractionLength > DBL_MAX / 10.0) return nullptr;

    G4double analogXS = 1.0 / analogInteractionLength;
    operation->SetBiasedCrossSection(fXSFactor * analogXS);
    operation->Sample();

    return operation;
}


    virtual void OperationApplied(const G4BiasingProcessInterface* callingProcess,
                                   G4BiasingAppliedCase biasingCase,
                                   G4VBiasingOperation* occurenceOperationApplied,
                                   G4double weightForOccurenceInteraction,
                                   G4VBiasingOperation* finalStateOperationApplied,
                                   const G4VParticleChange* particleChangeProduced) override
    {
        G4BOptnChangeCrossSection* operation = fChangeXSOperations[callingProcess];
        if (operation == occurenceOperationApplied) {
            operation->SetInteractionOccured();
        }
    }

    virtual G4VBiasingOperation*
    ProposeFinalStateBiasingOperation(const G4Track*,
                                       const G4BiasingProcessInterface*) override
    { return nullptr; }

    virtual G4VBiasingOperation*
    ProposeNonPhysicsBiasingOperation(const G4Track*,
                                       const G4BiasingProcessInterface*) override
    { return nullptr; }

private:
    G4double fXSFactor;
    std::map<const G4BiasingProcessInterface*, G4BOptnChangeCrossSection*> fChangeXSOperations;
};








///-----------------------------------------------------------------------------
/// Constructor
///-----------------------------------------------------------------------------
DetCon::DetCon()
{
	ConstructMaterials();
	DefineDimensions();
}



///-----------------------------------------------------------------------------
/// Destructor
///-----------------------------------------------------------------------------
DetCon::~DetCon()
{
	DestructMaterials();
}



///-----------------------------------------------------------------------------
/// Define dimensions
///-----------------------------------------------------------------------------
void DetCon::DefineDimensions()
{
	///-----------------------------------------------
	/// World dimensions: Laboratory size
	///-----------------------------------------------
	labX   = 500.0*mm; // World x dimension
	labY   = 500.0*mm; // World y dimension
	labZ   = 500.0*mm; // World z dimension


	///-----------------------------------------------
	/// Target dimensions (wire)
	///-----------------------------------------------
	tarX   =  10.0*mm; // Target x dimension
	tarY   =  10.0*mm; // Target y dimension
	tarZ   =  10.0*mm; // Target z dimension
}



///-----------------------------------------------------------------------------
/// Construc geometry
///-----------------------------------------------------------------------------
G4VPhysicalVolume* DetCon::Construct()
{
	///-----------------------------------------------
	/// World
	///-----------------------------------------------
	G4Box* solidLab = new G4Box("solidLab", labX / 2., labY / 2., labZ / 2.);
	G4LogicalVolume* lvLab = new G4LogicalVolume(solidLab, matTar, "lvLab");
	lvLab -> SetVisAttributes(new G4VisAttributes(0));
	G4VPhysicalVolume* pvLab = new G4PVPlacement(0, G4ThreeVector(), "pvLab", lvLab, 0, false, 0);

	///-----------------------------------------------
	/// Sensitive part
	///-----------------------------------------------
	G4Box* solidTar = new G4Box("solidTar", tarX / 2., tarY / 2., tarZ / 2.);
	G4LogicalVolume* lvTar = new G4LogicalVolume(solidTar, matTar, "lvTar");
	lvTar -> SetVisAttributes(new G4VisAttributes(G4Colour::Magenta()));
	auto* biasOperator = new G4VolumeBiasOperator("targetBias", 10000000.0);
	biasOperator -> AttachTo(lvTar);
	new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), "pvTar", lvTar, pvLab, false, 0);

	G4Tubs* solidWin = new G4Tubs("solidWin", 0., 50.*mm, 1.*um, 0., 360.*degree);
	G4LogicalVolume* lvWin = new G4LogicalVolume(solidWin, matWin, "lvWin");
	lvWin -> SetVisAttributes(new G4VisAttributes(G4Colour::White()));
	new G4PVPlacement(0, G4ThreeVector(0., 0., - 100.*mm), "pvWin", lvWin, pvLab, false, 0);




	///-----------------------------------------------
	/// Silicon detector
	///-----------------------------------------------
    const G4int    kNStrips        = 35;
    const G4double kStripPitch     = 1.0  * mm;   // center-to-center spacing
    const G4double kStripWidth     = 0.9  * mm;   // strip fills full pitch, no gap assumed
    const G4double kStripLength    = 4.0  * mm;   // along local y (parallel to global y)
    const G4double kStripThickness = 0.03 * mm;   // along local z (normal / depth direction)
    const G4double kDetectorDist   = 120.0 * mm;  // origin -> detector center
    const G4double kRotationAngle  = 50.0  * deg; // rotation about the Y-axis

    const G4double kArrayWidth = kNStrips * kStripPitch; // total width along pitch direction

    // ---- Single strip solid & logical volume ----
    G4Box* solidStrip = new G4Box("Strip_solid",
                                   kStripWidth / 2.0,
                                   kStripLength / 2.0,
                                   kStripThickness / 2.0);

    G4LogicalVolume* logicStrip =
        new G4LogicalVolume(solidStrip, matDet, "Strip_logical");

    // ---- Envelope holding the 35 strips (one per detector arm) ----
    G4Box* solidEnvelope = new G4Box("SiDetectorEnvelope_solid",
                                      kArrayWidth / 2.0,
                                      kStripLength / 2.0,
                                      kStripThickness / 2.0);

    G4LogicalVolume* logicEnvelope =
        new G4LogicalVolume(solidEnvelope, matDet, "SiDetectorEnvelope_logical");
	logicEnvelope -> SetVisAttributes(new G4VisAttributes(0));

    // Place the 35 strips side by side along local x, inside the envelope
    for (G4int i = 0; i < kNStrips; ++i) {
        G4double xPos = (i - (kNStrips - 1) / 2.0) * kStripPitch;
        new G4PVPlacement(nullptr,
                           G4ThreeVector(xPos, 0.0, 0.0),
                           logicStrip,
                           "Strip_phys",
                           logicEnvelope,
                           false,
                           i,
                           true);  // checkOverlaps
    }

    // ---- Place the two envelopes (left / right) into the world ----
    // Step 1: start at (0,0,kDetectorDist) with identity rotation.
    //         Local z (envelope thickness axis) already points along -z, i.e. at the origin.
    // Step 2: rotate the *whole placement* (position + orientation) about Y by +/-50 deg.
    //         Since position and orientation rotate together about the same axis,
    //         the normal keeps pointing at the origin, and local y stays parallel
    //         to global y throughout.

    G4Transform3D transformLeft =
        G4RotateY3D(kRotationAngle) * G4Translate3D(G4ThreeVector(0, 0, kDetectorDist));

    G4Transform3D transformRight =
        G4RotateY3D(-kRotationAngle) * G4Translate3D(G4ThreeVector(0, 0, kDetectorDist));

    new G4PVPlacement(transformLeft,
                       logicEnvelope,
                       "SiDetector_Left",
                       lvLab,
                       false,
                       0,
                       true);

    new G4PVPlacement(transformRight,
                       logicEnvelope,
                       "SiDetector_Right",
                       lvLab,
                       false,
                       1,
                       true);










	return pvLab;
}



void DetCon::ConstructMaterials()
{
	const G4double labTemp = 300.0 * kelvin;

	elHe4 = new G4Element("Helium4" , "He4",  2, 4.*g/mole);
	elAl  = new G4Element("Aluminum",  "Al", 13,  26.98154*g/mole);
	elSi  = new G4Element("Silicon" ,  "Si", 14, 28.*g/mole);

	matTar = new G4Material("Tar", 1.08e-6*g/cm3, 1, kStateGas, labTemp);
	matTar -> AddElement(elHe4, 1);

	matWin = new G4Material("Window", 2.7*g/cm3, 1, kStateSolid, labTemp);
	matWin -> AddElement(elAl, 1);

	matDet = new G4Material("Detector", 2.3*g/cm3, 1, kStateSolid, labTemp);
	matDet -> AddElement(elSi, 1);
}



void DetCon::DestructMaterials()
{
	delete matTar;

	delete elHe4;
}
