#include "classes/common_init.hh"
#include "classes/command_line_tools.hh"
#include "classes/numerical_integration.hh"
#include "classes/common_algorithms.hh"

#include "TGraph.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TSpline.h"

#include <string>
#include <map>


using namespace std;

//----------------------------------------------------------------------------------------------------

unsigned long int_ws_phi_size, int_ws_MX_size, int_ws_MY_size;
gsl_integration_workspace *int_ws_phi, *int_ws_MX, *int_ws_MY;

//----------------------------------------------------------------------------------------------------

double n_sm_si;
double si_m_x, si_m_y;

//----------------------------------------------------------------------------------------------------

TSpline* BuildSpline(TGraph *g)
{
	TSpline3 *s = new TSpline3("", g->GetX(), g->GetY(), g->GetN());
	s->SetName(g->GetName());
	return s;
}

//----------------------------------------------------------------------------------------------------

double dist_m_x(double m)
{
	const double &si = si_m_x;
	return exp( - (m * m) / 2. / si / si ) / sqrt(2. * M_PI ) / si;
}

//----------------------------------------------------------------------------------------------------

double dist_m_y(double m_y)
{
	const double &si = si_m_y;
	return exp( - (m_y * m_y) / 2. / si / si ) / sqrt(2. * M_PI ) / si;
}

//----------------------------------------------------------------------------------------------------

TSpline *dist_t_true_spline = nullptr;

// true distribution of t
double dist_t_true(double t)
{
	return dist_t_true_spline->Eval(t);
}

//----------------------------------------------------------------------------------------------------

// true distribution of th_x, th_y
double dist_th_x_th_y_true(double th_x, double th_y)
{
	double t = (th_x*th_x + th_y*th_y) * env.p*env.p;
	return dist_t_true(t);
}

//----------------------------------------------------------------------------------------------------

double IntegOverMX(double x, double *p, const void *)
{

	double th_x = p[0];
	double th_y = p[1];
	double m_x = x;
	double m_y = p[2];

	return dist_th_x_th_y_true(th_x - m_x, th_y - m_y) * dist_m_x(m_x);
}

//----------------------------------------------------------------------------------------------------

double IntegOverMY(double x, double *p, const void *)
{
	double m_y = x;
	double param[] = { p[0], p[1], m_y };

	double range_x = n_sm_si * si_m_x;
	double precision = 1E-2;
	double imx = RealIntegrate(IntegOverMX, param, nullptr, -range_x, +range_x, 0., precision,
		int_ws_MX_size, int_ws_MX, "IntegOverMY");

	return imx * dist_m_y(m_y);
}

//----------------------------------------------------------------------------------------------------

double dist_th_x_th_y_smea(double th_x, double th_y)
{
	double param[] = { th_x, th_y };
	double range = n_sm_si * si_m_y;
	double precision = 1E-2;
	return RealIntegrate(IntegOverMY, param, nullptr, -range, +range, 0., precision,
			int_ws_MY_size, int_ws_MY, "dist_th_x_th_y_sm");
}

//----------------------------------------------------------------------------------------------------

double IntegOverPhi(double x, double *p, const void *)
{
	double th = p[0];
	double phi = x;

	double th_x = th * cos(phi);
	double th_y = th * sin(phi);

	return dist_th_x_th_y_smea(th_x, th_y);
}

//----------------------------------------------------------------------------------------------------

double dist_t_sm(double t)
{
	double th = sqrt(t) / env.p;

	// calculate integrals over phi segments
	double param[] = { th };
	const double rel_precision = 1E-3;

	double phiSum = 0.;
	double integralSum = 0.;

	for (const auto &segment : anal.fc_G.GetIntersectionPhis(th))
	{
		const double phi_start = segment.x;
		const double phi_end = segment.y;

		phiSum += phi_end - phi_start;

		if (cfg.th_y_sign == +1)
			integralSum += RealIntegrate(IntegOverPhi, param, nullptr, phi_start, phi_end, 0., rel_precision, int_ws_phi_size, int_ws_phi, "dist_reco_t");
		else
			integralSum += RealIntegrate(IntegOverPhi, param, nullptr, -phi_end, -phi_start, 0., rel_precision, int_ws_phi_size, int_ws_phi, "dist_reco_t");
	}

	return (phiSum > 0.) ? integralSum / phiSum : 0.;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void MakeGraphs()
{
	TGraph *g_t_tr = new TGraph(); g_t_tr->SetName("g_t_tr"); g_t_tr->SetLineColor(1);
	TGraph *g_t_sm = new TGraph(); g_t_sm->SetName("g_t_sm"); g_t_sm->SetLineColor(2);
	TGraph *g_t_corr = new TGraph(); g_t_corr->SetName("g_t_corr"); g_t_sm->SetLineColor(4);

	for (double t = 1E-4; t <= 0.05; )
	{
		// evaluate distributions
		const double d_t_true = dist_t_true(t);
		const double d_t_sm = dist_t_sm(t);
		const double corr = (d_t_sm > 0) ? d_t_true / d_t_sm : 0.;

		// fill plots
		int idx = g_t_tr->GetN();
		g_t_tr->SetPoint(idx, t, d_t_true);
		g_t_sm->SetPoint(idx, t, d_t_sm);

		g_t_corr->SetPoint(idx, t, corr);

		// advance t
		double dt = 1E-4;
		if (t < 1E-3)
			dt = 2E-6;

		t += dt;
	}

	g_t_tr->Write();
	g_t_sm->Write();
	g_t_corr->Write();
}

//----------------------------------------------------------------------------------------------------

void MakeCorrectionHistograms(const vector<TH1D *> binning_hists)
{
	for (const TH1D *binning_hist : binning_hists)
	{
		TH1D *h_corr = new TH1D(*binning_hist);

		for (int bin = 1; bin <= h_corr->GetNbinsX(); bin++)
		{
			double l = h_corr->GetBinLowEdge(bin);
			double w = h_corr->GetBinWidth(bin);
			//double r = l + w;
	
			//printf("bin = %3u, l = %.5f, r = %.5f\n", bin, l, r);

			double v_corr;

			if (l+w < 3E-4 || l > 0.05)
			{
				v_corr = 0.;
			} else {
				// integrate distributions over the bin
				double i_tr = 0., i_sm = 0.;
				unsigned int N_div = 10;
				for (unsigned int di = 0; di < N_div; di++)
				{
					double t = l + w/N_div * (di + 0.5);
					i_tr += dist_t_true(t);
					i_sm += dist_t_sm(t);
				}
			
				v_corr = (i_sm > 0.) ? i_tr / i_sm : 0.;
			}
	
			//printf("\t1-corr = %.3E | %.3E\n", 1.-corr, 1.-g_unsm_corr->Eval((l+r)/2));
	
			h_corr->SetBinContent(bin, v_corr);
		}

		h_corr->Write();
	}
}

//----------------------------------------------------------------------------------------------------

void ProcessOne(const vector<TH1D *> binning_hists)
{
	MakeGraphs();

	MakeCorrectionHistograms(binning_hists);
}

//----------------------------------------------------------------------------------------------------

struct Model
{
	string label;
	string file;
	string object;
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
	
	// binnings
	vector<string> binnings = anal.binnings;

	// models
	string base_dir = getenv("BASE_DIR");
	vector<Model> models = {
		{ "fit-1", base_dir + "/fits_for_corr/fit.root", "g_fit_1" },
	};

	// smearing sigmas
	n_sm_si = 5;
	si_m_x = anal.si_th_x_2arm;
	si_m_y = anal.si_th_y_2arm;

	// build binnings
	vector<TH1D *> binning_hists;
	for (const auto &binning : binnings)
	{
		unsigned int N_bins;
		double *bin_edges;
		BuildBinning(anal, binning, bin_edges, N_bins);

		TH1D *h = new TH1D("", "", N_bins, bin_edges);
		h->SetName(binning.c_str());
		binning_hists.push_back(h);
	}

	// prepare output
	string fn_out = string("unfolding_cf_ni_") + cfg.diagonal_str + ".root";
	TFile *f_out = new TFile(fn_out.c_str(), "recreate");
	if (f_out->IsZombie())
	{
		printf("ERROR: can't open file `%s' for writing.\n", fn_out.c_str());
		return 3;
	}
	
	// initialise integration workspaces
	int_ws_phi_size = 100;
	int_ws_phi = gsl_integration_workspace_alloc(int_ws_phi_size);

	int_ws_MX_size = 100;
	int_ws_MX = gsl_integration_workspace_alloc(int_ws_MX_size);

	int_ws_MY_size = 100;
	int_ws_MY = gsl_integration_workspace_alloc(int_ws_MY_size);

	// loop over data fits
	for (const auto &model : models)
	{
		TDirectory *d_model = f_out->mkdir(model.label.c_str());

		// get input graph
		TFile *f_in = TFile::Open(model.file.c_str());
		TGraph *g_dsdt = (TGraph *) f_in->Get(model.object.c_str());

		// build spline
		dist_t_true_spline = BuildSpline(g_dsdt);

		// calculate correction
		gDirectory = d_model;

		ProcessOne(binning_hists);

		// clean up
		delete dist_t_true_spline;
		dist_t_true_spline = nullptr;

		delete f_in;
	}
		
	delete f_out;

	return 0;
}