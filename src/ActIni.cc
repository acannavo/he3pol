#include "ActIni.hh"
#include "PriGenAct.hh"
#include "RunAct.hh"
#include "EveAct.hh"
#include "SteAct.hh"



ActIni::ActIni() : G4VUserActionInitialization()
{
}



ActIni::~ActIni()
{
}



void ActIni::BuildForMaster() const
{
	SetUserAction(new RunAct);
}



void ActIni::Build() const
{
	SetUserAction(new PriGenAct);
	SetUserAction(new RunAct);
  
	EveAct* EA = new EveAct;
	SetUserAction(EA);
  
	SetUserAction(new SteAct(EA));
}  
