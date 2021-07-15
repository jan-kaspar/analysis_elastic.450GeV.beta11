#include "classes/command_line_tools.hh"
#include "classes/common_algorithms.hh"

#include "TFile.h"
#include "TGraph.h"
#include "TF1.h"

#include <cstdio>
#include <vector>
#include <string>

using namespace std;

//----------------------------------------------------------------------------------------------------

TGraph* GetGraph(const string &dir, const string &model, const string &scenario, const string &obj)
{
	string file = dir + "/" + model + "/" + scenario + ".root";
	TFile *f_in = TFile::Open(file.c_str());
	if (!f_in)
	{
		printf("ERROR: can't open file '%s'.\n", file.c_str());
		return nullptr;
	}

	TGraph *h_orig = (TGraph *) f_in->Get(obj.c_str());
	if (!h_orig)
	{
		printf("ERROR: can't load object '%s'.\n", obj.c_str());
		return nullptr;
	}

	gDirectory = nullptr;	// not to associate h_ret with f_in which will be closed in a moment
	TGraph *h_ret = new TGraph(*h_orig);

	delete f_in;

	return h_ret;
}

//----------------------------------------------------------------------------------------------------

TGraph* Divide(const TGraph *g_num, const TGraph *g_den)
{
	TGraph *g_out = new TGraph(*g_num);

	for (int i = 0; i < g_out->GetN(); i++)
	{
		double x, y;
		g_out->GetPoint(i, x, y);

		const double d = g_den->Eval(x);
		const double r = (d == 0.) ? 0. : y / d;

		g_out->SetPoint(i, x, r);
	}

	return g_out;
}

//----------------------------------------------------------------------------------------------------

TGraph* Multiply(const TGraph *g1, const TGraph *g2)
{
	TGraph *g_out = new TGraph(*g1);

	for (int i = 0; i < g_out->GetN(); i++)
	{
		double x, y;
		g_out->GetPoint(i, x, y);

		const double m = g2->Eval(x);

		g_out->SetPoint(i, x, y * m);
	}

	return g_out;
}

//----------------------------------------------------------------------------------------------------

void Scale(TGraph *g, double s)
{
	for (int i = 0; i < g->GetN(); i++)
	{
		double x, y;
		g->GetPoint(i, x, y);
		g->SetPoint(i, x, y * s);
	}
}

//----------------------------------------------------------------------------------------------------

double GetNormalisation(TGraph *g)
{
	return 1. / GetNormalizationFactor(g);
}

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: mc_process [option] [option] ...\n");
	printf("OPTIONS:\n");
	printf("    -input <directory>\n");
	printf("    -output <file>\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	// defaults
	string inputDirectory = ".";

	string outFileName = "ni_process.root";

	string ref_scenario = "none";

	string model_base = "fit_1"; 		// used for most scenarios
	string model_secondary = "fit_1";	// used only for unsmearing model uncertainty

	struct Scenario
	{
		string label;
		enum Mode { mDsdt, mUnsmearing } mode;
	};

	vector<Scenario> scenarios = {
		// TODO: uncomment other when ready

		{ "none", Scenario::mDsdt },

		{ "sh-thx", Scenario::mDsdt },
		{ "sh-thx-LRasym", Scenario::mDsdt },

		{ "sh-thy", Scenario::mDsdt },
		{ "sh-thy-LRasym", Scenario::mDsdt },
		{ "sh-thy-TBuncor", Scenario::mDsdt },
		{ "sh-thy-TBuncor-LRasym", Scenario::mDsdt },

		{ "tilt-thx-thy", Scenario::mDsdt },
		{ "tilt-thx-thy-LRasym", Scenario::mDsdt },

		{ "sc-thxy-mode1", Scenario::mDsdt },
		{ "sc-thxy-mode2", Scenario::mDsdt },
		{ "sc-thxy-mode3", Scenario::mDsdt },
		{ "sc-thxy-mode4", Scenario::mDsdt },

		/*
		{ "dx-sigma", Scenario::mDsdt },
		{ "dy-sigma", Scenario::mDsdt },
		{ "dx-non-gauss", Scenario::mDsdt },

		{ "eff-intercept", Scenario::mDsdt },
		{ "eff-slope", Scenario::mDsdt },
		*/

		{ "beam-mom", Scenario::mDsdt },

		/*
		{ "mx-sigma", Scenario::mUnsmearing },
		{ "my-sigma", Scenario::mUnsmearing },

		{ "norm", Scenario::mDsdt },
		*/
	};

	// parse command line
	for (int argi = 1; (argi < argc) && (cl_error == 0); ++argi)
	{
		if (strcmp(argv[argi], "-h") == 0 || strcmp(argv[argi], "--help") == 0)
		{
			cl_error = 1;
			continue;
		}

		if (TestStringParameter(argc, argv, argi, "-input", inputDirectory)) continue;

		if (TestStringParameter(argc, argv, argi, "-output", outFileName)) continue;

		printf("ERROR: unknown option '%s'.\n", argv[argi]);
		cl_error = 1;
	}

	if (cl_error)
	{
		PrintUsage();
		return 1;
	}

	// output file
	TFile *f_out = new TFile(outFileName.c_str(), "recreate");

	// process all scenarios
	for (const auto &scenario : scenarios)
	{
		//printf("* %s\n", scenario.label.c_str());

		TDirectory *d_scenario = f_out->mkdir(scenario.label.c_str());

		// get reference graphs
		TGraph *g_tr_ref = GetGraph(inputDirectory, model_base, ref_scenario, "g_dsdt_true");
		TGraph *g_re_ref = GetGraph(inputDirectory, model_base, ref_scenario, "g_dsdt_reco");

		// get graphs with effect
		TGraph *g_tr = GetGraph(inputDirectory, model_base, scenario.label, "g_dsdt_true");
		TGraph *g_re = GetGraph(inputDirectory, model_base, scenario.label, "g_dsdt_reco");

		// validate input
		if (!g_tr_ref || !g_re_ref || !g_re)
			continue;

		// make corrected graphs for scaling
		TGraph *g_1 = nullptr;
		TGraph *g_0 = nullptr;
		if (scenario.mode == Scenario::mDsdt)
		{
			TGraph *g_unsm_corr = Divide(g_tr_ref, g_re_ref);
			g_1 = Multiply(g_re, g_unsm_corr);

			g_0 = new TGraph(*g_tr_ref);

			delete g_unsm_corr;
		}

		if (scenario.mode == Scenario::mUnsmearing)
		{
			TGraph *g_unsm_corr = Divide(g_tr, g_re);
			g_1 = Multiply(g_re_ref, g_unsm_corr);

			g_0 = new TGraph(*g_tr_ref);

			delete g_unsm_corr;
		}

		// normalise histograms
		const double n_0 = GetNormalisation(g_0);
		Scale(g_0, n_0);

		const double n_1 = GetNormalisation(g_1);
		Scale(g_1, n_1);

		// evaluate effect
		TGraph *g_eff = Divide(g_1, g_0);

		gDirectory = d_scenario;
		g_eff->Write("g_eff");

		// clean up
		delete g_0;
		delete g_1;
		delete g_eff;
	}

	// extra: model uncertainty of the unsmearing correction
	TDirectory *d_scenario = f_out->mkdir("unsmearing-model");

	{
		// get histograms
		TGraph *g_tr_base = GetGraph(inputDirectory, model_base, ref_scenario, "g_dsdt_true");
		TGraph *g_re_base = GetGraph(inputDirectory, model_base, ref_scenario, "g_dsdt_reco");

		TGraph *g_tr_secondary = GetGraph(inputDirectory, model_secondary, ref_scenario, "g_dsdt_true");
		TGraph *g_re_secondary = GetGraph(inputDirectory, model_secondary, ref_scenario, "g_dsdt_reco");

		if (g_tr_base && g_re_base && g_tr_secondary && g_re_secondary)
		{
			// calculate unsmearing correction (secondary model)
			TGraph *g_unsm_corr = Divide(g_tr_secondary, g_re_secondary);

			// apply unsmearing correction to base reco histogram
			TGraph *g_bias = Multiply(g_re_base, g_unsm_corr);

			// normalise histograms
			const double n_base = GetNormalisation(g_tr_base);
			Scale(g_tr_base, n_base);

			const double n_bias = GetNormalisation(g_bias);
			Scale(g_bias, n_bias);

			// evaluate effect
			TGraph *g_eff = Divide(g_bias, g_tr_base);

			gDirectory = d_scenario;
			g_eff->Write("g_eff");

			// clean up
			delete g_unsm_corr;
			delete g_bias;
			delete g_eff;
		}
	}

	delete f_out;

	return 0;
}
