#ifndef ACTINI_h
#define ACTINI_h 1



#include "G4VUserActionInitialization.hh"



class ActIni : public G4VUserActionInitialization
{
public:
	ActIni();
	virtual ~ActIni();

	virtual void BuildForMaster() const;
	virtual void Build() const;
};



#endif
