#include "classes/command_line_tools.hh"

#include "TFile.h"
#include "TH1D.h"

#include <cstdio>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------------------------------

struct SHist
{
	TH1D *hist;
	double scale;
	SHist(TH1D *_h, double _s) : hist(_h), scale(_s) {}
};

//----------------------------------------------------------------------------------------------------

TH1D* Merge(const vector<SHist> &hists, bool sumBins)
{
	// prepare merged histogram
	TH1D *m = new TH1D(*hists[0].hist);

	// merge number of entries
	unsigned int entries = 0;
	for (unsigned int hi = 0; hi < hists.size(); hi++)
		entries += hists[hi].hist->GetEntries();

	m->SetEntries(entries);

	// merge bins
	for (int bi = 1; bi <= m->GetNbinsX(); bi++)
	{
		if (sumBins)
		{
			// sum bins
			double Sv = 0., Su2 = 0.;
			for (unsigned int hi = 0; hi < hists.size(); hi++)
			{
				double v = hists[hi].hist->GetBinContent(bi);
				double u = hists[hi].hist->GetBinError(bi) * hists[hi].scale;

				Sv += v;
				Su2 += u * u;
			}

			double v = Sv;
			double u = sqrt(Su2);

			m->SetBinContent(bi, v);
			m->SetBinError(bi, u);
		} else {
			// average bins
			double Svw = 0., Sw = 0.;
			for (unsigned int hi = 0; hi < hists.size(); hi++)
			{
				double v = hists[hi].hist->GetBinContent(bi);
				double e = hists[hi].hist->GetBinError(bi) * hists[hi].scale;
				double w = (e > 0.) ? 1./e/e : 0.;

				Sw += w;
				Svw += v * w;
			}

			double v = (Sw > 0.) ? Svw / Sw : 0.;
			double e = (Sw > 0.) ? 1. / sqrt(Sw) : 0.;

			m->SetBinContent(bi, v);
			m->SetBinError(bi, e);
		}
	}

	return m;
}

//----------------------------------------------------------------------------------------------------

struct Entry
{
	string input;
	double stat_unc_scale;
	string output;
	bool merge;

	Entry(const string &_i, double _sus, const string &_o, bool _m) : input(_i), stat_unc_scale(_sus), output(_o), merge(_m) {}
};

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: merge <option> <option> ...\n");
	printf("OPTIONS:\n");
	printf("    -output <file>     set output file\n");
	printf("    -entry <string>    add entry in format 'directory,scale,label,merge'\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	// defaults
	vector<string> input_entries;
	string output_file_name = "merged.root";

	// parse command line
	for (int argi = 1; (argi < argc) && (cl_error == 0); ++argi)
	{
		if (strcmp(argv[argi], "-h") == 0 || strcmp(argv[argi], "--help") == 0)
		{
			cl_error = 1;
			continue;
		}

		if (TestStringParameter(argc, argv, argi, "-output", output_file_name)) continue;

		if (strcmp(argv[argi], "-entry") == 0)
		{
			input_entries.push_back(argv[++argi]);
			continue;
		}

		printf("ERROR: unknown option '%s'.\n", argv[argi]);
		cl_error = 1;
	}

	if (cl_error)
	{
		PrintUsage();
		return 1;
	}

	// build list of entries
	vector<Entry> entries;

	if (! input_entries.empty())
	{
		for (const auto &ie : input_entries)
		{
			istringstream iss(ie);
			string dir; getline(iss, dir, ',');
			string scale; getline(iss, scale, ',');
			string label; getline(iss, label, ',');
			string merge; getline(iss, merge, ',');

			entries.emplace_back(dir, atof(scale.c_str()), label, atoi(merge.c_str()));
		}

	} else {
		printf("* using default list of entries\n");

		entries.push_back(Entry("fill7301/Totem1", 1., "fill7301", true));
		entries.push_back(Entry("fill7302/Totem1", 1., "fill7302", true));
	}

	vector<string> diagonals;
	diagonals.push_back("45b_56t");
	diagonals.push_back("45t_56b");

	vector<string> binnings;
	binnings.push_back("sb1");
	binnings.push_back("sb2");
	binnings.push_back("sb3");

	// print info
	printf("* %lu entries:\n", entries.size());
	for (const auto& e : entries)
		printf("  %s, %.3f, %s, %u\n", e.input.c_str(), e.stat_unc_scale, e.output.c_str(), e.merge);

	// prepare output
	TFile *f_out = new TFile(output_file_name.c_str(), "recreate");

	// loop over binnings
	for (unsigned int bi = 0; bi < binnings.size(); bi++)
	{
		printf("\t%s\n", binnings[bi].c_str());

		TDirectory *binningDir = f_out->mkdir(binnings[bi].c_str());

		// list of histograms for final merge
		vector<SHist> full_list_L, full_list_no_L;				

		// map: diagonal --> list of inputs
		map<string, vector<SHist> > full_map_L, full_map_no_L;

		for (unsigned int ei = 0; ei < entries.size(); ei++)
		{
			printf("\t\t%s\n", entries[ei].output.c_str());

			TDirectory *datasetDir = binningDir->mkdir(entries[ei].output.c_str());

			vector<SHist> ds_list_L, ds_list_no_L;
			for (unsigned int dgni = 0; dgni < diagonals.size(); dgni++)
			{
				printf("\t\t\t%s\n", diagonals[dgni].c_str());

				string fn = entries[ei].input + "/distributions_"+diagonals[dgni]+".root";
				TFile *f_in = TFile::Open(fn.c_str());
				if (!f_in)
				{
					printf("ERROR: File `%s' cannot be opened.\n", fn.c_str());
					return 1;
				}

				string on = "normalization+unfolding/"+binnings[bi]+"/h_t_normalized_unsmeared";
				TH1D *h_norm_L = (TH1D *) f_in->Get(on.c_str());
				if (!h_norm_L)
				{
					printf("ERROR: Object `%s' cannot be loaded from file `%s'.\n", on.c_str(), fn.c_str());
					return 2;
				}

				on = "normalization/"+binnings[bi]+"/h_t_normalized_no_L";
				TH1D *h_norm_no_L = (TH1D *) f_in->Get(on.c_str());
				if (!h_norm_no_L)
				{
					printf("ERROR: Object `%s' cannot be loaded from file `%s'.\n", on.c_str(), fn.c_str());
					return 2;
				}

				h_norm_L->SetName("h_dsdt");
				h_norm_no_L->SetName("h_dNdt");

				SHist sc_hist_L(h_norm_L, entries[ei].stat_unc_scale);
				SHist sc_hist_no_L(h_norm_no_L, entries[ei].stat_unc_scale);

				ds_list_L.push_back(sc_hist_L);
				ds_list_no_L.push_back(sc_hist_no_L);

				if (entries[ei].merge)
				{
					full_list_L.push_back(sc_hist_L);
					full_list_no_L.push_back(sc_hist_no_L);

					full_map_L[diagonals[dgni]].push_back(sc_hist_L);
					full_map_no_L[diagonals[dgni]].push_back(sc_hist_no_L);
				}

				gDirectory = datasetDir->mkdir(diagonals[dgni].c_str());
				h_norm_L->Write();
				h_norm_no_L->Write();
			}

			// save histogram merged from all inputs of a dataset
			gDirectory = datasetDir->mkdir("combined");
			Merge(ds_list_L, false)->Write();
			Merge(ds_list_no_L, true)->Write();
		}

		// save merged histograms
		TDirectory *mergedDir = binningDir->mkdir("merged");

		for (map<string, vector<SHist> >::iterator it = full_map_L.begin(); it != full_map_L.end(); ++it)
		{
			gDirectory = mergedDir->mkdir(it->first.c_str());
			Merge(it->second, false)->Write();

			auto it_no_L = full_map_no_L.find(it->first);
			Merge(it_no_L->second, true)->Write();
		}

		gDirectory = mergedDir->mkdir("combined");
		Merge(full_list_L, false)->Write();
		Merge(full_list_no_L, true)->Write();
	}

	delete f_out;

	return 0;
}
