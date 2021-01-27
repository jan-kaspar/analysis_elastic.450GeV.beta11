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

	vector<unsigned int> arms = {
		1
	};

	TFile *f_out = TFile::Open("single_arm.root", "recreate");

	for (const auto &arm : arms)
	{
		char buf[100];
		sprintf(buf, "arm %i", arm);
		TDirectory *d_arm = f_out->mkdir(buf);
		gDirectory = d_arm;

		const auto &aes = elements[arm];

		TH2D *h2_th_y_vs_th_x_all = new TH2D("h2_th_y_vs_th_x_all", ";th_x;th_y", 400, -800E-6, +800E-6, 400, -800E-6, +800E-6);
		TH2D *h2_th_y_vs_th_x_acc_vtx_neg = new TH2D("h2_th_y_vs_th_x_acc_vtx_neg", ";th_x;th_y", 400, -800E-6, +800E-6, 400, -800E-6, +800E-6);
		TH2D *h2_th_y_vs_th_x_acc_vtx_inc = new TH2D("h2_th_y_vs_th_x_acc_vtx_inc", ";th_x;th_y", 400, -800E-6, +800E-6, 400, -800E-6, +800E-6);

		TH1D *h_t_all = new TH1D("h_t_all", "t", 100, 0., 0.10);
		TH1D *h_t_acc_vtx_neg = new TH1D("h_t_acc_vtx_neg", ";t", 100, 0., 0.10);
		TH1D *h_t_acc_vtx_inc = new TH1D("h_t_acc_vtx_inc", ";t", 100, 0., 0.10);

		struct ApertureStat
		{
			unsigned int count = 0;
			double th_x_min = 9999E-6, th_y_min = 9999E-6;

			void Add(double th_x, double th_y)
			{
				count++;

				if (fabs(th_y) < 10E-6)
					th_x_min = min(th_x_min, fabs(th_x));

				if (fabs(th_x) < 100E-6)
					th_y_min = min(th_y_min, fabs(th_y));
			}
		};

		map<unsigned int, ApertureStat> apertureStat;

		unsigned int n_ev = (unsigned int) 1E7;

		for (unsigned int evi = 0; evi < n_ev; ++evi)
		{
			const double th_x = -2000E-6 + 4000E-6 * gRandom->Rndm();
			const double th_y = -2000E-6 + 4000E-6 * gRandom->Rndm();

			const double t = env.p*env.p * (th_x*th_x + th_y*th_y);

			const double vtx_x = gRandom->Gaus() * env.si_vtx_x * 1E-3; // conversion to m
			const double vtx_y = gRandom->Gaus() * env.si_vtx_y * 1E-3; // conversion to m

			const auto &apIndices_vtx_inc = CheckApertures(aes, th_x, th_y, vtx_x, vtx_y);
			const auto &apIndices_vtx_neg = CheckApertures(aes, th_x, th_y, 0., 0.);

			h2_th_y_vs_th_x_all->Fill(th_x, th_y);
			h_t_all->Fill(t);

			for (const auto &idx : apIndices_vtx_inc)
				apertureStat[idx].Add(th_x, th_y);

			if (apIndices_vtx_inc.empty())
			{
				h2_th_y_vs_th_x_acc_vtx_inc->Fill(th_x, th_y);
				h_t_acc_vtx_inc->Fill(t);
			}

			if (apIndices_vtx_neg.empty())
			{
				h2_th_y_vs_th_x_acc_vtx_neg->Fill(th_x, th_y);
				h_t_acc_vtx_neg->Fill(t);
			}
		}

		for (const auto &p : apertureStat)
			printf("%2i (%20s) --> %4.1f %%, min th_x = %6.1f, min th_y = %6.1f urad\n", p.first, aes[p.first].name.c_str(), double(p.second.count) / n_ev * 100,
				p.second.th_x_min * 1E6, p.second.th_y_min * 1E6);

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
	}

	delete f_out;

	return 0;
}
