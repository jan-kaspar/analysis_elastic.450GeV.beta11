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

	// loop over the chain entries
	for (event.toBegin(); ! event.atEnd(); ++event)
	{
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
	for (const auto &rpIt : data)
	{
		char buf[100];

		sprintf(buf, "RP %i", rpIt.first);
		TDirectory *d_rp = f_out->mkdir(buf);

		gDirectory = d_rp;

		for (const auto &runIt : rpIt.second)
		{
			TGraph *g = new TGraph();
			sprintf(buf, "run %i", runIt.first);
			g->SetName(buf);

			for (const auto &it : runIt.second)
				g->SetPoint(g->GetN(), it.first, it.second);

			g->Write();
		}
	}

	// clean up
	delete f_out;
	return 0;
}
