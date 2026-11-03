////////////////////////////////////////////////////////////////////////////////
//   HitData.hh
//
//   Silion strip hit
//
// - Author: Hoyong   Jeong   (hoyong5419@korea.ac.kr)
//           Antonino Cannavo (  acannavo@bnl.gov    )
////////////////////////////////////////////////////////////////////////////////



struct HitData
{
	std::vector<int>    stripID;
	std::vector<double> t;
	std::vector<double> dE;


	void Clear()
	{
		stripID . clear();
		t       . clear();
		dE      . clear();
	}
};



inline thread_local HitData gHit;
