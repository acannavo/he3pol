#ifndef DETCON_h
#define DETCON_h 1



///-----------------------------------------------------------------------------
/// Headers
///-----------------------------------------------------------------------------
#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"



class G4VPhysicalVolume;



///-----------------------------------------------------------------------------
/// Declaration of the class
///-----------------------------------------------------------------------------
class DetCon: public G4VUserDetectorConstruction
{
  public:
	DetCon();
	virtual  ~DetCon();
	virtual G4VPhysicalVolume* Construct();


  private:
	void DefineDimensions();
	void ConstructMaterials();
	void DestructMaterials();


	///---------------------------------------------------------
	/// Materials
	///---------------------------------------------------------
	///-------------------------------------
	/// Elements
	///-------------------------------------
	G4Element* elHe4;
	G4Element* elSi;
	G4Element* elAl;

	///-------------------------------------
	/// Materials
	///-------------------------------------
	G4Material* matTar;
	G4Material* matWin;
	G4Material* matDet;


	///---------------------------------------------------------
	/// World
	///---------------------------------------------------------
	// Dimensions
	G4double labX, labY, labZ;
	G4double tarX, tarY, tarZ;
	G4Box* solidLab;
	G4Box* solidTar;
	G4LogicalVolume* lvLab;
	G4LogicalVolume* lvTar;
};



#endif
