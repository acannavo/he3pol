////////////////////////////////////////////////////////////////////////////////
///   ConMan.cc
///
///   Definitions of ConMan class's member functions.
/// All actions must be initialized here in order to use multi thread.
///
/// - Authors: Hoyong   Jeong   (hoyong5419@korea.ac.kr)
///            Antonino Cannavo (  acannavo@bnl.gov    )
////////////////////////////////////////////////////////////////////////////////



///-----------------------------------------------------------------------------
/// Headers
///-----------------------------------------------------------------------------
#include "ConMan.hh"



///-----------------------------------------------------------------------------
/// Constructor
///-----------------------------------------------------------------------------
ConMan::ConMan()
{
	LoadDefault();
}



///-----------------------------------------------------------------------------
/// Destructor
///-----------------------------------------------------------------------------
ConMan::~ConMan()
{
}


///-----------------------------------------------------------------------------
/// Load default
///-----------------------------------------------------------------------------
void ConMan::LoadDefault()
{
	///---------------------------------------------------------
	/// Target config
	///---------------------------------------------------------
	m_Config["PRESSURE"]          = "5";


	return;
}


///-----------------------------------------------------------------------------
/// Load conf file
///-----------------------------------------------------------------------------
bool ConMan::Load(const std::string& filename)
{
	///---------------------------------------------------------
	/// Check config file
	///---------------------------------------------------------
	std::ifstream file(filename);
	if ( !file )
	{
		std::cerr << "Failed to open config file: " << filename << std::endl;
		return false;
	}


	///---------------------------------------------------------
	/// Read config
	///---------------------------------------------------------
	std::string line;
	while ( std::getline(file, line) )
	{
		line = Trim(line);
		///-------------------------------------
		/// Ignore comments and empty lines
		///-------------------------------------
		if ( line . empty() || line[0] == '#' ) continue;

		std::istringstream iss(line);
		std::string key, value;

		///-------------------------------------
		/// Ignore wrong formatting lines
		///-------------------------------------
		if ( !(iss >> key >> value) ) continue;

		m_Config[key] = value;
	}


	return true;
}



///-----------------------------------------------------------------------------
/// Config getter
///-----------------------------------------------------------------------------
std::string ConMan::Get(const std::string& key, const std::string& defaultValue) const
{
	auto it = m_Config . find(key);


	return (it != m_Config . end()) ? it -> second : defaultValue;
}



///-----------------------------------------------------------------------------
/// Print configuration
///-----------------------------------------------------------------------------
void ConMan::Print()
{
	std::cout << "[He3Pol Configuration]" << std::endl;
	for ( const auto& pair : m_Config )
	{
		std::cout << "Key: " << pair . first << ", Value: " << pair . second << std::endl;
	}


	return;
}



///-----------------------------------------------------------------------------
/// String trimmer
///-----------------------------------------------------------------------------
std::string ConMan::Trim(const std::string& str)
{
	size_t first = str . find_first_not_of(" \t");
	if ( first == std::string::npos ) return "";
	size_t last  = str . find_last_not_of(" \t");


	return str . substr(first, (last - first + 1));
}
