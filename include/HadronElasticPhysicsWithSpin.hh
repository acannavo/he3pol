////////////////////////////////////////////////////////////////////////////////
//   HadronElasticPhysicsWithSpin.hh
//
//   Copied from G4HadronElasticPhysics.hh and modified.
//
// - Author: Hoyong   Jeong   (hoyong5419@korea.ac.kr)
//           Antonino Cannavo (  acannavo@bnl.gov    )
////////////////////////////////////////////////////////////////////////////////



#ifndef HadronElasticPhysicsWithSpin_h
#define HadronElasticPhysicsWithSpin_h 1



//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
#include "globals.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4HadronElastic.hh"
#include "G4HadronicProcess.hh"



class G4VCrossSectionDataSet;
class G4ParticleDefinition;



//------------------------------------------------------------------------------
// Class declaration
//------------------------------------------------------------------------------
class HadronElasticPhysicsWithSpin : public G4VPhysicsConstructor
{
	public:
	explicit HadronElasticPhysicsWithSpin(G4int ver = 1, const G4String& nam = "hElasticWEL_CHIPS_XS");
	~HadronElasticPhysicsWithSpin() override = default;


	//----------------------------------------------------------
	// This method will be invoked in the Construct() method. 
	// each particle type will be instantiated
	//----------------------------------------------------------
	void ConstructParticle() override;


	//----------------------------------------------------------
	// This method will be invoked in the Construct() method.
	// each physics process will be instantiated and
	// registered to the process manager of each particle type 
	//----------------------------------------------------------
	void ConstructProcess() override;

	G4HadronicProcess* GetElasticProcess(const G4ParticleDefinition* part) const;

	G4HadronElastic* GetElasticModel(const G4ParticleDefinition* part) const;

	G4HadronicProcess* GetNeutronProcess() const;

	G4HadronElastic* GetNeutronModel() const;

	void AddXSection(const G4ParticleDefinition*, G4VCrossSectionDataSet*) const; 


	//----------------------------------------------------------
	// copy constructor and hide assignment operator
	//----------------------------------------------------------
	HadronElasticPhysicsWithSpin(HadronElasticPhysicsWithSpin &) = delete;
	HadronElasticPhysicsWithSpin & operator=(const HadronElasticPhysicsWithSpin &right) = delete;
};



#endif
