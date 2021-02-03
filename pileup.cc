#include "classes/common_init.hh"
#include "classes/command_line_tools.hh"

#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TChain.h"
#include "TH1D.h"
#include "TF1.h"

#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <csignal>

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

	bool pl_suff = false;			// number of planes sufficient for track reco
	bool pl_suff_destr = false;		// number of planes sufficient to destroy track reco when combined with a physics proton

	bool pl_too_full_u = false, pl_too_full_v = false;
	bool pat_suff = false;			// number of patterns sufficient for track reco
	bool pat_suff_destr = false;	// number of patterns sufficient to destroy track reco when combined with a physics proton
	bool pat_more = false;			// number of patterns too high for track reco

	bool tr = false;				// track reconstructed
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

		if (ds.size() == 0)
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

		for (const auto pat : ds)
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
	/*
	for (const auto &ds : *hTracks)
	{
		TotemRPDetId rpId(ds.detId());
		unsigned int rpDecId = 100*rpId.arm() + 10*rpId.station() + 1*rpId.rp();

		for (const auto tr : ds)
		{
			if (tr.isValid())
			{
				flags[rpDecId].tr = true;
				break;
			}
		}
	}
	*/

	for (const auto &tr : *hTracksLite)
	{
		CTPPSDetId rpId(tr.getRPId());
		unsigned int rpDecId = 100*rpId.arm() + 10*rpId.station() + 1*rpId.rp();

		flags[rpDecId].tr = true;
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

	// debug
/*
	for (const auto &fp : flags)
	{
		const auto f = fp.second;

		printf("* RP %u\n", fp.first);

		printf("    hits_per_plane_u (%u): ", f.hits_per_plane_u.size());
		for (const auto &p : f.hits_per_plane_u)
			printf("%u -> %u, ", p.first, p.second);
		printf("\n");

		printf("    hits_per_plane_v (%u): ", f.hits_per_plane_v.size());
		for (const auto &p : f.hits_per_plane_v)
			printf("%u -> %u, ", p.first, p.second);
		printf("\n");

		printf("    n_patterns_u = %u, n_patterns_v = %u\n", f.n_patterns_u, f.n_patterns_v);

		printf("    pl_suff = %u\n", f.pl_suff);
		printf("    pl_too_full_u = %u\n", f.pl_too_full_u);
		printf("    pl_too_full_v = %u\n", f.pl_too_full_v);
		printf("    pat_suff = %u\n", f.pat_suff);
		printf("    pat_more = %u\n", f.pat_more);
		printf("    tr = %u\n", f.tr);
	}
*/
}

//----------------------------------------------------------------------------------------------------

// map: element set, condition, period -> count 
typedef map<string, map<string, map<unsigned int, unsigned long> > > CounterMap;

//----------------------------------------------------------------------------------------------------

void UpdateCounters(map<unsigned int, RPFlags> &flags,
	unsigned int id_L_2_F, unsigned int id_L_1_F, unsigned int id_R_1_F, unsigned int id_R_2_F,
	CounterMap &c, unsigned int period)
{
	const auto &L_2_F = flags[id_L_2_F];
	const auto &L_1_F = flags[id_L_1_F];
	const auto &R_1_F = flags[id_R_1_F];
	const auto &R_2_F = flags[id_R_2_F];	

	c["total"]["total"][period]++;

	if (L_1_F.pl_suff) c["L_1_F"]["pl_suff"][period]++;
	if (L_1_F.pl_suff_destr) c["L_1_F"]["pl_suff_destr"][period]++;
	if (L_1_F.pat_suff) c["L_1_F"]["pat_suff"][period]++;
	if (L_1_F.pat_suff_destr) c["L_1_F"]["pat_suff_destr"][period]++;
	if (L_1_F.pat_more) c["L_1_F"]["pat_more"][period]++;
	if (L_1_F.tr) c["L_1_F"]["tr"][period]++;

	if (L_2_F.pl_suff) c["L_2_F"]["pl_suff"][period]++;
	if (L_2_F.pl_suff_destr) c["L_2_F"]["pl_suff_destr"][period]++;
	if (L_2_F.pat_suff) c["L_2_F"]["pat_suff"][period]++;
	if (L_2_F.pat_suff_destr) c["L_2_F"]["pat_suff_destr"][period]++;
	if (L_2_F.pat_more) c["L_2_F"]["pat_more"][period]++;
	if (L_2_F.tr) c["L_2_F"]["tr"][period]++;

	if (R_1_F.pl_suff) c["R_1_F"]["pl_suff"][period]++;
	if (R_1_F.pl_suff_destr) c["R_1_F"]["pl_suff_destr"][period]++;
	if (R_1_F.pat_suff) c["R_1_F"]["pat_suff"][period]++;
	if (R_1_F.pat_suff_destr) c["R_1_F"]["pat_suff_destr"][period]++;
	if (R_1_F.pat_more) c["R_1_F"]["pat_more"][period]++;
	if (R_1_F.tr) c["R_1_F"]["tr"][period]++;

	if (R_2_F.pl_suff) c["R_2_F"]["pl_suff"][period]++;
	if (R_2_F.pl_suff_destr) c["R_2_F"]["pl_suff_destr"][period]++;
	if (R_2_F.pat_suff) c["R_2_F"]["pat_suff"][period]++;
	if (R_2_F.pat_suff_destr) c["R_2_F"]["pat_suff_destr"][period]++;
	if (R_2_F.pat_more) c["R_2_F"]["pat_more"][period]++;
	if (R_2_F.tr) c["R_2_F"]["tr"][period]++;

	if (L_1_F.pl_suff && L_2_F.pl_suff) c["L_1_F, L_2_F"]["pl_suff && pl_suff"][period]++;
	if (L_1_F.pl_suff || L_2_F.pl_suff) c["L_1_F, L_2_F"]["pl_suff || pl_suff"][period]++;
	if (L_1_F.pl_suff_destr && L_2_F.pl_suff_destr) c["L_1_F, L_2_F"]["pl_suff_destr && pl_suff_destr"][period]++;
	if (L_1_F.pl_suff_destr || L_2_F.pl_suff_destr) c["L_1_F, L_2_F"]["pl_suff_destr || pl_suff_destr"][period]++;
	if (L_1_F.pat_suff && L_2_F.pat_suff) c["L_1_F, L_2_F"]["pat_suff && pat_suff"][period]++;
	if (L_1_F.pat_suff || L_2_F.pat_suff) c["L_1_F, L_2_F"]["pat_suff || pat_suff"][period]++;
	if (L_1_F.pat_suff_destr && L_2_F.pat_suff_destr) c["L_1_F, L_2_F"]["pat_suff_destr && pat_suff_destr"][period]++;
	if (L_1_F.pat_suff_destr || L_2_F.pat_suff_destr) c["L_1_F, L_2_F"]["pat_suff_destr || pat_suff_destr"][period]++;
	if (L_1_F.pat_more && L_2_F.pat_more) c["L_1_F, L_2_F"]["pat_more && pat_more"][period]++;
	if (L_1_F.pat_more || L_2_F.pat_more) c["L_1_F, L_2_F"]["pat_more || pat_more"][period]++;
	if (L_1_F.tr && L_2_F.tr) c["L_1_F, L_2_F"]["tr && tr"][period]++;
	if (L_1_F.tr || L_2_F.tr) c["L_1_F, L_2_F"]["tr || tr"][period]++;

	if (R_1_F.pl_suff && R_2_F.pl_suff) c["R_1_F, R_2_F"]["pl_suff && pl_suff"][period]++;
	if (R_1_F.pl_suff || R_2_F.pl_suff) c["R_1_F, R_2_F"]["pl_suff || pl_suff"][period]++;
	if (R_1_F.pl_suff_destr && R_2_F.pl_suff_destr) c["R_1_F, R_2_F"]["pl_suff_destr && pl_suff_destr"][period]++;
	if (R_1_F.pl_suff_destr || R_2_F.pl_suff_destr) c["R_1_F, R_2_F"]["pl_suff_destr || pl_suff_destr"][period]++;
	if (R_1_F.pat_suff && R_2_F.pat_suff) c["R_1_F, R_2_F"]["pat_suff && pat_suff"][period]++;
	if (R_1_F.pat_suff || R_2_F.pat_suff) c["R_1_F, R_2_F"]["pat_suff || pat_suff"][period]++;
	if (R_1_F.pat_suff_destr && R_2_F.pat_suff_destr) c["R_1_F, R_2_F"]["pat_suff_destr && pat_suff_destr"][period]++;
	if (R_1_F.pat_suff_destr || R_2_F.pat_suff_destr) c["R_1_F, R_2_F"]["pat_suff_destr || pat_suff_destr"][period]++;
	if (R_1_F.pat_more && R_2_F.pat_more) c["R_1_F, R_2_F"]["pat_more && pat_more"][period]++;
	if (R_1_F.pat_more || R_2_F.pat_more) c["R_1_F, R_2_F"]["pat_more || pat_more"][period]++;
	if (R_1_F.tr && R_2_F.tr) c["R_1_F, R_2_F"]["tr && tr"][period]++;
	if (R_1_F.tr || R_2_F.tr) c["R_1_F, R_2_F"]["tr || tr"][period]++;

	if ((L_1_F.pl_suff && L_2_F.pl_suff) && (R_1_F.pl_suff && R_2_F.pl_suff)) c["dgn"]["pl_suff && pl_suff, L && R"][period]++;
	if ((L_1_F.pl_suff && L_2_F.pl_suff) || (R_1_F.pl_suff && R_2_F.pl_suff)) c["dgn"]["pl_suff && pl_suff, L || R"][period]++;
	if ((L_1_F.pl_suff_destr && L_2_F.pl_suff_destr) && (R_1_F.pl_suff_destr && R_2_F.pl_suff_destr)) c["dgn"]["pl_suff_destr && pl_suff_destr, L && R"][period]++;
	if ((L_1_F.pl_suff_destr && L_2_F.pl_suff_destr) || (R_1_F.pl_suff_destr && R_2_F.pl_suff_destr)) c["dgn"]["pl_suff_destr && pl_suff_destr, L || R"][period]++;
	if ((L_1_F.pat_suff && L_2_F.pat_suff) && (R_1_F.pat_suff && R_2_F.pat_suff)) c["dgn"]["pat_suff && pat_suff, L && R"][period]++;
	if ((L_1_F.pat_suff && L_2_F.pat_suff) || (R_1_F.pat_suff && R_2_F.pat_suff)) c["dgn"]["pat_suff && pat_suff, L || R"][period]++;
	if ((L_1_F.pat_suff_destr && L_2_F.pat_suff_destr) && (R_1_F.pat_suff_destr && R_2_F.pat_suff_destr)) c["dgn"]["pat_suff_destr && pat_suff_destr, L && R"][period]++;
	if ((L_1_F.pat_suff_destr && L_2_F.pat_suff_destr) || (R_1_F.pat_suff_destr && R_2_F.pat_suff_destr)) c["dgn"]["pat_suff_destr && pat_suff_destr, L || R"][period]++;
	if ((L_1_F.pat_more || L_2_F.pat_more) && (R_1_F.pat_more || R_2_F.pat_more)) c["dgn"]["pat_more || pat_more, L && R"][period]++;
	if ((L_1_F.pat_more || L_2_F.pat_more) || (R_1_F.pat_more || R_2_F.pat_more)) c["dgn"]["pat_more || pat_more, L || R"][period]++;
	if ((L_1_F.tr && L_2_F.tr) && (R_1_F.tr && R_2_F.tr)) c["dgn"]["tr && tr, L && R"][period]++;
	if ((L_1_F.tr && L_2_F.tr) || (R_1_F.tr && R_2_F.tr)) c["dgn"]["tr && tr, L || R"][period]++;
}

//----------------------------------------------------------------------------------------------------

struct Period
{
	unsigned int run;
	signed int idx;
	time_t ts_first, ts_last;

	unsigned int arrayIndexBeforeSort;
	
	static const time_t width = 300;	// s

	Period(unsigned int _run, signed int _idx, time_t _tsf=0, time_t _tsl=0) :
		run(_run), idx(_idx), ts_first(_tsf), ts_last(_tsl) {}

	void Fill(time_t t)
	{
		ts_first = min(ts_first, t);
		ts_last = max(ts_last, t);
	}

	bool IsCompatible(unsigned int r, time_t t)
	{
		if (r != run)
			return false;

		const signed int t_idx = t / width;
		if (idx != t_idx)
			return false;

		return true;
	}

	bool operator< (const Period &other)
	{
		if (run < other.run) return true;
		if (run > other.run) return false;

		if (idx < other.idx) return true;
		return false;
	}
};

//----------------------------------------------------------------------------------------------------

vector<Period> periods;

unsigned int FillPeriod(unsigned int run, time_t timestamp)
{
	unsigned int pi = periods.size();
	while (pi > 0)
	{
		pi--;
		if (periods[pi].IsCompatible(run, timestamp))
		{
			periods[pi].Fill(timestamp);
			return pi;
		}
	}

	periods.push_back(Period(run, timestamp / Period::width, timestamp, timestamp));
	return periods.size() - 1;
}

//----------------------------------------------------------------------------------------------------

bool interrupt_loop = false;

void SigIntHandler(int)
{
	interrupt_loop = true;
}

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

	// prepare input
	fwlite::ChainEvent event(cfg.input_files);
	printf("* events in input chain: %llu\n", event.size());
	
	// prepare output
	TFile *outF = new TFile((string("pileup_") + cfg.diagonal_str + ".root").c_str(), "recreate");

	// prepare counters and histograms
	map<string, CounterMap> counters;	// map: diagonal label -> CounterMap

	// activate signal handler
	signal(SIGINT, SigIntHandler);

	// loop over events
	unsigned int ev_count = 0;
	for (event.toBegin(); !event.atEnd() && !interrupt_loop; ++event)
	{
		ev_count++;

		const unsigned int ev_run = event.id().run();
		const unsigned int ev_lumi_section = event.id().luminosityBlock();
		const unsigned int ev_bunch = event.bunchCrossing();
		const unsigned int ev_timestamp = event.time().unixTime() - cfg.timestamp0;
		const unsigned int periodIdx = FillPeriod(ev_run, ev_timestamp);

		// check whether the event is to be skipped
		if (anal.SkipEvent(ev_run, ev_lumi_section, ev_timestamp, ev_bunch))
			continue;

		// run analysis
		map<unsigned int, RPFlags> flags;
		EvaluateFlags(event, flags);
		
		UpdateCounters(flags, 25, 5, 104, 124, counters["45b_56t"], periodIdx);
		UpdateCounters(flags, 24, 4, 105, 125, counters["45t_56b"], periodIdx);
	}

	if (interrupt_loop)
		printf("WARNING: User interrupt!\n");

	printf("* events processed: %u\n", ev_count);

	// sort periods
	for (unsigned int i = 0; i < periods.size(); ++i)
		periods[i].arrayIndexBeforeSort = i;

	sort(periods.begin(), periods.end());

	// save results
	for (map<string, CounterMap>::iterator dgni = counters.begin(); dgni != counters.end(); ++dgni)
	{
		TDirectory *dgnDir = outF->mkdir(dgni->first.c_str());
		
		for (auto oi = dgni->second.begin(); oi != dgni->second.end(); ++oi)
		{
			TDirectory *objDir = dgnDir->mkdir(oi->first.c_str());

			for (auto qi = oi->second.begin(); qi != oi->second.end(); ++qi)
			{
				TDirectory *qDir = objDir->mkdir(qi->first.c_str());
				gDirectory = qDir;

				TGraphErrors *g_v = new TGraphErrors(); g_v->SetName("val");
				TGraphErrors *g_r = new TGraphErrors(); g_r->SetName("rel");
				TGraphErrors *g_rate = new TGraphErrors(); g_rate->SetName("rate");
				TGraphErrors *g_run = new TGraphErrors(); g_run->SetName("runs");

				for (const auto &p : periods)
				{
					unsigned int pi = p.arrayIndexBeforeSort;

					auto it = qi->second.find(pi);
					if (it == qi->second.end())
						continue;

					auto v = it->second;

					double t_l = p.ts_first, t_h = p.ts_last;
					double t = (t_h + t_l) / 2.;
					double te = (t_h - t_l) / 2.;
					double tw = t_h - t_l;

					double tot = dgni->second["total"]["total"][pi];

					int idx = g_v->GetN();
					g_v->SetPoint(idx, t, v);
					g_v->SetPointError(idx, te, sqrt(v));
					
					g_r->SetPoint(idx, t, v / tot);
					g_r->SetPointError(idx, te, sqrt(v) / tot);

					g_rate->SetPoint(idx, t, v / tw);
					g_rate->SetPointError(idx, te, sqrt(v) / tw);

					g_run->SetPoint(idx, t, p.run);
					g_run->SetPointError(idx, te, 0.);
				}

				g_v->Write();
				g_r->Write();
				g_rate->Write();
				g_run->Write();
			}
		}
	}

	delete outF;
	return 0;
}
