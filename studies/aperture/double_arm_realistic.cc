#include "classes/common_init.hh"
#include "classes/AcceptanceCalculator.hh"
#include "classes/Kinematics.hh"

#include "apertures.hh"
#include "aperture_list.hh"

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TRandom3.h"
#include "TGraph.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

TGraph* PlotFiductialCut(const FiducialCut &fc, double th_y_sign)
{
	TGraph *g = new TGraph();

	for (const auto &p : fc.points)
		g->SetPoint(g->GetN(), p.x, p.y * th_y_sign);

	if (!fc.points.empty())
		g->SetPoint(g->GetN(), fc.points[0].x, fc.points[0].y * th_y_sign);

	return g;
}

//----------------------------------------------------------------------------------------------------

int main()
{
	// defaults
	string cfg_file = "config.py";
	string diagonal_input = "45b_56t";

	// run initialisation
	if (Init(cfg_file, diagonal_input) != 0)
		return 2;

	InitElements();

	AcceptanceCalculator accCalc;
	accCalc.Init(cfg.th_y_sign, anal);
	accCalc.anal.si_th_x_LRdiff = sqrt(env.si_th_x_L*env.si_th_x_L + env.si_th_x_R*env.si_th_x_R); // force compatible settings
	accCalc.anal.si_th_y_LRdiff = sqrt(env.si_th_y_L*env.si_th_y_L + env.si_th_y_R*env.si_th_y_R);

	// print settings
	PrintConfiguration();

	printf("* accCalc.anal.si_th_x_LRdiff = %E\n", accCalc.anal.si_th_x_LRdiff);
	printf("* accCalc.anal.si_th_y_LRdiff = %E\n", accCalc.anal.si_th_y_LRdiff);

	gRandom->SetSeed(1);

	// prepare output
	TFile *f_out = TFile::Open("double_arm_realistic.root", "recreate");

	TH2D *h2_th_y_vs_th_x_all = new TH2D("h2_th_y_vs_th_x_all", ";th_x;th_y", 400, -500E-6, +500E-6, 400, -200E-6, +200E-6);
	TH2D *h2_th_y_vs_th_x_acc_vtx_neg = new TH2D("h2_th_y_vs_th_x_acc_vtx_neg", ";th_x;th_y", 400, -500E-6, +500E-6, 400, -200E-6, +200E-6);
	TH2D *h2_th_y_vs_th_x_acc_vtx_inc = new TH2D("h2_th_y_vs_th_x_acc_vtx_inc", ";th_x;th_y", 400, -500E-6, +500E-6, 400, -200E-6, +200E-6);

	TH1D *h_t_all = new TH1D("h_t_all", "t", 100, 0., 0.05);
	TH1D *h_t_acc_vtx_neg = new TH1D("h_t_acc_vtx_neg", ";t", 100, 0., 0.05);
	TH1D *h_t_acc_vtx_inc = new TH1D("h_t_acc_vtx_inc", ";t", 100, 0., 0.05);

	TH1D *h_t_acc_vtx_neg_std_corr = new TH1D("h_t_acc_vtx_neg_std_corr", ";t", 100, 0., 0.05);
	TH1D *h_t_acc_vtx_inc_std_corr = new TH1D("h_t_acc_vtx_inc_std_corr", ";t", 100, 0., 0.05);

	TH1D *h_th_x_LRdiff = new TH1D("h_th_x_LRdiff", ";th_x_R - th_x_L", 100, -50E-6, +50E-6);
	TH1D *h_th_y_LRdiff = new TH1D("h_th_y_LRdiff", ";th_y_R - th_y_L", 100, -50E-6, +50E-6);

	for (unsigned int evi = 0; evi < (unsigned int) 1E8; ++evi)
	{
		// elastic event
		const double th_x = -500E-6 + 1000E-6 * gRandom->Rndm();
		const double th_y = -500E-6 + 1000E-6 * gRandom->Rndm();

		const double t = env.p*env.p * (th_x*th_x + th_y*th_y);

		// beam divergence
		const double th_x_L = - th_x + gRandom->Gaus() * env.si_th_x_L;
		const double th_y_L = - th_y + gRandom->Gaus() * env.si_th_y_L;

		const double th_x_R = + th_x + gRandom->Gaus() * env.si_th_x_R;
		const double th_y_R = + th_y + gRandom->Gaus() * env.si_th_y_R;

		// vertex smearing
		const double vtx_x = gRandom->Gaus() * env.si_vtx_x * 1E-3; // conversion to m
		const double vtx_y = gRandom->Gaus() * env.si_vtx_y * 1E-3; // conversion to m

		// check whether in acceptance
		const bool inAcc_vtx_inc =
			CheckApertures(elements[0], th_x_L, th_y_L, vtx_x, vtx_y).empty() && CheckApertures(elements[1], th_x_R, th_y_R, vtx_x, vtx_y).empty();

		const bool inAcc_vtx_neg =
			CheckApertures(elements[0], th_x_L, th_y_L, 0., 0.).empty() && CheckApertures(elements[1], th_x_R, th_y_R, 0., 0.).empty();

		h2_th_y_vs_th_x_all->Fill(th_x, th_y);
		h_t_all->Fill(t);

		// calculate acceptance correction
		Kinematics k;
		k.th_x_L = - th_x_L;
		k.th_x_R = + th_x_R;
		k.th_x = (k.th_x_L + k.th_x_R) / 2.;

		k.th_y_L = - th_y_L;
		k.th_y_R = + th_y_R;
		k.th_y = (k.th_y_L + k.th_y_R) / 2.;

		k.th = sqrt(k.th_x*k.th_x + k.th_y*k.th_y);

		double phi_corr = 0., div_corr = 0.;
		const bool skip = accCalc.Calculate(k, phi_corr, div_corr);
		const double corr = div_corr * phi_corr;

		//if (!skip)
		//	printf("%.1f urad, %.1f urad --> phi_corr = %.3f, div_corr = %.3f\n", k.th_x_R*1E6, k.th_y_R*1E6, phi_corr, div_corr);

		// update plots

		if (inAcc_vtx_inc)
		{
			h2_th_y_vs_th_x_acc_vtx_inc->Fill(th_x, th_y);
			h_t_acc_vtx_inc->Fill(t);
			if (!skip)
				h_t_acc_vtx_inc_std_corr->Fill(t, corr);
		}

		if (inAcc_vtx_neg)
		{
			h2_th_y_vs_th_x_acc_vtx_neg->Fill(th_x, th_y);
			h_t_acc_vtx_neg->Fill(t);
			if (!skip)
				h_t_acc_vtx_neg_std_corr->Fill(t, corr);
		}

		h_th_x_LRdiff->Fill(k.th_x_R - k.th_x_L);
		h_th_y_LRdiff->Fill(k.th_y_R - k.th_y_L);
	}

	// save plots
	PlotFiductialCut(anal.fc_L, cfg.th_y_sign)->Write("fc_L");
	PlotFiductialCut(anal.fc_R, cfg.th_y_sign)->Write("fc_R");
	PlotFiductialCut(anal.fc_G, cfg.th_y_sign)->Write("fc_G");

	h2_th_y_vs_th_x_all->Write();
	h2_th_y_vs_th_x_acc_vtx_neg->Write();
	h2_th_y_vs_th_x_acc_vtx_inc->Write();

	TH2D *h2_th_y_vs_th_x_acc_vtx_neg_rat = new TH2D(*h2_th_y_vs_th_x_acc_vtx_neg);
	h2_th_y_vs_th_x_acc_vtx_neg_rat->Divide(h2_th_y_vs_th_x_all);
	h2_th_y_vs_th_x_acc_vtx_neg_rat->Write("h2_th_y_vs_th_x_acc_vtx_neg_rat");

	TH2D *h2_th_y_vs_th_x_acc_vtx_inc_rat = new TH2D(*h2_th_y_vs_th_x_acc_vtx_inc);
	h2_th_y_vs_th_x_acc_vtx_inc_rat->Divide(h2_th_y_vs_th_x_all);
	h2_th_y_vs_th_x_acc_vtx_inc_rat->Write("h2_th_y_vs_th_x_acc_vtx_inc_rat");

	h_t_all->Write();
	h_t_acc_vtx_neg->Write();
	h_t_acc_vtx_inc->Write();

	h_t_acc_vtx_neg_std_corr->Write();
	h_t_acc_vtx_inc_std_corr->Write();

	TH1D *h_t_acc_vtx_inc_over_neg = new TH1D(*h_t_acc_vtx_inc);
	h_t_acc_vtx_inc_over_neg->Divide(h_t_acc_vtx_neg);
	h_t_acc_vtx_inc_over_neg->Write("h_t_acc_vtx_inc_over_neg");

	TH1D *h_t_acc_vtx_neg_std_corr_over_all = new TH1D(*h_t_acc_vtx_neg_std_corr);
	h_t_acc_vtx_neg_std_corr_over_all->Divide(h_t_all);
	h_t_acc_vtx_neg_std_corr_over_all->Write("h_t_acc_vtx_neg_std_corr_over_all");

	TH1D *h_t_acc_vtx_inc_std_corr_over_all = new TH1D(*h_t_acc_vtx_inc_std_corr);
	h_t_acc_vtx_inc_std_corr_over_all->Divide(h_t_all);
	h_t_acc_vtx_inc_std_corr_over_all->Write("h_t_acc_vtx_inc_std_corr_over_all");

	h_th_x_LRdiff->Write();
	h_th_y_LRdiff->Write();

	delete f_out;

	return 0;
}
