#include "G4HadronElasticProcessWithSpin.hh"

#include "G4ParticleChange.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include <atomic>
#include <cmath>



G4HadronElasticProcessWithSpin::G4HadronElasticProcessWithSpin(const G4String& name) : G4HadronElasticProcess(name)
{
}



G4VParticleChange* G4HadronElasticProcessWithSpin::PostStepDoIt(const G4Track& track,const G4Step& step)
{
	// 1) standard elastic final state (theta and recoil kinematics are untouched)
	G4VParticleChange* vpc = G4HadronElasticProcess::PostStepDoIt(track, step);

	static std::atomic<int> nPrint{0};
	const bool doPrint = (nPrint . load() < 20);

	auto* pc = dynamic_cast<G4ParticleChange*>(vpc);
	if ( pc == nullptr )
	{
		if ( doPrint )
		{
			G4cout << "[WithSpin] dynamic_cast<G4ParticleChange*> FAILED -> no modification" << G4endl;
			++nPrint;
		}
		return vpc;
	}

	if ( ! fForcePhiZero ) return vpc;


	// 2) azimuth reference frame around the incident direction
	const G4ThreeVector kin = track.GetMomentumDirection().unit();
	G4ThreeVector ref(1., 0., 0.);
	ref -= ref.dot(kin) * kin;
	if ( ref . mag2() < 1e-12 )
	{
		ref = G4ThreeVector(0., 1., 0.);
		ref -= ref.dot(kin) * kin;
	}
	const G4ThreeVector ex = ref . unit();
	const G4ThreeVector ey = kin . cross(ex);


	// 3) current azimuth of the scattered projectile
	G4ThreeVector kout = *pc -> GetMomentumDirection();
	const G4double sinTheta = kin . cross(kout) . mag();
	if ( sinTheta < 1e-6 ) return vpc;  // azimuth undefined (forward / unscattered)

	const G4double phiOld = std::atan2(kout . dot(ey), kout . dot(ex));
	const G4double dphi   = 0.0 - phiOld;  // force phi = 0


	// 4) rotate projectile and all secondaries (recoil) by the same angle
	pc -> ProposeMomentumDirection(kout . rotate(dphi, kin));
	for ( G4int i = 0; i < pc -> GetNumberOfSecondaries(); ++i )
	{
		G4Track* sec = pc -> GetSecondary(i);
		G4ThreeVector d = sec -> GetMomentumDirection();
		sec -> SetMomentumDirection(d . rotate(dphi, kin));
	}

	if ( doPrint )
	{
		const G4ThreeVector kafter = *pc->GetMomentumDirection();
		G4cout << "[WithSpin] " << track.GetDefinition()->GetParticleName()
		       << " theta=" << std::asin(sinTheta) / deg << " deg"
		       << " phi_old=" << phiOld / deg << " deg"
		       << " phi_new=" << std::atan2(kafter.dot(ey), kafter.dot(ex)) / deg << " deg"
		       << " nSec=" << pc->GetNumberOfSecondaries() << G4endl;
		++nPrint;
	}


	return vpc;
}
