//////////////////////////////////////////////////////////////////////
//   G4HadronElasticProcessWithSpin.hh
//
//   After the standard elastic final state is generated, the whole
// final state (scattered projectile + recoil secondaries) is rotated
// about the incident direction so that the projectile azimuth follows
// asymmetry by analyzing power.
//
// - Authors: Hoyong   Jeong   (hoyong5419@korea.ac.kr)
//            Antonino Cannavo (  acannavo@bnl.gov    )
//////////////////////////////////////////////////////////////////////



#ifndef G4HadronElasticProcessWithSpin_h
#define G4HadronElasticProcessWithSpin_h 1



//--------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------
#include "G4HadronElasticProcess.hh"



//--------------------------------------------------------------------
// Class declaration
//--------------------------------------------------------------------
class G4HadronElasticProcessWithSpin : public G4HadronElasticProcess
{
	public:
	explicit G4HadronElasticProcessWithSpin(const G4String& name = "hadElastic");
	~G4HadronElasticProcessWithSpin() override = default;

	G4VParticleChange* PostStepDoIt(const G4Track& track, const G4Step& step) override;

	void SetForcePhiZero(G4bool val) { fForcePhiZero = val; }


	private:
	G4bool fForcePhiZero = true;
};



#endif
