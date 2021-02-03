#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TGraph.h"
#include "TCanvas.h"

int main()
{
	TFile *f_in = new TFile("../../4rp-before-new-framework/DS-merged/merged.root");
	TH1D *h_in = (TH1D *) f_in->Get("eb/merged/combined/h_dsdt");

	TF1 *ff = new TF1("ff", "[0]/x/x + [1] * exp(-[2]*x)", 1E-6, 0.10);

	TCanvas *c = new TCanvas();
	c->SetLogy(1);

	/*
	ff->SetParameter(0, 2.22E2);
	ff->SetParameter(1, 1.97E8);
	ff->SetParameter(2, 1.30E1);
	*/

	h_in->Fit(ff, "", "", 2E-4, 0.02);

	//printf("ff(0) = %f mb / GeV^2\n", ff->Eval(0.));

	TFile *f_out = new TFile("fit.root", "recreate");
	h_in->Write();
	ff->Write();

	TGraph *g_fit = new TGraph();

	for (double mt = 1E-7; mt < 0.05;)
	{
		const int idx = g_fit->GetN();
		const double dsdt = ff->Eval(mt);
		g_fit->SetPoint(idx, mt, dsdt);

		//printf("%u: %E, %E\n", idx, mt, dsdt);

		double dmt = 1E-4;
		if (mt < 1E-3)
			dmt = 2E-6;

		mt += dmt;
	}

	g_fit->Write("g_fit_1");

	delete f_out;

	return 0;
}
