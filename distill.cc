#include "classes/common_init.hh"
#include "classes/command_line_tools.hh"
#include "classes/common_event.hh"

#include "TFile.h"
#include "TTree.h"

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"

#include "DataFormats/CTPPSDetId/interface/TotemRPDetId.h"
#include "DataFormats/CTPPSReco/interface/CTPPSLocalTrackLite.h"

using namespace std;

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
	if (cfg.diagonal == dCombined)
		return rcIncompatibleDiagonal;

	// print settings
	PrintConfiguration();

	// prepare input
	fwlite::ChainEvent event(cfg.input_files);

	printf("* events in input chain: %llu\n", event.size());

	// define RP ids
	unsigned int rpId_L_2_F = 0, rpId_L_1_F = 0, rpId_R_1_F = 0, rpId_R_2_F = 0;

	if (cfg.diagonal == ad45b_56b)
	{
		rpId_L_2_F = 25; rpId_L_1_F = 5; rpId_R_1_F = 105; rpId_R_2_F = 125;
	}

	if (cfg.diagonal == d45b_56t)
	{
		rpId_L_2_F = 25; rpId_L_1_F = 5; rpId_R_1_F = 104; rpId_R_2_F = 124;
	}

	if (cfg.diagonal == d45t_56b)
	{
		rpId_L_2_F = 24; rpId_L_1_F = 4; rpId_R_1_F = 105; rpId_R_2_F = 125;
	}

	if (cfg.diagonal == ad45t_56t)
	{
		rpId_L_2_F = 24; rpId_L_1_F = 4; rpId_R_1_F = 104; rpId_R_2_F = 124;
	}

	printf("* RP ids:\n");
	printf("    rpId_L_2_F = %u\n", rpId_L_2_F);
	printf("    rpId_L_1_F = %u\n", rpId_L_1_F);
	printf("    rpId_R_1_F = %u\n", rpId_R_1_F);
	printf("    rpId_R_2_F = %u\n", rpId_R_2_F);

	// prepare ouput file
	TFile *f_out = new TFile((string("distill_") + cfg.diagonal_str + ".root").c_str(), "recreate");

	// set up output tree
	EventRed ev;
	TTree *t_out = new TTree("distilled", "bla");

	t_out->Branch("v_L_2_F", &ev.h.L_2_F.v); t_out->Branch("x_L_2_F", &ev.h.L_2_F.x); t_out->Branch("y_L_2_F", &ev.h.L_2_F.y);
	t_out->Branch("v_L_1_F", &ev.h.L_1_F.v); t_out->Branch("x_L_1_F", &ev.h.L_1_F.x); t_out->Branch("y_L_1_F", &ev.h.L_1_F.y);

	t_out->Branch("v_R_1_F", &ev.h.R_1_F.v); t_out->Branch("x_R_1_F", &ev.h.R_1_F.x); t_out->Branch("y_R_1_F", &ev.h.R_1_F.y);
	t_out->Branch("v_R_2_F", &ev.h.R_2_F.v); t_out->Branch("x_R_2_F", &ev.h.R_2_F.x); t_out->Branch("y_R_2_F", &ev.h.R_2_F.y);

	t_out->Branch("lumi_section", &ev.lumi_section);
	t_out->Branch("timestamp", &ev.timestamp);
	t_out->Branch("run_num", &ev.run_num);
	t_out->Branch("bunch_num", &ev.bunch_num);
	t_out->Branch("event_num", &ev.event_num);
	//t_out->Branch("trigger_num", &ev.trigger_num);
	//t_out->Branch("trigger_bits", &ev.trigger_bits);

	// loop over the chain entries
	unsigned int ev_count = 0;
	unsigned int ev_accepted = 0;
	for (event.toBegin(); ! event.atEnd(); ++event)
	{
		ev_count++;

		// fill meta data
		ev.lumi_section = event.id().luminosityBlock();
		ev.timestamp = event.time().unixTime() - cfg.timestamp0;
		ev.run_num = event.id().run();
		ev.bunch_num = event.bunchCrossing();
		ev.event_num = event.id().event();
		//ev.trigger_num = 0;
		//ev.trigger_bits = 0;

		// default output track data
		ev.h.L_2_F.v = false; ev.h.L_2_F.x = 0.; ev.h.L_2_F.y = 0.;
		ev.h.L_1_F.v = false; ev.h.L_1_F.x = 0.; ev.h.L_1_F.y = 0.;
		ev.h.R_1_F.v = false; ev.h.R_1_F.x = 0.; ev.h.R_1_F.y = 0.;
		ev.h.R_2_F.v = false; ev.h.R_2_F.x = 0.; ev.h.R_2_F.y = 0.;

		// load track data
		fwlite::Handle< vector<CTPPSLocalTrackLite> > tracks;
		tracks.getByLabel(event, "ctppsLocalTrackLiteProducer");

		// process track data
		for (const auto &tr : *tracks)
		{
			CTPPSDetId rpId(tr.getRPId());
			unsigned int rpDecId = 100*rpId.arm() + 10*rpId.station() + 1*rpId.rp();

			if (rpDecId == rpId_L_2_F)
			{
				ev.h.L_2_F.v = true; ev.h.L_2_F.x = tr.getX(); ev.h.L_2_F.y = tr.getY();
			}

			if (rpDecId == rpId_L_1_F)
			{
				ev.h.L_1_F.v = true; ev.h.L_1_F.x = tr.getX(); ev.h.L_1_F.y = tr.getY();
			}

			if (rpDecId == rpId_R_1_F)
			{
				ev.h.R_1_F.v = true; ev.h.R_1_F.x = tr.getX(); ev.h.R_1_F.y = tr.getY();
			}

			if (rpDecId == rpId_R_2_F)
			{
				ev.h.R_2_F.v = true; ev.h.R_2_F.x = tr.getX(); ev.h.R_2_F.y = tr.getY();
			}
		}

		unsigned N_L = 0;
		if (ev.h.L_1_F.v) N_L++;
		if (ev.h.L_2_F.v) N_L++;

		unsigned N_R = 0;
		if (ev.h.R_1_F.v) N_R++;
		if (ev.h.R_2_F.v) N_R++;

		bool accept = (N_L >= 1 && N_R >= 1);

		if (!accept)
			continue;

		ev_accepted++;

		// save record
		t_out->Fill();
	}

	printf("* events processed: %i\n", ev_count);
	printf("* events accepted: %i\n", ev_accepted);

	// save output tree
	gDirectory = f_out;
	t_out->Write();

	// clean up
	delete f_out;
	return 0;
}
