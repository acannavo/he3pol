////////////////////////////////////////////////////////////////////////////////
// Test version: after the standard elastic final state is generated, the whole
// final state (scattered projectile + recoil secondaries) is rotated about the
// incident direction so that the projectile azimuth becomes exactly phi = 0.
//
// Azimuth reference frame (right-handed): ez = incident direction,
// ex = lab +x projected onto the plane normal to ez (lab +y if degenerate),
// ey = ez x ex.  For a beam along +z: phi = 0  <=>  scattered projectile goes
// to +x, recoil goes to -x.
////////////////////////////////////////////////////////////////////////////////
#ifndef G4HadronElasticProcessWithSpin_h
#define G4HadronElasticProcessWithSpin_h 1

#include "G4HadronElasticProcess.hh"



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
