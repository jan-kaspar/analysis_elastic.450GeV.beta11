#include "classes/common_init.hh"
#include "classes/command_line_tools.hh"

#include "TFile.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TProfile.h"

#include <string>

using namespace std;

//----------------------------------------------------------------------------------------------------

void RunOneFit(const TGraph *g_run_boundaries, const TProfile *p, const TGraphErrors *g_rms, double unc_th)
{
	// local fit function
	TF1 *ff = new TF1("ff", "1 ++ x");

	// graph of fit results
	TGraph *g_fits = new TGraph();
	g_fits->SetName("g_fits");
	g_fits->SetTitle(";timestamp;RMS");

	// loop over runs
	for (int rgi = 0; rgi < g_run_boundaries->GetN();)
	{
		// get run boundaries
		double run, run_beg, run_end;
		g_run_boundaries->GetPoint(rgi, run_beg, run);
		g_run_boundaries->GetPoint(rgi+1, run_end, run);
		rgi += 2;

		printf("run = %.0f\n", run);

		// adjust boundaries to source profile
		int bi_beg = p->GetXaxis()->FindBin(run_beg);
		int bi_end = p->GetXaxis()->FindBin(run_end);

		double t_min = p->GetXaxis()->GetBinLowEdge(bi_beg);
		double t_max = p->GetXaxis()->GetBinLowEdge(bi_end) + p->GetXaxis()->GetBinWidth(bi_end);

		printf("    t_min = %.0f, t_max = %.0f\n", t_min, t_max);

		// copy selected points
		TGraphErrors *g_rms_sel = new TGraphErrors();

		for (int pi = 0; pi < g_rms->GetN(); ++pi)
		{
			double t, rms, rms_u;
			g_rms->GetPoint(pi, t, rms);
			rms_u = g_rms->GetErrorY(pi);

			if (t < t_min || t > t_max)
				continue;

			if (rms_u > unc_th)
				continue;

			int idx = g_rms_sel->GetN();
			g_rms_sel->SetPoint(idx, t, rms);
			g_rms_sel->SetPointError(idx, 0., rms_u);
		}

		printf("    points selected: %i\n", g_rms_sel->GetN());

		// data sufficient?
		if (g_rms_sel->GetN() < 3)
			continue;

		// make fit
		g_rms_sel->Fit(ff, "Q");

		// add points to g_fit
		const double a = ff->GetParameter(1);
		const double b = ff->GetParameter(0);

		int idx = g_fits->GetN();
		g_fits->SetPoint(idx, run_beg, a*run_beg + b);
		g_fits->SetPoint(idx+1, run_end, a*run_end + b);

		// clean up
		delete g_rms_sel;
	}

	// save output
	g_fits->Write();

	// clean up
	delete ff;
}

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

	// get input
	TFile *f_in = new TFile((string("distributions_") + cfg.diagonal_str + ".root").c_str());

	TGraph *g_run_boundaries = (TGraph *) f_in->Get("time dependences/g_run_boundaries");
	
	TProfile *p_diffLR_th_x_vs_time = (TProfile *) f_in->Get("time dependences/p_diffLR_th_x_vs_time");
	TGraphErrors *gRMS_diffLR_th_x_vs_time = (TGraphErrors *) f_in->Get("time dependences/gRMS_diffLR_th_x_vs_time");
	
	TProfile *p_diffLR_th_y_vs_time = (TProfile *) f_in->Get("time dependences/p_diffLR_th_y_vs_time");
	TGraphErrors *gRMS_diffLR_th_y_vs_time = (TGraphErrors *) f_in->Get("time dependences/gRMS_diffLR_th_y_vs_time");

	if (!g_run_boundaries || !p_diffLR_th_x_vs_time || !gRMS_diffLR_th_x_vs_time || !p_diffLR_th_y_vs_time || !gRMS_diffLR_th_y_vs_time)
	{
		printf("ERROR: input not found (%p, %p, %p, %p, %p)\n", g_run_boundaries, p_diffLR_th_x_vs_time, gRMS_diffLR_th_x_vs_time, p_diffLR_th_y_vs_time, gRMS_diffLR_th_y_vs_time);
		return 1;
	}
	
	// prepare output
	TFile *f_out = new TFile((string("resolution_fit_") + cfg.diagonal_str + ".root").c_str(), "recreate");

	// do fits
	printf("\n\n---------- d_x ----------\n");
	gDirectory = f_out->mkdir("d_x");
	RunOneFit(g_run_boundaries, p_diffLR_th_x_vs_time, gRMS_diffLR_th_x_vs_time, 3E-6);

	printf("\n\n---------- d_y ----------\n");
	gDirectory = f_out->mkdir("d_y");
	RunOneFit(g_run_boundaries, p_diffLR_th_y_vs_time, gRMS_diffLR_th_y_vs_time, 1E-6);

	// clean up
	delete f_out;
	delete f_in;
	return 0;
}
