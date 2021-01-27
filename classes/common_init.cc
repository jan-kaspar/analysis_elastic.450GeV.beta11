#include "common_init.hh"
#include <cstdio>
#include <string>

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSetReader/interface/ParameterSetReader.h"
#include "FWCore/Utilities/interface/Exception.h"

//----------------------------------------------------------------------------------------------------

Config cfg;
Environment env;
Analysis anal;

const int rcIncompatibleDiagonal = 123;

//----------------------------------------------------------------------------------------------------

int Init(const string &cfg_file, const string &diagonal_input)
{
	// check diagonal_input
	cfg.diagonal = dUnknown;
	cfg.diagonal_str = diagonal_input;

	if (cfg.diagonal_str == "45b_56t")
	{
		cfg.diagonal = d45b_56t;
		cfg.th_y_sign = +1.;
		cfg.python_object = "cfg_45b_56t";
	}

	if (cfg.diagonal_str == "45t_56b")
	{
		cfg.diagonal = d45t_56b;
		cfg.th_y_sign = -1.;
		cfg.python_object = "cfg_45t_56b";
	}
	
	if (cfg.diagonal_str == "combined")
	{
		cfg.diagonal = dCombined;
		cfg.python_object = "cfg";
	}
	
	if (cfg.diagonal_str == "45b_56b")
	{
		cfg.diagonal = ad45b_56b;
		cfg.th_y_sign = -1.;
		cfg.python_object = "cfg_45b_56b";
	}

	if (cfg.diagonal_str == "45t_56t")
	{
		cfg.diagonal = ad45t_56t;
		cfg.th_y_sign = +1.;
		cfg.python_object = "cfg_45t_56t";
	}

	if (cfg.diagonal == dUnknown)
	{
		printf("ERROR in Init: unrecognised diagonal %s\n", cfg.diagonal_str.c_str());
		return 1;
	}

	// load python config
	edm::ParameterSet config;
	
	try {
		// split path to directory and file names
		auto p = cfg_file.find_last_of('/');
		const string dir_name = (p != string::npos) ? cfg_file.substr(0, p) : ".";
		string file_name = (p != string::npos) ? cfg_file.substr(p+1) : cfg_file;

		// strip .py from file name
		p = file_name.find_last_of('.');
		file_name = (p != string::npos) ? file_name.substr(0, p) : file_name;

		// compile importing python code
		std::string code;
		code += "import sys\n";
		code += "import os\n";
		code += "sys.path.append(os.path.abspath('" + dir_name + "'))\n";
		code += "from " + file_name + " import *";

		// read in python config
		config = edm::readPSetsFrom(code)->getParameter<edm::ParameterSet>(cfg.python_object);
	}
	catch (const cms::Exception &e)
	{
		printf("ERROR in Init: cannot load object '%s' from file '%s':\n%s", cfg.python_object.c_str(), cfg_file.c_str(), e.what());
		return 2;
	}

	cfg.Load(config);

	const edm::ParameterSet &ps_env = config.getParameterSet("env");
	env.Load(ps_env);

	const edm::ParameterSet &ps_anal = config.getParameterSet("anal");
	anal.Load(ps_anal);

	return 0;
}

//----------------------------------------------------------------------------------------------------

void PrintConfiguration()
{
	printf("\n");
	printf("-------------------------------- config ---------------------------------\n");
	cfg.Print();
	printf("------------------------------ environment ------------------------------\n");
	env.Print();
	printf("------------------------------- analysis --------------------------------\n");
	anal.Print();
	printf("-------------------------------------------------------------------------\n");
	printf("\n");
}
