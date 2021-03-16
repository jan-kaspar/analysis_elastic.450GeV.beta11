#include <TH1.h>
#include "TFile.h"
#include "TH1D.h"

using namespace std;

int main()
{
	vector<string> binnings = {
		"sb1",
		"sb2",
		"sb3",
	};

	TFile *f_in = new TFile("/afs/cern.ch/work/j/jkaspar/work/analyses/elastic/450GeV/beta100/4rp/data/fill7291/Totem1/distributions_45b_56t.root");

	TFile *f_out = new TFile("reference.root", "recreate");

	for (const auto &binning : binnings)
	{
		TH1D *h = (TH1D*) f_in->Get(("acceptance correction/" + binning + "/h_t_after").c_str());
		h->Reset();
		h->Write(binning.c_str());
	}

	delete f_in;
	delete f_out;

	return 0;
}
