#include "Config.hh"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//----------------------------------------------------------------------------------------------------

void Config::Load(const edm::ParameterSet &ps)
{
	input_files = ps.getParameter<vector<string>>("input_files");
	distilled_files = ps.getParameter<vector<string>>("distilled_files");

	timestamp0 = ps.getParameter<double>("timestamp0");

	timestamp_min = ps.getParameter<double>("timestamp_min");
	timestamp_max = ps.getParameter<double>("timestamp_max");

	timber_dir = ps.getParameter<string>("timber_dir");
}

//----------------------------------------------------------------------------------------------------

void Config::Print() const
{
	printf("%lu input_files:\n", input_files.size());
	for (const auto &f : input_files)
		printf("  %s\n", f.c_str());

	printf("%lu distilled_files:\n", distilled_files.size());
	for (const auto &f : distilled_files)
		printf("  %s\n", f.c_str());

	printf("timestamp0 = %.1f\n", timestamp0);
	printf("timestamp_min = %.1f\n", timestamp_min);
	printf("timestamp_max = %.1f\n", timestamp_max);
}
