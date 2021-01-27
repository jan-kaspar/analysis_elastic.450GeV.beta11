#include "classes/common_init.hh"
#include "classes/common_algorithms.hh"
#include "classes/AcceptanceCalculator.hh"
#include "classes/Kinematics.hh"

#include "TFile.h"
#include "TGraph.h"

//----------------------------------------------------------------------------------------------------

AcceptanceCalculator accCalc;

//----------------------------------------------------------------------------------------------------

void RunOne()
{
	// run sampling
	vector<double> th_x_values = {
		0E-6,
		+100E-6,
		+200E-6,
		+300E-6,
	};

	for (const auto &th_x : th_x_values)
	{
		char buf[100];

		TGraph *g_with_app = new TGraph();
		sprintf(buf, "g_with_app_A_th_x_%+.1E", th_x);
		g_with_app->SetName(buf);
		g_with_app->SetTitle(";th_y");

		TGraph *g_without_app = new TGraph();
		sprintf(buf, "g_without_app_A_th_x_%+.1E", th_x);
		g_without_app->SetName(buf);
		g_without_app->SetTitle(";th_y");

		for (double th_y_abs = 20E-6; th_y_abs <= 80E-6; th_y_abs += 0.1E-6)
		{
			double th_y = cfg.th_y_sign * th_y_abs;

			Kinematics k;
			k.th_x = k.th_x_L = k.th_x_R = th_x;
			k.th_y = k.th_y_L = k.th_y_R = th_y;
			k.th = sqrt(k.th_x * k.th_x + k.th_y * k.th_y);

			{
				accCalc.gaussianOptimisation = true;

				double phi_corr, div_corr;
				bool skip = accCalc.Calculate(k, phi_corr, div_corr);
				double A = (skip) ? 0. : 1./div_corr;
	
				int idx = g_without_app->GetN();
				g_with_app->SetPoint(idx, th_y, A);
			}

			{
				accCalc.gaussianOptimisation = false;

				double phi_corr, div_corr;
				bool skip = accCalc.Calculate(k, phi_corr, div_corr);
				double A = (skip) ? 0. : 1./div_corr;
	
				int idx = g_without_app->GetN();
				g_without_app->SetPoint(idx, th_y, A);
			}
		}

		g_with_app->Write();
		g_without_app->Write();
	}

	TGraph *g_A_t_with_app = new TGraph();
	g_A_t_with_app->SetName("g_A_t_with_app");

	TGraph *g_A_t_without_app = new TGraph();
	g_A_t_without_app->SetName("g_A_t_without_app");

	for (double t = 0.; t <= 0.05; t += 0.0001)
	{
		Kinematics k;
		k.th = sqrt(t) / env.p;
		k.th_x_L = k.th_x_R = k.th_x = 0.;
		k.th_y_L = k.th_y_R = k.th_y = cfg.th_y_sign * k.th;

		accCalc.gaussianOptimisation = true;
		double phi_corr_with_app=0., div_corr_with_app=0.;
		bool skip_with_app = accCalc.Calculate(k, phi_corr_with_app, div_corr_with_app);
		double A_with_app = (skip_with_app) ? 0. : 1./phi_corr_with_app;
		g_A_t_with_app->SetPoint(g_A_t_with_app->GetN(), t, A_with_app);

		accCalc.gaussianOptimisation = false;
		double phi_corr_without_app=0., div_corr_without_app=0.;
		bool skip_without_app = accCalc.Calculate(k, phi_corr_without_app, div_corr_without_app);
		double A_without_app = (skip_without_app) ? 0. : 1./phi_corr_without_app;
		g_A_t_without_app->SetPoint(g_A_t_without_app->GetN(), t, A_without_app);
	}

	g_A_t_with_app->Write();
	g_A_t_without_app->Write();
}

//----------------------------------------------------------------------------------------------------

int main()
{
	// defaults
	string cfg_file = "config.py";

	// prepare output
	TFile *f_out = TFile::Open("validation_gaussian_approx.root", "recreate");

	// run for each diagonal
	for (string diagonal : { "45b_56t", "45t_56b" })
	{
		if (Init(cfg_file, diagonal) != 0)
			return 2;

		printf("* %s\n", diagonal.c_str());

		// initialise acceptance calculation
		accCalc.Init(cfg.th_y_sign, anal);
	
		// make subdirectory
		gDirectory = f_out->mkdir(diagonal.c_str());

		// run validation
		RunOne();
	}

	// cleaning
	delete f_out;

	return 0;
}
