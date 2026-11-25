////////////////////////////////////////////////////////////////////////////////
///
///   diffxs.cc
///
/// He3 + He4 elastic dsigma/dOmega of the default Geant4 model (hElasticLHEP = G4HadronElastic)
///
/// Reference:
///    ${G4SRC}/source/processes/hadronic/models/coherent_elastic/src/G4HadronElastic.cc
///
/// Shape: the 2-exponential |t| distribution of G4HadronElastic::SampleInvariantT
///   (A <= 62, non-pion branch):
///     dsigma/dt ~ aa*bb*exp(-bb t) + cc*dd*exp(-dd t),   t in GeV^2, 0 <= t <= tmax = 4 p_cms^2
///     bb = 14.5 A^(2/3),  dd = 20,  aa = A^2/bb,  cc = 1.4 A^(1/3)/dd
/// Size : normalized to the Glauber-Gribov elastic cross section (G4ComponentGGNuclNuclXsc).
///        dsigma/dOmega = sigma_el * f(t) * tmax/(4 pi),   f = dsigma/dt / sigma_el  (int f dt = 1)
/// This formula was checked against ApplyYourself() sampling for He3 on He4 with
/// Geant4 11.4.3; re-check it after a Geant4 upgrade.
///
/// Build:
///   g++ -std=c++17 -O2 diffxs.cc $(geant4-config --cflags) $(geant4-config --libs) -o diffxs
/// Run (after: source $(geant4-config --prefix)/bin/geant4.sh):
///   ./diffxs [Ekin_lab_MeV=6] [theta_step_deg=1] > xs.txt
///
/// Output columns: theta_cm[deg]  hElasticLHEP[mb/sr]  rutherford[mb/sr]
///
////////////////////////////////////////////////////////////////////////////////



///-----------------------------------------------------------------------------
/// Headers
///-----------------------------------------------------------------------------
#include "G4ComponentGGNuclNuclXsc.hh"
#include "G4He3.hh"
#include "G4NucleiProperties.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>



///-----------------------------------------------------------------------------
/// main function
///-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	//----------------------------------------------------------
	// Input arguments and their defaults
	//----------------------------------------------------------
	const G4double ekin = (argc > 1 ? std::atof(argv[1]) : 6.0) * MeV;
	const double dth = (argc > 2 ? std::atof(argv[2]) : 1.0);  // theta step [deg]


	//----------------------------------------------------------
	// Kinematic variables
	//----------------------------------------------------------
	const G4int zT = 2, aT = 4;  // He4 target
	const G4ParticleDefinition* he3 = G4He3::He3();
	const G4double m1 = he3 -> GetPDGMass();
	const G4double m2 = G4NucleiProperties::GetNuclearMass(aT, zT);
	const G4double e1 = m1 + ekin;
	const G4double plab = std::sqrt(ekin * (ekin + 2. * m1));
	const G4double sqrtS = std::sqrt(m1 * m1 + m2 * m2 + 2. * m2 * e1);
	const G4double pcms = plab * m2 / sqrtS;
	const G4double tmax = 4. * pcms * pcms;  // max |t|
	const G4double ecm = sqrtS - (m1 + m2);  // CM kinetic energy


	//----------------------------------------------------------
	// Total elastic cross section (Glauber-Gribov), used for the normalisation
	//----------------------------------------------------------
	G4ComponentGGNuclNuclXsc gg;
	const G4double sigEl = gg . GetElasticElementCrossSection(he3, ekin, zT, G4double(aT));
	std::fprintf(stderr, "sigma_el (Glauber-Gribov) = %g mb\n", sigEl / millibarn);


	//----------------------------------------------------------
	// Parameters of 2-exponential |t| distribution (t in GeV^2)
	// (retrieved from G4HadronElastic.cc file)
	//----------------------------------------------------------
	const double bb    = 14.5 * std::pow((double)aT, 2. / 3.);
	const double dd    = 20.;
	const double aa    = (double)aT * aT / bb;
	const double cc    = 1.4 * std::cbrt((double)aT) / dd;
	const double tmaxG = tmax / (GeV * GeV);
	const double q1    = 1. - std::exp(-std::min(bb * tmaxG, 18.));
	const double q2    = 1. - std::exp(-std::min(dd * tmaxG, 18.));


	//----------------------------------------------------------
	// Rutherford (non-relativistic, CM): (Z1 Z2 e^2 / 4 E_cm)^2 / sin^4(theta/2)
	// Being divided by sin^4 will be done in for loop below. (It depends on scattering angle.)
	//----------------------------------------------------------
	const G4double aR = 2. * 2. * elm_coupling / (4. * ecm);


	//----------------------------------------------------------
	// Looping over theta steps
	//----------------------------------------------------------
	std::printf("# Ekin_lab = %g MeV, p_cms = %g MeV/c, sigma_el = %g mb\n", ekin / MeV, pcms / MeV, sigEl / millibarn);
	std::printf("# theta_cm[deg]  hElasticLHEP[mb/sr]  rutherford[mb/sr]\n");
	for ( double th = dth; th < 180.; th += dth )
	{
		//--------------------------------------
		// |t| = (t_max / 2)*(1 − cosθ) in GeV^2
		//--------------------------------------
		const double tc = .5 * tmaxG * (1. - std::cos(th * deg));

		//--------------------------------------
		// 2-exponential |t| distribution f(t), normalised on [0, tmax]
		//--------------------------------------
		const double f = (aa*bb*std::exp(-bb*tc) + cc*dd*std::exp(-dd*tc))
		               / (aa*q1                  + cc*q2                 );

		//--------------------------------------
		// From ds/dt to ds/dOmega, multiply by dt/dOmega = tmax/(4 pi)
		//--------------------------------------
		const double hadElas = sigEl * f * tmaxG / (4. * pi);

		//--------------------------------------
		// Rutherford case
		//--------------------------------------
		const double s = std::sin(.5 * th * deg);
		const double ruth = aR * aR / (s * s * s * s);

		//--------------------------------------
		// Print result
		//--------------------------------------
		std::printf("%g %g %g %g\n", th, hadElas / millibarn, ruth / millibarn, (hadElas + ruth) / millibarn);
	}


	return 0;
}
