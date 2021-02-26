#include <stdexcept>
#include "TDirectory.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"

#include "DataFormats/CTPPSDetId/interface/TotemRPDetId.h"
#include "DataFormats/CTPPSReco/interface/CTPPSLocalTrackLite.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

struct Range
{
	double min = +1E100, max = -1E100;

	void fill(double v)
	{
		min = std::min(min, v);
		max = std::max(max, v);
	}
};

//----------------------------------------------------------------------------------------------------

int main()
{
	// defaults
	vector<string> input_files = {
		// fill 7301
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324575.0_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324575.1_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324576.0_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324576.1_re_reco_Totem1.root",

		// fill 7302
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324578.0_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324578.1_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324578.2_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324578.3_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324578.4_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324578.5_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324578.6_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324578.7_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324579.0_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324579.1_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324579.2_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324580.0_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324580.1_re_reco_Totem1.root",
		"root://eostotem.cern.ch///eos/totem/data/offline/2018/450GeV/beta11/Totem1/version5/run_324581.0_re_reco_Totem1.root"
	};

	// prepare input
	fwlite::ChainEvent event(input_files);
	printf("* events in input chain: %llu\n", event.size());

	// prepare ouput file
	TFile *f_out = new TFile("rate_per_ls.root", "recreate");

	// prepare data structures
	map<unsigned int, map<unsigned int, map<unsigned int, unsigned int>>> data; // data[rp id][run][ls] = number of tracks

	map<unsigned int, map<unsigned int, Range>> timestamps; // data[rp id][run][ls] = timestamp range

	// loop over the chain entries
	for (event.toBegin(); ! event.atEnd(); ++event)
	{
		// metadata
		const int timestamp0 = 1539468000;
		int timestamp = event.time().unixTime() - timestamp0;

		timestamps[event.id().run()][event.id().luminosityBlock()].fill(timestamp);

		// load track data
		fwlite::Handle< vector<CTPPSLocalTrackLite> > tracks;
		tracks.getByLabel(event, "ctppsLocalTrackLiteProducer");

		// process track data
		for (const auto &tr : *tracks)
		{
			CTPPSDetId rpId(tr.getRPId());
			unsigned int rpDecId = 100*rpId.arm() + 10*rpId.station() + 1*rpId.rp();

			data[rpDecId][event.id().run()][event.id().luminosityBlock()]++;
		}

	}

	// save plots
	char buf[100];

	for (const auto &rpIt : data)
	{
		sprintf(buf, "RP %i", rpIt.first);
		TDirectory *d_rp = f_out->mkdir(buf);

		for (const auto &runIt : rpIt.second)
		{
			sprintf(buf, "run %i", runIt.first);
			TDirectory *d_run = d_rp->mkdir(buf);
		
			gDirectory = d_run;

			TGraph *g_rate = new TGraph();
			g_rate->SetName("g_rate");

			for (const auto &it : runIt.second)
				g_rate->SetPoint(g_rate->GetN(), it.first, it.second);

			g_rate->Write();
		}
	}

	TDirectory *d_timestamps = f_out->mkdir("timestamps");

	for (const auto &runIt : timestamps)
	{
		sprintf(buf, "run %i", runIt.first);
		TDirectory *d_run = d_timestamps->mkdir(buf);
	
		gDirectory = d_run;

		TGraph *g_rate = new TGraph();
		g_rate->SetName("g_rate");

		TGraph *g_timestamps = new TGraph();
		g_timestamps->SetName("g_timestamps");

		for (const auto &it : runIt.second)
		{
			g_timestamps->SetPoint(g_timestamps->GetN(), it.first, it.second.min);
			g_timestamps->SetPoint(g_timestamps->GetN(), it.first, it.second.max);
		}

		g_timestamps->Write();
	}

	// clean up
	delete f_out;
	return 0;
}
