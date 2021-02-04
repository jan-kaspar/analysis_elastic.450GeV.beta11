#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"

int main()
{
	TFile *f_in = new TFile("../../data/merged.root");
	TH1D *h_in = (TH1D *) f_in->Get("ub/merged/combined/h_dNdt");
	//TH1D *h_in = (TH1D *) f_in->Get("ub/merged/45t_56b/h_dsdt");

	TF1 *ff = new TF1("ff", "[0] * exp(-[1]*x)", 1E-6, 0.02);

	TCanvas *c = new TCanvas();
	c->SetLogy(1);

	h_in->Fit(ff, "", "", 0.015, 0.055);

	//printf("ff(0) = %f mb / GeV^2\n", ff->Eval(0.));

	TFile *f_out = new TFile("dN_dt_fit.root", "recreate");
	h_in->Write();
	ff->Write();

	delete f_out;

	return 0;
}
