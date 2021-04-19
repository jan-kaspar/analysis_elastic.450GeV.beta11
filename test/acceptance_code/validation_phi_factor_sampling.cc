#include "classes/common_init.hh"
#include "classes/AcceptanceCalculator.hh"
//#include "classes/Kinematics.hh"

#include "TFile.h"
#include "TGraph.h"

#include <cstdio>
#include <memory>

using namespace std;

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: validation_phi_factor_sampling [options]\n");
	printf("OPTIONS:\n");
	printf("    -output <file> set output file name\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	// defaults
	string cfg_file = "config.py";
	string diagonal_input = "45b_56t";

	string outputFileName = "validation_phi_factor_sampling.root";

	// parse command line
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-output") == 0)
		{
			if (argc-1 > i)
				outputFileName = argv[++i];
			continue;
		}

		printf("ERROR: unknown parameter `%s'.\n", argv[i]);
		PrintUsage();
		return 3;
	}

	// print settings
	printf(">> executed with these parameters:\n");
	printf("\toutputFileName = %s\n", outputFileName.c_str());

	// run initialisation
	if (Init(cfg_file, diagonal_input) != 0)
		return 2;

	// initialise acceptance calculation
	AcceptanceCalculator accCalc;
	accCalc.Init(cfg.th_y_sign, anal);
	accCalc.SamplePhiFactor();

	// prepare output
	unique_ptr<TGraph> g_pf_calc_vs_th(new TGraph), g_pf_inter_vs_th(new TGraph), g_pf_diff_vs_th(new TGraph), g_pf_rel_diff_vs_th(new TGraph);

	// sample phi factor
	for (double th = 200E-6; th < 700E-6; th += 0.05E-6)
	{
		accCalc.useSampledPhiFactor = false;
		const double pf_calc = accCalc.PhiFactor(th);

		accCalc.useSampledPhiFactor = true;
		const double pf_inter = accCalc.PhiFactor(th);

		const double pf_diff = pf_inter - pf_calc;
		const double pf_rel_diff = (pf_calc > 0) ? pf_diff / pf_calc : 0.;

		if (isnan(pf_calc) || isinf(pf_calc) || pf_calc > 5E3)
			continue;

		//printf("th = %.3E, pf_calc = %.3E\n", th, pf_calc);

		int idx = g_pf_calc_vs_th->GetN();
		g_pf_calc_vs_th->SetPoint(idx, th, pf_calc);
		g_pf_inter_vs_th->SetPoint(idx, th, pf_inter);
		g_pf_diff_vs_th->SetPoint(idx, th, pf_diff);
		g_pf_rel_diff_vs_th->SetPoint(idx, th, pf_rel_diff);
	}

	// save results
	unique_ptr<TFile> f_out(TFile::Open(outputFileName.c_str(), "recreate"));

	g_pf_calc_vs_th->Write("g_pf_calc_vs_th");
	g_pf_inter_vs_th->Write("g_pf_inter_vs_th");
	g_pf_diff_vs_th->Write("g_pf_diff_vs_th");
	g_pf_rel_diff_vs_th->Write("g_pf_rel_diff_vs_th");

	return 0;
}
