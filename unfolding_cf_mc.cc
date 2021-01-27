#include "classes/common_init.hh"
#include "classes/command_line_tools.hh"
#include "classes/common_algorithms.hh"
#include "classes/AcceptanceCalculator.hh"
#include "classes/Kinematics.hh"

#include "TGraph.h"
#include "TFile.h"
#include "TH1D.h"
#include "TSpline.h"
#include "TRandom3.h"

#include <string>
#include <map>

using namespace std;

//----------------------------------------------------------------------------------------------------

TSpline* BuildSpline(TGraph *g)
{
	TSpline3 *s = new TSpline3("", g->GetX(), g->GetY(), g->GetN());
	s->SetName(g->GetName());
	return s;
}

//----------------------------------------------------------------------------------------------------

struct Model
{
	string label;
	string file;
	string object;
};

//----------------------------------------------------------------------------------------------------

struct PlotGroup
{
	TH1D *h_t_tr = nullptr, *h_t_sm = nullptr;

	void Init(unsigned int Nbins, double *binEdges)
	{
		h_t_tr = new TH1D("", ";t", Nbins, binEdges); h_t_tr->Sumw2();
		h_t_sm = new TH1D("", ";t", Nbins, binEdges); h_t_sm->Sumw2();
	}

	void Write() const
	{
		h_t_tr->Scale(1., "width");
		h_t_tr->SetLineColor(4);
		h_t_tr->Write("h_t_tr");

		h_t_sm->Scale(1., "width");
		h_t_sm->SetLineColor(2);
		h_t_sm->Write("h_t_sm");

		TH1D *h_corr = new TH1D(*h_t_tr);
		for (int bi = 1; bi <= h_corr->GetNbinsX(); ++bi)
		{
			double v_tr = h_t_tr->GetBinContent(bi);
			double u_tr = h_t_tr->GetBinError(bi);

			double v_sm = h_t_sm->GetBinContent(bi);
			double u_sm = h_t_sm->GetBinError(bi);

			double corr = (v_sm > 0.) ? v_tr / v_sm : 0.;
			double corr_u = (v_sm > 0. && v_tr > 0.) ? corr * sqrt( pow(u_tr/v_tr, 2.) + pow(u_sm/v_sm, 2.) ) : 0.;

			h_corr->SetBinContent(bi, corr);
			h_corr->SetBinError(bi, corr_u);
		}

		h_corr->SetLineColor(8);
		h_corr->Write("h_corr");
	}
};

//----------------------------------------------------------------------------------------------------
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

	unsigned int N_ev = (unsigned int) 1E7;
	unsigned int seed = 1;


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
	if (cfg.diagonal == dCombined)
		return rcIncompatibleDiagonal;

	// print info
	PrintConfiguration();

	// apply settings
	gRandom->SetSeed(seed);
	
	// binnings
	vector<string> binnings = anal.binnings;

	// models
	string base_dir = getenv("BASE_DIR");
	vector<Model> models = {
		{ "fit-1", base_dir + "/fits_for_corr/fit.root", "g_fit_1" },
	};

	printf("\n");
	printf("N_ev = %u = %.1E\n", N_ev, (double) N_ev);
	printf("seed = %u\n", seed);

	// initialize acceptance calculator
	AcceptanceCalculator accCalc;
	accCalc.Init(cfg.th_y_sign, anal);

	// load input
	vector<TSpline *> modelSplines;
	for (const auto &model : models)
	{
		TFile *f_in = TFile::Open(model.file.c_str());
		TGraph *g_dsdt = (TGraph *) f_in->Get(model.object.c_str());

		modelSplines.push_back(BuildSpline(g_dsdt));
	}

	// book histograms
	vector<vector<PlotGroup>> plots;
	for (unsigned int mi = 0; mi < models.size(); ++mi)
	{
		vector<PlotGroup> bv;

		for (unsigned int bi = 0; bi < binnings.size(); ++bi)
		{
			unsigned int N_bins;
			double *bin_edges;
			BuildBinning(anal, binnings[bi], bin_edges, N_bins);

			PlotGroup pg;
			pg.Init(N_bins, bin_edges);

			bv.push_back(pg);
		}

		plots.push_back(bv);
	}

	// prepare output
	string fn_out = string("unfolding_cf_mc_") + cfg.diagonal_str + ".root";
	TFile *f_out = new TFile(fn_out.c_str(), "recreate");
	if (f_out->IsZombie())
	{
		printf("ERROR: can't open file `%s' for writing.\n", fn_out.c_str());
		return 3;
	}

	// generator distribution
	double v1 = 100., m1 = 0.005;
	double v2 = 10., m2 = 0.02;
	double v3 = 1., m3 = anal.t_max_full;

	double norm = v1 * (m1-0.) + v2 * (m2-m1) + v3 * (m3-m2);

	double f1 = v1 * (m1-0.) / norm;
	double f2 = f1 + v2 * (m2-m1) / norm;
	double f3 = 1.;

	// event loop
	for (unsigned int evi = 0; evi < N_ev; ++evi)
	{
		double k = gRandom->Rndm();
		double t = 0., sw = 0.;
		if (k <= f1) { t = k/f1 * m1; sw = v1; }
		if (f1 < k && k <= f2) { t = m1 + (k-f1)/(f2-f1)*(m2-m1); sw = v2; }
		if (f2 < k && k <= f3) { t = m2 + (k-f2)/(f3-f2)*(m3-m2); sw = v3; }

		// generate true event
		Kinematics k_tr;
		k_tr.t = t;
		k_tr.phi = gRandom->Rndm() * M_PI * cfg.th_y_sign;

		k_tr.TPhiToThetas(env);

		// simulate smearing
		Kinematics k_sm = k_tr;
		k_sm.th_x += gRandom->Gaus() * anal.si_th_x_2arm;
		k_sm.th_y += gRandom->Gaus() * anal.si_th_y_2arm;

		k_sm.ThetasToTPhi(env);

		// phi acceptance correction
		bool skip = accCalc.PhiComponentCut(k_sm.th_x, k_sm.th_y);
		double phiFactor = 0.;
		if (!skip)
			phiFactor = accCalc.PhiFactor(k_sm.th) / 2.;

		// calculate event weights
		vector<double> ws;
		for (const auto spline : modelSplines)
			ws.push_back(spline->Eval(k_tr.t) / sw);

		// fill true event
		for (unsigned int mi = 0; mi < models.size(); ++mi)
		{
			for (unsigned int bi = 0; bi < binnings.size(); ++bi)
			{
				plots[mi][bi].h_t_tr->Fill(k_tr.t, ws[mi]);

				if (!skip)
					plots[mi][bi].h_t_sm->Fill(k_sm.t, ws[mi] * phiFactor);
			}
		}
	}

	// save plots
	for (unsigned int mi = 0; mi < models.size(); ++mi)
	{
		TDirectory *d_model = f_out->mkdir(models[mi].label.c_str());

		for (unsigned int bi = 0; bi < binnings.size(); ++bi)
		{
			TDirectory *d_binning = d_model->mkdir(binnings[bi].c_str());
			gDirectory = d_binning;

			plots[mi][bi].Write();
		}
	}

	// clean up
	delete f_out;

	return 0;
}
