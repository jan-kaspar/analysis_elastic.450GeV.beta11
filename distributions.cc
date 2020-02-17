#include "common_definitions.h"
#include "common_algorithms.h"
#include "AcceptanceCalculator.h"
#include "parameters.h"
#include "common.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TChain.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TMath.h"
#include "TF1.h"
#include "TTree.h"

#include <cmath>

using namespace std;

//----------------------------------------------------------------------------------------------------

void ProfileToRMSGraph(TProfile *p, TGraphErrors *g)
{
	for (int bi = 1; bi <= p->GetNbinsX(); ++bi)
	{
		double c = p->GetBinCenter(bi);

		double N = p->GetBinEntries(bi);
		double Sy = p->GetBinContent(bi) * N;
		double Syy = p->GetSumw2()->At(bi);

		double si_sq = Syy/N - Sy*Sy/N/N;
		double si = (si_sq >= 0.) ? sqrt(si_sq) : 0.;
		double si_unc_sq = si_sq / 2. / N;	// Gaussian approximation
		double si_unc = (si_unc_sq >= 0.) ? sqrt(si_unc_sq) : 0.;

		int idx = g->GetN();
		g->SetPoint(idx, c, si);
		g->SetPointError(idx, 0., si_unc);
	}
}

//----------------------------------------------------------------------------------------------------

TGraphErrors* MakeRelDiff(TH1D *input)
{
	TGraphErrors *g_rel_diff = new TGraphErrors();

	for (int i = 1; i <= input->GetNbinsX(); ++i)
	{
		double t = input->GetBinCenter(i);
		double v = input->GetBinContent(i);
		double v_u = input->GetBinError(i);

		if (t > 0.4)
			continue;

		double ref = 530. * exp(-19.6*t);

		double rv = v / ref - 1.;
		double rv_u = v_u / ref;

		int idx = g_rel_diff->GetN();
		g_rel_diff->SetPoint(idx, t, rv);
		g_rel_diff->SetPointError(idx, 0., rv_u);
	}

	return g_rel_diff;
}

//----------------------------------------------------------------------------------------------------

unsigned int SuppressLowStatisticsBins(TProfile *p, int threshold)
{
	unsigned int reasonableBins = 0;
	for (int bi = 1; bi <= p->GetNbinsX(); ++bi)
	{
		if (p->GetBinEntries(bi) < threshold)
		{
			p->SetBinContent(bi, 0.);
			p->SetBinError(bi, 0.);
		} else
			reasonableBins++;
	}

	return reasonableBins;
}

//----------------------------------------------------------------------------------------------------

void HideLowTBins(TH1D *h, double threshold)
{
	for (int bi = 1; bi <= h->GetNbinsX(); ++bi)
	{
		if (h->GetBinCenter(bi) < threshold)
		{
			h->SetBinContent(bi, 0.);
			h->SetBinError(bi, 0.);
		}
	}
}

//----------------------------------------------------------------------------------------------------

TGraph* PlotFiductialCut(const FiducialCut &fc, double th_y_sign)
{
	TGraph *g = new TGraph();

	vector<double> th_x_values = { fc.th_x_m - 400E-6, fc.th_x_m, fc.th_x_p, fc.th_x_p + 400E-6 };

	for (const double &th_x : th_x_values)
	{
		g->SetPoint(g->GetN(), th_x, th_y_sign * fc.GetThYLimit(th_x));
	}

	return g;
}

//----------------------------------------------------------------------------------------------------

struct OpticsMatchingInput
{
	TGraph *g_thl_x_R_vs_thl_x_L;
	TGraph *g_y_R_220_F_vs_y_L_220_F;
	TGraph *g_thl_y_R_vs_y_R_220_F;
	TGraph *g_thl_y_L_vs_y_L_220_F;
	TGraph *g_thl_x_R_vs_x_R_220_F;
	TGraph *g_thl_x_L_vs_x_L_220_F;
	TGraph *g_x_L_220_F_vs_y_L_220_F;
	TGraph *g_x_L_210_F_vs_y_L_210_F;
	TGraph *g_x_R_210_F_vs_y_R_210_F;
	TGraph *g_x_R_220_F_vs_y_R_220_F;

	TGraph *g_vtx_x_L_vs_vtx_x_R;

	OpticsMatchingInput()
	{
		// constraints from New Journal of Physics 16 (2014) 103041

		// R1 = theta_x_loc_R / theta_x_loc_L
		g_thl_x_R_vs_thl_x_L = new TGraph(); g_thl_x_R_vs_thl_x_L->SetName("g_thl_x_R_vs_thl_x_L"); g_thl_x_R_vs_thl_x_L->SetTitle(";#theta_{x,loc}^{L}   (rad);#theta_{x,loc}^{R}   (rad)");

		// R2 = y_R_220_F / y_L_220_F
		g_y_R_220_F_vs_y_L_220_F = new TGraph(); g_y_R_220_F_vs_y_L_220_F->SetName("g_y_R_220_F_vs_y_L_220_F"); g_y_R_220_F_vs_y_L_220_F->SetTitle(";y^{L-220-F}   (mm);y^{R-220-F}   (mm)");

		// R3 = theta_y_loc_R / y_R_220_F
		g_thl_y_R_vs_y_R_220_F = new TGraph(); g_thl_y_R_vs_y_R_220_F->SetName("g_thl_y_R_vs_y_R_220_F"); g_thl_y_R_vs_y_R_220_F->SetTitle(";y^{R-220-F}   (mm);#theta_{y,loc}^{R}   (rad)");

		// R4 = theta_y_loc_L / y_L_220_F
		g_thl_y_L_vs_y_L_220_F = new TGraph(); g_thl_y_L_vs_y_L_220_F->SetName("g_thl_y_L_vs_y_L_220_F"); g_thl_y_L_vs_y_L_220_F->SetTitle(";y^{L-220-F}   (mm);#theta_{y,loc}^{L}   (rad)");

		// R5 = theta_x_loc_R / x_R_220_F
		g_thl_x_R_vs_x_R_220_F = new TGraph(); g_thl_x_R_vs_x_R_220_F->SetName("g_thl_x_R_vs_x_R_220_F"); g_thl_x_R_vs_x_R_220_F->SetTitle(";x^{R-220-F}   (mm);#theta_{x,loc}^{R}   (rad)");

		// R6 = theta_x_loc_L / x_L_220_F
		g_thl_x_L_vs_x_L_220_F = new TGraph(); g_thl_x_L_vs_x_L_220_F->SetName("g_thl_x_L_vs_x_L_220_F"); g_thl_x_L_vs_x_L_220_F->SetTitle(";x^{L-220-F}   (mm);#theta_{x,loc}^{L}   (rad)");

		// R7 = x_L_220_F / y_L_220_F
		g_x_L_220_F_vs_y_L_220_F = new TGraph(); g_x_L_220_F_vs_y_L_220_F->SetName("g_x_L_220_F_vs_y_L_220_F"); g_x_L_220_F_vs_y_L_220_F->SetTitle(";y^{L-220-F}   (mm);x^{L-220-F}   (mm)");

		// R8 = x_L_210_F / y_L_210_F
		g_x_L_210_F_vs_y_L_210_F = new TGraph(); g_x_L_210_F_vs_y_L_210_F->SetName("g_x_L_210_F_vs_y_L_210_F"); g_x_L_210_F_vs_y_L_210_F->SetTitle(";y^{L-210-F}   (mm);x^{L-210-F}   (mm)");

		// R9 = x_R_210_F / y_R_210_F
		g_x_R_210_F_vs_y_R_210_F = new TGraph(); g_x_R_210_F_vs_y_R_210_F->SetName("g_x_R_210_F_vs_y_R_210_F"); g_x_R_210_F_vs_y_R_210_F->SetTitle(";y^{R-210-F}   (mm);x^{R-210-F}   (mm)");

		// R10 = x_R_220_F / y_R_220_F
		g_x_R_220_F_vs_y_R_220_F = new TGraph(); g_x_R_220_F_vs_y_R_220_F->SetName("g_x_R_220_F_vs_y_R_220_F"); g_x_R_220_F_vs_y_R_220_F->SetTitle(";y^{R-220-F}   (mm);x^{R-220-F}   (mm)");

		// control plot
		g_vtx_x_L_vs_vtx_x_R = new TGraph(); g_vtx_x_L_vs_vtx_x_R->SetName("g_vtx_x_L_vs_vtx_x_R"); g_vtx_x_L_vs_vtx_x_R->SetTitle(";vtx_x^{R}   (mm);vtx_x^{L}   (mm)");
	}

	void Fill(const HitData &h_al, double thl_x_L, double thl_x_R, double thl_y_L, double thl_y_R, double vtx_x_L, double vtx_x_R)
	{
		int idx = g_thl_y_L_vs_y_L_220_F->GetN();

		if (idx < 10000)
		{
			g_thl_x_R_vs_thl_x_L->SetPoint(idx, thl_x_L, thl_x_R);
			g_y_R_220_F_vs_y_L_220_F->SetPoint(idx, h_al.L_2_F.y, h_al.R_2_F.y);

			g_thl_y_R_vs_y_R_220_F->SetPoint(idx, h_al.R_2_F.y, thl_y_R);
			g_thl_y_L_vs_y_L_220_F->SetPoint(idx, h_al.L_2_F.y, thl_y_L);

			g_thl_x_R_vs_x_R_220_F->SetPoint(idx, h_al.R_2_F.x, thl_x_R);
			g_thl_x_L_vs_x_L_220_F->SetPoint(idx, h_al.L_2_F.x, thl_x_L);

			g_x_L_220_F_vs_y_L_220_F->SetPoint(idx, h_al.L_2_F.y, h_al.L_2_F.x);
			g_x_L_210_F_vs_y_L_210_F->SetPoint(idx, h_al.L_1_F.y, h_al.L_1_F.x);
			g_x_R_210_F_vs_y_R_210_F->SetPoint(idx, h_al.R_1_F.y, h_al.R_1_F.x);
			g_x_R_220_F_vs_y_R_220_F->SetPoint(idx, h_al.R_2_F.y, h_al.R_2_F.x);

			g_vtx_x_L_vs_vtx_x_R->SetPoint(idx, vtx_x_R, vtx_x_L);
		}
	}

	void Write() const
	{
		g_thl_x_R_vs_thl_x_L->Write();
		g_y_R_220_F_vs_y_L_220_F->Write();

		g_thl_y_R_vs_y_R_220_F->Write();
		g_thl_y_L_vs_y_L_220_F->Write();

		g_thl_x_R_vs_x_R_220_F->Write();
		g_thl_x_L_vs_x_L_220_F->Write();

		g_x_L_220_F_vs_y_L_220_F->Write();
		g_x_L_210_F_vs_y_L_210_F->Write();
		g_x_R_210_F_vs_y_R_210_F->Write();
		g_x_R_220_F_vs_y_R_220_F->Write();

		g_vtx_x_L_vs_vtx_x_R->Write();
	}
};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if (argc < 2)
		return 1;

	// init diagonal settings
	Init(argv[1]);
	if (diagonal == dCombined || diagonal == ad45b_56b || diagonal == ad45t_56t)
		return rcIncompatibleDiagonal;

	// default parameters
	unsigned int detailsLevel = 0; 	// 0: no details, 1: some details, >= 2 all details
	bool overrideCutSelection = false;	// whether the default cut selection should be overriden by the command-line selection
	string cutSelectionString;
	string outputDir = ".";
	string inputDir = ".";
	double input_n_si = 4.0;
	int time_group_divisor = 0;
	int time_group_remainder = 0;
	int event_group_divisor = 0;
	int event_group_index = 0;

	// parse command line arguments, starting from index 2
	for (int i = 2; i < argc; i++)
	{
		//printf("%u => %s\n", i, argv[i]);

		if (strcmp(argv[i], "-no-details") == 0)
		{
			detailsLevel = 0;
			continue;
		}

		if (strcmp(argv[i], "-details") == 0)
		{
			if (argc-1 > i)
				detailsLevel = atoi(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-cuts") == 0)
		{
			if (argc-1 > i)
			{
				cutSelectionString = argv[++i];
				overrideCutSelection = true;
			}
			continue;
		}

		if (strcmp(argv[i], "-output-dir") == 0)
		{
			if (argc-1 > i)
				outputDir = argv[++i];
			continue;
		}

		if (strcmp(argv[i], "-input-dir") == 0)
		{
			if (argc-1 > i)
				inputDir = argv[++i];
			continue;
		}

		if (strcmp(argv[i], "-n-si") == 0)
		{
			if (argc-1 > i)
				input_n_si = atof(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-tg-divisor") == 0)
		{
			if (argc-1 > i)
				time_group_divisor = atoi(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-tg-remainder") == 0)
		{
			if (argc-1 > i)
				time_group_remainder = atoi(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-eg-divisor") == 0)
		{
			if (argc-1 > i)
				event_group_divisor = (int) atof(argv[++i]);
			continue;
		}


		if (strcmp(argv[i], "-eg-index") == 0)
		{
			if (argc-1 > i)
				event_group_index = (int) atof(argv[++i]);
			continue;
		}

		printf("ERROR: unknown parameter `%s'.\n", argv[i]);
		return 3;
	}

	printf("* detailsLevel = %u\n", detailsLevel);
	printf("* outputDir = %s\n", outputDir.c_str());
	printf("* inputDir = %s\n", inputDir.c_str());
	printf("* input n_si = %.3f\n", input_n_si);
	printf("* time_group_divisor = %i\n", time_group_divisor);
	printf("* time_group_remainder = %i\n", time_group_remainder);
	printf("* event_group_divisor = %i\n", event_group_divisor);
	printf("* event_group_index = %i\n", event_group_index);

	// select cuts
	anal.BuildCuts(); 
	anal.n_si = input_n_si;

	if (overrideCutSelection)
	{
		anal.cuts.clear();
		char buf[100];
		strcpy(buf, cutSelectionString.c_str());

		// workaround for a Bash bug? --> keeping quotes around the parameter
		for (unsigned int i = 0; i < strlen(buf); i++)
			if (buf[i] == '"')
				buf[i] = ' ';

		printf("* setting cuts from selection string [%s]\n", buf);

		if (strcmp(buf, "  ") == 0 || strlen(buf) == 0)
			printf("* cut selection list empty\n");
		else {
			char *pch = strtok(buf, ",");
			while (pch != NULL)
			{
				unsigned int cut = atoi(pch);
				if (cut < 1 || cut > 10)
				{
					printf("ERROR: invalid cut number %u.\n", cut);
					return 2;
				}
				anal.cuts.push_back(cut);
				pch = strtok (NULL, ",");
			}
		}
	}

	// print info
	printf("\n");
	printf("------------------------------ environment ------------------------------\n");
	env.Print();
	printf("\n");
	printf("------------------------------- analysis --------------------------------\n");
	anal.Print();
	printf("\n");

	// alignment init
	for (unsigned int i = 0; i < alignmentSources.size(); ++i)
	{
		printf("\n---------- alignment source %u ----------\n", i);
		alignmentSources[i].Init();
	}
	printf("\n\n");

	// binnings
	// TODO
	vector<string> binnings;
	binnings.push_back("ub");
	binnings.push_back("eb");

	// initialise acceptance calculation
	AcceptanceCalculator accCalc;
	accCalc.Init(th_y_sign, anal);

	// get input
	TChain *ch_in = new TChain("distilled");
	printf(">> input chain\n");
	for (const auto &ntupleDir : distilledNtuples)
	{
		string f = inputDir + "/" + ntupleDir + "/distill_" + argv[1] + ".root";
		printf("    %s\n", f.c_str());
		ch_in->Add(f.c_str());
	}
	printf("%llu entries\n", ch_in->GetEntries());

	// init output file
	TFile *outF = new TFile((outputDir+"/distributions_" + argv[1] + ".root").c_str(), "recreate");

	//FILE *f_out_cand = fopen((outputDir+"/candidates_" + argv[1] + ".txt").c_str(), "w");

	// get input data
	EventRed ev;
	ch_in->SetBranchAddress("v_L_1_F", &ev.h.L_1_F.v); ch_in->SetBranchAddress("x_L_1_F", &ev.h.L_1_F.x); ch_in->SetBranchAddress("y_L_1_F", &ev.h.L_1_F.y);
	ch_in->SetBranchAddress("v_L_2_F", &ev.h.L_2_F.v); ch_in->SetBranchAddress("x_L_2_F", &ev.h.L_2_F.x); ch_in->SetBranchAddress("y_L_2_F", &ev.h.L_2_F.y);

	ch_in->SetBranchAddress("v_R_1_F", &ev.h.R_1_F.v); ch_in->SetBranchAddress("x_R_1_F", &ev.h.R_1_F.x); ch_in->SetBranchAddress("y_R_1_F", &ev.h.R_1_F.y);
	ch_in->SetBranchAddress("v_R_2_F", &ev.h.R_2_F.v); ch_in->SetBranchAddress("x_R_2_F", &ev.h.R_2_F.x); ch_in->SetBranchAddress("y_R_2_F", &ev.h.R_2_F.y);

	ch_in->SetBranchAddress("lumi_section", &ev.lumi_section);
	ch_in->SetBranchAddress("timestamp", &ev.timestamp);
	ch_in->SetBranchAddress("run_num", &ev.run_num);
	ch_in->SetBranchAddress("bunch_num", &ev.bunch_num);
	ch_in->SetBranchAddress("event_num", &ev.event_num);

	// get time-dependent corrections
	TGraph *corrg_pileup = NULL;
	if (anal.use_pileup_efficiency_fits)
	{
		string path = inputDir + "/pileup_fit_combined.root";
		TFile *puF = TFile::Open(path.c_str());
		if (!puF)
			printf("ERROR: pile-up correction file `%s' cannot be opened.\n", path.c_str());
		if (diagonal == d45b_56t)
			corrg_pileup = (TGraph *) puF->Get("45b_56t/dgn");
		if (diagonal == d45t_56b)
			corrg_pileup = (TGraph *) puF->Get("45t_56b/dgn");
	}

	// get time-dependent resolution
	TGraph *g_d_x_RMS = NULL;
	TGraph *g_d_y_RMS = NULL;
	if (anal.use_resolution_fits)
	{
		string path = inputDir + "/resolution_fit_" + argv[1] + ".root";
		TFile *resFile = TFile::Open(path.c_str());
		if (!resFile)
			printf("ERROR: resolution file `%s' cannot be opened.\n", path.c_str());

		g_d_x_RMS = (TGraph *) resFile->Get("d_x/g_fits");
		g_d_y_RMS = (TGraph *) resFile->Get("d_y/g_fits");

		printf("\n>> using time-dependent resolutions: %p, %p\n", g_d_x_RMS, g_d_y_RMS);
	}

	// get th_y* dependent efficiency correction
	TF1 *f_3outof4_efficiency_L_F = NULL;
	TF1 *f_3outof4_efficiency_L_N = NULL;
	TF1 *f_3outof4_efficiency_R_N = NULL;
	TF1 *f_3outof4_efficiency_R_F = NULL;
	if (anal.use_3outof4_efficiency_fits)
	{
		string path = inputDir + "/eff3outof4_details_fit_old.root";
		TFile *effFile = TFile::Open(path.c_str());
		if (!effFile)
			printf("ERROR: 3-out-of-4 efficiency file `%s' cannot be opened.\n", path.c_str());

		string diagonal = argv[1];
		f_3outof4_efficiency_L_F = (TF1 *) effFile->Get( (diagonal + "/L_F/fit").c_str() );
		f_3outof4_efficiency_L_N = (TF1 *) effFile->Get( (diagonal + "/L_N/fit").c_str() );
		f_3outof4_efficiency_R_N = (TF1 *) effFile->Get( (diagonal + "/R_N/fit").c_str() );
		f_3outof4_efficiency_R_F = (TF1 *) effFile->Get( (diagonal + "/R_F/fit").c_str() );

		printf("\n>> using 3-out-of-4 fits: %p, %p, %p, %p\n",
			f_3outof4_efficiency_L_F, f_3outof4_efficiency_L_N, f_3outof4_efficiency_R_N, f_3outof4_efficiency_R_F);
	}

	// get unsmearing correction
	printf("\n>> unsmearing_file = %s\n", unsmearing_file.c_str());
	printf(">> unsmearing_object = %s\n", unsmearing_object.c_str());
	TF1 *unsmearing_correction_fit = NULL;
	TFile *unsmearing_correction_file = TFile::Open(unsmearing_file.c_str());
	if (!unsmearing_correction_file)
	{
		printf("ERROR: unfolding file `%s' can not be opened.\n", unsmearing_file.c_str());
	} else {
		unsmearing_correction_fit = (TF1 *) unsmearing_correction_file->Get(unsmearing_object.c_str());
		if (!unsmearing_correction_fit)
			printf("ERROR: unsmearing correction object `%s' cannot be loaded.\n", unsmearing_object.c_str());
	}

	// book metadata histograms
	unsigned int timestamp_bins = timestamp_max - timestamp_min + 1;

	TH1D *h_timestamp_input = new TH1D("h_timestamp_input", ";timestamp;rate   (Hz)", timestamp_bins, timestamp_min-0.5, timestamp_max+0.5);
	TH1D *h_timestamp_dgn = new TH1D("h_timestamp_dgn", ";timestamp;rate   (Hz)", timestamp_bins, timestamp_min-0.5, timestamp_max+0.5);
	TH1D *h_timestamp_B0 = new TH1D("h_timestamp_B0", ";timestamp;rate   (Hz)", timestamp_bins, timestamp_min-0.5, timestamp_max+0.5);
	TH1D *h_timestamp_sel = new TH1D("h_timestamp_sel", ";timestamp;rate   (Hz)", timestamp_bins, timestamp_min-0.5, timestamp_max+0.5);

	TGraph *g_run_vs_timestamp = new TGraph(); g_run_vs_timestamp->SetName("g_run_vs_timestamp"); g_run_vs_timestamp->SetTitle(";timestamp;run");
	TGraph *g_ev_num_vs_timestamp = new TGraph(); g_ev_num_vs_timestamp->SetName("g_ev_num_vs_timestamp"); g_ev_num_vs_timestamp->SetTitle(";timestamp;ev_num");
	TGraph *g_tr_num_vs_timestamp = new TGraph(); g_tr_num_vs_timestamp->SetName("g_tr_num_vs_timestamp"); g_tr_num_vs_timestamp->SetTitle(";timestamp;tr_num");
	TGraph *g_bunch_num_vs_timestamp = new TGraph(); g_bunch_num_vs_timestamp->SetName("g_bunch_num_vs_timestamp"); g_bunch_num_vs_timestamp->SetTitle(";timestamp;bunch");
	TGraph *g_selected_bunch_num_vs_timestamp = new TGraph(); g_selected_bunch_num_vs_timestamp->SetName("g_selected_bunch_num_vs_timestamp"); g_selected_bunch_num_vs_timestamp->SetTitle(";timestamp;selected_bunch");

	TGraph *g_timestamp_vs_ev_idx_dgn = new TGraph(); g_timestamp_vs_ev_idx_dgn->SetName("g_timestamp_vs_ev_idx_dgn"); g_timestamp_vs_ev_idx_dgn->SetTitle(";event index in distilled TTree;timestamp");
	TGraph *g_timestamp_vs_ev_idx_sel = new TGraph(); g_timestamp_vs_ev_idx_sel->SetName("g_timestamp_vs_ev_idx_sel"); g_timestamp_vs_ev_idx_sel->SetTitle(";event index in distilled TTree;timestamp");

	// book hit-distribution histograms
	TH2D *h_y_L_2_F_vs_x_L_2_F_noal_sel = new TH2D("h_y_L_2_F_vs_x_L_2_F_noal_sel", ";x^{L,1,F};y^{L,1,F}", 150, -15., 15., 800, -80., +80.);
	TH2D *h_y_L_1_F_vs_x_L_1_F_noal_sel = new TH2D("h_y_L_1_F_vs_x_L_1_F_noal_sel", ";x^{L,1,N};y^{L,1,N}", 150, -15., 15., 800, -80., +80.);
	TH2D *h_y_R_1_F_vs_x_R_1_F_noal_sel = new TH2D("h_y_R_1_F_vs_x_R_1_F_noal_sel", ";x^{R,1,N};y^{R,1,N}", 150, -15., 15., 800, -80., +80.);
	TH2D *h_y_R_2_F_vs_x_R_2_F_noal_sel = new TH2D("h_y_R_2_F_vs_x_R_2_F_noal_sel", ";x^{R,1,F};y^{R,1,F}", 150, -15., 15., 800, -80., +80.);

	TH2D *h_y_L_2_F_vs_x_L_2_F_al_nosel = new TH2D("h_y_L_2_F_vs_x_L_2_F_al_nosel", ";x^{L,1,F};y^{L,1,F}", 150, -15., 15., 800, -80., +80.);
	TH2D *h_y_L_1_F_vs_x_L_1_F_al_nosel = new TH2D("h_y_L_1_F_vs_x_L_1_F_al_nosel", ";x^{L,1,N};y^{L,1,N}", 150, -15., 15., 800, -80., +80.);
	TH2D *h_y_R_1_F_vs_x_R_1_F_al_nosel = new TH2D("h_y_R_1_F_vs_x_R_1_F_al_nosel", ";x^{R,1,N};y^{R,1,N}", 150, -15., 15., 800, -80., +80.);
	TH2D *h_y_R_2_F_vs_x_R_2_F_al_nosel = new TH2D("h_y_R_2_F_vs_x_R_2_F_al_nosel", ";x^{R,1,F};y^{R,1,F}", 150, -15., 15., 800, -80., +80.);

	TH2D *h_y_L_2_F_vs_x_L_2_F_al_sel = new TH2D("h_y_L_2_F_vs_x_L_2_F_al_sel", ";x^{L,1,F};y^{L,1,F}", 150, -15., 15., 800, -80., +80.);
	TH2D *h_y_L_1_F_vs_x_L_1_F_al_sel = new TH2D("h_y_L_1_F_vs_x_L_1_F_al_sel", ";x^{L,1,N};y^{L,1,N}", 150, -15., 15., 800, -80., +80.);
	TH2D *h_y_R_1_F_vs_x_R_1_F_al_sel = new TH2D("h_y_R_1_F_vs_x_R_1_F_al_sel", ";x^{R,1,N};y^{R,1,N}", 150, -15., 15., 800, -80., +80.);
	TH2D *h_y_R_2_F_vs_x_R_2_F_al_sel = new TH2D("h_y_R_2_F_vs_x_R_2_F_al_sel", ";x^{R,1,F};y^{R,1,F}", 150, -15., 15., 800, -80., +80.);

	TGraph *g_y_L_2_F_vs_x_L_2_F_al_sel = new TGraph(); g_y_L_2_F_vs_x_L_2_F_al_sel->SetName("g_y_L_2_F_vs_x_L_2_F_al_sel");
	TGraph *g_y_L_1_F_vs_x_L_1_F_al_sel = new TGraph(); g_y_L_1_F_vs_x_L_1_F_al_sel->SetName("g_y_L_1_F_vs_x_L_1_F_al_sel");
	TGraph *g_y_R_1_F_vs_x_R_1_F_al_sel = new TGraph(); g_y_R_1_F_vs_x_R_1_F_al_sel->SetName("g_y_R_1_F_vs_x_R_1_F_al_sel");
	TGraph *g_y_R_2_F_vs_x_R_2_F_al_sel = new TGraph(); g_y_R_2_F_vs_x_R_2_F_al_sel->SetName("g_y_R_2_F_vs_x_R_2_F_al_sel");

	// book alignment histograms
	map<signed int, TGraph *> g_y_L_1_F_vs_x_L_1_F_sel, g_y_L_2_F_vs_x_L_2_F_sel;
	map<signed int, TGraph *> g_y_R_1_F_vs_x_R_1_F_sel, g_y_R_2_F_vs_x_R_2_F_sel;

	map<signed int, TGraph *> g_w_vs_timestamp_sel;

	map<signed int, TH1D *> tm_h_th_x_L, tm_h_th_x_R;
	map<signed int, TProfile *> tm_p_diffLR_th_x, tm_p_diffLR_th_y;
	map<signed int, TProfile *> tm_p_x_L_F_vs_th_x_L, tm_p_x_R_F_vs_th_x_R;

	// book cut histograms
	map<unsigned int, TH1D *> h_cq;
	map<unsigned int, TH2D *> h2_cq, h2_cq_full;
	map<unsigned int, TGraph *> g_cq;
	map<unsigned int, TProfile *> p_cq, p_cq_time;
	for (unsigned int i = 1; i <= anal.N_cuts; ++i)
	{
		char name[100], title[100];

		double x_min=0., x_max=0., y_min=0., y_max = 0.;
		double q_max = 0.;

		if (i == 1) { x_min = -1000E-6; x_max = +1000E-6; y_min = -1000E-6; y_max = 1000E-6; q_max = 500E-6; }
		if (i == 2) { x_min = 0E-6; x_max = +1000E-6; y_min = 0E-6; y_max = 1000E-6; q_max = 300E-6; }

		if (i == 3) { x_min = -1000E-6; x_max = +1000E-6; y_min = -15.; y_max = 15.; q_max = 5.; }
		if (i == 4) { x_min = -1000E-6; x_max = +1000E-6; y_min = -15.; y_max = 15.; q_max = 5.; }

		if (i == 5) { x_min = +0.; x_max = +30.; y_min = -10.; y_max = +5.; q_max = 0.5; }
		if (i == 6) { x_min = -30.; x_max = -0.; y_min = -5.; y_max = +5.; q_max = 0.5; }

		if (i == 7) { x_min = -1000E-6; x_max = +1000E-6; y_min = -5.; y_max = +5.; q_max = 2.; }
		if (i == 8) { x_min = -1000E-6; x_max = +1000E-6; y_min = -5.; y_max = +5.; q_max = 5.; }

		if (i == 9) { x_min = -10.; x_max = +10.; y_min = -5.; y_max = +5.; q_max = 2.0; }
		if (i == 10) { x_min = -10.; x_max = +10.; y_min = -5.; y_max = +5.; q_max = 2.0; }

		if ((i == 2 || i == 5 || i == 6) && diagonal == d45t_56b)
		{
			swap(x_min, x_max);
			swap(y_min, y_max);
			for (auto p : {&x_min, &x_max, &y_min, &y_max})
				*p = - (*p);
		}

		unsigned int bins_1D = 100;
		unsigned int bins_2D = 100;
		if (i == 1 || i == 2)
			bins_2D = 100;

		sprintf(name, "h_cq%i", i); sprintf(title, ";cq%i", i); h_cq[i] = new TH1D(name, title, bins_1D, -q_max, +q_max);

		sprintf(name, "h2_cq%i", i); sprintf(title, ";%s;%s", anal.cqaN[i].c_str(), anal.cqbN[i].c_str()); h2_cq[i] = new TH2D(name, title, bins_2D, x_min, x_max, bins_2D, y_min, y_max);
		sprintf(name, "h2_cq_full%i", i); sprintf(title, ";%s;%s", anal.cqaN[i].c_str(), anal.cqbN[i].c_str()); h2_cq_full[i] = new TH2D(name, title, bins_2D, x_min, x_max, bins_2D, y_min, y_max);

		sprintf(name, "g_cq%i", i); sprintf(title, ";%s;%s", anal.cqaN[i].c_str(), anal.cqbN[i].c_str()); g_cq[i] = new TGraph(); g_cq[i]->SetName(name); g_cq[i]->SetTitle(title);
		sprintf(name, "p_cq%i", i); sprintf(title, ";%s;%s", anal.cqaN[i].c_str(), anal.cqbN[i].c_str()); p_cq[i] = new TProfile(name, title, 300, x_min, x_max);
		sprintf(name, "p_cq_time%i", i); sprintf(title, ";time   (s);mean of cq%i", i); p_cq_time[i] = new TProfile(name, title, 31, timestamp_min, timestamp_max);
	}

	// book histograms for selected hits
	/*
	TProfile *p_x_vs_y_L_F = new TProfile("p_x_vs_y_L_F", ";y^{L,F};x^{L,F};", 50, 0., 0.);
	TProfile *p_x_vs_y_L_N = new TProfile("p_x_vs_y_L_N", ";y^{L,N};x^{L,N};", 50, 0., 0.);
	TProfile *p_x_vs_y_R_N = new TProfile("p_x_vs_y_R_N", ";y^{R,N};x^{R,N};", 50, 0., 0.);
	TProfile *p_x_vs_y_R_F = new TProfile("p_x_vs_y_R_F", ";y^{R,F};x^{R,F};", 50, 0., 0.);

	TProfile *p_x_vs_y_L_F_noal = new TProfile("p_x_vs_y_L_F_noal", ";y^{L,F};x^{L,F};", 50, 0., 0.);
	TProfile *p_x_vs_y_L_N_noal = new TProfile("p_x_vs_y_L_N_noal", ";y^{L,N};x^{L,N};", 50, 0., 0.);
	TProfile *p_x_vs_y_R_N_noal = new TProfile("p_x_vs_y_R_N_noal", ";y^{R,N};x^{R,N};", 50, 0., 0.);
	TProfile *p_x_vs_y_R_F_noal = new TProfile("p_x_vs_y_R_F_noal", ";y^{R,F};x^{R,F};", 50, 0., 0.);

	TH2D *h_x_L_diffFN_vs_x_L_N = new TH2D("h_x_L_diffFN_vs_x_L_N", ";x^{LN};x^{LF} - x^{LN}", 300, -0., +0., 300, -0., +0.);
	TH2D *h_x_R_diffFN_vs_x_R_N = new TH2D("h_x_R_diffFN_vs_x_R_N", ";x^{RN};x^{RF} - x^{RN}", 300, -0., +0., 300, -0., +0.);

	TGraph *g_x_L_diffFN_vs_x_L_N = new TGraph(); g_x_L_diffFN_vs_x_L_N->SetName("g_x_L_diffFN_vs_x_L_N"); g_x_L_diffFN_vs_x_L_N->SetTitle(";x^{LN};x^{LF} - x^{LN}");
	TGraph *g_x_R_diffFN_vs_x_R_N = new TGraph(); g_x_R_diffFN_vs_x_R_N->SetName("g_x_R_diffFN_vs_x_R_N"); g_x_R_diffFN_vs_x_R_N->SetTitle(";x^{RN};x^{RF} - x^{RN}");

	TH2D *h_y_L_diffFN_vs_y_L_N = new TH2D("h_y_L_diffFN_vs_y_L_N", ";y^{LN};y^{LF} - y^{LN}", 300, -0., +0., 300, -0., +0.);
	TH2D *h_y_R_diffFN_vs_y_R_N = new TH2D("h_y_R_diffFN_vs_y_R_N", ";y^{RN};y^{RF} - y^{RN}", 300, -0., +0., 300, -0., +0.);

	TGraph *g_y_L_diffFN_vs_y_L_N = new TGraph(); g_y_L_diffFN_vs_y_L_N->SetName("g_y_L_diffFN_vs_y_L_N"); g_y_L_diffFN_vs_y_L_N->SetTitle(";y^{LN};y^{LF} - y^{LN}");
	TGraph *g_y_R_diffFN_vs_y_R_N = new TGraph(); g_y_R_diffFN_vs_y_R_N->SetName("g_y_R_diffFN_vs_y_R_N"); g_y_R_diffFN_vs_y_R_N->SetTitle(";y^{RN};y^{RF} - y^{RN}");

	TH2D *h_y_L_ratioFN_vs_y_L_N = new TH2D("h_y_L_ratioFN_vs_y_L_N", ";y^{LN};y^{LF} / y^{LN}", 300, -30., +30., 300, 1.08, 1.14);
	TH2D *h_y_R_ratioFN_vs_y_R_N = new TH2D("h_y_R_ratioFN_vs_y_R_N", ";y^{RN};y^{RF} / y^{RN}", 300, -30., +30., 300, 1.08, 1.14);
	*/

	// book angluar histograms
	TH1D *th_x_diffLR = new TH1D("th_x_diffLR", ";#theta_{x}^{R} - #theta_{x}^{L}", 1000, -500E-6, +500E-6); th_x_diffLR->Sumw2();
	TH1D *th_y_diffLR = new TH1D("th_y_diffLR", ";#theta_{y}^{R} - #theta_{y}^{L}", 500, -50E-6, +50E-6); th_y_diffLR->Sumw2();

	TH1D *th_x_diffLF = new TH1D("th_x_diffLF", ";#theta_{x}^{L} - #theta_{x}", 400, -200E-6, +200E-6); th_x_diffLF->Sumw2();
	TH1D *th_x_diffRF = new TH1D("th_x_diffRF", ";#theta_{x}^{R} - #theta_{x}", 400, -200E-6, +200E-6); th_x_diffRF->Sumw2();

	TH2D *h_th_x_diffLR_vs_th_x = new TH2D("h_th_x_diffLR_vs_th_x", ";#theta_{x};#theta_{x}^{R} - #theta_{x}^{L}", 100, -300E-6, +300E-6, 120, -120E-6, +120E-6);
	TH2D *h_th_y_diffLR_vs_th_y = new TH2D("h_th_y_diffLR_vs_th_y", ";#theta_{y};#theta_{y}^{R} - #theta_{y}^{L}", 100, -500E-6, +500E-6, 120, -120E-6, +120E-6);

	TH2D *h_th_x_diffLR_vs_vtx_x = new TH2D("h_th_x_diffLR_vs_vtx_x", ";vtx_{x};#theta_{x}^{R} - #theta_{x}^{L}", 100, -300E-3, +300E-3, 120, -120E-6, +120E-6);

	TH2D *h2_th_x_diffLR_vs_th_x = new TH2D("h2_th_x_diffLR_vs_th_x", ";#theta_{x};#theta_{x}^{R} - #theta_{x}^{L}", 100, -500E-6, +500E-6, 100, 150E-6, 150E-6);
	TH2D *h2_th_x_L_diffNF_vs_th_x_L = new TH2D("h2_th_x_L_diffNF_vs_th_x_L", ";#theta_{x}^{L};#theta_{x}^{LF} - #theta_{x}^{LN}", 100, -500E-6, +500E-6, 100, 100E-6, 100E-6);
	TH2D *h2_th_x_R_diffNF_vs_th_x_R = new TH2D("h2_th_x_R_diffNF_vs_th_x_R", ";#theta_{x}^{R};#theta_{x}^{RF} - #theta_{x}^{RN}", 100, -500E-6, +500E-6, 100, 100E-6, 100E-6);

	TH2D *h2_th_y_diffLR_vs_th_y = new TH2D("h2_th_y_diffLR_vs_th_y", ";#theta_{y};#theta_{y}^{R} - #theta_{y}^{L}", 200, -500E-6, +500E-6, 100, 100E-6, 100E-6);
	TH2D *h2_th_y_L_diffNF_vs_th_y_L = new TH2D("h2_th_y_L_diffNF_vs_th_y_L", ";#theta_{y}^{L};#theta_{y}^{LF} - #theta_{y}^{LN}", 200, -500E-6, +500E-6, 100, 20E-6, 20E-6);
	TH2D *h2_th_y_R_diffNF_vs_th_y_R = new TH2D("h2_th_y_R_diffNF_vs_th_y_R", ";#theta_{y}^{R};#theta_{y}^{RF} - #theta_{y}^{RN}", 200, -500E-6, +500E-6, 100, 20E-6, 20E-6);

	TProfile *p_th_x_diffLR_vs_th_x = new TProfile("p_th_x_diffLR_vs_th_x", ";#theta_{x};#theta_{x}^{R} - #theta_{x}^{L}", 200, -500E-6, +500E-6);
	TProfile *p_th_x_L_diffNF_vs_th_x_L = new TProfile("p_th_x_L_diffNF_vs_th_x_L", ";#theta_{x}^{L};#theta_{x}^{LF} - #theta_{x}^{LN}", 200, -500E-6, +500E-6);
	TProfile *p_th_x_R_diffNF_vs_th_x_R = new TProfile("p_th_x_R_diffNF_vs_th_x_R", ";#theta_{x}^{R};#theta_{x}^{RF} - #theta_{x}^{RN}", 200, -500E-6, +500E-6);

	TProfile *p_th_y_diffLR_vs_th_y = new TProfile("p_th_y_diffLR_vs_th_y", ";#theta_{y};#theta_{y}^{R} - #theta_{y}^{L}", 500, -500E-6, +500E-6);
	TProfile *p_th_y_L_diffNF_vs_th_y_L = new TProfile("p_th_y_L_diffNF_vs_th_y_L", ";#theta_{y}^{L};#theta_{y}^{LF} - #theta_{y}^{LN}", 500, -500E-6, +500E-6);
	TProfile *p_th_y_R_diffNF_vs_th_y_R = new TProfile("p_th_y_R_diffNF_vs_th_y_R", ";#theta_{y}^{R};#theta_{y}^{RF} - #theta_{y}^{RN}", 500, -500E-6, +500E-6);

	TProfile *p_th_x_diffLR_vs_vtx_x = new TProfile("p_th_x_diffLR_vs_vtx_x", ";vtx_{x};#theta_{x}^{R} - #theta_{x}^{L}", 200, -500E-3, +500E-3);

	TH1D *th_x_diffLR_safe = new TH1D("th_x_diffLR_safe", ";#theta_{x}^{R} - #theta_{x}^{L}", 100, 0., 0.); th_x_diffLR_safe->Sumw2();
	TH1D *th_y_diffLR_safe = new TH1D("th_y_diffLR_safe", ";#theta_{y}^{R} - #theta_{y}^{L}", 100, 0., 0.); th_y_diffLR_safe->Sumw2();

	TProfile *p_th_x_vs_th_y = new TProfile("p_th_x_vs_th_y", ";#theta_{y}^{R};#theta_{x}", 100, -500E-6, +500E-6);
	TProfile *p_th_x_L_vs_th_y_L = new TProfile("p_th_x_L_vs_th_y_L", ";#theta_{y}^{L};#theta_{x}^{L}", 100, -500E-6, +500E-6);
	TProfile *p_th_x_R_vs_th_y_R = new TProfile("p_th_x_R_vs_th_y_R", ";#theta_{y}^{R};#theta_{x}^{R}", 100, -500E-6, +500E-6);

	TH2D *h_th_y_L_vs_th_x_L = new TH2D("h_th_y_L_vs_th_x_L", ";#theta_{x}^{L};#theta_{y}^{L}", 100, -1000E-6, +1000E-6, 100, -1000E-6, +1000E-6);
	TH2D *h_th_y_R_vs_th_x_R = new TH2D("h_th_y_R_vs_th_x_R", ";#theta_{x}^{R};#theta_{y}^{R}", 100, -1000E-6, +1000E-6, 100, -1000E-6, +1000E-6);
	TH2D *h_th_y_vs_th_x = new TH2D("h_th_y_vs_th_x", ";#theta_{x};#theta_{y}", 100, -1000E-6, +1000E-6, 100, -1000E-6, +1000E-6);

	TGraph *g_th_y_L_vs_th_x_L = new TGraph(); g_th_y_L_vs_th_x_L->SetName("g_th_y_L_vs_th_x_L"); g_th_y_L_vs_th_x_L->SetTitle(";#theta_{x}^{L};#theta_{y}^{L}");
	TGraph *g_th_y_R_vs_th_x_R = new TGraph(); g_th_y_R_vs_th_x_R->SetName("g_th_y_R_vs_th_x_R"); g_th_y_R_vs_th_x_R->SetTitle(";#theta_{x}^{R};#theta_{y}^{R}");
	TGraph *g_th_y_vs_th_x = new TGraph(); g_th_y_vs_th_x->SetName("g_th_y_vs_th_x"); g_th_y_vs_th_x->SetTitle(";#theta_{x}^{L};#theta_{y}^{L}");

	TH2D *h_th_y_L_vs_th_y_R = new TH2D("h_th_y_L_vs_th_y_R", ";#theta_{y}^{R};#theta_{y}^{L}", 300, -150E-6, +150E-6, 300, -150E-6, +150E-6);

	TH1D *h_th_x = new TH1D("h_th_x", ";#theta_{x}", 250, -500E-6, +500E-6); h_th_x->SetLineColor(1);
	TH1D *h_th_y = new TH1D("h_th_y", ";#theta_{y}", 250, -500E-6, +500E-6); h_th_y->SetLineColor(1);
	TH1D *h_th_y_flipped = new TH1D("h_th_y_flipped", ";#theta_{y}", 250, -500E-6, +500E-6); h_th_y_flipped->SetLineColor(1);

	TH1D *h_th_x_L = new TH1D("h_th_x_L", ";#theta_{x}^{L}", 250, -500E-6, +500E-6); h_th_x_L->SetLineColor(2);
	TH1D *h_th_x_R = new TH1D("h_th_x_R", ";#theta_{x}^{R}", 250, -500E-6, +500E-6); h_th_x_R->SetLineColor(4);

	TH1D *h_th_y_L = new TH1D("h_th_y_L", ";#theta_{y}^{L}", 250, -500E-6, +500E-6); h_th_y_L->SetLineColor(2);
	TH1D *h_th_y_R = new TH1D("h_th_y_R", ";#theta_{y}^{R}", 250, -500E-6, +500E-6); h_th_y_R->SetLineColor(4);

	// vertex histograms
	TH1D *h_vtx_x = new TH1D("h_vtx_x", ";x^{*}", 100, -0.5, +0.5); h_vtx_x->SetLineColor(1);
	TH1D *h_vtx_x_L = new TH1D("h_vtx_x_L", ";x^{*,L}", 100, -0.5, +0.5); h_vtx_x_L->SetLineColor(2);
	TH1D *h_vtx_x_R = new TH1D("h_vtx_x_R", ";x^{*,R}", 100, -0.5, +0.5); h_vtx_x_R->SetLineColor(4);

	TH1D *h_vtx_y = new TH1D("h_vtx_y", ";y^{*}", 100, -0.5, +0.5); h_vtx_y->SetLineColor(1);
	TH1D *h_vtx_y_L = new TH1D("h_vtx_y_L", ";y^{*,L}", 100, -0.5, +0.5); h_vtx_y_L->SetLineColor(2);
	TH1D *h_vtx_y_R = new TH1D("h_vtx_y_R", ";y^{*,R}", 100, -0.5, +0.5); h_vtx_y_R->SetLineColor(4);

	TH1D *h_vtx_x_safe = new TH1D("h_vtx_x_safe", ";x^{*}", 100, -0.5, +0.5); h_vtx_x_safe->SetLineColor(6);
	TH1D *h_vtx_y_safe = new TH1D("h_vtx_y_safe", ";y^{*}", 100, -0.5, +0.5); h_vtx_y_safe->SetLineColor(6);

	TH2D *h_vtx_x_L_vs_vtx_x_R = new TH2D("h_vtx_x_L_vs_vtx_x_R", ";x^{*,R};x^{*,L}", 100, -0.5, +0.5, 100, -0.5, +0.5);
	TH2D *h_vtx_y_L_vs_vtx_y_R = new TH2D("h_vtx_y_L_vs_vtx_y_R", ";y^{*,R};y^{*,L}", 100, -0.5, +0.5, 100, -0.5, +0.5);

	TH2D *h_vtx_x_L_vs_th_x_L = new TH2D("h_vtx_x_L_vs_th_x_L", ";#theta_{x}^{L};x^{*,L}", 100, -600E-6, +600E-6, 100, -0.5, +0.5);
	TH2D *h_vtx_x_R_vs_th_x_R = new TH2D("h_vtx_x_R_vs_th_x_R", ";#theta_{x}^{R};x^{*,R}", 100, -600E-6, +600E-6, 100, -0.5, +0.5);
	TH2D *h_vtx_y_L_vs_th_y_L = new TH2D("h_vtx_y_L_vs_th_y_L", ";#theta_{y}^{L};y^{*,L}", 100, -600E-6, +600E-6, 100, -0.5, +0.5);
	TH2D *h_vtx_y_R_vs_th_y_R = new TH2D("h_vtx_y_R_vs_th_y_R", ";#theta_{y}^{R};y^{*,R}", 100, -600E-6, +600E-6, 100, -0.5, +0.5);

	TH1D *h_vtx_x_diffLR = new TH1D("h_vtx_x_diffLR", ";x^{*,R} - x^{*,L}", 100, -0.5, +0.5); h_vtx_x_diffLR->Sumw2();
	TH1D *h_vtx_y_diffLR = new TH1D("h_vtx_y_diffLR", ";y^{*,R} - y^{*,L}", 100, -0.5, +0.5); h_vtx_y_diffLR->Sumw2();

	TH1D *h_vtx_x_diffLR_safe = new TH1D("h_vtx_x_diffLR_safe", ";vtx_{x}^{R} - vtx_{x}^{L}", 100, -0.5, +0.5); h_vtx_x_diffLR_safe->Sumw2(); h_vtx_x_diffLR_safe->SetLineColor(6);
	TH1D *h_vtx_y_diffLR_safe = new TH1D("h_vtx_y_diffLR_safe", ";vtx_{y}^{R} - vtx_{y}^{L}", 100, -0.5, +0.5); h_vtx_y_diffLR_safe->Sumw2(); h_vtx_y_diffLR_safe->SetLineColor(6);

	TH1D *h_vtx_x_diffLR_safe_corr = new TH1D("h_vtx_x_diffLR_safe_corr", ";vtx_{x}^{R} - vtx_{x}^{L}", 100, -0.5, +0.5); h_vtx_x_diffLR_safe_corr->Sumw2(); h_vtx_x_diffLR_safe_corr->SetLineColor(6);
	TH1D *h_vtx_y_diffLR_safe_corr = new TH1D("h_vtx_y_diffLR_safe_corr", ";vtx_{y}^{R} - vtx_{y}^{L}", 100, -0.5, +0.5); h_vtx_y_diffLR_safe_corr->Sumw2(); h_vtx_y_diffLR_safe_corr->SetLineColor(6);

	TH2D *h_vtx_x_diffLR_vs_th_x = new TH2D("h_vtx_x_diffLR_vs_th_x", ";#theta_{x};x^{*,R} - x^{*,L}", 100, -600E-6, +600E-6, 100, -0.5, +0.5);
	TH2D *h_vtx_y_diffLR_vs_th_y = new TH2D("h_vtx_y_diffLR_vs_th_y", ";#theta_{y};y^{*,R} - y^{*,L}", 200, -600E-6, +600E-6, 100, -0.5, +0.5);

	TProfile *p_vtx_x_diffLR_vs_th_x = new TProfile("p_vtx_x_diffLR_vs_th_x", ";#theta_{x};x^{*,R} - x^{*,L}", 100, -600E-6, +600E-6);
	TProfile *p_vtx_y_diffLR_vs_th_y = new TProfile("p_vtx_y_diffLR_vs_th_y", ";#theta_{y};y^{*,R} - y^{*,L}", 200, -600E-6, +600E-6);

	TH2D *h_vtx_x_diffLR_vs_vtx_x = new TH2D("h_vtx_x_diffLR_vs_vtx_x", ";x^{*};x^{*,R} - x^{*,L}", 100, -0.5, +0.5, 100, -0.5, +0.5);
	TH2D *h_vtx_y_diffLR_vs_vtx_y = new TH2D("h_vtx_y_diffLR_vs_vtx_y", ";y^{*};y^{*,R} - y^{*,L}", 100, -0.5, +0.5, 100, -0.5, +0.5);

	/*
	TProfile *p_x_L_F_vs_th_x = new TProfile("p_x_L_F_vs_th_x", ";#theta_{x};x^{L,F}", 100, 0., 0.);
	TProfile *p_x_L_N_vs_th_x = new TProfile("p_x_L_N_vs_th_x", ";#theta_{x};x^{L,N}", 100, 0., 0.);
	TProfile *p_x_R_F_vs_th_x = new TProfile("p_x_R_F_vs_th_x", ";#theta_{x};x^{R,F}", 100, 0., 0.);
	TProfile *p_x_R_N_vs_th_x = new TProfile("p_x_R_N_vs_th_x", ";#theta_{x};x^{R,N}", 100, 0., 0.);

	TProfile *p_x_L_F_vs_vtx_x = new TProfile("p_x_L_F_vs_vtx_x", ";vtx_{x};x^{L,F}", 100, 0., 0.);
	TProfile *p_x_L_N_vs_vtx_x = new TProfile("p_x_L_N_vs_vtx_x", ";vtx_{x};x^{L,N}", 100, 0., 0.);
	TProfile *p_x_R_F_vs_vtx_x = new TProfile("p_x_R_F_vs_vtx_x", ";vtx_{x};x^{R,F}", 100, 0., 0.);
	TProfile *p_x_R_N_vs_vtx_x = new TProfile("p_x_R_N_vs_vtx_x", ";vtx_{x};x^{R,N}", 100, 0., 0.);

	TProfile *p_vtx_x_L_vs_th_x = new TProfile("p_vtx_x_L_vs_th_x", ";#theta_{x};x^{*,L}", 100, 0., 0.);
	TProfile *p_vtx_x_L_vs_th_x_L = new TProfile("p_vtx_x_L_vs_th_x_L", ";#theta_{x}^{L};x^{*,L}", 100, 0., 0.);
	TProfile *p_vtx_x_R_vs_th_x = new TProfile("p_vtx_x_R_vs_th_x", ";#theta_{x};x^{*,R}", 100, 0., 0.);
	TProfile *p_vtx_x_R_vs_th_x_R = new TProfile("p_vtx_x_R_vs_th_x_R", ";#theta_{x}^{R};x^{*,R}", 100, 0., 0.);
	*/

	// input for optics matching
	OpticsMatchingInput opticsMatchingIntput_full;

	// time-dependence histograms
	TProfile *p_diffLR_th_x_vs_time = new TProfile("p_diffLR_th_x_vs_time", ";timestamp;mean of #Delta^{R-L}#theta_{x}", 31, timestamp_min, timestamp_max);
	TGraphErrors *gRMS_diffLR_th_x_vs_time = new TGraphErrors; gRMS_diffLR_th_x_vs_time->SetName("gRMS_diffLR_th_x_vs_time"); gRMS_diffLR_th_x_vs_time->SetTitle(";timestamp;RMS of #Delta^{R-L}#theta_{x}");

	TProfile *p_diffNF_th_x_L_vs_time = new TProfile("p_diffNF_th_x_L_vs_time", ";timestamp;mean of #Delta^{F-N}#theta_{x}^{L}", 31, timestamp_min, timestamp_max);
	TGraphErrors *gRMS_diffNF_th_x_L_vs_time = new TGraphErrors; gRMS_diffNF_th_x_L_vs_time->SetName("gRMS_diffNF_th_x_L_vs_time"); gRMS_diffNF_th_x_L_vs_time->SetTitle(";timestamp;RMS of #Delta^{F-N}#theta_{x}^{L}");

	TProfile *p_diffNF_th_x_R_vs_time = new TProfile("p_diffNF_th_x_R_vs_time", ";timestamp;mean of #Delta^{F-N}#theta_{x}^{R}", 31, timestamp_min, timestamp_max);
	TGraphErrors *gRMS_diffNF_th_x_R_vs_time = new TGraphErrors; gRMS_diffNF_th_x_R_vs_time->SetName("gRMS_diffNF_th_x_R_vs_time"); gRMS_diffNF_th_x_R_vs_time->SetTitle(";timestamp;RMS of #Delta^{F-N}#theta_{x}^{R}");

	TProfile *p_diffLR_th_y_vs_time = new TProfile("p_diffLR_th_y_vs_time", ";timestamp;mean of #Delta^{R-L}#theta_{y}", 31, timestamp_min, timestamp_max);
	TGraphErrors *gRMS_diffLR_th_y_vs_time = new TGraphErrors; gRMS_diffLR_th_y_vs_time->SetName("gRMS_diffLR_th_y_vs_time"); gRMS_diffLR_th_y_vs_time->SetTitle(";timestamp;RMS of #Delta^{R-L}#theta_{y}");

	TProfile *p_diffNF_th_y_L_vs_time = new TProfile("p_diffNF_th_y_L_vs_time", ";timestamp;mean of #Delta^{F-N}#theta_{y}^{L}", 31, timestamp_min, timestamp_max);
	TGraphErrors *gRMS_diffNF_th_y_L_vs_time = new TGraphErrors; gRMS_diffNF_th_y_L_vs_time->SetName("gRMS_diffNF_th_y_L_vs_time"); gRMS_diffNF_th_y_L_vs_time->SetTitle(";timestamp;RMS of #Delta^{F-N}#theta_{y}^{L}");

	TProfile *p_diffNF_th_y_R_vs_time = new TProfile("p_diffNF_th_y_R_vs_time", ";timestamp;mean of #Delta^{F-N}#theta_{y}^{R}", 31, timestamp_min, timestamp_max);
	TGraphErrors *gRMS_diffNF_th_y_R_vs_time = new TGraphErrors; gRMS_diffNF_th_y_R_vs_time->SetName("gRMS_diffNF_th_y_R_vs_time"); gRMS_diffNF_th_y_R_vs_time->SetTitle(";timestamp;RMS of #Delta^{F-N}#theta_{y}^{R}");

	TProfile *p_vtx_x_vs_time = new TProfile("p_vtx_x_vs_time", ";timestamp;mean of x^{*}", 31, timestamp_min, timestamp_max);
	TGraphErrors *gRMS_vtx_x_vs_time = new TGraphErrors; gRMS_vtx_x_vs_time->SetName("gRMS_vtx_x_vs_time"); gRMS_vtx_x_vs_time->SetTitle(";timestamp;RMS of x^{*}");

	TProfile *p_vtx_y_vs_time = new TProfile("p_vtx_y_vs_time", ";timestamp;mean of y^{*}", 31, timestamp_min, timestamp_max);
	TGraphErrors *gRMS_vtx_y_vs_time = new TGraphErrors; gRMS_vtx_y_vs_time->SetName("gRMS_vtx_y_vs_time"); gRMS_vtx_y_vs_time->SetTitle(";timestamp;RMS of y^{*}");

	TProfile *p_diffLR_vtx_x_vs_time = new TProfile("p_diffLR_vtx_x_vs_time", ";timestamp;mean of #Delta^{R-L}x^{*}", 31, timestamp_min, timestamp_max);
	TGraphErrors *gRMS_diffLR_vtx_x_vs_time = new TGraphErrors; gRMS_diffLR_vtx_x_vs_time->SetName("gRMS_diffLR_vtx_x_vs_time"); gRMS_diffLR_vtx_x_vs_time->SetTitle(";timestamp;RMS of #Delta^{R-L}x^{*}");

	TProfile *p_diffLR_vtx_y_vs_time = new TProfile("p_diffLR_vtx_y_vs_time", ";timestamp;mean of #Delta^{R-L}y^{*}", 31, timestamp_min, timestamp_max);
	TGraphErrors *gRMS_diffLR_vtx_y_vs_time = new TGraphErrors; gRMS_diffLR_vtx_y_vs_time->SetName("gRMS_diffLR_vtx_y_vs_time"); gRMS_diffLR_vtx_y_vs_time->SetTitle(";timestamp;RMS of #Delta^{R-L}y^{*}");

	TProfile *p_th_x_vs_time = new TProfile("p_th_x_vs_time", ";timestamp;#theta_{x}^{R}", 31, timestamp_min, timestamp_max);
	TProfile *p_th_x_R_vs_time = new TProfile("p_th_x_R_vs_time", ";timestamp;#theta_{x}^{R}", 31, timestamp_min, timestamp_max);
	TProfile *p_th_x_L_vs_time = new TProfile("p_th_x_L_vs_time", ";timestamp;#theta_{x}^{L}", 31, timestamp_min, timestamp_max);
	TProfile *p_th_x_R1F_vs_time = new TProfile("p_th_x_R1F_vs_time", ";timestamp;#theta_{x}^{R}", 31, timestamp_min, timestamp_max);
	TProfile *p_th_x_R2F_vs_time = new TProfile("p_th_x_R2F_vs_time", ";timestamp;#theta_{x}^{R}", 31, timestamp_min, timestamp_max);
	TProfile *p_th_x_L1F_vs_time = new TProfile("p_th_x_L1F_vs_time", ";timestamp;#theta_{x}^{R}", 31, timestamp_min, timestamp_max);
	TProfile *p_th_x_L2F_vs_time = new TProfile("p_th_x_L2F_vs_time", ";timestamp;#theta_{x}^{R}", 31, timestamp_min, timestamp_max);

	TProfile *p_th_y_R_vs_time = new TProfile("p_th_y_R_vs_time", ";timestamp;#theta_{y}^{R}", 31, timestamp_min, timestamp_max);
	TProfile *p_th_y_L_vs_time = new TProfile("p_th_y_L_vs_time", ";timestamp;#theta_{y}^{L}", 31, timestamp_min, timestamp_max);

	TProfile *p_input_beam_div_x_vs_time = new TProfile("p_input_beam_div_x_vs_time", ";timestamp", 31, timestamp_min, timestamp_max);
	TProfile *p_input_beam_div_y_vs_time = new TProfile("p_input_beam_div_y_vs_time", ";timestamp", 31, timestamp_min, timestamp_max);

	// book acceptance-correction histograms
	TProfile *p_t_ub_div_corr = new TProfile("p_t_ub_div_corr", ";t_ub_{y}", 2000., 0., 0.2);

	map<unsigned int, TH1D*> bh_t_Nev_before, bh_t_Nev_after_no_corr;
	map<unsigned int, TH1D*> bh_t_before, bh_t_after_no_corr, bh_t_after;
	map<unsigned int, TProfile*> bp_t_phi_corr, bp_t_full_corr;

	for (unsigned int bi = 0; bi < binnings.size(); ++bi)
	{
		unsigned int N_bins;
		double *bin_edges;
		BuildBinning(anal, binnings[bi], bin_edges, N_bins);

		bh_t_Nev_before[bi] = new TH1D("h_t_Nev_before", ";|t|;events per bin", N_bins, bin_edges); bh_t_Nev_before[bi]->Sumw2();
		bh_t_Nev_after_no_corr[bi] = new TH1D("h_t_Nev_after_no_corr", ";|t|;events per bin", N_bins, bin_edges); bh_t_Nev_after_no_corr[bi]->Sumw2();
		bh_t_before[bi] = new TH1D("h_t_before", ";|t|", N_bins, bin_edges); bh_t_before[bi]->Sumw2();
		bh_t_after[bi] = new TH1D("h_t_after", ";|t|", N_bins, bin_edges); bh_t_after[bi]->Sumw2();
		bh_t_after_no_corr[bi] = new TH1D("h_t_after_no_corr", ";|t|", N_bins, bin_edges); bh_t_after_no_corr[bi]->Sumw2();
		bp_t_phi_corr[bi] = new TProfile("p_t_phi_corr", ";t", N_bins, bin_edges, "s");
		bp_t_full_corr[bi] = new TProfile("p_t_full_corr", ";t", N_bins, bin_edges, "s");
	}

	TH2D *h_th_y_vs_th_x_before = new TH2D("h_th_y_vs_th_x_before", ";#theta_{x};#theta_{y}", 100, -1000E-6, +1000E-6, 100, -1000E-6, +1000E-6); h_th_y_vs_th_x_before->Sumw2();
	TH2D *h_th_y_vs_th_x_after = new TH2D("h_th_y_vs_th_x_after", ";#theta_{x};#theta_{y}", 100, -1000E-6, +1000E-6, 100, -1000E-6, +1000E-6); h_th_y_vs_th_x_after->Sumw2();
	TH2D *h_th_vs_phi_after = new TH2D("h_th_vs_phi_after", ";#phi;#theta", 50, -M_PI, +M_PI, 50, 150E-6, 550E-6); h_th_vs_phi_after->Sumw2();

	TGraph *g_weight_vs_th_y = new TGraph(); g_weight_vs_th_y->SetName("g_weight_vs_th_y"); g_weight_vs_th_y->SetTitle(";#theta_{y};weight");

	TGraph *g_th_y_vs_th_x_acc = new TGraph(); g_th_y_vs_th_x_acc->SetName("g_th_y_vs_th_x_acc"); g_th_y_vs_th_x_acc->SetTitle(";#theta_{x}^{L};#theta_{y}^{L}");

	// book normalization histograms
	TProfile *p_norm_corr = new TProfile("p_norm_corr", ";timestamp", 1000, 16E3, 113E3);
	TProfile *p_3outof4_corr = new TProfile("p_3outof4_corr", ";#theta_{y}^{*}", 120, -120E-6, 120E-6);

	map<unsigned int, TH1D*> bh_t_normalized;
	for (unsigned int bi = 0; bi < binnings.size(); ++bi)
	{
		bh_t_normalized[bi] = new TH1D(* bh_t_after[bi]);
		bh_t_normalized[bi]->SetName("h_t_normalized");
	}

	TH2D *h_th_y_vs_th_x_normalized = new TH2D("h_th_y_vs_th_x_normalized", ";#theta_{x};#theta_{y}", 150, -600E-6, +600E-6, 150, -600E-6, +600E-6); h_th_y_vs_th_x_normalized->Sumw2();

	TGraph *g_norm_corr_vs_div_corr = new TGraph(); g_norm_corr_vs_div_corr->SetName("g_norm_corr_vs_div_corr"); g_norm_corr_vs_div_corr->SetTitle(";div_corr;norm_corr");

	// book background histograms
	map<unsigned int, TH1D *> hb_cq;
	for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
	{
		char name[100], title[100];
		sprintf(name, "hb_cq%i", ci); sprintf(title, ";cq%i", ci); hb_cq[ci] = new TH1D(name, title, 6000, -1000, +1000); 
	}

	TH1D *hb_th_x_L = new TH1D("hb_th_x_L", ";-#theta_{x}^{L}", 100, 0, 0); hb_th_x_L->Sumw2();
	TH1D *hb_th_x_R = new TH1D("hb_th_x_R", ";#theta_{x}^{R}", 100, 0, 0); hb_th_x_R->Sumw2();
	TH1D *hb_th_y_L = new TH1D("hb_th_y_L", ";#theta_{y}^{L}", 100, 0, 0); hb_th_y_L->Sumw2();
	TH1D *hb_th_y_R = new TH1D("hb_th_y_R", ";#theta_{y}^{R}", 100, 0, 0); hb_th_y_R->Sumw2();

	TH2D *hb_th_y_L_vs_th_x_L = new TH2D("hb_th_y_L_vs_th_x_L", ";-#theta_{x}^{L};#theta_{y}^{L}", 100, -2E-3, +2E-3, 200, -120E-6, 120E-6);
	TH2D *hb_th_y_R_vs_th_x_R = new TH2D("hb_th_y_R_vs_th_x_R", ";#theta_{x}^{R};#theta_{y}^{R}", 100, -2E-3, +2E-3, 200, -120E-6, 120E-6);

	TH1D *hb_th_y_diffLR = new TH1D("hb_th_y_diffLR", ";#theta_{y}^{R} - #theta_{y}^{L}", 500, 0, 0);
	TH1D *hb_th_x_diffLR = new TH1D("hb_th_x_diffLR", ";#theta_{x}^{R} - #theta_{x}^{L}", 500, 0, 0);

	TH2D *hb_th_y_diffLR_vs_th_y = new TH2D("hb_th_y_diffLR_vs_th_y", ";#theta_{y};#theta_{y}^{R} - #theta_{y}^{L}", 100, 0, 0, 100, 0, 0);
	TH2D *hb_th_x_diffLR_vs_th_y = new TH2D("hb_th_x_diffLR_vs_th_y", ";#theta_{y};#theta_{x}^{R} - #theta_{x}^{L}", 100, 0, 0, 100, 0, 0);

	TH2D *hb_th_y_L_vs_th_y_R = new TH2D("hb_th_y_L_vs_th_y_R", ";-#theta_{y}^{R};#theta_{y}^{L}", 100, 0, 0, 100, 0, 0);
	TH2D *hb_th_x_L_vs_th_x_R = new TH2D("hb_th_x_L_vs_th_x_R", ";-#theta_{x}^{R};#theta_{x}^{L}", 100, 0, 0, 100, 0, 0);

	TH1D *hb_th_y_6cut = new TH1D("hb_th_y_6cut", ";|#theta_{y}|", 200, 0, 100E-6); hb_th_y_6cut->Sumw2();
	TH1D *hb_th_y_6cut_cut7fail = new TH1D("hb_th_y_6cut_cut7fail", ";|#theta_{y}|", 100, 0, 100E-6); hb_th_y_6cut_cut7fail->Sumw2();

	// zero counters
	unsigned long n_ev_full = 0;
	map<unsigned int, unsigned long> n_ev_cut;
	for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
		n_ev_cut[ci] = 0;

	double th_min = 1E100;
	double th_y_L_min = +1E100, th_y_R_min = +1E100;

	unsigned int N_anal=0;
	unsigned int N_4outof4=0, N_el=0, N_el_RP_trig=0;
	unsigned int N_el_T2trig=0, N_4outof4_T2trig=0;
	unsigned int N_el_raw=0;

	map<unsigned int, pair<unsigned int, unsigned int> > runTimestampBoundaries;
	map<pair<unsigned int, unsigned int>, pair<unsigned int, unsigned int> > lumiSectionBoundaries;

	unsigned int lumi_section_min = 100000, lumi_section_max = 0;

	// build histograms
	for (int ev_idx = 0; ev_idx < ch_in->GetEntries(); ++ev_idx)
	{
		ch_in->GetEntry(ev_idx);

		// update lumisection info
		lumi_section_min = min(lumi_section_min, ev.lumi_section);
		lumi_section_max = max(lumi_section_max, ev.lumi_section);

		auto lsbit = lumiSectionBoundaries.find({ev.run_num, ev.lumi_section});
		if (lsbit == lumiSectionBoundaries.end())
		{
			lumiSectionBoundaries.insert({{ev.run_num, ev.lumi_section}, {ev.timestamp, ev.timestamp}});
		} else {
			lsbit->second.first = min(lsbit->second.first, ev.timestamp);
			lsbit->second.second = max(lsbit->second.second, ev.timestamp);
		}

		// remove troublesome runs
		if (SkipRun(ev.run_num))
			continue;

		// update timestamp run boundaries
		auto rtbit = runTimestampBoundaries.find(ev.run_num);
		if (rtbit == runTimestampBoundaries.end())
		{
			runTimestampBoundaries.insert({ev.run_num, {ev.timestamp, ev.timestamp}});
		} else {
			rtbit->second.first = min(rtbit->second.first, ev.timestamp);
			rtbit->second.second = max(rtbit->second.second, ev.timestamp);
		}

		// check time - selected?
		if (anal.SkipTime(ev.timestamp))
			continue;

		if (time_group_divisor != 0)
		{
			double time_group_interval = 1.;	// s
			int time_group = int(ev.timestamp / time_group_interval);
			if ( (time_group % time_group_divisor) != time_group_remainder)
				continue;
		}

		// check lumi section - selected?
		if (ev.lumi_section < anal.lumi_section_min || ev.lumi_section > anal.lumi_section_max)
			continue;

		// select the elastic-trigger bunch(es) only
		if (SkipBunch(ev.run_num, ev.bunch_num))
			continue;

		// apply fine alignment
		HitData h_al = ev.h;
		for (unsigned int i = 0; i < alignmentSources.size(); ++i)
		{
			AlignmentData alData = alignmentSources[i].Eval(ev.timestamp);
			h_al = h_al.ApplyAlignment(alData);
		}

		// fill pre-selection histograms
		h_y_L_2_F_vs_x_L_2_F_al_nosel->Fill(h_al.L_2_F.x, h_al.L_2_F.y);
		h_y_L_1_F_vs_x_L_1_F_al_nosel->Fill(h_al.L_1_F.x, h_al.L_1_F.y);
		h_y_R_1_F_vs_x_R_1_F_al_nosel->Fill(h_al.R_1_F.x, h_al.R_1_F.y);
		h_y_R_2_F_vs_x_R_2_F_al_nosel->Fill(h_al.R_2_F.x, h_al.R_2_F.y);

		g_bunch_num_vs_timestamp->SetPoint(g_bunch_num_vs_timestamp->GetN(), ev.timestamp, ev.bunch_num);

		// diagonal cut
		bool allDiagonalRPs = ev.h.L_1_F.v && ev.h.L_2_F.v && ev.h.R_1_F.v && ev.h.R_2_F.v;

		h_timestamp_input->Fill(ev.timestamp);

		if (!allDiagonalRPs)
			continue;

		N_anal++;

		h_timestamp_dgn->Fill(ev.timestamp);

		if (detailsLevel >= 2)
		{
			h_timestamp_B0->Fill(ev.timestamp);
			g_run_vs_timestamp->SetPoint(g_run_vs_timestamp->GetN(), ev.timestamp, ev.run_num);
			//g_ev_num_vs_timestamp->SetPoint(g_ev_num_vs_timestamp->GetN(), ev.timestamp, ev.event_num);
			//g_tr_num_vs_timestamp->SetPoint(g_tr_num_vs_timestamp->GetN(), ev.timestamp, ev.trigger_num);
		}

		// run reconstruction
		Kinematics k = DoReconstruction(h_al, env);

		/*
		printf("------------------ event %i --------------\n", ev_idx);
		printf("th_x_L = %E, th_x_R = %E, th_x = %E\n", k.th_x_L, k.th_x_R, k.th_x);
		printf("th_y_L = %E, th_y_R = %E, th_y = %E\n", k.th_y_L, k.th_y_R, k.th_y);
		printf("L_2_F: x / L_x = %E\n", h_al.L_2_F.x / env.L_x_L_2_F);
		printf("L_2_F: x / L_x = %E\n", h_al.L_2_F.x / env.L_x_L_2_F);
		printf("R_2_F: x / L_x = %E\n", h_al.R_2_F.x / env.L_x_R_2_F);
		printf("R_2_F: x / L_x = %E\n", h_al.R_2_F.x / env.L_x_R_2_F);
		*/

		// alternative theta_x reconstruction
		/*
		// this is used in the old 90m analysis (for the PRL publication)
		double ta_x = (h_al.x_R_F - h_al.x_R_N - h_al.x_L_F + h_al.x_L_N) / (env.L_x_R_F - env.L_x_R_N) / 2.;
		*/

		// alternative theta_y reconstruction
		/*
		double D_y_L = - env.L_y_L_N * env.v_y_L_F + env.L_y_L_F * env.v_y_L_N;
		double ta_y_L = (env.v_y_L_F * h_al.y_L_N - env.v_y_L_N * h_al.y_L_F) / D_y_L;

		double D_y_R = env.L_y_R_N * env.v_y_R_F - env.L_y_R_F * env.v_y_R_N;
		double ta_y_R = (env.v_y_R_F * h_al.y_R_N - env.v_y_R_N * h_al.y_R_F) / D_y_R;

		double ta_y = (ta_y_L + ta_y_R) / 2.;
		*/

		// cut evaluation
		CutData cd;
		bool select = anal.EvaluateCuts(h_al, k, cd);
		//bool bckg = !select;

		// increment counters
		n_ev_full++;
		for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
		{
			if (cd.ct[ci])
				n_ev_cut[ci]++;
		}

		// fill background distributions
		for (unsigned int qi = 1; qi <= anal.N_cuts; ++qi)
		{
			bool reduced_select = true;
			for (unsigned int i = 0; i < anal.cuts.size(); i++)
			{
				unsigned int ci = anal.cuts[i];
				if (ci != qi)
					reduced_select &= cd.ct[ci];
			}

			if (reduced_select) {
				hb_cq[qi]->Fill(cd.cv[qi] / anal.csi[qi]);
			}
		}

		/*
		if (bckg) {
			hb_th_y_L->Fill(th_y_L);
			hb_th_y_R->Fill(th_y_R);
			hb_th_x_L->Fill(-th_x_L);
			hb_th_x_R->Fill(th_x_R);

			hb_th_y_L_vs_th_x_L->Fill(-th_x_L, th_y_L);
			hb_th_y_R_vs_th_x_R->Fill(th_x_R, th_y_R);

			hb_th_y_diffLR->Fill(th_y_R - th_y_L);
			hb_th_x_diffLR->Fill(th_x_R - th_x_L);

			hb_th_y_diffLR_vs_th_y->Fill(th_y, th_y_R - th_y_L);
			hb_th_x_diffLR_vs_th_y->Fill(th_y, th_x_R - th_x_L);

			hb_th_y_L_vs_th_y_R->Fill(th_y_R, th_y_L);
			hb_th_x_L_vs_th_x_R->Fill(th_x_R, th_x_L);
		}
		*/

		// fill no-cut histograms
		for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
		{
			//h2_cq_full[ci]->Fill(ccb[ci]*cqa[ci] - cca[ci]*cqb[ci], cca[ci]*cqa[ci] + ccb[ci]*cqb[ci] + ccc[ci]);
			h2_cq_full[ci]->Fill(cd.cqa[ci], cd.cqb[ci]);
		}

		/*
		bool select_6cut = true;
		for (unsigned int i = 0; i < cuts.size(); i++)
			if (cuts[i] != 7)
				select_6cut &= ct[cuts[i]];

		if (select_6cut)
			hb_th_y_6cut->Fill(fabs(th_y));

		if (select_6cut && !ct[7])
			hb_th_y_6cut_cut7fail->Fill(fabs(th_y));
		*/

		N_4outof4++;

		// elastic cut
		if (!select)
			continue;

		/*
		fprintf(f_out_cand, "elastic candidate: ");
		fprintf(f_out_cand, "L_2_F.x = %E, L_2_F.y = %E, ", ev.h.L_2_F.x, ev.h.L_2_F.y);
		fprintf(f_out_cand, "L_1_F.x = %E, L_1_F.y = %E, ", ev.h.L_1_F.x, ev.h.L_1_F.y);
		fprintf(f_out_cand, "R_1_F.x = %E, R_1_F.y = %E, ", ev.h.R_1_F.x, ev.h.R_1_F.y);
		fprintf(f_out_cand, "R_2_F.x = %E, R_2_F.y = %E\n", ev.h.R_2_F.x, ev.h.R_2_F.y);
		*/

		g_selected_bunch_num_vs_timestamp->SetPoint(g_selected_bunch_num_vs_timestamp->GetN(), ev.timestamp, ev.bunch_num);

		N_el++;

		// check event group
		if (event_group_divisor > 0)
		{
			int event_group = (N_el_raw-1) / event_group_divisor;
			if (event_group < event_group_index)
				continue;
			if (event_group > event_group_index)
				break;
		}

		// determine normalization factors (luminosity + corrections)
		double inefficiency_3outof4 = anal.inefficiency_3outof4;
		if (anal.use_3outof4_efficiency_fits)
		{
			inefficiency_3outof4 = 0.;
			inefficiency_3outof4 += 1. - f_3outof4_efficiency_L_F->Eval(k.th_y * 1E6);
			inefficiency_3outof4 += 1. - f_3outof4_efficiency_L_N->Eval(k.th_y * 1E6);
			inefficiency_3outof4 += 1. - f_3outof4_efficiency_R_N->Eval(k.th_y * 1E6);
			inefficiency_3outof4 += 1. - f_3outof4_efficiency_R_F->Eval(k.th_y * 1E6);
		}

		double inefficiency_shower_near = anal.inefficiency_shower_near;

		double inefficiency_pile_up = anal.inefficiency_pile_up;
		if (anal.use_pileup_efficiency_fits)
			inefficiency_pile_up = corrg_pileup->Eval(ev.timestamp);

		double inefficiency_trigger = anal.inefficiency_trigger;

		double norm_corr =
			1./(1. - (inefficiency_3outof4 + inefficiency_shower_near))
			* 1./(1. - inefficiency_pile_up)
			* 1./(1. - inefficiency_trigger);

		p_norm_corr->Fill(ev.timestamp, norm_corr);
		p_3outof4_corr->Fill(k.th_y, inefficiency_3outof4);

		// TODO: add inefficiency_DAQ
		double normalization = anal.bckg_corr * norm_corr / anal.L_int;

		// data for alignment
		// (SHOULD use hit positions WITHOUT alignment corrections, i.e. ev.h)
		signed int period = int((ev.timestamp - anal.alignment_t0) / anal.alignment_ts);
		if (detailsLevel >= 2)
		{
			if (g_w_vs_timestamp_sel.find(period) == g_w_vs_timestamp_sel.end())
			{
				g_w_vs_timestamp_sel[period] = new TGraph();

				g_y_L_1_F_vs_x_L_1_F_sel[period] = new TGraph();
				g_y_L_2_F_vs_x_L_2_F_sel[period] = new TGraph();

				g_y_R_1_F_vs_x_R_1_F_sel[period] = new TGraph();
				g_y_R_2_F_vs_x_R_2_F_sel[period] = new TGraph();

				tm_h_th_x_L[period] = new TH1D("", ";#theta_{x}^{L}", 100, -150E-6, +150E-6);
				tm_h_th_x_R[period] = new TH1D("", ";#theta_{x}^{R}", 100, -150E-6, +150E-6);

				tm_p_diffLR_th_x[period] = new TProfile("", ";#theta_{x}   (#murad);#Delta^{R-L} #theta_{x}   (#murad)", 300, -300E-6, +300E-6);
				tm_p_diffLR_th_y[period] = new TProfile("", ";#theta_{y}   (#murad);#Delta^{R-L} #theta_{y}   (#murad)", 200, -500E-6, +500E-6);

				tm_p_x_L_F_vs_th_x_L[period] = new TProfile("", ";#theta_{x}^{L}   (#murad);x^{LF}   (mm)", 200, -200E-6, +200E-6);
				tm_p_x_R_F_vs_th_x_R[period] = new TProfile("", ";#theta_{x}^{R}   (#murad);x^{RF}   (mm)", 200, -200E-6, +200E-6);
			}

			int idx = g_w_vs_timestamp_sel[period]->GetN();
			g_w_vs_timestamp_sel[period]->SetPoint(idx, ev.timestamp, norm_corr);

			g_y_L_1_F_vs_x_L_1_F_sel[period]->SetPoint(idx, ev.h.L_1_F.x, ev.h.L_1_F.y);
			g_y_L_2_F_vs_x_L_2_F_sel[period]->SetPoint(idx, ev.h.L_2_F.x, ev.h.L_2_F.y);

			g_y_R_1_F_vs_x_R_1_F_sel[period]->SetPoint(idx, ev.h.R_1_F.x, ev.h.R_1_F.y);
			g_y_R_2_F_vs_x_R_2_F_sel[period]->SetPoint(idx, ev.h.R_2_F.x, ev.h.R_2_F.y);
		}

		// fill raw histograms
		h_timestamp_sel->Fill(ev.timestamp);
		if (detailsLevel >= 2)
		{
			g_timestamp_vs_ev_idx_sel->SetPoint(g_timestamp_vs_ev_idx_sel->GetN(), ev_idx, ev.timestamp);
		}

		for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
		{
			h_cq[ci]->Fill(cd.cv[ci]);
			h2_cq[ci]->Fill(cd.cqa[ci], cd.cqb[ci]);
			//h2_cq[ci]->Fill(ccb[ci]*cqa[ci] - cca[ci]*cqb[ci], cca[ci]*cqa[ci] + ccb[ci]*cqb[ci] + ccc[ci]);
			if (detailsLevel >= 2)
				g_cq[ci]->SetPoint(g_cq[ci]->GetN(), cd.cqa[ci], cd.cqb[ci]);
			p_cq[ci]->Fill(cd.cqa[ci], cd.cqb[ci]);
			p_cq_time[ci]->Fill(ev.timestamp, cd.cv[ci]);
		}

		h_y_L_1_F_vs_x_L_1_F_noal_sel->Fill(ev.h.L_1_F.x, ev.h.L_1_F.y);
		h_y_L_2_F_vs_x_L_2_F_noal_sel->Fill(ev.h.L_2_F.x, ev.h.L_2_F.y);
		h_y_R_1_F_vs_x_R_1_F_noal_sel->Fill(ev.h.R_1_F.x, ev.h.R_1_F.y);
		h_y_R_2_F_vs_x_R_2_F_noal_sel->Fill(ev.h.R_2_F.x, ev.h.R_2_F.y);

		h_y_L_1_F_vs_x_L_1_F_al_sel->Fill(h_al.L_1_F.x, h_al.L_1_F.y);
		h_y_L_2_F_vs_x_L_2_F_al_sel->Fill(h_al.L_2_F.x, h_al.L_2_F.y);
		h_y_R_1_F_vs_x_R_1_F_al_sel->Fill(h_al.R_1_F.x, h_al.R_1_F.y);
		h_y_R_2_F_vs_x_R_2_F_al_sel->Fill(h_al.R_2_F.x, h_al.R_2_F.y);

		{
			int idx = g_y_L_2_F_vs_x_L_2_F_al_sel->GetN();
			if (idx < 100000)
			{
				g_y_L_1_F_vs_x_L_1_F_al_sel->SetPoint(idx, h_al.L_1_F.x, h_al.L_1_F.y);
				g_y_L_2_F_vs_x_L_2_F_al_sel->SetPoint(idx, h_al.L_2_F.x, h_al.L_2_F.y);
				g_y_R_1_F_vs_x_R_1_F_al_sel->SetPoint(idx, h_al.R_1_F.x, h_al.R_1_F.y);
				g_y_R_2_F_vs_x_R_2_F_al_sel->SetPoint(idx, h_al.R_2_F.x, h_al.R_2_F.y);
			}
		}

		/*
		p_x_vs_y_L_F->Fill(h_al.y_L_F, h_al.x_L_F);
		p_x_vs_y_L_N->Fill(h_al.y_L_N, h_al.x_L_N);
		p_x_vs_y_R_N->Fill(h_al.y_R_N, h_al.x_R_N);
		p_x_vs_y_R_F->Fill(h_al.y_R_F, h_al.x_R_F);

		p_x_vs_y_L_F_noal->Fill(ev.h.y_L_F, ev.h.x_L_F);
		p_x_vs_y_L_N_noal->Fill(ev.h.y_L_N, ev.h.x_L_N);
		p_x_vs_y_R_N_noal->Fill(ev.h.y_R_N, ev.h.x_R_N);
		p_x_vs_y_R_F_noal->Fill(ev.h.y_R_F, ev.h.x_R_F);
		*/

		/*
		h_x_L_diffFN_vs_x_L_N->Fill(h_al.L_1_F.x, h_al.L_2_F.x - h_al.L_1_F.x);
		h_x_R_diffFN_vs_x_R_N->Fill(h_al.R_1_F.x, h_al.R_2_F.x - h_al.R_1_F.x);

		h_y_L_diffFN_vs_y_L_N->Fill(h_al.L_1_F.y, h_al.L_2_F.y - h_al.L_1_F.y);
		h_y_R_diffFN_vs_y_R_N->Fill(h_al.R_1_F.y, h_al.R_2_F.y - h_al.R_1_F.y);

		{
			int idx = g_x_L_diffFN_vs_x_L_N->GetN();

			g_x_L_diffFN_vs_x_L_N->SetPoint(idx, h_al.L_1_F.x, h_al.L_2_F.x - h_al.L_1_F.x);
			g_x_R_diffFN_vs_x_R_N->SetPoint(idx, h_al.R_1_F.x, h_al.R_2_F.x - h_al.R_1_F.x);

			g_y_L_diffFN_vs_y_L_N->SetPoint(idx, h_al.L_1_F.y, h_al.L_2_F.y - h_al.L_1_F.y);
			g_y_R_diffFN_vs_y_R_N->SetPoint(idx, h_al.R_1_F.y, h_al.R_2_F.y - h_al.R_1_F.y);
		}

		h_y_L_ratioFN_vs_y_L_N->Fill(h_al.L_1_F.y, h_al.L_2_F.y / h_al.L_1_F.y);
		h_y_R_ratioFN_vs_y_R_N->Fill(h_al.R_1_F.y, h_al.R_2_F.y / h_al.R_1_F.y);
		*/

		th_x_diffLR->Fill(k.th_x_R - k.th_x_L);
		th_y_diffLR->Fill(k.th_y_R - k.th_y_L);

		th_x_diffLF->Fill(k.th_x_L - k.th_x);
		th_x_diffRF->Fill(k.th_x_R - k.th_x);

		h_th_x_diffLR_vs_th_x->Fill(k.th_x, k.th_x_R - k.th_x_L);
		h_th_y_diffLR_vs_th_y->Fill(k.th_y, k.th_y_R - k.th_y_L);
		h_th_x_diffLR_vs_vtx_x->Fill(k.vtx_x, k.th_x_R - k.th_x_L);

		h2_th_x_diffLR_vs_th_x->Fill(k.th_x, k.th_x_R - k.th_x_L);
		h2_th_x_L_diffNF_vs_th_x_L->Fill(k.th_x_L, k.th_x_L_2_F - k.th_x_L_1_F);
		h2_th_x_R_diffNF_vs_th_x_R->Fill(k.th_x_R, k.th_x_R_2_F - k.th_x_R_1_F);

		h2_th_y_diffLR_vs_th_y->Fill(k.th_y, k.th_y_R - k.th_y_L);
		h2_th_y_L_diffNF_vs_th_y_L->Fill(k.th_y_L, k.th_y_L_2_F - k.th_y_L_1_F);
		h2_th_y_R_diffNF_vs_th_y_R->Fill(k.th_y_R, k.th_y_R_2_F - k.th_y_R_1_F);

		p_th_x_diffLR_vs_th_x->Fill(k.th_x, k.th_x_R - k.th_x_L);
		p_th_x_L_diffNF_vs_th_x_L->Fill(k.th_x_L, k.th_x_L_2_F - k.th_x_L_1_F);
		p_th_x_R_diffNF_vs_th_x_R->Fill(k.th_x_R, k.th_x_R_2_F - k.th_x_R_1_F);

		p_th_y_diffLR_vs_th_y->Fill(k.th_y, k.th_y_R - k.th_y_L);
		p_th_y_L_diffNF_vs_th_y_L->Fill(k.th_y_L, k.th_y_L_2_F - k.th_y_L_1_F);
		p_th_y_R_diffNF_vs_th_y_R->Fill(k.th_y_R, k.th_y_R_2_F - k.th_y_R_1_F);

		p_th_x_diffLR_vs_vtx_x->Fill(k.vtx_x, k.th_x_R - k.th_x_L);

		const double safe_th_y_min = anal.fc_G_l.th_y_0 + 10E-6;
		const double safe_th_y_max = anal.fc_G_h.th_y_0 - 10E-6;
		const bool safe = fabs(k.th_y) > safe_th_y_min && fabs(k.th_y) < safe_th_y_max;

		if (safe)
		{
			th_y_diffLR_safe->Fill(k.th_y_R - k.th_y_L);
			th_x_diffLR_safe->Fill(k.th_x_R - k.th_x_L);
		}

		p_th_x_vs_th_y->Fill(k.th_y, k.th_x);
		p_th_x_L_vs_th_y_L->Fill(k.th_y_L, k.th_x_L);
		p_th_x_R_vs_th_y_R->Fill(k.th_y_R, k.th_x_R);

		h_th_y_L_vs_th_x_L->Fill(k.th_x_L, k.th_y_L);
		h_th_y_R_vs_th_x_R->Fill(k.th_x_R, k.th_y_R);
		h_th_y_vs_th_x->Fill(k.th_x, k.th_y);

		if (detailsLevel >= 1)
		{
			g_th_y_L_vs_th_x_L->SetPoint(g_th_y_L_vs_th_x_L->GetN(), k.th_x_L, k.th_y_L);
			g_th_y_R_vs_th_x_R->SetPoint(g_th_y_R_vs_th_x_R->GetN(), k.th_x_R, k.th_y_R);
			g_th_y_vs_th_x->SetPoint(g_th_y_vs_th_x->GetN(), k.th_x, k.th_y);
		}

		h_th_y_L_vs_th_y_R->Fill(k.th_y_R, k.th_y_L);

		h_th_x->Fill(k.th_x);
		h_th_y->Fill(k.th_y);
		h_th_y_flipped->Fill(-k.th_y);

		h_th_x_L->Fill(k.th_x_L);
		h_th_x_R->Fill(k.th_x_R);

		h_th_y_L->Fill(k.th_y_L);
		h_th_y_R->Fill(k.th_y_R);


		// fill vertex histograms

		h_vtx_x->Fill(k.vtx_x);
		h_vtx_x_L->Fill(k.vtx_x_L);
		h_vtx_x_R->Fill(k.vtx_x_R);

		h_vtx_y->Fill(k.vtx_y);
		h_vtx_y_L->Fill(k.vtx_y_L);
		h_vtx_y_R->Fill(k.vtx_y_R);

		h_vtx_x_L_vs_vtx_x_R->Fill(k.vtx_x_R, k.vtx_x_L);
		h_vtx_y_L_vs_vtx_y_R->Fill(k.vtx_y_R, k.vtx_y_L);

		h_vtx_x_L_vs_th_x_L->Fill(k.th_x_L, k.vtx_x_L);
		h_vtx_x_R_vs_th_x_R->Fill(k.th_x_R, k.vtx_x_R);
		h_vtx_y_L_vs_th_y_L->Fill(k.th_y_L, k.vtx_y_L);
		h_vtx_y_R_vs_th_y_R->Fill(k.th_y_R, k.vtx_y_R);

		h_vtx_x_diffLR->Fill(k.vtx_x_R - k.vtx_x_L);
		h_vtx_y_diffLR->Fill(k.vtx_y_R - k.vtx_y_L);

		h_vtx_x_diffLR_vs_th_x->Fill(k.th_x, k.vtx_x_R - k.vtx_x_L);
		h_vtx_y_diffLR_vs_th_y->Fill(k.th_y, k.vtx_y_R - k.vtx_y_L);

		p_vtx_x_diffLR_vs_th_x->Fill(k.th_x, k.vtx_x_R - k.vtx_x_L);
		p_vtx_y_diffLR_vs_th_y->Fill(k.th_y, k.vtx_y_R - k.vtx_y_L);

		h_vtx_x_diffLR_vs_vtx_x->Fill(k.vtx_x, k.vtx_x_R - k.vtx_x_L);
		h_vtx_y_diffLR_vs_vtx_y->Fill(k.vtx_y, k.vtx_y_R - k.vtx_y_L);

		if (safe)
		{
			h_vtx_x_safe->Fill(k.vtx_x);
			h_vtx_y_safe->Fill(k.vtx_y);

			h_vtx_x_diffLR_safe->Fill(k.vtx_x_R - k.vtx_x_L);
			h_vtx_y_diffLR_safe->Fill(k.vtx_y_R - k.vtx_y_L);

			h_vtx_x_diffLR_safe_corr->Fill((k.vtx_x_R - k.vtx_x_L) - 1080.*k.th_x);
			h_vtx_y_diffLR_safe_corr->Fill((k.vtx_y_R - k.vtx_y_L) + 156. *k.th_y);
		}

		if (true)
		{
			if (safe)
			{
				constexpr double d_RP = (220.000 - 213.000) * 1E3;	// mm

				const double thl_x_L = (h_al.L_2_F.x - h_al.L_1_F.x) / d_RP;
				const double thl_x_R = (h_al.R_2_F.x - h_al.R_1_F.x) / d_RP;

				const double thl_y_L = (h_al.L_2_F.y - h_al.L_1_F.y) / d_RP;
				const double thl_y_R = (h_al.R_2_F.y - h_al.R_1_F.y) / d_RP;

				opticsMatchingIntput_full.Fill(h_al, thl_x_L, thl_x_R, thl_y_L, thl_y_R, k.vtx_x_L, k.vtx_x_R);
			}
		}

		th_y_L_min = min(th_y_L_min, k.th_y_L);
		th_y_R_min = min(th_y_R_min, k.th_y_R);

		if (fabs(k.th_y) > safe_th_y_min && fabs(k.th_y) < safe_th_y_max)
		{
			p_diffLR_th_x_vs_time->Fill(ev.timestamp, k.th_x_R - k.th_x_L);
			p_diffNF_th_x_L_vs_time->Fill(ev.timestamp, k.th_x_L_2_F - k.th_x_L_1_F);
			p_diffNF_th_x_R_vs_time->Fill(ev.timestamp, k.th_x_R_2_F - k.th_x_R_1_F);

			p_diffLR_th_y_vs_time->Fill(ev.timestamp, k.th_y_R - k.th_y_L);
			p_diffNF_th_y_L_vs_time->Fill(ev.timestamp, k.th_y_L_2_F - k.th_y_L_1_F);
			p_diffNF_th_y_R_vs_time->Fill(ev.timestamp, k.th_y_R_2_F - k.th_y_R_1_F);

			p_vtx_x_vs_time->Fill(ev.timestamp, k.vtx_x);
			p_vtx_y_vs_time->Fill(ev.timestamp, k.vtx_y);

			p_diffLR_vtx_x_vs_time->Fill(ev.timestamp, k.vtx_x_R - k.vtx_x_L);
			p_diffLR_vtx_y_vs_time->Fill(ev.timestamp, k.vtx_y_R - k.vtx_y_L);

			if (detailsLevel >= 2)
			{
				tm_h_th_x_L[period]->Fill(k.th_x_L);
				tm_h_th_x_R[period]->Fill(k.th_x_R);

				tm_p_diffLR_th_x[period]->Fill(k.th_x, k.th_x_R - k.th_x_L);
				tm_p_diffLR_th_y[period]->Fill(k.th_y, k.th_y_R - k.th_y_L);

				tm_p_x_L_F_vs_th_x_L[period]->Fill(k.th_x_L, h_al.L_2_F.x);
				tm_p_x_R_F_vs_th_x_R[period]->Fill(k.th_x_R, h_al.R_2_F.x);
			}
		}

		p_th_x_vs_time->Fill(ev.timestamp, k.th_x);
		p_th_x_L_vs_time->Fill(ev.timestamp, k.th_x_L);
		p_th_x_R_vs_time->Fill(ev.timestamp, k.th_x_R);
		p_th_x_L1F_vs_time->Fill(ev.timestamp, k.th_x_L_1_F);
		p_th_x_L2F_vs_time->Fill(ev.timestamp, k.th_x_L_2_F);
		p_th_x_R1F_vs_time->Fill(ev.timestamp, k.th_x_R_1_F);
		p_th_x_R2F_vs_time->Fill(ev.timestamp, k.th_x_R_2_F);

		p_th_y_R_vs_time->Fill(ev.timestamp, k.th_y_R);
		p_th_y_L_vs_time->Fill(ev.timestamp, k.th_y_L);

		// set time-dependent resolutions
		if (anal.use_resolution_fits)
		{
			anal.si_th_x_LRdiff = accCalc.anal.si_th_x_LRdiff = g_d_x_RMS->Eval(ev.timestamp);
			anal.si_th_y_LRdiff = accCalc.anal.si_th_y_LRdiff = g_d_y_RMS->Eval(ev.timestamp);
		}

		p_input_beam_div_x_vs_time->Fill(ev.timestamp, anal.si_th_x_1arm_L);
		p_input_beam_div_y_vs_time->Fill(ev.timestamp, anal.si_th_y_1arm);

		// calculate acceptance divergence correction
		double phi_corr = 0., div_corr = 0.;
		bool skip = accCalc.Calculate(k, phi_corr, div_corr);

		for (unsigned int bi = 0; bi < binnings.size(); bi++)
		{
			bh_t_Nev_before[bi]->Fill(k.t, 1.);
			bh_t_before[bi]->Fill(k.t, 1.);
		}

		h_th_y_vs_th_x_before->Fill(k.th_x, k.th_y, 1.);

		if (skip)
			continue;

		double corr = div_corr * phi_corr;

		th_min = min(th_min, k.th);

		// fill acceptance histograms
		p_t_ub_div_corr->Fill(k.t_y, div_corr);

		for (unsigned int bi = 0; bi < binnings.size(); bi++)
		{
			bh_t_Nev_after_no_corr[bi]->Fill(k.t, 1.);

			bh_t_after_no_corr[bi]->Fill(k.t, 1.);
			bh_t_after[bi]->Fill(k.t, corr);

			bp_t_phi_corr[bi]->Fill(k.t, phi_corr);
			bp_t_full_corr[bi]->Fill(k.t, corr);
		}

		h_th_y_vs_th_x_after->Fill(k.th_x, k.th_y, div_corr);
		h_th_vs_phi_after->Fill(k.phi, k.th, div_corr);

		if (detailsLevel >= 2)
			g_weight_vs_th_y->SetPoint(g_weight_vs_th_y->GetN(), k.th_y, div_corr);

		// apply normalization
		for (unsigned int bi = 0; bi < binnings.size(); bi++)
			bh_t_normalized[bi]->Fill(k.t, corr * normalization);

		h_th_y_vs_th_x_normalized->Fill(k.th_x, k.th_y, div_corr * normalization);

		if (detailsLevel >= 1)
		{
			g_th_y_vs_th_x_acc->SetPoint(g_th_y_vs_th_x_acc->GetN(), k.th_x, k.th_y);
			g_norm_corr_vs_div_corr->SetPoint(g_norm_corr_vs_div_corr->GetN(), div_corr, norm_corr);
		}
	}

	printf("---------------------------- after event loop ---------------------------\n");

	printf("\nlumi sections: min = %u, max = %u\n", lumi_section_min, lumi_section_max);

	printf("\n");
	for (auto &p : lumiSectionBoundaries)
	{
		printf("run = %u, LS = %u: from %u to %u\n", p.first.first, p.first.second, p.second.first, p.second.second);
	}

	printf("\n");
	for (auto &p : runTimestampBoundaries)
	{
		printf("run %u: from %u to %u\n", p.first, p.second.first, p.second.second);
	}

	printf(">> th_min = %E\n", th_min);
	printf(">> th_y_L_min = %E\n", th_y_L_min);
	printf(">> th_y_R_min = %E\n", th_y_R_min);

	printf("\n");
	printf("N_anal = %u\n", N_anal);

	printf("N_4outof4 = %u\n", N_4outof4);
	printf("N_el = %u\n", N_el);
	printf("N_el_RP_trig = %u\n", N_el_RP_trig);
	printf("N_el_T2trig = %u\n", N_el_T2trig);
	printf("N_4outof4_T2trig = %u\n", N_4outof4_T2trig);

	// derived plots
	TGraphErrors *th_y_sigmaLR_vs_th_y = new TGraphErrors();
	th_y_sigmaLR_vs_th_y->SetName("th_y_sigmaLR_vs_th_y");
	th_y_sigmaLR_vs_th_y->SetTitle(";#theta_{y};RMS of #Delta^{R-L} #theta_{y}");
	ProfileToRMSGraph(p_th_y_diffLR_vs_th_y, th_y_sigmaLR_vs_th_y);

	TGraphErrors *th_x_sigmaLR_vs_th_x = new TGraphErrors();
	th_x_sigmaLR_vs_th_x->SetName("th_x_sigmaLR_vs_th_x");
	th_x_sigmaLR_vs_th_x->SetTitle(";#theta_{x};RMS of #Delta^{R-L} #theta_{x}");
	ProfileToRMSGraph(p_th_x_diffLR_vs_th_x, th_x_sigmaLR_vs_th_x);

	// normalize histograms
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		bh_t_before[bi]->Scale(1., "width");
		bh_t_after_no_corr[bi]->Scale(1., "width");
		bh_t_after[bi]->Scale(1., "width");

		bh_t_normalized[bi]->Scale(1., "width");
	}

	h_th_y_vs_th_x_normalized->Scale(1., "width");

	hb_th_x_L->Scale(1., "width");
	hb_th_x_R->Scale(1., "width");
	hb_th_y_L->Scale(1., "width");
	hb_th_y_R->Scale(1., "width");

	th_y_diffLR->Scale(1., "width");
	th_x_diffLR->Scale(1., "width");
	th_y_diffLR_safe->Scale(1., "width");
	th_x_diffLR_safe->Scale(1., "width");

	// hide bins with high uncertainty
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
		HideLowTBins(bh_t_normalized[bi], anal.t_min_fit);

	// fit histograms
	//double th_y_low_bound = (diagonal == d45b_56t) ? (anal.th_y_lcut_L+anal.th_y_lcut_R)/2. + 5E-6 : -((anal.th_y_hcut_L+anal.th_y_hcut_R)/2. - 5E-6);
	//double th_y_high_bound = (diagonal == d45b_56t) ? (anal.th_y_hcut_L+anal.th_y_hcut_R)/2. - 5E-6 : -((anal.th_y_lcut_L+anal.th_y_lcut_R)/2. + 5E-6);
	const double th_y_low_bound = (diagonal == d45b_56t) ? 280E-6 : -430E-6;
	const double th_y_high_bound = (diagonal == d45b_56t) ? 430E-6 : -280E-6;

	printf("\n* th_y fit bounds: from %E to %E\n", th_y_low_bound, th_y_high_bound);

	printf("\n* fitting p_th_x_diffLR_vs_th_x\n");
	p_th_x_diffLR_vs_th_x->Fit("pol1", "", "", -250E-6, +250E-6);
	printf("\n* fitting p_th_x_L_diffNF_vs_th_x_L\n");
	p_th_x_L_diffNF_vs_th_x_L->Fit("pol1", "", "", -250E-6, +250E-6);
	printf("\n* fitting p_th_x_R_diffNF_vs_th_x_R\n");
	p_th_x_R_diffNF_vs_th_x_R->Fit("pol1", "", "", -250E-6, +250E-6);

	printf("\n* fitting p_th_y_diffLR_vs_th_y\n");
	p_th_y_diffLR_vs_th_y->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);
	printf("\n* fitting p_th_y_L_diffNF_vs_th_y_L\n");
	p_th_y_L_diffNF_vs_th_y_L->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);
	printf("\n* fitting p_th_y_R_diffNF_vs_th_y_R\n");
	p_th_y_R_diffNF_vs_th_y_R->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);

	/*
	printf("\n* fitting p_x_L_F_vs_th_x\n");
	p_x_L_F_vs_th_x->Fit("pol1");
	printf("\n* fitting p_x_L_N_vs_th_x\n");
	p_x_L_N_vs_th_x->Fit("pol1");
	printf("\n* fitting p_x_R_F_vs_th_x\n");
	p_x_R_F_vs_th_x->Fit("pol1");
	printf("\n* fitting p_x_R_N_vs_th_x\n");
	p_x_R_N_vs_th_x->Fit("pol1");

	printf("\n* fitting p_x_L_F_vs_vtx_x\n");
	p_x_L_F_vs_vtx_x->Fit("pol1");
	printf("\n* fitting p_x_L_N_vs_vtx_x\n");
	p_x_L_N_vs_vtx_x->Fit("pol1");
	printf("\n* fitting p_x_R_F_vs_vtx_x\n");
	p_x_R_F_vs_vtx_x->Fit("pol1");
	printf("\n* fitting p_x_R_N_vs_vtx_x\n");
	p_x_R_N_vs_vtx_x->Fit("pol1");

	printf("\n* fitting p_vtx_x_L_vs_th_x_L\n");
	p_vtx_x_L_vs_th_x_L->Fit("pol1", "", "", -120E-6, +120E-6);
	printf("* fitting p_vtx_x_L_vs_th_x\n");
	p_vtx_x_L_vs_th_x->Fit("pol1", "", "", -120E-6, +120E-6);
	printf("* fitting p_vtx_x_R_vs_th_x_R\n");
	p_vtx_x_R_vs_th_x_R->Fit("pol1", "", "", -120E-6, +120E-6);
	printf("* fitting p_vtx_x_R_vs_th_x\n");
	p_vtx_x_R_vs_th_x->Fit("pol1", "", "", -120E-6, +120E-6);
	*/

	printf("* fitting p_vtx_x_diffLR_vs_th_x\n");
	p_vtx_x_diffLR_vs_th_x->Fit("pol1", "", "", -120E-6, +120E-6);

	printf("* fitting p_th_x_vs_th_y\n");
	p_th_x_vs_th_y->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);
	printf("* fitting p_th_x_L_vs_th_y_L\n");
	p_th_x_L_vs_th_y_L->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);
	printf("* fitting p_th_x_R_vs_th_y_R\n");
	p_th_x_R_vs_th_y_R->Fit("pol1", "", "", th_y_low_bound, th_y_high_bound);

	printf("* fitting h_th_x_R\n");
	h_th_x_R->Fit("gaus", "", "", -50E-6, +50E-6);
	printf("* fitting h_th_x_L\n");
	h_th_x_L->Fit("gaus", "", "", -50E-6, +50E-6);

	printf("* fitting p_diffLR_th_x_vs_time\n");
	p_diffLR_th_x_vs_time->Fit("pol1");
	printf("* fitting p_diffLR_th_y_vs_time\n");
	p_diffLR_th_y_vs_time->Fit("pol1");

	printf("* fitting p_th_x_R_vs_time\n");
	p_th_x_R_vs_time->Fit("pol1");
	printf("* fitting p_th_y_R_vs_time\n");
	p_th_y_R_vs_time->Fit("pol1");
	printf("* fitting p_th_x_L_vs_time\n");
	p_th_x_L_vs_time->Fit("pol1");
	printf("* fitting p_th_y_L_vs_time\n");
	p_th_y_L_vs_time->Fit("pol1");

	th_y_diffLR_safe->Fit("gaus");
	th_x_diffLR_safe->Fit("gaus");

	// apply unfolding correction
	map<unsigned int, TH1D *>  bh_t_normalized_unsmeared;
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		bh_t_normalized_unsmeared[bi] = new TH1D(* bh_t_normalized[bi]);
		bh_t_normalized_unsmeared[bi]->SetName("h_t_normalized_unsmeared");

		for (int bin = 1; bin <= bh_t_normalized_unsmeared[bi]->GetNbinsX(); ++bin)
		{
			double c = bh_t_normalized_unsmeared[bi]->GetBinCenter(bin);
			double v = bh_t_normalized_unsmeared[bi]->GetBinContent(bin);
			double v_u = bh_t_normalized_unsmeared[bi]->GetBinError(bin);

			double corr = (unsmearing_correction_fit == NULL) ? 0. : unsmearing_correction_fit->Eval(c);

			bh_t_normalized_unsmeared[bi]->SetBinContent(bin, v * corr);
			bh_t_normalized_unsmeared[bi]->SetBinError(bin, v_u * corr);
		}
	}

	// derived plots
	map<unsigned int, TGraphErrors *> bh_t_normalized_rel_diff, bh_t_normalized_unsmeared_rel_diff;
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		bh_t_normalized_rel_diff[bi] = MakeRelDiff(bh_t_normalized[bi]);
		bh_t_normalized_rel_diff[bi]->SetName("h_t_eb_normalized_rel_diff");

		bh_t_normalized_unsmeared_rel_diff[bi] = MakeRelDiff(bh_t_normalized_unsmeared[bi]);
		bh_t_normalized_unsmeared_rel_diff[bi]->SetName("h_t_eb_normalized_unsmeared_rel_diff");
	}
	// save histograms
	TCanvas *c;

	gDirectory = outF->mkdir("metadata");
	if (detailsLevel >= 2)
	{
		h_timestamp_input->SetLineColor(1);
		h_timestamp_dgn->SetLineColor(2);
		h_timestamp_B0->SetLineColor(6);
		h_timestamp_sel->SetLineColor(4);

		/*
		h_timestamp_dgn->Write();
		h_timestamp_B0->Write();
		h_timestamp_sel->Write();
		*/

		c = new TCanvas("rate cmp");
		h_timestamp_input->Draw();
		h_timestamp_dgn->Draw("sames");
		//h_timestamp_B0->Draw("sames");
		h_timestamp_sel->Draw("sames");
		c->Write();

		//g_timestamp_vs_ev_idx_dgn->Write();
		g_timestamp_vs_ev_idx_sel->Write();

		g_run_vs_timestamp->Write();
		//g_ev_num_vs_timestamp->Write();
		//g_tr_num_vs_timestamp->Write();
		g_bunch_num_vs_timestamp->Write();
		g_selected_bunch_num_vs_timestamp->Write();
	}

	TDirectory *hitDistDir = outF->mkdir("hit distributions");
	gDirectory = hitDistDir->mkdir("vertical, aligned, before selection");
	h_y_L_2_F_vs_x_L_2_F_al_nosel->Write();
	h_y_L_1_F_vs_x_L_1_F_al_nosel->Write();
	h_y_R_1_F_vs_x_R_1_F_al_nosel->Write();
	h_y_R_2_F_vs_x_R_2_F_al_nosel->Write();

	gDirectory = hitDistDir->mkdir("vertical, not aligned, after selection");
	h_y_L_2_F_vs_x_L_2_F_noal_sel->Write();
	h_y_L_1_F_vs_x_L_1_F_noal_sel->Write();
	h_y_R_1_F_vs_x_R_1_F_noal_sel->Write();
	h_y_R_2_F_vs_x_R_2_F_noal_sel->Write();

	gDirectory = hitDistDir->mkdir("vertical, aligned, after selection");
	h_y_L_2_F_vs_x_L_2_F_al_sel->Write();
	h_y_L_1_F_vs_x_L_1_F_al_sel->Write();
	h_y_R_1_F_vs_x_R_1_F_al_sel->Write();
	h_y_R_2_F_vs_x_R_2_F_al_sel->Write();

	g_y_L_2_F_vs_x_L_2_F_al_sel->Write();
	g_y_L_1_F_vs_x_L_1_F_al_sel->Write();
	g_y_R_1_F_vs_x_R_1_F_al_sel->Write();
	g_y_R_2_F_vs_x_R_2_F_al_sel->Write();

	TDirectory *alDir = outF->mkdir("alignment");

	TF1 *ff = new TF1("ff", "[0] + [1]*x");

	TGraphErrors* g_ext_diffLR_th_x_vs_time = new TGraphErrors(); g_ext_diffLR_th_x_vs_time->SetName("g_ext_diffLR_th_x_vs_time");
	TGraphErrors* g_ext_diffLR_th_y_vs_time = new TGraphErrors(); g_ext_diffLR_th_y_vs_time->SetName("g_ext_diffLR_th_y_vs_time");

	TGraphErrors* g_L_L_F_vs_time = new TGraphErrors(); g_L_L_F_vs_time->SetName("g_L_L_F_vs_time");
	TGraphErrors* g_L_R_F_vs_time = new TGraphErrors(); g_L_R_F_vs_time->SetName("g_L_R_F_vs_time");

	for (map<signed int, TGraph *>::iterator pid = g_w_vs_timestamp_sel.begin(); pid != g_w_vs_timestamp_sel.end(); ++pid)
	{
		signed int period = pid->first;

		char buf[100];
		sprintf(buf, "%i", period);
		gDirectory = alDir->mkdir(buf);

		g_w_vs_timestamp_sel[period]->SetName("g_w_vs_timestamp_sel"); g_w_vs_timestamp_sel[period]->Write();

		g_y_L_1_F_vs_x_L_1_F_sel[period]->SetName("g_y_L_1_F_vs_x_L_1_F_sel"); g_y_L_1_F_vs_x_L_1_F_sel[period]->Write();
		g_y_L_2_F_vs_x_L_2_F_sel[period]->SetName("g_y_L_2_F_vs_x_L_2_F_sel"); g_y_L_2_F_vs_x_L_2_F_sel[period]->Write();

		g_y_R_1_F_vs_x_R_1_F_sel[period]->SetName("g_y_R_1_F_vs_x_R_1_F_sel"); g_y_R_1_F_vs_x_R_1_F_sel[period]->Write();
		g_y_R_2_F_vs_x_R_2_F_sel[period]->SetName("g_y_R_2_F_vs_x_R_2_F_sel"); g_y_R_2_F_vs_x_R_2_F_sel[period]->Write();

		tm_h_th_x_L[period]->Write("tm_h_th_x_L");
		tm_h_th_x_R[period]->Write("tm_h_th_x_R");

		TProfile *p_th_x = tm_p_diffLR_th_x[period];
		p_th_x->SetName("p_diffLR_th_x");
		unsigned int reasonableBins_x = SuppressLowStatisticsBins(p_th_x, 5);
		p_th_x->Fit(ff, "Q");
		p_th_x->Write();
		double v_x = ff->GetParameter(0), u_x = ff->GetParError(0);

		TProfile *p_th_y = tm_p_diffLR_th_y[period];
		p_th_y->SetName("p_diffLR_th_y");
		unsigned int reasonableBins_y = SuppressLowStatisticsBins(p_th_y, 5);
		p_th_y->Fit(ff, "Q");
		p_th_y->Write();
		double v_y = ff->GetParameter(0), u_y = ff->GetParError(0);

		double time = anal.alignment_t0 + (period + 0.5) * anal.alignment_ts;
		double time_beg = anal.alignment_t0 + (period + 0.0) * anal.alignment_ts;
		double time_end = anal.alignment_t0 + (period + 1.0) * anal.alignment_ts;
		printf("period %u: from %.0f to %0.f\n", period, time_beg, time_end);

		if (reasonableBins_x > 9)
		{
			int idx = g_ext_diffLR_th_x_vs_time->GetN();
			g_ext_diffLR_th_x_vs_time->SetPoint(idx, time, v_x);
			g_ext_diffLR_th_x_vs_time->SetPointError(idx, 0., u_x);
		}

		if (reasonableBins_y > 9)
		{
			int idx = g_ext_diffLR_th_y_vs_time->GetN();
			g_ext_diffLR_th_y_vs_time->SetPoint(idx, time, v_y);
			g_ext_diffLR_th_y_vs_time->SetPointError(idx, 0., u_y);
		}

		TProfile *p_L_L = tm_p_x_L_F_vs_th_x_L[period];
		p_L_L->SetName("p_x_L_F_vs_th_x_L");
		unsigned int reasonableBins_L_L = SuppressLowStatisticsBins(p_L_L, 5);
		p_L_L->Fit(ff, "Q");
		p_L_L->Write();
		double L_L = ff->GetParameter(1), u_L_L = ff->GetParError(1);

		if (reasonableBins_L_L > 9)
		{
			int idx = g_L_L_F_vs_time->GetN();
			g_L_L_F_vs_time->SetPoint(idx, time, -L_L);
			g_L_L_F_vs_time->SetPointError(idx, 0., u_L_L);
		}

		TProfile *p_L_R = tm_p_x_R_F_vs_th_x_R[period];
		p_L_R->SetName("p_x_R_F_vs_th_x_R");
		unsigned int reasonableBins_L_R = SuppressLowStatisticsBins(p_L_R, 5);
		p_L_R->Fit(ff, "Q");
		p_L_R->Write();
		double L_R = ff->GetParameter(1), u_L_R = ff->GetParError(1);

		if (reasonableBins_L_R > 9)
		{
			int idx = g_L_R_F_vs_time->GetN();
			g_L_R_F_vs_time->SetPoint(idx, time, +L_R);
			g_L_R_F_vs_time->SetPointError(idx, 0., u_L_R);
		}
	}

	TDirectory *cutDir = outF->mkdir("elastic cuts");
	for (unsigned int ci = 1; ci <= anal.N_cuts; ++ci)
	{
		char buf[100];
		sprintf(buf, "cut %u", ci);
		gDirectory = cutDir->mkdir(buf);

		printf("* RMS of cut distribution %u\n", ci);
		printf("%E\n", h_cq[ci]->GetRMS());

		printf("* fitting cut distribution %u\n", ci);
		h_cq[ci]->Fit("gaus", "", "", -3., +3.);

		h_cq[ci]->Write();
		h2_cq[ci]->Write();
		h2_cq_full[ci]->Write();
		g_cq[ci]->Write();

		p_cq[ci]->Write();

		p_cq_time[ci]->Write();

		TGraphErrors *g_cq_time = new TGraphErrors();
		sprintf(buf, "g_cq_RMS%u", ci); g_cq_time->SetName(buf);
		sprintf(buf, ";time   (s);RMS of cq%u", ci); g_cq_time->SetTitle(buf);
		ProfileToRMSGraph(p_cq_time[ci], g_cq_time);
		g_cq_time->Write();

		h2_cq_full[ci]->Draw();

		sprintf(buf, "plot_before_cq%u", ci);
		c = new TCanvas(buf);
		c->SetLogz(1);
		h2_cq_full[ci]->Draw("colz");

		const double x_min = h2_cq_full[ci]->GetXaxis()->GetXmin();
		const double x_max = h2_cq_full[ci]->GetXaxis()->GetXmax();
		double qa[2] = {x_min, x_max};
		double qbp[2]= {(+anal.n_si*anal.csi[ci] - anal.cca[ci]*qa[0] - anal.ccc[ci])/anal.ccb[ci],
			(+anal.n_si*anal.csi[ci] - anal.cca[ci]*qa[1] - anal.ccc[ci])/anal.ccb[ci]};
		double qbm[2]= {(-anal.n_si*anal.csi[ci] - anal.cca[ci]*qa[0] - anal.ccc[ci])/anal.ccb[ci],
			(-anal.n_si*anal.csi[ci] - anal.cca[ci]*qa[1] - anal.ccc[ci])/anal.ccb[ci]};
		TGraph *gP = new TGraph(2, qa, qbp); gP->Draw("l");
		TGraph *gM = new TGraph(2, qa, qbm); gM->Draw("l");
		c->Write();

		sprintf(buf, "plot_after_cq%u", ci);
		c = new TCanvas(buf);
		c->SetLogz(1);
		h2_cq[ci]->Draw("colz");
		gP = new TGraph(2, qa, qbp); gP->Draw("l");
		gM = new TGraph(2, qa, qbm); gM->Draw("l");
		gP->Draw("l");
		gM->Draw("l");
		c->Write();

		TGraph *g_cut_parameters = new TGraph();
		g_cut_parameters->SetPoint(0, 0, anal.cca[ci]);
		g_cut_parameters->SetPoint(1, 1, anal.ccb[ci]);
		g_cut_parameters->SetPoint(2, 2, anal.ccc[ci]);
		g_cut_parameters->SetPoint(3, 3, anal.csi[ci]);
		g_cut_parameters->SetPoint(4, 4, anal.n_si);
		g_cut_parameters->Write("g_cut_parameters");
	}

	gDirectory = outF->mkdir("selected - hits");
	/*
	p_x_vs_y_L_F->Write();
	p_x_vs_y_L_N->Write();
	p_x_vs_y_R_N->Write();
	p_x_vs_y_R_F->Write();

	p_x_vs_y_L_F_noal->Write();
	p_x_vs_y_L_N_noal->Write();
	p_x_vs_y_R_N_noal->Write();
	p_x_vs_y_R_F_noal->Write();

	h_x_L_diffFN_vs_x_L_N->Write();
	h_x_R_diffFN_vs_x_R_N->Write();

	h_y_L_diffFN_vs_y_L_N->Write();
	h_y_R_diffFN_vs_y_R_N->Write();

	g_x_L_diffFN_vs_x_L_N->Write();
	g_x_R_diffFN_vs_x_R_N->Write();

	g_y_L_diffFN_vs_y_L_N->Write();
	g_y_R_diffFN_vs_y_R_N->Write();

	h_y_L_ratioFN_vs_y_L_N->Write();
	h_y_R_ratioFN_vs_y_R_N->Write();
	*/

	gDirectory = outF->mkdir("selected - angles");
	th_x_diffLR->Write();
	th_y_diffLR->Write();

	th_x_diffLF->Write();
	th_x_diffRF->Write();

	h_th_x_diffLR_vs_th_x->Write();
	h_th_y_diffLR_vs_th_y->Write();
	h_th_x_diffLR_vs_vtx_x->Write();

	h2_th_x_diffLR_vs_th_x->Write();
	h2_th_x_L_diffNF_vs_th_x_L->Write();
	h2_th_x_R_diffNF_vs_th_x_R->Write();

	h2_th_y_diffLR_vs_th_y->Write();
	h2_th_y_L_diffNF_vs_th_y_L->Write();
	h2_th_y_R_diffNF_vs_th_y_R->Write();

	p_th_x_diffLR_vs_th_x->Write();
	p_th_x_L_diffNF_vs_th_x_L->Write();
	p_th_x_R_diffNF_vs_th_x_R->Write();

	p_th_y_diffLR_vs_th_y->Write();
	p_th_y_L_diffNF_vs_th_y_L->Write();
	p_th_y_R_diffNF_vs_th_y_R->Write();

	p_th_x_diffLR_vs_vtx_x->Write();

	th_x_sigmaLR_vs_th_x->Write();
	th_y_sigmaLR_vs_th_y->Write();

	th_x_diffLR_safe->Write();
	th_y_diffLR_safe->Write();

	p_th_x_vs_th_y->Write();
	p_th_x_L_vs_th_y_L->Write();
	p_th_x_R_vs_th_y_R->Write();

	h_th_y_L_vs_th_x_L->Write();
	h_th_y_R_vs_th_x_R->Write();
	h_th_y_vs_th_x->Write();

	g_th_y_L_vs_th_x_L->Write();
	g_th_y_R_vs_th_x_R->Write();
	g_th_y_vs_th_x->Write();

	h_th_y_L_vs_th_y_R->Write();

	if (detailsLevel > 2)
	{
		c = new TCanvas();
		c->SetLogz(1);
		h_th_y_L_vs_th_y_R->Draw("colz");
		//g_th_y_L_vs_th_y_R->Draw("p");
		c->Write("canvas_th_y_L_vs_th_y_R");
	}

	h_th_x->Write();
	h_th_y->Write();
	h_th_y_flipped->Write();

	h_th_x_L->Write();
	h_th_x_R->Write();

	h_th_y_L->Write();
	h_th_y_R->Write();

	gDirectory = outF->mkdir("selected - vertex");
	h_vtx_x->Write();
	h_vtx_x_L->Write();
	h_vtx_x_R->Write();

	h_vtx_y->Write();
	h_vtx_y_L->Write();
	h_vtx_y_R->Write();

	h_vtx_x_safe->Write();
	h_vtx_y_safe->Write();

	h_vtx_x_L_vs_vtx_x_R->Write();
	h_vtx_y_L_vs_vtx_y_R->Write();

	h_vtx_x_L_vs_th_x_L->Write();
	h_vtx_x_R_vs_th_x_R->Write();
	h_vtx_y_L_vs_th_y_L->Write();
	h_vtx_y_R_vs_th_y_R->Write();

	h_vtx_x_diffLR->Write();
	h_vtx_y_diffLR->Write();

	h_vtx_x_diffLR_safe->Write();
	h_vtx_y_diffLR_safe->Write();

	h_vtx_x_diffLR_safe_corr->Write();
	h_vtx_y_diffLR_safe_corr->Write();

	h_vtx_x_diffLR_vs_th_x->Write();
	h_vtx_y_diffLR_vs_th_y->Write();

	p_vtx_x_diffLR_vs_th_x->Write();
	p_vtx_y_diffLR_vs_th_y->Write();

	h_vtx_x_diffLR_vs_vtx_x->Write();
	h_vtx_y_diffLR_vs_vtx_y->Write();

	/*
	p_x_L_F_vs_th_x->Write();
	p_x_L_N_vs_th_x->Write();
	p_x_R_F_vs_th_x->Write();
	p_x_R_N_vs_th_x->Write();

	p_x_L_F_vs_vtx_x->Write();
	p_x_L_N_vs_vtx_x->Write();
	p_x_R_F_vs_vtx_x->Write();
	p_x_R_N_vs_vtx_x->Write();

	p_vtx_x_L_vs_th_x_L->Write();
	p_vtx_x_L_vs_th_x->Write();
	p_vtx_x_R_vs_th_x_R->Write();
	p_vtx_x_R_vs_th_x->Write();
	*/


	TDirectory *opticsDir = outF->mkdir("optics");

	gDirectory = opticsDir->mkdir("matching input, full");
	opticsMatchingIntput_full.Write();


	gDirectory = outF->mkdir("binning");
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		TGraph *g = new TGraph();
		g->SetName(("g_binning_"+binnings[bi]).c_str());
		g->SetTitle(";bin center;bin width");

		TH1D *h = bh_t_Nev_before[bi];
		for (int bin = 1; bin <= h->GetNbinsX(); bin++)
			g->SetPoint(g->GetN(), h->GetBinCenter(bin), h->GetBinWidth(bin));

		g->Write();
	}

	gDirectory = outF->mkdir("time dependences");

	TGraph *g_run_boundaries = new TGraph();
	g_run_boundaries->SetName("g_run_boundaries");
	g_run_boundaries->SetTitle(";timestamp;run");
	for (auto &p : runTimestampBoundaries)
	{
		const int idx = g_run_boundaries->GetN();
		g_run_boundaries->SetPoint(idx, p.second.first, p.first);
		g_run_boundaries->SetPoint(idx+1, p.second.second, p.first);
	}
	g_run_boundaries->Write();

	p_diffLR_th_x_vs_time->Write();
	ProfileToRMSGraph(p_diffLR_th_x_vs_time, gRMS_diffLR_th_x_vs_time);
	gRMS_diffLR_th_x_vs_time->Write();

	p_diffNF_th_x_L_vs_time->Write();
	ProfileToRMSGraph(p_diffNF_th_x_L_vs_time, gRMS_diffNF_th_x_L_vs_time);
	gRMS_diffNF_th_x_L_vs_time->Write();

	p_diffNF_th_x_R_vs_time->Write();
	ProfileToRMSGraph(p_diffNF_th_x_R_vs_time, gRMS_diffNF_th_x_R_vs_time);
	gRMS_diffNF_th_x_R_vs_time->Write();

	p_diffLR_th_y_vs_time->Write();
	ProfileToRMSGraph(p_diffLR_th_y_vs_time, gRMS_diffLR_th_y_vs_time);
	gRMS_diffLR_th_y_vs_time->Write();

	p_diffNF_th_y_L_vs_time->Write();
	ProfileToRMSGraph(p_diffNF_th_y_L_vs_time, gRMS_diffNF_th_y_L_vs_time);
	gRMS_diffNF_th_y_L_vs_time->Write();

	p_diffNF_th_y_R_vs_time->Write();
	ProfileToRMSGraph(p_diffNF_th_y_R_vs_time, gRMS_diffNF_th_y_R_vs_time);
	gRMS_diffNF_th_y_R_vs_time->Write();

	p_vtx_x_vs_time->Write();
	ProfileToRMSGraph(p_vtx_x_vs_time, gRMS_vtx_x_vs_time);
	gRMS_vtx_x_vs_time->Write();

	p_vtx_y_vs_time->Write();
	ProfileToRMSGraph(p_vtx_y_vs_time, gRMS_vtx_y_vs_time);
	gRMS_vtx_y_vs_time->Write();

	p_diffLR_vtx_x_vs_time->Write();
	ProfileToRMSGraph(p_diffLR_vtx_x_vs_time, gRMS_diffLR_vtx_x_vs_time);
	gRMS_diffLR_vtx_x_vs_time->Write();

	p_diffLR_vtx_y_vs_time->Write();
	ProfileToRMSGraph(p_diffLR_vtx_y_vs_time, gRMS_diffLR_vtx_y_vs_time);
	gRMS_diffLR_vtx_y_vs_time->Write();

	TGraphErrors *g_beam_div_x_vs_time = new TGraphErrors; g_beam_div_x_vs_time->SetName("g_beam_div_x_vs_time"); g_beam_div_x_vs_time->SetTitle(";timestamp;beam divergence in x");
	TGraphErrors *g_sensor_res_x_vs_time = new TGraphErrors; g_sensor_res_x_vs_time->SetName("g_sensor_res_x_vs_time"); g_sensor_res_x_vs_time->SetTitle(";timestamp;sensor resolution in x");
	for (int i = 0; i <= gRMS_vtx_x_vs_time->GetN(); ++i)
	{
		double time=0., si_diff=0., si_vtx=0.;
		gRMS_vtx_x_vs_time->GetPoint(i, time, si_vtx);
		gRMS_diffLR_th_x_vs_time->GetPoint(i, time, si_diff);

		const double beta_st_x = 11.; // m
		const double si_bdx = si_vtx * sqrt(2.) / beta_st_x * 1E-3;	// in rad
		const double si_srx = sqrt(si_diff*si_diff/2. - si_bdx*si_bdx);

		g_beam_div_x_vs_time->SetPoint(i, time, si_bdx);
		g_sensor_res_x_vs_time->SetPoint(i, time, si_srx);
	}

	g_beam_div_x_vs_time->Write();
	g_sensor_res_x_vs_time->Write();

	p_th_x_vs_time->Write();
	p_th_x_L_vs_time->Write();
	p_th_x_R_vs_time->Write();
	p_th_x_L1F_vs_time->Write();
	p_th_x_L2F_vs_time->Write();
	p_th_x_R1F_vs_time->Write();
	p_th_x_R2F_vs_time->Write();

	p_th_y_R_vs_time->Write();
	p_th_y_L_vs_time->Write();

	g_ext_diffLR_th_x_vs_time->Write();
	g_ext_diffLR_th_y_vs_time->Write();

	p_input_beam_div_x_vs_time->Write();
	p_input_beam_div_y_vs_time->Write();

	g_L_L_F_vs_time->Write();
	g_L_R_F_vs_time->Write();

	TDirectory *fidCutDir = outF->mkdir("fiducial cuts");
	gDirectory = fidCutDir;
	PlotFiductialCut(anal.fc_L_l, th_y_sign)->Write("fc_L_l");
	PlotFiductialCut(anal.fc_L_h, th_y_sign)->Write("fc_L_h");
	PlotFiductialCut(anal.fc_R_l, th_y_sign)->Write("fc_R_l");
	PlotFiductialCut(anal.fc_R_h, th_y_sign)->Write("fc_R_h");
	PlotFiductialCut(anal.fc_G_l, th_y_sign)->Write("fc_G_l");
	PlotFiductialCut(anal.fc_G_h, th_y_sign)->Write("fc_G_h");

	TDirectory *accDir = outF->mkdir("acceptance correction");
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		gDirectory = accDir->mkdir(binnings[bi].c_str());
		bh_t_Nev_before[bi]->Write();
		bh_t_Nev_after_no_corr[bi]->Write();
		bh_t_before[bi]->Write();
		bh_t_after_no_corr[bi]->Write();
		bh_t_after[bi]->Write();
		bp_t_phi_corr[bi]->Write();
		bp_t_full_corr[bi]->Write();

		c = new TCanvas("t cmp");
		c->SetLogy(1);
		bh_t_after[bi]->Draw("");
		bh_t_before[bi]->Draw("same");
		c->Write();
	}

	gDirectory = accDir;

	p_t_ub_div_corr->Write();

	h_th_y_vs_th_x_before->Write();
	h_th_y_vs_th_x_after->Write();
	h_th_vs_phi_after->Write();

	g_weight_vs_th_y->Write();

	g_th_y_vs_th_x_acc->Write();

	TDirectory *normDir = outF->mkdir("normalization");
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		gDirectory = normDir->mkdir(binnings[bi].c_str());
		bh_t_normalized[bi]->Write();

		TH1D *h_t_normalized_no_L = new TH1D(* bh_t_normalized[bi]);
		h_t_normalized_no_L->Scale(anal.L_int);
		h_t_normalized_no_L->Write("h_t_normalized_no_L");

		bh_t_normalized_rel_diff[bi]->Write();
	}

	gDirectory = normDir;
	p_norm_corr->Write();
	p_3outof4_corr->Write();

	h_th_y_vs_th_x_normalized->Write();

	g_norm_corr_vs_div_corr->Write();

	TDirectory *normUnfDir = outF->mkdir("normalization+unfolding");
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		gDirectory = normUnfDir->mkdir(binnings[bi].c_str());

		bh_t_normalized_unsmeared[bi]->Write();
		bh_t_normalized_unsmeared_rel_diff[bi]->Write();
	}

	gDirectory = outF->mkdir("background");
	for (map<unsigned int, TH1D *>::iterator it = hb_cq.begin(); it != hb_cq.end(); ++it)
		it->second->Write();

	if (detailsLevel >= 2)
	{
		hb_th_y_L->SetLineColor(2);
		hb_th_y_R->SetLineColor(4);

		hb_th_y_L->Write();
		hb_th_y_R->Write();

		c = new TCanvas("th_y LR cmp");
		c->SetLogy(1);
		hb_th_y_L->Draw("");
		hb_th_y_R->Draw("same");
		c->Write();

		hb_th_x_L->SetLineColor(2);
		hb_th_x_R->SetLineColor(4);

		hb_th_x_L->Write();
		hb_th_x_R->Write();

		c = new TCanvas("th_x LR cmp");
		c->SetLogy(1);
		hb_th_x_L->Draw("");
		hb_th_x_R->Draw("same");
		c->Write();

		hb_th_y_L_vs_th_x_L->Write();
		hb_th_y_R_vs_th_x_R->Write();

		hb_th_y_diffLR->Write();
		hb_th_x_diffLR->Write();

		hb_th_y_diffLR_vs_th_y->Write();
		hb_th_x_diffLR_vs_th_y->Write();

		hb_th_y_L_vs_th_y_R->Write();
		hb_th_x_L_vs_th_x_R->Write();

		hb_th_y_6cut->Write();
		hb_th_y_6cut_cut7fail->Write();
		TH1D *hb_th_y_6cut_cut7failRatio = new TH1D(*hb_th_y_6cut_cut7fail);
		hb_th_y_6cut_cut7failRatio->SetName("hb_th_y_6cut_cut7failRatio");
		hb_th_y_6cut_cut7failRatio->Divide(hb_th_y_6cut);
		hb_th_y_6cut_cut7failRatio->Write();
	}

	// print counters
	for (map<unsigned int, unsigned long>::iterator it = n_ev_cut.begin(); it != n_ev_cut.end(); ++it)
		printf("\tcut %u: %lu\n", it->first, it->second);

	// clean up
	delete outF;

	//fclose(f_out_cand);

	return 0;
}
