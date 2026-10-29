#ifndef He3He4ElasticModel_h
#define He3He4ElasticModel_h 1

#include "G4HadronElastic.hh"
#include "globals.hh"

#include <utility>
#include <vector>

// He3 + He4 elastic scattering with a user-supplied angular distribution.
//
// Only the angular distribution (momentum transfer t) is replaced; total cross
// section, recoil kinematics and secondary creation stay in G4HadronElastic.
// For any other projectile/target the standard G4HadronElastic sampling is used.
//
// Table file (text), one row per point, '#' starts a comment:
//     Elab[MeV]   theta_cm[deg]   dsigma/dOmega[any unit, only the shape matters]
// Rows of the same Elab form one angular distribution. Elab is the He3 lab
// kinetic energy. theta_cm is the He3 scattering angle in the He3+He4 CM frame.
//
// Sampling is restricted to [theta_min, theta_max] of each energy block.
class He3He4ElasticModel : public G4HadronElastic
{
 public:
  explicit He3He4ElasticModel(const G4String& tableFile,
                              const G4String& name = "He3He4Elastic");
  ~He3He4ElasticModel() override = default;

  G4double SampleInvariantT(const G4ParticleDefinition* part, G4double plab,
                            G4int Z, G4int A) override;

 private:
  struct Table
  {
    G4double ekin = 0.;           // He3 lab kinetic energy [Geant4 units]
    std::vector<G4double> theta;  // CM angle grid [rad]
    std::vector<G4double> cdf;    // normalised cumulative distribution
  };

  void LoadTables(const G4String& file);
  static Table BuildTable(G4double ekin,
                          const std::vector<std::pair<G4double, G4double>>& raw);
  static G4double SampleCosTheta(const Table& tab);

  std::vector<Table> fTables;  // sorted by ekin
  G4int fNWarn = 0;
};

#endif
