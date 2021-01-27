#include <TDirectory.h>
#include "classes/common_init.hh"
#include "classes/command_line_tools.hh"

#include "TFile.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TGraph.h"

#include "Fit/Fitter.h"

#include <memory>
#include <cstdio>
#include <vector>
#include <memory>

using namespace std;

//----------------------------------------------------------------------------------------------------

struct Point
{
	double x, y, v, e;
};

vector<Point> points;

//----------------------------------------------------------------------------------------------------

class S2_FCN
{
	public:
		S2_FCN() {}

		double operator() (const double *par) const;

		static double f(double x, double y, const double* par);

		static void InitFit(ROOT::Fit::FitConfig &, double v_min, double v_max);

		static void PrintResults(const double *params);

		static TGraph* GetResultGraph(const ROOT::Fit::FitResult &);

		TH2D* MakeDiffMap(const TH2D *orig, const double* par);
};

//----------------------------------------------------------------------------------------------------

double S2_FCN::f(double x, double y, const double* par)
{
	const double &mu_x = par[0];
	const double &mu_y = par[1];

	const double &A_1 = par[2];
	const double &si_1_x = par[3];
	const double &si_1_y = par[4];

	const double &A_2 = par[5];
	const double &si_2_x = par[6];
	const double &si_2_y = par[7];
		
	const double de_1_x = (x - mu_x) / si_1_x;
	const double de_1_y = (y - mu_y) / si_1_y;

	const double de_2_x = (x - mu_x) / si_2_x;
	const double de_2_y = (y - mu_y) / si_2_y;

	return
		A_1 * exp(- (de_1_x*de_1_x + de_1_y*de_1_y) / 2.)
		+ A_2 * exp(- (de_2_x*de_2_x + de_2_y*de_2_y) / 2.);
}

//----------------------------------------------------------------------------------------------------

void S2_FCN::InitFit(ROOT::Fit::FitConfig &cfg, double v_min, double v_max)
{
	cfg.ParSettings(0).Set("mu_x", 0E-6, 1E-6);	// in rad
	cfg.ParSettings(1).Set("mu_y", 0E-6, 1E-6);

	cfg.ParSettings(2).Set("A_1", v_min, v_min/10);
	cfg.ParSettings(3).Set("si_1_x", 20E-6, 1E-6);
	cfg.ParSettings(4).Set("si_1_y", 20E-6, 1E-6);

	cfg.ParSettings(5).Set("A_2", v_max*10, v_max*10/10);
	cfg.ParSettings(6).Set("si_2_x", 5E-6, 1E-6);
	cfg.ParSettings(7).Set("si_2_y", 5E-6, 1E-6);
}

//----------------------------------------------------------------------------------------------------

void S2_FCN::PrintResults(const double *params)
{
	printf("mu_x = %.1f urad\n", params[0]*1E6);
	printf("mu_y = %.1f urad\n", params[1]*1E6);

	printf("A_1 = %.3E\n", params[2]);
	printf("si_1_x = %.1f urad\n", params[3]*1E6);
	printf("si_1_y = %.1f urad\n", params[4]*1E6);

	printf("A_2 = %.3E\n", params[5]);
	printf("si_2_x = %.1f urad\n", params[6]*1E6);
	printf("si_2_y = %.1f urad\n", params[7]*1E6);
}

//----------------------------------------------------------------------------------------------------

TGraph* S2_FCN::GetResultGraph(const ROOT::Fit::FitResult &r)
{
	TGraph *g = new TGraph();

	for (int i = 0; i < 8; ++i)
	{
		g->SetPoint(i, r.GetParams()[i], r.GetErrors()[i]);
	}

	return g;
}

//----------------------------------------------------------------------------------------------------

double S2_FCN::operator() (const double* par) const
{
	//printf("--------------------------------------------------\n");

	double S2 = 0.;

	for (unsigned int i = 0; i < points.size(); i++)
	{
		const Point &p = points[i];

		double de_f = (p.v - f(p.x, p.y, par)) / p.e;

		S2 += de_f * de_f;
	}

	//printf("S2 = %E\n", S2);

	return S2;
}

//----------------------------------------------------------------------------------------------------

TH2D* S2_FCN::MakeDiffMap(const TH2D *orig, const double* par)
{
	printf(">> MakeDiffMap\n");

	/*
	double bs_x = orig->GetXaxis()->GetBinWidth(1);
	double bs_y = orig->GetYaxis()->GetBinWidth(1);

	double lim_x = 110E-6;	int N_x = ceil(lim_x / bs_x);	lim_x = bs_x * N_x;
	double lim_y = 110E-6;	int N_y = ceil(lim_y / bs_y);	lim_y = bs_y * N_y;

	printf("N_x = %u, lim_x = %E\n", N_x, lim_x);
	printf("N_y = %u, lim_y = %E\n", N_y, lim_y);

	TH2D *h = new TH2D("diff map", "", 2*N_x, -lim_x, +lim_x, 2*N_y, -lim_y, +lim_y);
	*/

	TH2D *h = new TH2D(*orig);
	h->SetName("diff map");
	h->Reset();

	for (unsigned int i = 0; i < points.size(); i++)
	{
		const Point &p = points[i];

		const int bi_x = h->GetXaxis()->FindBin(p.x);
		const int bi_y = h->GetXaxis()->FindBin(p.y);

		const double de_f = (p.v - f(p.x, p.y, par)) / p.e;

		h->SetBinContent(bi_x, bi_y, fabs(de_f));
	}

	return h;
}

//----------------------------------------------------------------------------------------------------

TGraph* GetFitProjectionY(const double *params, const vector<double> &th_x_bin_centers)
{
	TGraph *g = new TGraph();

	for (double th_y = -200E-6; th_y <= +200E-6; th_y += 5E-6)
	{
		double sum = 0.;
		for (const auto &th_x : th_x_bin_centers)
			sum += S2_FCN::f(th_x, th_y, params);

		int idx = g->GetN();
		g->SetPoint(idx, th_y, sum);
	}

	return g;
}

//----------------------------------------------------------------------------------------------------

void MakeFit(TH2D *h_45b, TH2D *h_45t)
{
	printf(">> MakeFit\n");
	printf("%p, %p\n", h_45b, h_45t);

	// select points
	vector<TH2D *> hists;
	hists.push_back(h_45b);
	hists.push_back(h_45t);

	double v_min = 1E100;
	double v_max = -1;

	for (unsigned int hi = 0; hi < hists.size(); hi++)
	{
		TH2D *h = hists[hi];
		for (int xi = 1; xi <= h->GetNbinsX(); xi++)
		{
			for (int yi = 1; yi <= h->GetNbinsY(); yi++)
			{
				Point p;
				p.x = h->GetXaxis()->GetBinCenter(xi);
				p.y = h->GetYaxis()->GetBinCenter(yi);
				p.v = h->GetBinContent(xi, yi);
				p.e = h->GetBinError(xi, yi);

				if (p.v > 0)
					v_min = min(p.v, v_min);
				v_max = max(p.v, v_max);

				// exclude empty bins
				if (p.v == 0. && p.e == 0.)
					continue;

				// NB: points outside the fiducial cuts already removed in distributions.cc

				points.push_back(p);

				//printf("th_x = %.0f, th_y = %.0f, v = %.3E, e = %.3E\n", p.x*1E6, p.y*1E6, p.v, p.e);
			}
		}
	}

	printf("number of points: %lu\n", points.size());

	// initialize fitter
	unique_ptr<S2_FCN> s2_fcn(new S2_FCN);
	unique_ptr<ROOT::Fit::Fitter> fitter(new ROOT::Fit::Fitter);

	constexpr unsigned int n_params = 8;
	double pStart[] = {0, 0, 0, 0, 0, 0, 0, 0};
	fitter->SetFCN(n_params, *s2_fcn, pStart, 0, true);

	// set initial parameters
	S2_FCN::InitFit(fitter->Config(), v_min, v_max);

	// run fit
	fitter->FitFCN();
	fitter->FitFCN();

	// get parameters
	const ROOT::Fit::FitResult &result = fitter->Result();
	const double *params = result.GetParams();

	// print results
	printf("\n");

	const double chiSq = (*s2_fcn)(params);
	const unsigned int ndf = points.size() - n_params;
	printf("chiSq = %E, ndf = %i, chiSq/ndf = %.3f\n", chiSq, ndf, chiSq/ndf);

	S2_FCN::PrintResults(params);

	printf("\n");

	// save results
	TGraph *g_results = S2_FCN::GetResultGraph(result);
	g_results->Write("g_results");

	// make checks
	TH2D *m = s2_fcn->MakeDiffMap(h_45b, params);
	m->Write();

	TH2D *h2_th_y_vs_thx_input = new TH2D(*h_45b);
	h2_th_y_vs_thx_input->Reset();
	for (const auto &p : points)
	{
		const auto &bi = h2_th_y_vs_thx_input->FindBin(p.x, p.y);
		h2_th_y_vs_thx_input->SetBinContent(bi, p.v);
		h2_th_y_vs_thx_input->SetBinError(bi, p.e);
	}
	h2_th_y_vs_thx_input->Write("h2_th_y_vs_thx_input");

	gDirectory = gDirectory->mkdir("th_x slices");

	vector<pair<double, double>> th_x_slices = {
		{ -200E-6, -150E-6 },
		{ -150E-6, -100E-6 },
		{ -100E-6, -75E-6 },
		{ -75E-6, -50E-6 },
		{ -50E-6, -25E-6 },
		{ -25E-6, -0E-6 },
		{ 0E-6, 25E-6 },
		{ 25E-6, 50E-6 },
		{ 50E-6, 75E-6 },
		{ 75E-6, 100E-6 },
		{ 100E-6, 150E-6 },
		{ 150E-6, 200E-6 },
	};

	for (const auto &slice : th_x_slices)
	{
		const auto &th_x_min = slice.first;
		const auto &th_x_max = slice.second;

		const auto bi_min = h2_th_y_vs_thx_input->GetXaxis()->FindBin(th_x_min);
		const auto bi_max = h2_th_y_vs_thx_input->GetXaxis()->FindBin(th_x_max);

		vector<double> th_x_bin_centers;
		for (int bi = bi_min; bi <= bi_max; ++bi)
			th_x_bin_centers.push_back(h2_th_y_vs_thx_input->GetXaxis()->GetBinCenter(bi));

		TH1D *h_th_y = h2_th_y_vs_thx_input->ProjectionY("proj", bi_min, bi_max);
		h_th_y->SetLineColor(4);

		char buf[100];
		sprintf(buf, "%+.1f_%+.1f", th_x_min * 1E6, th_x_max * 1E6);

		TCanvas *c = new TCanvas;
		c->SetName(buf);

		h_th_y->Draw();

		TGraph *g = GetFitProjectionY(params, th_x_bin_centers);
		g->SetName("fit");
		g->SetLineColor(2);
		g->Draw("l");

		c->Write();
	}
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
	if (cfg.diagonal != dCombined)
		return rcIncompatibleDiagonal;

	// print settings
	PrintConfiguration();

	// get input data
	TFile *f_in_45b = new TFile("distributions_45b_56t.root");
	TH2D *h_45b = (TH2D *) f_in_45b->Get("normalization/h2_th_y_vs_th_x_normalized");
	
	TFile *f_in_45t = new TFile("distributions_45t_56b.root");
	TH2D *h_45t = (TH2D *) f_in_45t->Get("normalization/h2_th_y_vs_th_x_normalized");
	
	// prepare output
	TFile *f_out = new TFile("alignment_final.root", "recreate");

	// run fit
	MakeFit(h_45b, h_45t);

	// clean up
	delete f_out;

	return 0;
}
