void plottxs()
{
	//----------------------------------------------------------
	// Glauber-Gribov XS of GEANT4
	// Can be obtained by executing ./diffxs [elab]
	//----------------------------------------------------------
	const double elab[3] = {   4.   ,   5.   ,   6.    };
	const double xs[3]   = { 551.603, 594.167, 616.058 };


	//----------------------------------------------------------
	// Open SONIK&Barnard data
	//----------------------------------------------------------
	TTree* t = new TTree("t", "SONIK&Barnard data");
	const Long64_t nrows = t -> ReadFile("../data/He3He4_xs.dat", "elab/D:xs/D");
	std::printf("read %lld rows from %s\n", nrows, "../data/He3He4_xs.dat");
	if ( nrows <= 0 ) return;
	double elabd  = 0;
	double xsd    = 0;
	t -> SetBranchAddress("elab", &elabd);
	t -> SetBranchAddress("xs"  , &xsd  );


	//----------------------------------------------------------
	// Graph: GEANT4
	//----------------------------------------------------------
	TGraph* gg = new TGraph(3, elab, xs);
	gg -> SetTitle("GEANT4's Total XS;E_{lab} [MeV];#sigma [mb]");
	gg -> SetMarkerStyle(20);
	gg -> SetMarkerColor(kRed);


	//----------------------------------------------------------
	// Graph: SONIK&Barnard
	//----------------------------------------------------------
	TGraph* gd = new TGraph();
	gd -> SetTitle("SONIK&Barnard's Total XS;E_{lab} [MeV];#sigma [mb]");
	gd -> SetMarkerStyle(21);
	for ( int i = 0; i < t -> GetEntries(); i++ )
	{
		t -> GetEntry(i);
		gd -> SetPoint(i, elabd, xsd);
	}

	gd -> Draw("AP");
	gg -> Draw("PLsame");
	gd -> GetXaxis() -> SetLimits(3.9, 6.1);
	auto leg = gPad -> BuildLegend();


	return;
}
