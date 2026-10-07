#include "G4ParticleGun.hh"
#include "G4IonTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"
#include "G4Event.hh"

#include "PriGenAct.hh"



PriGenAct::PriGenAct()
{
	m_PG = new G4ParticleGun();

	//------------------------------------------------
	// Set particle definition
	//------------------------------------------------
	G4ParticleTable* PT = G4ParticleTable::GetParticleTable();
//	m_Par = PT -> FindParticle("He3"); // It failed.
	m_Par = PT -> FindParticle(1000020030);
	// PDGID of He3 = 1000020030
	m_PG -> SetParticleDefinition(m_Par);


	//------------------------------------------------
	// Time distribution
	//------------------------------------------------
	m_NHe3PerMicroBunch = 5e7;
	m_SizeMicroBunch    = 20. * ns;


	//------------------------------------------------
	// Momentum
	//------------------------------------------------
	m_MomDir = G4ThreeVector(0., 0., 1.);
	m_PG -> SetParticleMomentumDirection(m_MomDir);
	m_PG -> SetParticleEnergy(6. * MeV);


	//------------------------------------------------
	// Polarization: up
	//------------------------------------------------
	pol = G4ThreeVector(0.0, 1.0, 0.0);
	m_PG -> SetParticlePolarization(pol);

	m_PG -> SetParticlePosition(G4ThreeVector(0., 0., - 101. * mm));
}



PriGenAct::~PriGenAct()
{
	delete m_PG;
}



void PriGenAct::GeneratePrimaries(G4Event* anEvent)
{
	//------------------------------------------------
	// Shoot
	//------------------------------------------------
	m_PG -> GeneratePrimaryVertex(anEvent);
}
