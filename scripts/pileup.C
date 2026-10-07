void pileup(const char* fname, int N = 50000000, double T = 20. /*ns*/, double binNs = 0.05, double tail = 40. /*ns*/)
{
	TH1::AddDirectory(kFALSE);
	TFile file(fname);
	auto* tree = (TTree*) file . Get("tree");
	const unsigned long int Ntotal = tree -> GetEntries();
	if ( N > Ntotal ) N = Ntotal;

	int eventID;
	int stripID3;
	int stripID4;
	double time3;
	double time4;
	double theta3;
	double theta4;
	double energy3;
	double energy4;
	std::vector<int>* stripID = nullptr;
	std::vector<double>* t  = nullptr;
	std::vector<double>* dE = nullptr;

	tree -> SetBranchAddress("eventID" ,  &eventID);
	tree -> SetBranchAddress("stripID3",  &stripID3);
	tree -> SetBranchAddress("stripID4",  &stripID4);
	tree -> SetBranchAddress("time3"   ,  &time3   );
	tree -> SetBranchAddress("time4"   ,  &time4   );
	tree -> SetBranchAddress("theta3"  ,  &theta3  );
	tree -> SetBranchAddress("theta4"  ,  &theta4  );
	tree -> SetBranchAddress("energy3" ,  &energy3 );
	tree -> SetBranchAddress("energy4" ,  &energy4 );
	tree -> SetBranchAddress("stripID" ,  &stripID );
	tree -> SetBranchAddress("t"       ,  &t       );
	tree -> SetBranchAddress("dE"      ,  &dE      );

	const double dt = T / N;
	const int nBins = int((T + tail) / binNs);

	// Define container for current
	TH1D* hIL[12];
	TH1D* hIR[12];
	TString sTemp;
	for ( unsigned short int i = 0; i < 12; i++ )
	{
		sTemp . Form("hIL_%d", i);
		hIL[i] = new TH1D(sTemp . Data(), sTemp . Data(), nBins, 0., T + tail);
		hIL[i] -> SetFillColor(kGreen);
		hIL[i] -> GetXaxis() -> SetTitle("t (ns)");
		hIL[i] -> GetYaxis() -> SetTitle("dE (MeV)");
		hIL[i] -> GetYaxis() -> SetRangeUser(0., 5.);
		sTemp . Form("hIR_%d", i);
		hIR[i] = new TH1D(sTemp . Data(), sTemp . Data(), nBins, 0., T + tail);
		hIR[i] -> SetFillColor(kGreen);
		hIR[i] -> GetXaxis() -> SetTitle("t (ns)");
		hIR[i] -> GetYaxis() -> SetTitle("dE (MeV)");
		hIR[i] -> GetYaxis() -> SetRangeUser(0., 5.);
	}


	// Random sampling among entries (When N < Ntotal)
	std::vector<unsigned long int> idx(tree -> GetEntries());
	std::iota(idx . begin(), idx . end(), 0);
	std::shuffle(idx . begin(), idx . end(), std::mt19937(12345)); // <- change seed if want to see other combination

	cout << "[MC truth] " << endl;
	for ( unsigned long int i = 0; i < N; i++ )
	{
		tree -> GetEntry(idx[i]);

		const double shift = dt * i;

		if ( stripID3 != -1 ) cout << "He3: " << stripID3 << ", " << time3 + shift << " ns, " << energy3 << " MeV" << endl;
		if ( stripID4 != -1 ) cout << "He4: " << stripID4 << ", " << time4 + shift << " ns, " << energy4 << " MeV" << endl;

		for ( size_t ihit = 0; ihit < stripID -> size(); ihit++ )
		{
//			cout << stripID -> at(ihit) << ", " << t -> at(ihit) << ", " << dE -> at(ihit) << endl;
			if   ( stripID -> at(ihit) < 100 ) hIL[stripID -> at(ihit)      ] -> Fill(t -> at(ihit) + shift, dE -> at(ihit));
			else                               hIR[stripID -> at(ihit) - 100] -> Fill(t -> at(ihit) + shift, dE -> at(ihit));
		}
	}


	// Visualize
	TCanvas* cL = new TCanvas("cL", "Left Strips" , 1800, 600);
	TCanvas* cR = new TCanvas("cR", "Right Strips", 1800, 600);
	cL -> Divide(4, 3);
	cR -> Divide(4, 3);
	for ( unsigned short int i = 0; i < 12; i++ )
	{
		cL -> cd(i+1);
		hIL[i] -> Draw("hist");
		cR -> cd(i+1);
		hIR[i] -> Draw("hist");
	}




	return;
}
