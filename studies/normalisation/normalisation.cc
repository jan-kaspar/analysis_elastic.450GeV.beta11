#include "classes/common_algorithms.hh"
#include "classes/command_line_tools.hh"

#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"

#include <cstdio>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: program <option> <option>\n");
	printf("OPTIONS:\n");
	printf("    -input <string>           input specification\n");
	printf("    -binning <string>         binning\n");

	printf("    -print-details <bool>     print details\n");
	printf("    -print-python <bool>      print results in python format\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	// defaults
	string input_spec = "";

	string binning = "sb1";

	vector<string> diagonals;
	diagonals.push_back("45b_56t");
	diagonals.push_back("45t_56b");

	bool print_details = true;
	bool print_python = false;

	// parse command line
	for (int argi = 1; (argi < argc) && (cl_error == 0); ++argi)
	{
		if (strcmp(argv[argi], "-h") == 0 || strcmp(argv[argi], "--help") == 0)
		{
			cl_error = 1;
			continue;
		}

		if (TestStringParameter(argc, argv, argi, "-input", input_spec)) continue;

		if (TestStringParameter(argc, argv, argi, "-binning", binning)) continue;

		if (TestBoolParameter(argc, argv, argi, "-print-details", print_details)) continue;
		if (TestBoolParameter(argc, argv, argi, "-print-python", print_python)) continue;

		printf("ERROR: unknown option '%s'.\n", argv[argi]);
		cl_error = 1;
	}

	if (cl_error)
	{
		PrintUsage();
		return 1;
	}

	// validate input
	if (input_spec.empty())
	{
		printf("ERROR: input not specified.\n");
		PrintUsage();
		return 2;
	}

	// process data
	for (unsigned int dgni = 0; dgni < diagonals.size(); dgni++)
	{
		if (print_details)
			printf("* %s\n", diagonals[dgni].c_str());

		TFile *f_in = TFile::Open((input_spec + "/distributions_" + diagonals[dgni]+".root").c_str());
		TH1D *h_in = (TH1D *) f_in->Get(("normalization/" + binning + "/h_t_normalized_no_L").c_str());

		const auto L = GetNormalizationFactor(h_in, print_details);

		if (print_details)
			printf("    L = %.3f\n", L);

		if (print_python)
			printf("cfg_%s.anal.L_int = %.1f\n", diagonals[dgni].c_str(), L);

		delete f_in;
	}

	return 0;
}
