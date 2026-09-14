#ifndef PRIGENACT_h
#define PRIGENACT_h 1



#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4ParticleDefinition.hh"




class G4ParticleGun;



class PriGenAct: public G4VUserPrimaryGeneratorAction
{
	public:
	PriGenAct();
	~PriGenAct();

	virtual void GeneratePrimaries(G4Event* anEvent);


	private:
	G4ParticleGun* m_PG;

	G4ThreeVector m_GunPos;
	G4ParticleDefinition* m_Par;
	G4ParticleTable* m_PT;
	G4ThreeVector m_MomDir;
	G4double mom;
	G4double kinEgy;
	G4ThreeVector pol;
};



#endif
