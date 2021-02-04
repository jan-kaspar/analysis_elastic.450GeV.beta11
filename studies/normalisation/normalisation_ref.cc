#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"

#include <string>
#include <vector>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------------------------------

TF1 *ff = nullptr;

void ProcessOne(TH1D *h)
{
	// settings
	const double t_min_goal = 0.005, t_max_goal = 0.010;

	const double dsi_dt_ref = 200.;

	// determine limits
	const int bi_min = h->GetXaxis()->FindBin(t_min_goal);
	const int bi_max = h->GetXaxis()->FindBin(t_max_goal);

	const double t_min = h->GetXaxis()->GetBinLowEdge(bi_min);
	const double t_max = h->GetXaxis()->GetBinLowEdge(bi_max) + h->GetXaxis()->GetBinWidth(bi_max);

	printf("        summing from bin %i (left edge %.4f) to bin %i (right edge %.4f)\n", bi_min, t_min, bi_max, t_max);

	// sum bin content
	double c_hist = 0;
	for (int bi = bi_min; bi <= bi_max; ++bi)
	{
		c_hist += h->GetBinContent(bi) * h->GetBinWidth(bi);
	}

	const double dN_dt = c_hist / (t_max - t_min);

	printf("            L = %.4E\n", dN_dt / dsi_dt_ref);
}

//----------------------------------------------------------------------------------------------------

int main()
{
	// configuration
	string binning = "eb";

	vector<string> datasets;
	datasets.push_back("data/fill7280/Totem1");
	datasets.push_back("data/fill7281/Totem1");
	datasets.push_back("data/fill7282/Totem1");
	datasets.push_back("data/fill7283/Totem1");
	datasets.push_back("data/fill7284/Totem1");
	datasets.push_back("data/fill7285/Totem1");
	datasets.push_back("data/fill7289/Totem1");
	datasets.push_back("data/fill7291/Totem1");

	vector<string> diagonals;
	diagonals.push_back("45b_56t");
	diagonals.push_back("45t_56b");
	//diagonals.push_back("combined");

	// process data
	for (unsigned int dsi = 0; dsi < datasets.size(); dsi++)
	{
		printf("\n\n%s\n", datasets[dsi].c_str());

		for (unsigned int dgni = 0; dgni < diagonals.size(); dgni++)
		{
			printf("    %s\n", diagonals[dgni].c_str());

			TFile *f_in = TFile::Open(("../../"+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root").c_str());
			TH1D *h_in = (TH1D *) f_in->Get(("normalization/"+binning+"/h_t_normalized_no_L").c_str());

			ProcessOne(h_in);

			delete f_in;
		}
	}

	return 0;
}
