#include <TDirectory.h>
#include "classes/common_init.hh"
#include "classes/command_line_tools.hh"

#include "TFile.h"
#include "TH2D.h"
#include "TF2.h"
#include "TGraph.h"
#include "TCanvas.h"

#include <cstdio>
#include <vector>
#include <string>

using namespace std;

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: program <option> <option>\n");
	printf("OPTIONS:\n");
	printf("    -cfg <file>       config file\n");
	printf("    -dgn <string>     diagonal\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	// defaults
	string cfg_file = "config.py";
	string diagonal_input = "";

	// parse command line
	for (int argi = 1; (argi < argc) && (cl_error == 0); ++argi)
	{
		if (strcmp(argv[argi], "-h") == 0 || strcmp(argv[argi], "--help") == 0)
		{
			cl_error = 1;
			continue;
		}

		if (TestStringParameter(argc, argv, argi, "-cfg", cfg_file)) continue;
		if (TestStringParameter(argc, argv, argi, "-dgn", diagonal_input)) continue;

		printf("ERROR: unknown option '%s'.\n", argv[argi]);
		cl_error = 1;
	}

	if (cl_error)
	{
		PrintUsage();
		return 1;
	}

	// run initialisation
	if (Init(cfg_file, diagonal_input) != 0)
		return 2;

	// compatibility check
	if (cfg.diagonal != d45b_56t && cfg.diagonal != d45t_56b)
		return rcIncompatibleDiagonal;

	// print settings
	PrintConfiguration();

	// settings
	vector<string> units;
	units.push_back("L_2_F");
	units.push_back("L_1_F");
	units.push_back("R_1_F");
	units.push_back("R_2_F");

	const double th_y_min = 40E-6;
	const double th_y_max = 140E-6;

	printf("th_y_min = %E\n", th_y_min);
	printf("th_y_max = %E\n", th_y_max);

	// prepare input file
	string fn_in = "eff3outof4_" + cfg.diagonal_str + ".root";
	TFile *f_in = new TFile(fn_in.c_str());
	if (f_in->IsZombie())
	{
		printf("ERROR: can't load file '%s'\n", fn_in.c_str());
		return 1;
	}

	// prepare output
	TFile *f_out = new TFile(("eff3outof4_fit_" + cfg.diagonal_str + ".root").c_str(), "recreate");

	// loop over RPs
	for (unsigned int ui = 0; ui < units.size(); ui++)
	{
		printf("\n\n>> %s\n", units[ui].c_str());

		TDirectory *d_unit = f_out->mkdir(units[ui].c_str());
		gDirectory = d_unit;

		// get input
		TH2D *h_eff = (TH2D *) f_in->Get(("excluded RPs "+units[ui]+"/n_si 3.0/th_x, th_y dependence (unif)/h_simple_ratio_vs_th_x_th_y").c_str());
		if (h_eff == nullptr)
		{
			printf("ERROR: can't load h_eff for unit '%s'.\n", units[ui].c_str());
			continue;
		}

		// zero bins which should be excluded from fit
		TH2D *h_eff_for_fit = new TH2D(*h_eff);
		for (int bi_x = 1; bi_x <= h_eff_for_fit->GetNbinsX(); ++bi_x)
		{
			for (int bi_y = 1; bi_y <= h_eff_for_fit->GetNbinsY(); ++bi_y)
			{
				const double th_x = h_eff_for_fit->GetXaxis()->GetBinCenter(bi_x);
				const double th_y = h_eff_for_fit->GetYaxis()->GetBinCenter(bi_y);

				bool discard = (th_y < th_y_min || th_y > th_y_max || !anal.fc_G.Satisfied(th_x, th_y));

				if (discard)
				{
					h_eff_for_fit->SetBinContent(bi_x, bi_y, 0.);
					h_eff_for_fit->SetBinError(bi_x, bi_y, 0.);
				}
			}
		}

		h_eff_for_fit->Write();

		// make fit
		TF2 *ff = nullptr;
		if (cfg.diagonal_str == "45b_56t" && units[ui] == "L_1_F")
		{
			ff = new TF2("ff", "[0]/2 * (1. + TMath::Erf( (x*cos([1]) + y*sin([1]) - [2])/[3] ))");
			ff->SetParameters(0.97, 1.67, -32E-6, 55E-6);
			ff->SetParLimits(0, 0.9, 1.0);
			ff->SetParLimits(1, 1.64, 1.70);
			ff->SetParLimits(2, -20E-6, +44E-6);
			ff->SetParLimits(3, 45E-6, 65E-6);
			h_eff_for_fit->Fit(ff);
		} else {
			ff = new TF2("ff", "[0]");
			h_eff_for_fit->Fit(ff);
		}

		ff->Write("ff");

		// save slices
		const vector<tuple<double, double>> th_x_slices = {
			{-250E-6, -200E-6},
			{-200E-6, -100E-6},
			{-100E-6, 0E-6},
			{0E-6, +100E-6},
			{+100E-6, +200E-6},
			{+200E-6, +250E-6},
		};

		for (const auto [th_x_min, th_x_max] : th_x_slices)
		{
			char buf[100];
			sprintf(buf, "slice_th_x_%+.0f_%+.0f", th_x_min*1E6, th_x_max*1E6);
			
			gDirectory = d_unit->mkdir(buf);

			auto bi_min = h_eff_for_fit->GetXaxis()->FindBin(th_x_min);
			auto bi_max = h_eff_for_fit->GetXaxis()->FindBin(th_x_max);
			unsigned n_bins = bi_max - bi_min + 1;

			TH1D *h_th_y_slice = h_eff_for_fit->ProjectionY("h_th_y_slice", bi_min, bi_max);
			h_th_y_slice->Scale(1./n_bins);

			const double th_x_mean = (th_x_max + th_x_min) / 2.;

			TGraph *g_fit = new TGraph();
			g_fit->SetName("g_fit");
			g_fit->SetLineColor(2);
			for (double th_y = 0E-6; th_y <= 150E-6; th_y += 1E-6)
			{
				auto idx = g_fit->GetN();
				g_fit->SetPoint(idx, th_y, ff->Eval(th_x_mean, th_y));
			}

			TCanvas *c = new TCanvas();
			h_th_y_slice->Draw();
			g_fit->Draw("l");
			c->Write("cmp");
		}
	}

	delete f_out;
	return 0;
}
