#include "G4Run.hh"
#include "G4SystemOfUnits.hh"

#include "RunAct.hh"
#include "Ana.hh"



RunAct::RunAct() : G4UserRunAction()
{
	//------------------------------------------------
	// Create analysis manager
	// The choice of analysis technology is done via
	// selectin of a namespace in Ana.hh
	//------------------------------------------------
	G4AnalysisManager* AM = G4AnalysisManager::Instance();
	G4cout << "Using " << AM -> GetType() << G4endl;


	//------------------------------------------------
	// Default settings
	//------------------------------------------------
	AM -> SetVerboseLevel(0);


	//------------------------------------------------
	// Creating ntuple
	//------------------------------------------------
	AM -> CreateNtuple("tree", "Energy Deposition");
	AM -> CreateNtupleIColumn("eventID"); // Column ID =  0
	char buf[64];
	G4String name;
	for ( unsigned short int i = 0; i < 35; i++ )
	{
		std::snprintf(buf, sizeof(buf), "eDepL%02d", i);
		name = buf;
		AM -> CreateNtupleDColumn(name);
		std::snprintf(buf, sizeof(buf), "tL%02d", i);
		name = buf;
		AM -> CreateNtupleDColumn(name);
		std::snprintf(buf, sizeof(buf), "eDepR%02d", i);
		name = buf;
		AM -> CreateNtupleDColumn(name);
		std::snprintf(buf, sizeof(buf), "tR%02d", i);
		name = buf;
		AM -> CreateNtupleDColumn(name);
	}
	AM -> FinishNtuple();
}



RunAct::~RunAct()
{
	delete G4AnalysisManager::Instance();
}



void RunAct::BeginOfRunAction(const G4Run* /*run*/)
{
	//------------------------------------------------
	// Inform the runManager to save random number seed
	//------------------------------------------------
//	G4RunManager::GetRunManager()->SetRandomNumberStore(true);


	//------------------------------------------------
	// Get analysis manager
	//------------------------------------------------
	G4AnalysisManager* AM = G4AnalysisManager::Instance();


	//------------------------------------------------
	// Open an output file
	// The default file name is set in RunAct::RunAct(),
	// it can be overwritten in a macro
	//------------------------------------------------
	// Get current time to include it to file name
	// This time info is going to be used to generate output file name.
	time_t rawTime;
	struct tm* timeInfo;
	char buffer[80];
	time(&rawTime);
	timeInfo = localtime(&rawTime);
	strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", timeInfo);
	std::string sTime(buffer);

	// Open an output file
	// The default file name is set in LSRunAct::LSRunAct(),
	// and it can be overwritten in a macro
	G4String fileName = "blpmc_";
	fileName += sTime;
	fileName += ".root";
	G4cout << fileName << G4endl;

	AM -> OpenFile(fileName);
	G4cout << "Using " << AM -> GetType() << G4endl;
}



void RunAct::EndOfRunAction(const G4Run* /*run*/)
{
	//------------------------------------------------
	// Save histograms & ntuple
	//------------------------------------------------
	G4AnalysisManager* AM = G4AnalysisManager::Instance();
	AM -> Write();
	AM -> CloseFile();
}
