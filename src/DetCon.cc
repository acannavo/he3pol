////////////////////////////////////////////////////////////////////////////////
///
///   DetCon.cc
///
///   Definitions of DetCon class's member functions. And it describes geometry
/// of simulation.
///
/// - Author: Hoyong   Jeong   (hoyong5419@korea.ac.kr)
///           Antonino Cannavo (  acannavo@bnl.gov    )
///
////////////////////////////////////////////////////////////////////////////////



///-----------------------------------------------------------------------------
/// Headers
///-----------------------------------------------------------------------------
#include "G4VPhysicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4GDMLParser.hh"

#include "DetCon.hh"
#include "Global.hh"
#include "FirstElasticBiasOperator.hh"



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
	///---------------------------------------------------------
	/// World dimensions: Laboratory size
	///---------------------------------------------------------
	labX = 500.0*mm; // World x dimension
	labY = 500.0*mm; // World y dimension
	labZ = 500.0*mm; // World z dimension


	///---------------------------------------------------------
	/// Biased target dimensions
	///---------------------------------------------------------
	tarX =  10.0*mm; // Target x dimension
	tarY =  10.0*mm; // Target y dimension
	tarZ =  20.0*mm; // Target z dimension
}



///-----------------------------------------------------------------------------
/// Construc geometry
///-----------------------------------------------------------------------------
G4VPhysicalVolume* DetCon::Construct()
{
	///---------------------------------------------------------
	/// Geometry from GDML
	///---------------------------------------------------------
	G4GDMLParser gdmlParser;
	gdmlParser . SetOverlapCheck(true);
	gdmlParser . Read("geometry/GDML_He3Pol_Simplified_merged-worldVOL.gdml");


	///---------------------------------------------------------
	/// World
	///---------------------------------------------------------
	G4Box* solidLab = new G4Box("solidLab", labX / 2., labY / 2., labZ / 2.);
	G4LogicalVolume* lvLab = new G4LogicalVolume(solidLab, matTar, "lvLab");
	lvLab -> SetVisAttributes(new G4VisAttributes(0));
	G4VPhysicalVolume* pvLab = new G4PVPlacement(0, G4ThreeVector(), "pvLab", lvLab, 0, false, 0);


	///---------------------------------------------------------
	/// Chamber
	///---------------------------------------------------------
	G4LogicalVolume* lvChamGDML = gdmlParser . GetVolume("LV_Vacuum_chamber");
	new G4PVPlacement(0, G4ThreeVector(), "pvCham", lvChamGDML, pvLab, false, 0);


	///---------------------------------------------------------
	/// Target -> need this?
	///---------------------------------------------------------
//	G4LogicalVolume* lvTarGDML = gdmlParser . GetVolume("LV_HeGas_volume");
//	lvTarGDML -> SetVisAttributes(new G4VisAttributes(0));
//	new G4PVPlacement(0, G4ThreeVector(), "pvTar", lvTarGDML, pvLab, false, 0);


	///---------------------------------------------------------
	/// Target, biased
	///---------------------------------------------------------
	G4Box* solidTarBiased = new G4Box("solidTarBiased", tarX / 2., tarY / 2., tarZ / 2.);
	G4LogicalVolume* lvTarBiased = new G4LogicalVolume(solidTarBiased, matTar, "lvTarBiased");
	lvTarBiased -> SetVisAttributes(new G4VisAttributes(G4Colour::Green()));
	auto* biasOperator = new FirstElasticBiasOperator("targetBias", 1.e7, "hadElastic");
	biasOperator -> AttachTo(lvTarBiased);
//	new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), "pvTarBiased", lvTarBiased, lvTarGDML, false, 0);
	new G4PVPlacement(0, G4ThreeVector(0., 0., 4.4*mm), "pvTarBiased", lvTarBiased, pvLab, false, 0);


	///---------------------------------------------------------
	/// Flange
	///---------------------------------------------------------
	G4LogicalVolume* lvFlangeGDML = gdmlParser . GetVolume("LV_Flange_window");
	lvFlangeGDML -> SetMaterial(matWin);
	new G4PVPlacement(0, G4ThreeVector(), "pvFlange", lvFlangeGDML, pvLab, false, 0);


	///---------------------------------------------------------
	/// Window
	///---------------------------------------------------------
//	G4Tubs* solidWin = new G4Tubs("solidWin", 0., 12.*mm, 1.*um, 0., 360.*degree);
//	G4LogicalVolume* lvWin = new G4LogicalVolume(solidWin, matWin, "lvWin");
//	lvWin -> SetVisAttributes(new G4VisAttributes(G4Colour::Magenta()));
//	new G4PVPlacement(0, G4ThreeVector(0., 0., - 144.*mm), "pvWin", lvWin, pvLab, false, 0);


	///---------------------------------------------------------
	/// Silicon detector
	///---------------------------------------------------------
	const G4int    kNStrips        =    35;
	const G4double kStripPitch     =   1.0 * mm;  // center-to-center spacing
	const G4double kStripWidth     =   0.9 * mm;  // strip fills full pitch, no gap assumed
	const G4double kStripLength    =   4.0 * mm;  // along local y (parallel to global y)
	const G4double kStripThickness =  0.03 * mm;  // along local z (normal / depth direction)
	const G4double kDetectorDist   = 127.0 * mm;  // origin -> detector center
	const G4double kRotationAngle  =  49.0 * deg; // rotation about the Y-axis

	const G4double kArrayWidth = kNStrips * kStripPitch; // total width along pitch direction

	//--------------------------------------
	// Single strip solid & logical volume
	//--------------------------------------
	G4Box* solidStrip = new G4Box("Strip_solid",kStripWidth / 2., kStripLength / 2., kStripThickness / 2.);
	G4LogicalVolume* logicStrip = new G4LogicalVolume(solidStrip, matDet, "Strip_logical");

	//--------------------------------------
	// Envelope holding the 35 strips (one per detector arm)
	//--------------------------------------
	G4Box* solidEnvelope = new G4Box("SiDetectorEnvelope_solid", kArrayWidth / 2., kStripLength / 2., kStripThickness / 2.);
	G4LogicalVolume* logicEnvelope = new G4LogicalVolume(solidEnvelope, matDet, "SiDetectorEnvelope_logical");
	logicEnvelope -> SetVisAttributes(new G4VisAttributes(0));

	//--------------------------------------
	// Place the 35 strips side by side along local x, inside the envelope
	//--------------------------------------
	for ( G4int i = 0; i < kNStrips; ++i )
	{
		G4double xPos = (i - (kNStrips - 1) / 2.) * kStripPitch;
		new G4PVPlacement(0, G4ThreeVector(xPos, 0., 0.), logicStrip, "Strip_phys", logicEnvelope, false, i, true);  // checkOverlaps
	}

	//--------------------------------------
	// Place the two envelopes (left / right) into the world ----
	//
	// Step 1: start at (0,0,kDetectorDist) with identity rotation.
	//         Local z (envelope thickness axis) already points along -z, i.e. at the origin.
	// Step 2: rotate the *whole placement* (position + orientation) about Y by +/-50 deg.\
	//         Since position and orientation rotate together about the same axis,
	//         the normal keeps pointing at the origin, and local y stays parallel
	//         to global y throughout.
	//--------------------------------------
	G4Transform3D transformLeft  = G4RotateY3D( kRotationAngle) * G4Translate3D(G4ThreeVector(0, 0, kDetectorDist));
	G4Transform3D transformRight = G4RotateY3D(-kRotationAngle) * G4Translate3D(G4ThreeVector(0, 0, kDetectorDist));

	new G4PVPlacement(transformLeft , logicEnvelope, "SiDetector_Left" , lvLab, false, 0, true);
	new G4PVPlacement(transformRight, logicEnvelope, "SiDetector_Right", lvLab, false, 1, true);


	return pvLab;
}



void DetCon::ConstructMaterials()
{
	const G4double labTemp = 300.0 * kelvin;

	elHe4 = new G4Element("Helium4" , "He4",  2,   4.     *g/mole);
	elN   = new G4Element("Nitrogen",   "N",  7,  14.007  *g/mole);
	elAl  = new G4Element("Aluminum",  "Al", 13,  26.98154*g/mole);
	elSi  = new G4Element("Silicon" ,  "Si", 14,  28.     *g/mole);

	matTar = new G4Material("Tar", 1.08e-6*std::stod(gCM -> Get("PRESSURE"))/5.*g/cm3, 1, kStateGas, labTemp);
	matTar -> AddElement(elHe4, 1);

	matWin = new G4Material("Window", 3.17*g/cm3, 2, kStateSolid, labTemp);
	matWin -> AddElement(elSi, 3);
	matWin -> AddElement(elN , 4);

	matDet = new G4Material("Detector", 2.3*g/cm3, 1, kStateSolid, labTemp);
	matDet -> AddElement(elSi, 1);
}

void DetCon::DestructMaterials()
{
	delete matTar;
	delete elHe4;
}
