#ifndef He3He4ElasticXS_h
#define He3He4ElasticXS_h 1

#include "G4DynamicParticle.hh"
#include "G4Exception.hh"
#include "G4SystemOfUnits.hh"  // millibarn, MeV, ...
#include "G4VCrossSectionDataSet.hh"
#include "globals.hh"

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

// He3 + He (Z = 2) elastic cross section from a user table.
//
// Table file (text), '#' starts a comment:
//     Ekin[MeV]   sigma[mb]
// Ekin is the He3 lab kinetic energy. Linear interpolation between points.
//
// The data set is applicable ONLY for He3 on Z = 2 inside the tabulated energy
// range. Outside that range G4CrossSectionDataStore falls through to the data
// sets registered earlier (e.g. Glauber-Gribov), so register this one LAST:
//     hel->AddDataSet(new He3He4ElasticXS("He3He4_xs.dat"));
//
// Note: the element-wise path is used when the target element has the natural
// abundance flag (e.g. NIST "G4_He"). A hand-made G4Element built from a custom
// G4Isotope goes through the isotope-wise path (IsIsoApplicable), which is not
// overridden here.
class He3He4ElasticXS : public G4VCrossSectionDataSet
{
 public:
  explicit He3He4ElasticXS(const G4String& tableFile,
                           const G4String& name = "He3He4ElasticXS")
    : G4VCrossSectionDataSet(name)
  {
    Load(tableFile);
  }

  ~He3He4ElasticXS() override = default;

  G4bool IsElementApplicable(const G4DynamicParticle* dp, G4int Z,
                             const G4Material* = nullptr) override
  {
    if (Z != 2 || dp->GetDefinition()->GetPDGEncoding() != 1000020030) return false;
    const G4double e = dp->GetKineticEnergy();
    return e >= fEnergy.front() && e <= fEnergy.back();
  }

  G4double GetElementCrossSection(const G4DynamicParticle* dp, G4int /*Z*/,
                                  const G4Material* = nullptr) override
  {
    return Interpolate(dp->GetKineticEnergy());
  }

  void CrossSectionDescription(std::ostream& out) const override
  {
    out << "He3He4ElasticXS: user-tabulated He3 + He elastic cross section ("
        << fEnergy.size() << " points, " << fEnergy.front() / MeV << " - "
        << fEnergy.back() / MeV << " MeV)\n";
  }

 private:
  G4double Interpolate(G4double e) const
  {
    auto it = std::upper_bound(fEnergy.begin(), fEnergy.end(), e);
    if (it == fEnergy.begin()) return fSigma.front();
    if (it == fEnergy.end()) return fSigma.back();
    const std::size_t i = static_cast<std::size_t>(it - fEnergy.begin());
    const G4double f = (e - fEnergy[i - 1]) / (fEnergy[i] - fEnergy[i - 1]);
    return fSigma[i - 1] * (1. - f) + fSigma[i] * f;
  }

  void Load(const G4String& file)
  {
    std::ifstream in(file);
    if (!in) {
      G4ExceptionDescription ed;
      ed << "Cannot open cross section table: " << file;
      G4Exception("He3He4ElasticXS::Load", "He3He4XS_001", FatalException, ed);
      return;
    }

    std::vector<std::pair<G4double, G4double>> raw;
    std::string line;
    while (std::getline(in, line)) {
      if (line.empty() || line[0] == '#') continue;
      std::istringstream ss(line);
      G4double e = 0., s = 0.;
      if (!(ss >> e >> s)) continue;
      raw.emplace_back(e * MeV, s * millibarn);
    }
    std::sort(raw.begin(), raw.end());
    raw.erase(std::unique(raw.begin(), raw.end(),
                          [](const std::pair<G4double, G4double>& a,
                             const std::pair<G4double, G4double>& b) {
                            return a.first == b.first;
                          }),
              raw.end());

    if (raw.size() < 2) {
      G4ExceptionDescription ed;
      ed << "Need at least 2 distinct energy points in: " << file;
      G4Exception("He3He4ElasticXS::Load", "He3He4XS_002", FatalException, ed);
      return;
    }
    for (const auto& p : raw) {
      fEnergy.push_back(p.first);
      fSigma.push_back(p.second);
    }
  }

  std::vector<G4double> fEnergy;  // sorted, Geant4 units
  std::vector<G4double> fSigma;   // Geant4 units
};

#endif
