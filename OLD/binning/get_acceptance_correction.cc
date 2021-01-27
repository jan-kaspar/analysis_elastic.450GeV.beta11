#include "TFile.h"
#include "TProfile.h"

int main()
{
	TFile *f_in = TFile::Open("../DS-fill7302/Totem1/distributions_45b_56t.root");
	TProfile *acc_corr = (TProfile *) f_in->Get("acceptance correction/eb/p_t_full_corr");

	TFile *f_out = TFile::Open("acceptance_correction.root", "recreate");
	acc_corr->Write();

	delete f_out;
	delete f_in;
}
