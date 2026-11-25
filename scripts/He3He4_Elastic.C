// =============================================================================
//  He3He4_Elastic.C  --  4He(3He,3He)4He elastic scattering parametrization
//
//  Two user functions (3He beam on a 4He target at rest):
//
//     double He3He4_CrossSection (double E3lab_MeV, double thetaLab_deg, int frame=0)
//         dsigma/dOmega of the scattered 3He in mb/sr.
//         frame = 0 : lab-frame cross section, thetaLab = lab angle of the 3He
//         frame = 1 : c.m. cross section at the c.m. angle that corresponds to thetaLab
//
//     double He3He4_AnalyzingPower(double E3lab_MeV, double thetaLab_deg)
//         analyzing power A_y of the spin-1/2 3He (Madison convention,
//         y || k_in x k_out); frame independent.
//
//  c.m.-angle versions: He3He4_CrossSectionCM(E,thcm), He3He4_AnalyzingPowerCM(E,thcm).
//
//  Model: partial-wave amplitudes (Coulomb + nuclear), spin 1/2 x 0
//    S, P waves : Coulomb-modified effective-range expansion (CM-ERE).
//                 P-wave 1/a fixed by the 7Be 3/2- (B=1.5866 MeV) and 1/2- (B=1.1575 MeV)
//                 bound-state poles (Poudel & Phillips, J.Phys.G 49 045102 (2022)).
//    D waves    : CM-ERE, scattering-volume term only (|delta_D| < 4 deg).
//    F waves    : 7/2- and 5/2- single-level resonances with Coulomb penetrability
//                 P_3 ~ C0^2 k^7 prod(1+eta^2/n^2).
//  Parameters refitted to SONIK (Paneru et al., PRC 109 015802 (2024); Paneru thesis 2020)
//  E = 3.6, 4.3, 5.5 MeV, Barnard et al. NP 50 (1964) 629 cross sections (3.5-6.1 MeV) and
//  Boykin et al. NP A195 (1972) 241 A_y (3.3-6.04 MeV).
//
//  VALIDITY: 3.5 <= E3lab <= 6.1 MeV (fit range). Designed for 4-6 MeV,
//            theta_lab(3He) = 37-60 deg (theta_cm ~ 64-101 deg); any angle 20-160 deg cm is
//            covered by the data used. Outside 3.5-6.1 MeV the function still evaluates
//            but is an extrapolation.
//
//  Usage (ROOT):  root -l He3He4_Elastic.C          -> prints a table and draws maps
//                 .L He3He4_Elastic.C  then call the functions / TF2 wrappers.
//  The core is plain C++11 (no ROOT needed):  g++ -std=c++11 -DHE3HE4_TEST He3He4_Elastic.C
// =============================================================================
#include <cmath>
#include <complex>
#include <cstdio>

namespace he3he4 {
typedef std::complex<double> cd;

// ---- constants -------------------------------------------------------------
const double kHbarc = 197.3269804;          // MeV fm
const double kAlpha = 1.0/137.035999;
const double kM3 = 2809.43, kM4 = 3728.40;  // MeV  (as in Poudel & Phillips)
const double kMu = kM3*kM4/(kM3+kM4);
const double kKc = 4.0*kAlpha*kMu/kHbarc;   // Z1 Z2 alpha mu / hbar c  [fm^-1]
const double kGamLab = 3.0160293/4.0026032; // m(3He)/m(4He) for lab<->cm kinematics
const double kB32 = 1.5866, kB12 = 1.1575;  // 7Be binding energies w.r.t. 3He+4He [MeV]
const double kPi = 3.14159265358979323846;

// ---- fitted parameters (fm units; energies in MeV, c.m.) ---------------------
struct Par {
  const double a0   = 57.67150356;
  const double r0   = 0.7116703662;
  const double P0   = 1.321022594;
  const double r1p  = -0.07991713301;
  const double P1p  = 1.628638668;
  const double Q1p  = 1.929760404;
  const double r1m  = -0.0552250886;
  const double P1m  = 1.710226208;
  const double Q1m  = 2.252816067;
  const double ER7  = 2.991277955;
  const double G7   = 0.1598933147;
  const double b7   = -0.5904765561;
  const double ER5  = 5.34556747;
  const double G5   = 1.54641785;
  const double b5   = -0.4480256703;
  const double ia2p = 0.1325128292;
  const double ia2m = 0.1499516933;
};

// ---- special functions --------------------------------------------------------
inline cd digamma(cd z) {           // Re z > 0
  cd r = 0.0;
  while (std::real(z) < 10.0) { r -= 1.0/z; z += 1.0; }
  cd z2 = 1.0/(z*z);
  r += std::log(z) - 0.5/z
     - z2*(1.0/12 - z2*(1.0/120 - z2*(1.0/252 - z2*(1.0/240 - z2*(1.0/132)))));
  return r;
}
inline double ReH(double eta)  { return std::real(digamma(cd(1.0, eta))) - std::log(eta); }
inline double C02(double eta)  { double x = 2*kPi*eta; return x/std::expm1(x); }
inline double wl(double eta, int l) { double w = 1; for (int n = 1; n <= l; ++n) w *= 1 + eta*eta/(n*n); return w; }
inline double kOfE(double Ecm) { return std::sqrt(2*kMu*Ecm)/kHbarc; }
inline double EcmOfElab(double E3lab) { return E3lab*kM4/(kM3+kM4); }

// cot(delta_l) from the CM-ERE function K_l(k^2)
inline double cotERE(double k, int l, double K) {
  double eta = kKc/k, w = wl(eta, l);
  return (K - 2*kKc*std::pow(k, 2*l)*w*ReH(eta)) / (w*C02(eta)*std::pow(k, 2*l+1));
}
// 1/a1 such that the P-wave amplitude has a bound-state pole at binding energy B
inline double invA1(double r1, double P1, double Q1, double B) {
  double g = std::sqrt(2*kMu*B)/kHbarc, etaB = kKc/g, k2 = -g*g;
  double Hb = std::real(digamma(cd(etaB, 0))) + 0.5/etaB - std::log(etaB);
  return r1/2*k2 + P1/4*k2*k2 + Q1/8*k2*k2*k2 - 2*kKc*k2*(1 - etaB*etaB)*Hb;
}
inline double pen3(double Ecm) { double k = kOfE(Ecm), eta = kKc/k; return wl(eta,3)*C02(eta)*std::pow(k,7); }
inline double resCot(double Ecm, double ER, double G, double b) {
  return (ER - Ecm)/(G/2) * pen3(ER)/pen3(Ecm) * std::exp(b*(ER - Ecm));
}

// nuclear phase shifts [rad]; dp[l] -> J=l+1/2, dm[l] -> J=l-1/2
inline void Phases(double E3lab, double dp[4], double dm[4]) {
  const Par P;
  double Ecm = EcmOfElab(E3lab), k = kOfE(Ecm), k2 = k*k;
  dp[0] = std::atan2(1.0, cotERE(k, 0, -1/P.a0 + P.r0/2*k2 + P.P0/4*k2*k2)); dm[0] = 0;
  dp[1] = std::atan2(1.0, cotERE(k, 1, -invA1(P.r1p,P.P1p,P.Q1p,kB32) + P.r1p/2*k2 + P.P1p/4*k2*k2 + P.Q1p/8*k2*k2*k2));
  dm[1] = std::atan2(1.0, cotERE(k, 1, -invA1(P.r1m,P.P1m,P.Q1m,kB12) + P.r1m/2*k2 + P.P1m/4*k2*k2 + P.Q1m/8*k2*k2*k2));
  dp[2] = std::atan2(1.0, cotERE(k, 2, -P.ia2p));
  dm[2] = std::atan2(1.0, cotERE(k, 2, -P.ia2m));
  dp[3] = std::atan2(1.0, resCot(Ecm, P.ER7, P.G7, P.b7));   // 7/2-
  dm[3] = std::atan2(1.0, resCot(Ecm, P.ER5, P.G5, P.b5));   // 5/2-
}

// spin-non-flip f and spin-flip g amplitudes [fm], c.m. angle in degrees
inline void Amplitudes(double E3lab, double thcm_deg, cd& f, cd& g) {
  double Ecm = EcmOfElab(E3lab), k = kOfE(Ecm), eta = kKc/k;
  double th = thcm_deg*kPi/180, x = std::cos(th), s = std::sin(th), s2 = std::pow(std::sin(th/2), 2);
  f = -eta/(2*k*s2) * std::exp(cd(0, -eta*std::log(s2)));   // Rutherford (exp(2i sigma0) dropped)
  g = 0.0;
  double dp[4], dm[4]; Phases(E3lab, dp, dm);
  double Pl[4]  = {1, x, 0.5*(3*x*x - 1), 0.5*(5*x*x*x - 3*x)};
  double P1l[4] = {0, s, 3*x*s, 1.5*(5*x*x - 1)*s};            // sin(th) dP_l/dx
  double sig = 0;
  for (int l = 0; l < 4; ++l) {
    if (l > 0) sig += std::atan(eta/l);
    cd c  = std::exp(cd(0, 2*sig));
    cd Tp = std::exp(cd(0, dp[l]))*std::sin(dp[l]);
    cd Tm = std::exp(cd(0, dm[l]))*std::sin(dm[l]);
    f += c/k*Pl[l]*((l+1.0)*Tp + double(l)*Tm);
    if (l > 0) g += c/k*P1l[l]*(Tp - Tm);
  }
}

// kinematics: lab angle of the scattered 3He -> c.m. angle (single valued, m3 < m4)
inline double ThetaCMfromLab(double thlab_deg) {
  double t = thlab_deg*kPi/180;
  return (t + std::asin(kGamLab*std::sin(t)))*180/kPi;
}
inline double ThetaLabfromCM(double thcm_deg) {
  double t = thcm_deg*kPi/180;
  return std::atan2(std::sin(t), std::cos(t) + kGamLab)*180/kPi;
}
inline double JacobianCMtoLab(double thcm_deg) {   // dsig/dOmega_lab = J * dsig/dOmega_cm
  double c = std::cos(thcm_deg*kPi/180), q = 1 + kGamLab*kGamLab + 2*kGamLab*c;
  return std::pow(q, 1.5)/std::fabs(1 + kGamLab*c);
}
inline double RutherfordCM(double E3lab, double thcm_deg) {   // mb/sr
  double k = kOfE(EcmOfElab(E3lab)), eta = kKc/k, s2 = std::pow(std::sin(thcm_deg*kPi/360), 2);
  return 10*std::pow(eta/(2*k*s2), 2);
}
} // namespace he3he4

// =============================== user functions ===============================
double He3He4_CrossSectionCM(double E3lab_MeV, double thetaCM_deg) {
  std::complex<double> f, g; he3he4::Amplitudes(E3lab_MeV, thetaCM_deg, f, g);
  return 10.0*(std::norm(f) + std::norm(g));                 // fm^2 -> mb
}
double He3He4_AnalyzingPowerCM(double E3lab_MeV, double thetaCM_deg) {
  std::complex<double> f, g; he3he4::Amplitudes(E3lab_MeV, thetaCM_deg, f, g);
  return 2.0*std::imag(f*std::conj(g))/(std::norm(f) + std::norm(g));
}
double He3He4_CrossSection(double E3lab_MeV, double thetaLab_deg, int frame = 0) {
  double thcm = he3he4::ThetaCMfromLab(thetaLab_deg);
  double s = He3He4_CrossSectionCM(E3lab_MeV, thcm);
  return frame == 0 ? s*he3he4::JacobianCMtoLab(thcm) : s;
}
double He3He4_AnalyzingPower(double E3lab_MeV, double thetaLab_deg) {
  return He3He4_AnalyzingPowerCM(E3lab_MeV, he3he4::ThetaCMfromLab(thetaLab_deg));
}
// TF1/TF2-style wrappers: x[0] = E3lab (MeV), x[1] = theta_lab (deg)
double He3He4_CrossSection_TF2(double* x, double*)   { return He3He4_CrossSection(x[0], x[1]); }
double He3He4_AnalyzingPower_TF2(double* x, double*) { return He3He4_AnalyzingPower(x[0], x[1]); }

// ================================ ROOT demo ====================================
#if defined(__has_include)
#if __has_include("TF2.h")
#include "TF2.h"
#include "TCanvas.h"
#include "TStyle.h"
void He3He4_Elastic() {
  printf(" E3lab[MeV] thLab[deg] thCM[deg]  dsig/dOm_lab[mb/sr]  dsig/dOm_cm[mb/sr]   A_y\n");
  for (double E = 4.0; E <= 6.001; E += 0.5)
    for (double t = 37; t <= 60.01; t += 7.666666)
      printf("  %6.3f    %6.2f    %6.2f      %10.3f          %10.3f       %+6.3f\n", E, t,
             he3he4::ThetaCMfromLab(t), He3He4_CrossSection(E, t), He3He4_CrossSection(E, t, 1),
             He3He4_AnalyzingPower(E, t));
  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("cHe3He4", "3He+4He elastic", 1200, 500); c->Divide(2, 1);
  TF2* fs = new TF2("fSigmaLab", He3He4_CrossSection_TF2, 4, 6, 37, 60, 0);
  TF2* fa = new TF2("fAy",       He3He4_AnalyzingPower_TF2, 4, 6, 37, 60, 0);
  fs->SetNpx(200); fs->SetNpy(100); fa->SetNpx(200); fa->SetNpy(100);
  fs->SetTitle("d#sigma/d#Omega_{lab} (mb/sr);E_{lab}(^{3}He) [MeV];#theta_{lab}(^{3}He) [deg]");
  fa->SetTitle("A_{y};E_{lab}(^{3}He) [MeV];#theta_{lab}(^{3}He) [deg]");
  c->cd(1); fs->Draw("colz"); c->cd(2); fa->Draw("colz");
}
#endif
#endif

#ifdef HE3HE4_TEST
int main() {   // g++ -std=c++11 -DHE3HE4_TEST He3He4_Elastic.C && ./a.out E thlab
  for (double E = 4.0; E <= 6.001; E += 0.5)
    for (double t = 37; t <= 60.01; t += 11.5)
      printf("%6.3f %6.2f %6.2f %10.4f %10.4f %+7.4f\n", E, t, he3he4::ThetaCMfromLab(t),
             He3He4_CrossSection(E, t), He3He4_CrossSection(E, t, 1), He3He4_AnalyzingPower(E, t));
  return 0;
}
#endif
