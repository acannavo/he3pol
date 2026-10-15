////////////////////////////////////////////////////////////////////////////////
//   ConMan.hh
//
//   This file is a header for ConMan class. This class manages various
// variable configurations used in simulation.
//
// - Author: Hoyong Jeong     (hoyong5419@korea.ac.kr)
//           Antonino Cannavo (acannavo@bnl.gov      )
////////////////////////////////////////////////////////////////////////////////



#ifndef CONMAN_H
#define CONMAN_H



#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>


class ConMan
{
	public:
	ConMan();
	~ConMan();

	void LoadDefault();
	bool Load(const std::string& filename);
	std::string Get(const std::string& key, const std::string& defaultValue = "") const;
	void Print();


	private:
	std::map<std::string, std::string> m_Config;
	static std::string Trim(const std::string& str);
};



#endif // CONMAN_H
