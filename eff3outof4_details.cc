#include "classes/common_init.hh"
#include "classes/command_line_tools.hh"
#include "classes/common_event.hh"

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"

#include <vector>
#include <map>
#include <string>

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"

#include "DataFormats/CTPPSDetId/interface/TotemRPDetId.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSet.h"

#include "DataFormats/CTPPSReco/interface/TotemRPRecHit.h"
#include "DataFormats/CTPPSReco/interface/TotemRPUVPattern.h"
//#include "DataFormats/CTPPSReco/interface/TotemRPLocalTrack.h"
#include "DataFormats/CTPPSReco/interface/CTPPSLocalTrackLite.h"

using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------

struct RPFlags
{
	map<unsigned int, unsigned int> hits_per_plane_u, hits_per_plane_v;

	unsigned int n_patterns_u = 0, n_patterns_v = 0;

	bool pl_insuff = false;			// number of planes insufficient for track reco
	bool pl_suff = false;			// number of planes sufficient for track reco
	bool pl_suff_destr = false;		// number of planes sufficient to destroy track reco when combined with a physics proton

	bool pl_too_full_u = false, pl_too_full_v = false;
	bool pat_suff = false;			// number of patterns sufficient for track reco
	bool pat_suff_destr = false;	// number of patterns sufficient to destroy track reco when combined with a physics proton
	bool pat_more = false;			// number of patterns too high for track reco

	bool tr = false;				// track reconstructed

	double x = 0., y = 0.;			// track position
};

//----------------------------------------------------------------------------------------------------

void EvaluateFlags(const fwlite::ChainEvent &event, map<unsigned int, RPFlags> &flags)
{
	fwlite::Handle< DetSetVector<TotemRPRecHit> > hRecHits;
	hRecHits.getByLabel(event, "totemRPRecHitProducer");

	fwlite::Handle< DetSetVector<TotemRPUVPattern> > hPatterns;
	hPatterns.getByLabel(event, "totemRPUVPatternFinder");

	//fwlite::Handle< DetSetVector<TotemRPLocalTrack> > hTracks;
	//hTracks.getByLabel(event, "totemRPLocalTrackFitter");

	fwlite::Handle< vector<CTPPSLocalTrackLite> > hTracksLite;
	hTracksLite.getByLabel(event, "ctppsLocalTrackLiteProducer");

	// process rec hits
	for (const auto &ds : *hRecHits)
	{
		TotemRPDetId rpId(ds.detId());
		unsigned int rpDecId = 100*rpId.arm() + 10*rpId.station() + 1*rpId.rp();
		unsigned int plane = rpId.plane();

		if (ds.empty())
			continue;

		if (rpId.isStripsCoordinateUDirection())
			flags[rpDecId].hits_per_plane_u[plane] += ds.size();
		else
			flags[rpDecId].hits_per_plane_v[plane] += ds.size();
	}

	// process patterns
	for (const auto &ds : *hPatterns)
	{
		TotemRPDetId rpId(ds.detId());
		unsigned int rpDecId = 100*rpId.arm() + 10*rpId.station() + 1*rpId.rp();

		for (const auto& pat : ds)
		{
			if (!pat.getFittable())
				continue;

			if (pat.getProjection() == TotemRPUVPattern::projU)
				flags[rpDecId].n_patterns_u++;

			if (pat.getProjection() == TotemRPUVPattern::projV)
				flags[rpDecId].n_patterns_v++;
		}
	}

	// process tracks
	for (const auto &tr : *hTracksLite)
	{
		CTPPSDetId rpId(tr.getRPId());
		unsigned int rpDecId = 100*rpId.arm() + 10*rpId.station() + 1*rpId.rp();

		flags[rpDecId].tr = true;
		flags[rpDecId].x = tr.getX();
		flags[rpDecId].y = tr.getY();
	}

	// apply logic
	for (auto &p : flags)
	{
		RPFlags &f = p.second;

		/* 
		reconstruction settings:
			maxHitsPerPlaneToSearch = 5
			minPlanesPerProjectionToSearch = 3
			minPlanesPerProjectionToFit = 3
		*/

		f.pl_insuff = (f.hits_per_plane_u.size() < 3 || f.hits_per_plane_v.size() < 3);
		f.pl_suff = (f.hits_per_plane_u.size() >= 3 && f.hits_per_plane_v.size() >= 3);
		f.pl_suff_destr = (f.hits_per_plane_u.size() >= 3 || f.hits_per_plane_v.size() >= 3);

		unsigned int n_too_full_u = 0;
		for (const auto &it : f.hits_per_plane_u)
			if (it.second > 5)
				n_too_full_u++;
		f.pl_too_full_u = (n_too_full_u >= 3);

		unsigned int n_too_full_v = 0;
		for (const auto &it : f.hits_per_plane_v)
			if (it.second > 5)
				n_too_full_v++;
		f.pl_too_full_v = (n_too_full_v >= 3);

		const unsigned int n_patterns_eff_u = (f.pl_too_full_u) ? 100 : f.n_patterns_u;
		const unsigned int n_patterns_eff_v = (f.pl_too_full_v) ? 100 : f.n_patterns_v;

		f.pat_suff = (n_patterns_eff_u > 0 && n_patterns_eff_v > 0);
		f.pat_more = (n_patterns_eff_u > 1 || n_patterns_eff_v > 1);

		f.pat_suff_destr = (n_patterns_eff_u > 0 || n_patterns_eff_v > 0);
	}
}

//----------------------------------------------------------------------------------------------------

struct HistGroup
{
	bool initialised = false;

	TH1D *h_y = nullptr, *h_th_x = nullptr, *h_th_y = nullptr;
	TH2D *h_th_x_th_y = nullptr;

	void Init()
	{
		initialised = true;

		gDirectory = nullptr;

		h_y = new TH1D("", ";y   (mm)", 100, -40., +40.);

		h_th_x = new TH1D("", ";#theta_{x}   (#murad)", 200, -400., +400.);
		h_th_y = new TH1D("", ";#theta_{y}   (#murad)", 100, -150., +150.);

		h_th_x_th_y = new TH2D("", ";#theta_{x}   (#murad);#theta_{y}   (#murad)", 40, -400., 400., 60, -150., 150.);
	}

	void Fill(double y, double th_x, double th_y)
	{
		if (!initialised)
			Init();

		h_y->Fill(y);

		h_th_x->Fill(th_x * 1E6);
		h_th_y->Fill(th_y * 1E6);
		h_th_x_th_y->Fill(th_x * 1E6, th_y * 1E6);
	}

	void Write()
	{
		h_y->Write();

		h_th_x->Write();
		h_th_y->Write();
		h_th_x_th_y->Write();
	}
};

//----------------------------------------------------------------------------------------------------

// map: element set, condition -> group of histograms
typedef map<string, map<string, HistGroup> > CounterMap;

//----------------------------------------------------------------------------------------------------

void AnalyzeOnePot(const string &pot_excl,
	const unsigned int id_L_2_F, const unsigned int id_L_1_F,
	const unsigned int id_R_1_F, const unsigned int id_R_2_F,
	map<unsigned int, RPFlags> &flags, const vector<AlignmentData> &alignment, CounterMap &c)
{
	// which pots are selected for track/event definition
	const bool sel_L_2_F = (pot_excl.find("L_2_F") == string::npos);
	const bool sel_L_1_F = (pot_excl.find("L_1_F") == string::npos);
	const bool sel_R_1_F = (pot_excl.find("R_1_F") == string::npos);
	const bool sel_R_2_F = (pot_excl.find("R_2_F") == string::npos);

	// do all selected pots have tracks?
	const bool skip = (sel_L_2_F && !flags[id_L_2_F].tr) || (sel_L_1_F && !flags[id_L_1_F].tr)
		|| (sel_R_1_F && !flags[id_R_1_F].tr) || (sel_R_2_F && !flags[id_R_2_F].tr);

	if (skip)
		return;

	// apply alignment
	HitData h_raw;
	h_raw.L_2_F.x = flags[id_L_2_F].x; h_raw.L_2_F.y = flags[id_L_2_F].y;
	h_raw.L_1_F.x = flags[id_L_1_F].x; h_raw.L_1_F.y = flags[id_L_1_F].y;
	h_raw.R_1_F.x = flags[id_R_1_F].x; h_raw.R_1_F.y = flags[id_R_1_F].y;
	h_raw.R_2_F.x = flags[id_R_2_F].x; h_raw.R_2_F.y = flags[id_R_2_F].y;
	
	HitData h_al = h_raw;
	for (const auto al : alignment)
		h_al = h_al.ApplyAlignment(al);

	// calculate elastic kinematics for the selected tracks
	double th_y_L_sel = 0., th_x_L_sel = 0., norm_L = 0.;
	if (sel_L_2_F) { norm_L += 1.; th_x_L_sel += -h_al.L_2_F.x / env.L_x_L_2_F; th_y_L_sel += -h_al.L_2_F.y / env.L_y_L_2_F; }
	if (sel_L_1_F) { norm_L += 1.; th_x_L_sel += -h_al.L_1_F.x / env.L_x_L_1_F; th_y_L_sel += -h_al.L_1_F.y / env.L_y_L_1_F; }
	th_x_L_sel /= norm_L; th_y_L_sel /= norm_L;
	
	double th_y_R_sel = 0., th_x_R_sel = 0., norm_R = 0.;
	if (sel_R_2_F) { norm_R += 1.; th_x_R_sel += +h_al.R_2_F.x / env.L_x_R_2_F; th_y_R_sel += +h_al.R_2_F.y / env.L_y_R_2_F; }
	if (sel_R_1_F) { norm_R += 1.; th_x_R_sel += +h_al.R_1_F.x / env.L_x_R_1_F; th_y_R_sel += +h_al.R_1_F.y / env.L_y_R_1_F; }
	th_x_R_sel /= norm_R; th_y_R_sel /= norm_R;

	const double th_x_sel = (th_x_R_sel + th_x_L_sel) / 2.;
	const double th_y_sel = (th_y_R_sel + th_y_L_sel) / 2.;

	const double de_th_x_sel = th_x_R_sel - th_x_L_sel;
	const double de_th_y_sel = th_y_R_sel - th_y_L_sel;

	// can this be elastic event
	// si_de_... determined by eff3outof4.cc
	const double si_de_th_x = 46E-6;
	const double si_de_th_y = 8.5E-6;
	const double n_si = 3.;

	const bool cut_th_x = (fabs(de_th_x_sel) < n_si * si_de_th_x);
	const bool cut_th_y = (fabs(de_th_y_sel) < n_si * si_de_th_y);

	if (!cut_th_x || !cut_th_y)
		return;

	// pot under test
	unsigned int id_test;
	double y_test = 0., th_x_test = 0., th_y_test = 0.;
	if (pot_excl.compare("L_2_F") == 0) { id_test = id_L_2_F; y_test = - h_al.L_2_F.y; th_x_test = - h_al.L_2_F.x / env.L_x_L_2_F; th_y_test = - h_al.L_2_F.y / env.L_y_L_2_F; }
	if (pot_excl.compare("L_1_F") == 0) { id_test = id_L_1_F; y_test = - h_al.L_1_F.y; th_x_test = - h_al.L_1_F.x / env.L_x_L_1_F; th_y_test = - h_al.L_1_F.y / env.L_y_L_1_F; }
	if (pot_excl.compare("R_1_F") == 0) { id_test = id_R_1_F; y_test = + h_al.R_1_F.y; th_x_test = + h_al.R_1_F.x / env.L_x_R_1_F; th_y_test = + h_al.R_1_F.y / env.L_y_R_1_F; }
	if (pot_excl.compare("R_2_F") == 0) { id_test = id_R_2_F; y_test = + h_al.R_2_F.y; th_x_test = + h_al.R_2_F.x / env.L_x_R_2_F; th_y_test = + h_al.R_2_F.y / env.L_y_R_2_F; }

	// reference quantities
	const double th_x_ref = th_x_sel;
	const double th_y_ref = th_y_sel;
	const double y_ref = y_test;

	// what happens with the test pot
	const RPFlags &flags_test = flags[id_test];

	const bool rp_test_pl_insuff = flags_test.pl_insuff;

	const bool rp_test_pl_suff = flags_test.pl_suff;
	const bool rp_test_pl_suff_no_track = rp_test_pl_suff && !flags_test.tr;

	const bool rp_test_pat_more = flags_test.pat_more;

	const bool rp_test_track = flags_test.tr;

	const bool rp_test_track_compatible = rp_test_track
		&& (fabs(th_x_test - th_x_ref) < n_si * si_de_th_x)
		&& (fabs(th_y_test - th_y_ref) < n_si * si_de_th_y);

	c[pot_excl]["anything"].Fill(y_ref, th_x_ref, th_y_ref);
	
	if (rp_test_pl_insuff)
		c[pot_excl]["pl_insuff"].Fill(y_ref, th_x_ref, th_y_ref);
	if (rp_test_pl_suff_no_track)
		c[pot_excl]["pl_suff_no_track"].Fill(y_ref, th_x_ref, th_y_ref);
	if (rp_test_pat_more)
		c[pot_excl]["pat_more"].Fill(y_ref, th_x_ref, th_y_ref);
	if (rp_test_track)
		c[pot_excl]["track"].Fill(y_ref, th_x_ref, th_y_ref);
	if (rp_test_track_compatible)
		c[pot_excl]["track_compatible"].Fill(y_ref, th_x_ref, th_y_ref);
}

//----------------------------------------------------------------------------------------------------

void AnalyzeDiagonal(const unsigned int id_L_2_F, const unsigned int id_L_1_F,
	const unsigned int id_R_1_F, const unsigned int id_R_2_F,
	map<unsigned int, RPFlags> &flags, const vector<AlignmentData> &al, CounterMap &c)
{
	AnalyzeOnePot("L_2_F", id_L_2_F, id_L_1_F, id_R_1_F, id_R_2_F, flags, al, c);
	AnalyzeOnePot("L_1_F", id_L_2_F, id_L_1_F, id_R_1_F, id_R_2_F, flags, al, c);
	AnalyzeOnePot("R_1_F", id_L_2_F, id_L_1_F, id_R_1_F, id_R_2_F, flags, al, c);
	AnalyzeOnePot("R_2_F", id_L_2_F, id_L_1_F, id_R_1_F, id_R_2_F, flags, al, c);
}

//----------------------------------------------------------------------------------------------------

TH1D* MakeSimpleRatio(TH1D *num, TH1D *den)
{
	TH1D *rat = new TH1D(*num);

	for (int i = 1; i <= rat->GetNbinsX(); i++)
	{
		double n = num->GetBinContent(i);
		double d = den->GetBinContent(i);

		double r = (d > 0.) ? n/d : 0.;
		double v = d * r * (1. - r);
		double e = (v >= 0. && d > 0.) ? sqrt(v) / d : 0.;

		rat->SetBinContent(i, r);
		rat->SetBinError(i, e);
	}

	return rat;
}

//----------------------------------------------------------------------------------------------------

TH2D* MakeSimpleRatio(TH2D *num, TH2D *den)
{
	TH2D *rat = new TH2D(*num);
	for (int i = 1; i <= rat->GetNbinsX(); i++)
	{
		for (int j = 1; j <= rat->GetNbinsY(); j++)
		{
			double n = num->GetBinContent(i, j);
			double d = den->GetBinContent(i, j);
	
			double r = (d > 0.) ? n/d : 0.;
			double v = d * r * (1. - r);
			double e = (v >= 0. && d > 0.) ? sqrt(v) / d : 0.;
	
			rat->SetBinContent(i, j, r);
			rat->SetBinError(i, j, e);
		}
	}

	return rat;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: program <option> <option>\n");
	printf("OPTIONS:\n");
	printf("    -cfg <file>       config file\n");
	printf("    -dgn <string>     diagonal\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	// defaults
	string cfg_file = "config.py";
	string diagonal_input = "";

	// parse command line
	for (int argi = 1; (argi < argc) && (cl_error == 0); ++argi)
	{
		if (strcmp(argv[argi], "-h") == 0 || strcmp(argv[argi], "--help") == 0)
		{
			cl_error = 1;
			continue;
		}

		if (TestStringParameter(argc, argv, argi, "-cfg", cfg_file)) continue;
		if (TestStringParameter(argc, argv, argi, "-dgn", diagonal_input)) continue;

		printf("ERROR: unknown option '%s'.\n", argv[argi]);
		cl_error = 1;
	}

	if (cl_error)
	{
		PrintUsage();
		return 1;
	}

	// run initialisation
	if (Init(cfg_file, diagonal_input) != 0)
		return 2;

	// compatibility check
	if (cfg.diagonal != dCombined)
		return rcIncompatibleDiagonal;

	// print settings
	PrintConfiguration();
	
	// alignment init
	for (unsigned int i = 0; i < anal.alignment_sources.size(); ++i)
	{
		printf("\n---------- alignment source %u ----------\n", i);
		anal.alignment_sources[i].Init();
	}
	printf("\n\n");

	// get input
	fwlite::ChainEvent event(cfg.input_files);
	printf("* events in input chain: %llu\n", event.size());
	
	// prepare output
	TFile *f_out = new TFile("eff3outof4_details.root", "recreate");

	// prepare counters and histograms
	map<string, CounterMap> counters;	// map: diagonal label -> CounterMap

	// loop over events
	unsigned int ev_count = 0;
	for (event.toBegin(); !event.atEnd(); ++event)
	{
		ev_count++;

		const unsigned int ev_run_num = event.id().run();
		const unsigned int ev_lumi_section = event.id().luminosityBlock();
		const unsigned int ev_timestamp = event.time().unixTime() - cfg.timestamp0;
		const unsigned int ev_bunch_num = event.bunchCrossing();

		// check whether the event is to be skipped
		if (anal.SkipEvent(ev_run_num, ev_lumi_section, ev_timestamp, ev_bunch_num))
			continue;

		// evaluate flags
		map<unsigned int, RPFlags> flags;
		EvaluateFlags(event, flags);

		// get fine alignment
		vector<AlignmentData> alignmentData;
		for (const auto &src : anal.alignment_sources)
			alignmentData.push_back(src.Eval(event.time().unixTime() - cfg.timestamp0));

		// run analysis
		AnalyzeDiagonal(25, 5, 104, 124, flags, alignmentData, counters["45b_56t"]);
		AnalyzeDiagonal(24, 4, 105, 125, flags, alignmentData, counters["45t_56b"]);
	}

	// save results
	for (map<string, CounterMap>::iterator dgni = counters.begin(); dgni != counters.end(); ++dgni)
	{
		TDirectory *dgnDir = f_out->mkdir(dgni->first.c_str());
		
		for (CounterMap::iterator oi = dgni->second.begin(); oi != dgni->second.end(); ++oi)
		{
			TDirectory *objDir = dgnDir->mkdir(oi->first.c_str());

			TH1D *h_th_x_tot = new TH1D(*oi->second["anything"].h_th_x);
			TH1D *h_th_y_tot = new TH1D(*oi->second["anything"].h_th_y);
			TH2D *h_th_x_th_y_tot = new TH2D(*oi->second["anything"].h_th_x_th_y);

			for (map<string, HistGroup>::iterator ci = oi->second.begin(); ci != oi->second.end(); ++ci)
			{
				TDirectory *condDir = objDir->mkdir(ci->first.c_str());
				gDirectory = condDir;

				char buf[100];

				//--------------------

				sprintf(buf, "th_x");
				ci->second.h_th_x->SetName(buf);
				ci->second.h_th_x->Write();
				
				TH1D *h_rat_th_x = MakeSimpleRatio(ci->second.h_th_x, h_th_x_tot);
				sprintf(buf, "th_x : rel");
				h_rat_th_x->SetName(buf);
				h_rat_th_x->Write();

				//--------------------

				sprintf(buf, "th_y");
				ci->second.h_th_y->SetName(buf);
				ci->second.h_th_y->Write();
				
				TH1D *h_rat_th_y = MakeSimpleRatio(ci->second.h_th_y, h_th_y_tot);
				sprintf(buf, "th_y : rel");
				h_rat_th_y->SetName(buf);
				h_rat_th_y->Write();

				//--------------------

				sprintf(buf, "th_x, th_y");
				ci->second.h_th_x_th_y->SetName(buf);
				ci->second.h_th_x_th_y->Write();
				
				TH2D *h_rat_th_x_th_y = MakeSimpleRatio(ci->second.h_th_x_th_y, h_th_x_th_y_tot);
				sprintf(buf, "th_x, th_y : rel");
				h_rat_th_x_th_y->SetName(buf);
				h_rat_th_x_th_y->Write();
			}

			delete h_th_x_tot;
			delete h_th_y_tot;
			delete h_th_x_th_y_tot;
		}
	}

	delete f_out;
	return 0;
}
