#include "TFile.h"
#include "TProfile.h"

int main()
{
	TFile *f_in = TFile::Open("../../data/fill7302/Totem1/distributions_45b_56t.root");
	TProfile *acc_corr = (TProfile *) f_in->Get("acceptance correction/sb1/p_t_full_corr");

	TFile *f_out = TFile::Open("acceptance_correction.root", "recreate");
	acc_corr->Write();

	delete f_out;
	delete f_in;
}
