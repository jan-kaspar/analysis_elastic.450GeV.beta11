#include "TFile.h"
#include "TProfile.h"
#include "TGraph.h"
#include "TF1.h"

#include <cmath>

//----------------------------------------------------------------------------------------------------

TGraph* MakeFixStatUncBinSizeGraph(TProfile *p_acc_corr, TF1 *ff_dNdt, double u, const char *name)
{
	TGraph *g_bs = new TGraph();
	g_bs->SetName(name);
	g_bs->SetTitle(";t;bin size");

	for (int i = 0; i < p_acc_corr->GetNbinsX(); i++)
	{
		double t = p_acc_corr->GetBinCenter(i);
		double acc_corr = p_acc_corr->GetBinContent(i);

		if (acc_corr == 0)
			continue;

		const double A = 1./acc_corr;

		const double dNdt = ff_dNdt->Eval(t);
		const double bin_size = 1. / (u*u * dNdt * A);

		g_bs->SetPoint(g_bs->GetN(), t, bin_size);

		//printf("%.3f, dsdt=%E, A=%E, bin_size=%E\n", t, dsdt, A, bin_size);
	}

	return g_bs;
}

//----------------------------------------------------------------------------------------------------

int main()
{
	// get input
	TFile *f_acc_corr = new TFile("acceptance_correction.root");
	TProfile *p_acc_corr = (TProfile *) f_acc_corr->Get("p_t_full_corr");

	TFile *f_dNdt = new TFile("dN_dt_fit.root");
	TF1 *ff_dNdt = (TF1 *) f_dNdt->Get("ff");
	
	TFile *f_simu = new TFile("simu.root");
	TGraph *g_rms_t = (TGraph *) f_simu->Get("g_RMS_de_t_vs_t");

	// prepare output
	TFile *f_out = new TFile("generators.root", "recreate");

	g_rms_t->SetName("g_rms_t");
	g_rms_t->Write();

	MakeFixStatUncBinSizeGraph(p_acc_corr, ff_dNdt, 0.005, "g_bs_stat_unc_0.005")->Write();
	MakeFixStatUncBinSizeGraph(p_acc_corr, ff_dNdt, 0.010, "g_bs_stat_unc_0.010")->Write();
	MakeFixStatUncBinSizeGraph(p_acc_corr, ff_dNdt, 0.020, "g_bs_stat_unc_0.020")->Write();

	delete f_out;

	return 0;
}
