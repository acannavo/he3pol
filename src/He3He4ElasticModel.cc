#include "He3He4ElasticModel.hh"

#include "G4Exception.hh"
#include "G4NucleiProperties.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <map>
#include <sstream>

He3He4ElasticModel::He3He4ElasticModel(const G4String& tableFile, const G4String& name)
  : G4HadronElastic(name)
{
  LoadTables(tableFile);
}

void He3He4ElasticModel::LoadTables(const G4String& file)
{
  std::ifstream in(file);
  if (!in) {
    G4ExceptionDescription ed;
    ed << "Cannot open angular distribution table: " << file;
    G4Exception("He3He4ElasticModel::LoadTables", "He3He4_001", FatalException, ed);
    return;
  }

  std::map<G4double, std::vector<std::pair<G4double, G4double>>> raw;
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#') continue;
    std::istringstream ss(line);
    G4double e = 0., th = 0., ds = 0.;
    if (!(ss >> e >> th >> ds)) continue;
    raw[e * MeV].emplace_back(th * deg, ds);
  }

  for (auto& kv : raw) {
    auto& v = kv.second;
    std::sort(v.begin(), v.end());
    if (v.size() < 2) continue;
    fTables.push_back(BuildTable(kv.first, v));
  }

  if (fTables.empty()) {
    G4ExceptionDescription ed;
    ed << "No valid energy block (>= 2 angle points) in table: " << file;
    G4Exception("He3He4ElasticModel::LoadTables", "He3He4_002", FatalException, ed);
    return;
  }
  G4cout << "[He3He4ElasticModel] loaded " << fTables.size()
         << " energy block(s) from " << file << G4endl;
}

He3He4ElasticModel::Table He3He4ElasticModel::BuildTable(
  G4double ekin, const std::vector<std::pair<G4double, G4double>>& raw)
{
  const G4int nGrid = 2000;
  const G4double th0 = raw.front().first;
  const G4double th1 = raw.back().first;

  // dsigma/dOmega(theta): log-linear if both neighbours > 0, linear otherwise
  auto dsdo = [&raw](G4double th) -> G4double {
    auto it = std::lower_bound(
      raw.begin(), raw.end(), th,
      [](const std::pair<G4double, G4double>& p, G4double x) { return p.first < x; });
    if (it == raw.begin()) return it->second;
    if (it == raw.end()) return raw.back().second;
    auto prev = it - 1;
    const G4double dth = it->first - prev->first;
    if (dth <= 0.) return it->second;
    const G4double f = (th - prev->first) / dth;
    if (prev->second > 0. && it->second > 0.) {
      return std::exp(std::log(prev->second) * (1. - f) + std::log(it->second) * f);
    }
    return prev->second * (1. - f) + it->second * f;
  };

  Table tab;
  tab.ekin = ekin;
  tab.theta.resize(nGrid + 1);
  tab.cdf.assign(nGrid + 1, 0.);

  G4double fPrev = 0.;
  for (G4int i = 0; i <= nGrid; ++i) {
    const G4double th = th0 + (th1 - th0) * i / nGrid;
    tab.theta[i] = th;
    const G4double f = dsdo(th) * std::sin(th);  // dsigma/dtheta ~ dsigma/dOmega * sin(theta)
    if (i > 0) tab.cdf[i] = tab.cdf[i - 1] + 0.5 * (f + fPrev) * (th - tab.theta[i - 1]);
    fPrev = f;
  }

  const G4double norm = tab.cdf.back();
  if (norm <= 0.) {
    G4Exception("He3He4ElasticModel::BuildTable", "He3He4_003", FatalException,
                "Angular distribution integrates to zero or negative.");
    return tab;
  }
  for (auto& c : tab.cdf) c /= norm;
  return tab;
}

G4double He3He4ElasticModel::SampleCosTheta(const Table& tab)
{
  const G4double u = G4UniformRand();
  auto it = std::upper_bound(tab.cdf.begin(), tab.cdf.end(), u);
  std::ptrdiff_t k = it - tab.cdf.begin();
  const std::ptrdiff_t last = static_cast<std::ptrdiff_t>(tab.cdf.size()) - 1;
  if (k < 1) k = 1;
  if (k > last) k = last;

  const G4double c0 = tab.cdf[k - 1];
  const G4double c1 = tab.cdf[k];
  const G4double f = (c1 > c0) ? (u - c0) / (c1 - c0) : 0.5;
  const G4double th = tab.theta[k - 1] + f * (tab.theta[k] - tab.theta[k - 1]);
  return std::cos(th);
}

G4double He3He4ElasticModel::SampleInvariantT(const G4ParticleDefinition* part,
                                              G4double plab, G4int Z, G4int A)
{
  // Anything other than He3 on He4: standard sampling
  if (part->GetPDGEncoding() != 1000020030 || Z != 2 || A != 4) {
    return G4HadronElastic::SampleInvariantT(part, plab, Z, A);
  }

  // Same CM kinematics as in G4HadronElastic::ApplyYourself
  const G4double m1 = part->GetPDGMass();
  const G4double m2 = G4NucleiProperties::GetNuclearMass(A, Z);
  const G4double e1 = std::sqrt(plab * plab + m1 * m1);
  const G4double ekin = e1 - m1;
  const G4double pcms = plab * m2 / std::sqrt(m1 * m1 + m2 * m2 + 2. * m2 * e1);
  const G4double tmax = 4. * pcms * pcms;

  // Choose the energy block (stochastic interpolation between neighbours)
  const Table* tab = nullptr;
  if (ekin <= fTables.front().ekin) {
    tab = &fTables.front();
  }
  else if (ekin >= fTables.back().ekin) {
    tab = &fTables.back();
  }
  else {
    auto hi = std::upper_bound(fTables.begin(), fTables.end(), ekin,
                               [](G4double e, const Table& t) { return e < t.ekin; });
    auto lo = hi - 1;
    const G4double f = (ekin - lo->ekin) / (hi->ekin - lo->ekin);
    tab = (G4UniformRand() < f) ? &(*hi) : &(*lo);
  }

  // Warn (a few times) if the beam energy is well outside the tabulated range
  const G4double eLo = fTables.front().ekin;
  const G4double eHi = fTables.back().ekin;
  if (fNWarn < 5 && (ekin < 0.95 * eLo || ekin > 1.05 * eHi)) {
    G4cout << "[He3He4ElasticModel] WARNING: Ekin = " << ekin / MeV
           << " MeV is outside the table range [" << eLo / MeV << ", " << eHi / MeV
           << "] MeV; using the nearest block." << G4endl;
    ++fNWarn;
  }

  // t = tmax (1 - cos(theta_cm)) / 2   (inverse of cost = 1 - 2t/tmax in ApplyYourself)
  const G4double cost = SampleCosTheta(*tab);
  return 0.5 * tmax * (1.0 - cost);
}
