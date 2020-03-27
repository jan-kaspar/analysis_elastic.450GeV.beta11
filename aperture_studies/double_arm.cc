#include "../common_definitions.h"

#include "apertures.h"
#include "aperture_list.h"

#include "TH2D.h"
#include "TRandom3.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

int main()
{
	Environment env;
	env.InitNominal();

	InitElements();

	gRandom->SetSeed(1);

	TFile *f_out = TFile::Open("double_arm.root", "recreate");

	TH2D *h2_th_y_vs_th_x_all = new TH2D("h2_th_y_vs_th_x_all", ";th_x;th_y", 400, -800E-6, +800E-6, 400, -800E-6, +800E-6);
	TH2D *h2_th_y_vs_th_x_acc_vtx_neg = new TH2D("h2_th_y_vs_th_x_acc_vtx_neg", ";th_x;th_y", 400, -800E-6, +800E-6, 400, -800E-6, +800E-6);
	TH2D *h2_th_y_vs_th_x_acc_vtx_inc = new TH2D("h2_th_y_vs_th_x_acc_vtx_inc", ";th_x;th_y", 400, -800E-6, +800E-6, 400, -800E-6, +800E-6);

	TH1D *h_t_all = new TH1D("h_t_all", "t", 100, 0., 0.10);
	TH1D *h_t_acc_vtx_neg = new TH1D("h_t_acc_vtx_neg", ";t", 100, 0., 0.10);
	TH1D *h_t_acc_vtx_inc = new TH1D("h_t_acc_vtx_inc", ";t", 100, 0., 0.10);

	for (unsigned int evi = 0; evi < 10000000; ++evi)
	{
		// elastic event
		const double th_x = -2000E-6 + 4000E-6 * gRandom->Rndm();
		const double th_y = -2000E-6 + 4000E-6 * gRandom->Rndm();

		const double t = env.p*env.p * (th_x*th_x + th_y*th_y);

		// beam divergence
		const double th_x_L = - th_x + gRandom->Rndm() * env.si_th_x_L;
		const double th_y_L = - th_y + gRandom->Rndm() * env.si_th_y_L;

		const double th_x_R = + th_x + gRandom->Rndm() * env.si_th_x_R;
		const double th_y_R = + th_y + gRandom->Rndm() * env.si_th_y_R;

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

		if (inAcc_vtx_inc)
		{
			h2_th_y_vs_th_x_acc_vtx_inc->Fill(th_x, th_y);
			h_t_acc_vtx_inc->Fill(t);
		}

		if (inAcc_vtx_neg)
		{
			h2_th_y_vs_th_x_acc_vtx_neg->Fill(th_x, th_y);
			h_t_acc_vtx_neg->Fill(t);
		}
	}

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

	TH1D *h_t_acc_vtx_inc_over_neg = new TH1D(*h_t_acc_vtx_inc);
	h_t_acc_vtx_inc_over_neg->Divide(h_t_acc_vtx_neg);
	h_t_acc_vtx_inc_over_neg->Write("h_t_acc_vtx_inc_over_neg");

	delete f_out;

	return 0;
}
