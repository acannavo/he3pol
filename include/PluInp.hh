#ifndef PLUINP_h
#define PLUINP_h 1

////////////////////////////////////////////////////////////////////////////////
///
///   PluInp.hh
///
///   This file is a header for PluInp class. PluInp class manages
/// Pluto input.
///
/// - Author: Hoyong Jeong     (hoyong5419@korea.ac.kr)
///           Antonino Cannavo (acannavo@bnl.gov      )
///
////////////////////////////////////////////////////////////////////////////////

#include <vector>

#include "TROOT.h"
#include "TClass.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"

#include "PParticle.h"

class PluInp
{
	public:
	PluInp();
	~PluInp();

	bool IsGood();
	void Clear();
	void ReadEntry(long iEntry);

	int GetNPar();
	vector<int>    GetID();
	vector<double> GetPX();
	vector<double> GetPY();
	vector<double> GetPZ();
	vector<double> GetTheta();
	vector<double> GetPhi();
	vector<double> GetKE();
	vector<double> GetX();
	vector<double> GetY();
	vector<double> GetZ();


	private:
	TFile* m_File;
	TTree* m_Tree;

	bool m_Good; 
	// Declaration of leave types
	int m_NPar;
	TClonesArray* m_ParArr;
	PParticle* m_Par;

	// Particle properties
	vector<int>    m_ID;
	vector<double> m_PX;
	vector<double> m_PY;
	vector<double> m_PZ;
	vector<double> m_Theta;
	vector<double> m_Phi;
	vector<double> m_KE;
	vector<double> m_X;
	vector<double> m_Y;
	vector<double> m_Z;
};

#endif
